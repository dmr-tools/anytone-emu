use std::io::{Read, Write};
use std::collections::VecDeque;
use log::{error, info, debug};
use std::io::{Result, Error, ErrorKind};
use crate::request::{Request, RequestType};
use crate::response::Response;
use crate::devicehandler::DeviceHandler;

#[derive(PartialEq)]
enum DeviceState {
  Open, Program, SError
}

pub struct Device<T : Read+Write, H: DeviceHandler> {
  state: DeviceState,
  interface: T,
  buffer: VecDeque<u8>,
  handler: H
}

impl<T: Read+Write, H: DeviceHandler> Device<T,H> {
  pub fn new(mut interface: T, mut handler: H) -> Device<T,H> {
    Device { 
      state: DeviceState::Open, 
      interface: interface, 
      buffer: VecDeque::new(),
      handler : handler
    }
  }

  fn has_string(&self, string: &[u8]) -> bool {
    if self.buffer.len()<string.len() {
      return false;
    }
    for i in 0..string.len() {
      if self.buffer[i] != string[i] {
        return false;
      }  
    }
    return true;
  }

  pub fn read_request(&mut self) -> Result<Request> {
    let mut data = [0u8; 256];
    
    let n = self.interface.read(&mut data)?;
    self.buffer.extend(data[..n].iter());

    if DeviceState::Open == self.state {
      if self.has_string(b"PROGRAM") {
        self.buffer.drain(..7);
        return Ok(Request::program());
      }
    } else if DeviceState::Program == self.state {
      if (self.buffer.len() >= 1) && (0x02 == self.buffer[0]) {
        self.buffer.drain(..1);
        return Ok(Request::device_info());
      }
      if (self.buffer.len() >= 6) && (b'R' == self.buffer[0]) {
        let packet = self.buffer.drain(..6).collect::<Vec<_>>();
        return Ok(Request::read(
          u32::from_be_bytes(<[u8;4]>::try_from(&packet[1..5]).unwrap()), 
          packet[5]));      
      }
    }

    Err(Error::new(ErrorKind::Other, "Unknown request."))
  }

  pub fn process(&mut self, request : & Request) -> Result<Response> {
    if DeviceState::Open == self.state {
      if RequestType::Program == request.request_type {
        self.state = DeviceState::Program;
        return Ok(Response::program_ok());
      }
    } else if DeviceState::Program == self.state {
      if RequestType::DeviceInfo == request.request_type {
        debug!("Device info");
        return Ok(Response::device_info(
            self.handler.model(), 
            self.handler.version()));
      } else if RequestType::Read == request.request_type {
        debug!("Read from addr {} length {}",request.address, request.length);
        return Ok(Response::read(request.address, &self.handler.read(request.address, request.length)?));
      }
    }

    Err(Error::new(ErrorKind::Other, "Not request not implemented yet."))
  }

  pub fn send_response(&mut self, response: &Response) -> Result<()> {
    self.interface.write(&(response.packet))?;

    Ok(())
  }
}

