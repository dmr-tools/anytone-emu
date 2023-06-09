

#[derive(PartialEq)]
pub enum RequestType {
  Program, DeviceInfo, Read, Write
}

pub struct Request {
  pub request_type: RequestType,
  pub address: u32,
  pub length: u8,
  pub payload: Vec<u8>
}

impl Request {
  pub fn program() -> Self {
    Request {
      request_type : RequestType::Program,
      address : 0,
      length : 0,
      payload : Vec::new()
    }
  }

  pub fn device_info() -> Self {
    Request {
      request_type : RequestType::DeviceInfo,
      address: 0,
      length: 0,
      payload : Vec::new()
    }
  }

  pub fn read(addr:u32, len:u8) -> Self {
    Request {
      request_type: RequestType::Read,
      address: addr,
      length : len,
      payload : Vec::new()
    }
  }
}