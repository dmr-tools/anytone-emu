use std::io::{Result, Error, ErrorKind};
use std::io::Read;

pub trait Reader : Drop {
  fn read(&mut self, addr : u32) -> Result<[u8;16]>;
}

