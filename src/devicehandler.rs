use std::io::{Result, Error, ErrorKind};

pub trait DeviceHandler {
  fn model(&self) -> &[u8; 6];
  fn version(&self) -> &[u8; 6];

  fn read(&mut self, address : u32) -> Result<&[u8;16]>;
  fn write(&mut self, address: u32, payload : &[u8; 16]) -> Result<()>;
}


pub struct DMR6X2UV {
  current_write_address : u32
}

impl DMR6X2UV {
  pub fn new() -> Self {
    DMR6X2UV {
      current_write_address : 0
    }
  }
}

impl DeviceHandler for DMR6X2UV {
  fn model(&self) -> &[u8; 6] {
    return b"6X2UV2";
  }

  fn version(&self) -> &[u8; 6] {
    return b"V100\x00\x00";
  }

  fn read(&mut self, address : u32) -> Result<&[u8; 16]> {
    if 0x02fa0000 == address {
      return Ok(b"\x00\x00\x00\x00\x01\x01\x01\x00\x00\x01\x01\x20\x20\x20\x20\xff");
    } else if 0x02fa0010 == address {
      return Ok(b"\x44\x38\x37\x38\x55\x56\x00\x01\x00\xff\xff\xff\xff\xff\xff\xff");
    } else if 0x02fa0020 == address {
      return Ok(b"\xff\xff\xff\xff\x00\x00\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00");
    } if 0x02fa0030 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa0040 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa0050 == address {
      return Ok(b"\x31\x32\x33\x34\x35\x36\x37\x38\xff\xff\xff\xff\xff\xff\xff\xff");
    } else if 0x02fa0060 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa0070 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa0080 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa0090 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00a0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00b0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00c0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00d0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00e0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } else if 0x02fa00f0 == address {
      return Ok(b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
    } 

    Err(Error::new(ErrorKind::Other, "Unknown address."))
  }

  fn write(&mut self, address: u32, payload : &[u8;16]) -> Result<()> {
    if self.current_write_address != address {
      println!("--------------------------------------------------------");
    }
    let line = payload.iter().map(|x| format!("{:02x}", x)).fold(String::new(), |a, b| a+" "+&b);
    println!("{:08x}: {}",address, line);
    self.current_write_address += 10;
    Ok(())
  }
}