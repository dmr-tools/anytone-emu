use std::io::{Result, Error, ErrorKind};

pub trait DeviceHandler {
  fn model(&self) -> &[u8; 6];
  fn version(&self) -> &[u8; 6];

  fn read(&self, address : u32) -> Result<&[u8;16]>;
  fn write(&self, address: u32, payload : &[u8; 16]) -> Result<()>;
}


pub struct DMR6X2UV {

}

impl DMR6X2UV {
  pub fn new() -> Self {
    DMR6X2UV {

    }
  }
}

impl DeviceHandler for DMR6X2UV {
  fn model(&self) -> &[u8; 6] {
    return b"6X2UV\x00";
  }

  fn version(&self) -> &[u8; 6] {
    return b"V102\x00\x00";
  }

  fn read(&self, address : u32) -> Result<&[u8; 16]> {
    if 0x02fa0020 == address {
      return Ok(b"\xff\xff\xff\xff\x00\x00\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00");
    }

    Err(Error::new(ErrorKind::Other, "Unknown address."))
  }

  fn write(&self, address: u32, payload : &[u8;16]) -> Result<()> {
    return Err(Error::new(ErrorKind::Other, "Not implemented yet."))
  }
}