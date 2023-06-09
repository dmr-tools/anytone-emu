use core::cmp::min;


#[derive(PartialEq)]
pub enum RequestType {
  Program, DeviceInfo, Read, Write
}

pub struct Request {
  pub request_type: RequestType,
  pub address: u32,
  pub length: u8,
  pub payload: [u8; 16]
}

impl Request {
  pub fn program() -> Self {
    Request {
      request_type : RequestType::Program,
      address : 0,
      length : 0,
      payload : [0;16]
    }
  }

  pub fn device_info() -> Self {
    Request {
      request_type : RequestType::DeviceInfo,
      address: 0,
      length: 0,
      payload : [0;16]
    }
  }

  pub fn read(addr:u32, len:u8) -> Self {
    Request {
      request_type: RequestType::Read,
      address: addr,
      length : len,
      payload : [0;16]
    }
  }

  pub fn write(addr : u32, payload: [u8;16]) -> Self {
    Request { 
      request_type: RequestType::Write, 
      address: addr, 
      length: min(16, payload.len().try_into().unwrap()), 
      payload: payload 
    }
  }
}