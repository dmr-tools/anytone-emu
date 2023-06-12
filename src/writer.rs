use std::io::{Result, Error, ErrorKind};
use std::io::Write;

pub trait Writer : Drop {
  fn write(&mut self, addr : u32, data : &[u8;16] ) -> Result<()>;
  fn reset(&mut self) -> Result<()>;
}

