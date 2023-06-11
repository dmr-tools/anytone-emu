use std::io::{Read, Write};
use std::collections::VecDeque;
use log::{error, info, debug};
use std::io::{Result, Error, ErrorKind};
use crate::request::{Request, RequestType};
use crate::response::Response;
use crate::interface::DeviceInterface;
use crate::devicehandler::DeviceHandler;

#[derive(PartialEq)]
enum DeviceState {
  Open, Program, Error, Closed
}

pub struct Device {
  state: DeviceState,
  interface: Box<dyn DeviceInterface+'static>,
  buffer: VecDeque<u8>,
  handler: Box<dyn DeviceHandler+'static>
}

impl Device {
  pub fn new(mut interface: Box<dyn DeviceInterface+'static>, mut handler: Box<dyn DeviceHandler+'static>) -> Device {
    Device { 
      state: DeviceState::Open, 
      interface: interface, 
      buffer: VecDeque::new(),
      handler: handler
    }
  }

  pub fn is_ready(&self) -> bool {
    return DeviceState::Error != self.state && DeviceState::Closed != self.state && self.interface.is_open();
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

  pub fn read_request(&mut self) -> Result<Option<Request>> {
    let mut data = [0u8; 256];
    
    let n = self.interface.read(&mut data)?;
    self.buffer.extend(data[..n].iter());

    if DeviceState::Open == self.state {
      if self.has_string(b"PROGRAM") {
        self.buffer.drain(..7);
        return Ok(Some(Request::program()));
      }
    } else if DeviceState::Program == self.state {
      if (self.buffer.len() >= 1) && (0x02 == self.buffer[0]) {
        self.buffer.drain(..1);
        return Ok(Some(Request::device_info()));
      } else if (self.buffer.len() >= 6) && (b'R' == self.buffer[0]) {
        let packet = self.buffer.drain(..6).collect::<Vec<_>>();
        return Ok(Some(Request::read(
          u32::from_be_bytes(<[u8;4]>::try_from(&packet[1..5]).unwrap()), 
          packet[5])));      
      } else if (self.buffer.len() >= 8) && (b'W' == self.buffer[0]) {
        let header = self.buffer.drain(..6).collect::<Vec<_>>();
        let address = u32::from_be_bytes(<[u8;4]>::try_from(&header[1..5]).unwrap());
        let length : usize = header[5].into();
        let payload: [u8;16] = self.buffer.drain(..16).collect::<Vec<_>>().try_into().unwrap();
        let crc_ack = self.buffer.drain(..2);
        return Ok(Some(Request::write(
          address, 
          payload)));      
      } else if self.has_string(b"END") {
        return Ok(Some(Request::end()));
      } else if 0 != self.buffer.len() {
        let line = self.buffer.iter().map(|x| format!("{:02x}", x)).fold(String::new(), |a, b| a+" "+&b);
        return Err(Error::new(ErrorKind::Other, format!("Unkown request: {}.", line)));
      }
    }

    Ok(None)
  }

  pub fn process(&mut self, request : & Request) -> Result<Option<Response>> {
    if DeviceState::Open == self.state {
      if RequestType::Program == request.request_type {
        self.state = DeviceState::Program;
        return Ok(Some(Response::program_ok()));
      }
    } else if DeviceState::Program == self.state {
      if RequestType::DeviceInfo == request.request_type {
        debug!("Device info");
        return Ok(Some(Response::device_info(
            self.handler.model(), 
            self.handler.version())));
      } else if RequestType::Read == request.request_type {
        debug!("Read from addr {:08x}h length {:02x}h",request.address, request.length);
        return Ok(Some(Response::read(
          request.address, 
          self.handler.read(request.address)?)));
      } else if RequestType::Write == request.request_type {
        debug!("Write to address {:08x}h length {:02x}h", request.address, request.length);
        self.handler.write(request.address, &(request.payload.into()))?;
        return Ok(Some(Response::write()))
      } else if RequestType::End == request.request_type {
        debug!("End of transfer.");
        self.handler.end()?;
        self.state = DeviceState::Closed;
        return Ok(None);
      }
    }

    Err(Error::new(ErrorKind::Other, "Not request not implemented yet."))
  }

  pub fn send_response(&mut self, response: &Response) -> Result<()> {
    self.interface.write(&(response.packet))?;

    Ok(())
  }
}

