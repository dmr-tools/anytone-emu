use std::io::{Result, Error, ErrorKind};

use nix::NixPath;

use crate::reader::Reader;
use crate::writer::Writer;

pub trait DeviceHandler {
  fn model(&self) -> &[u8; 6];
  fn version(&self) -> &[u8; 6];
  
  fn read(&mut self, address : u32) -> Result<&[u8;16]>;
  fn write(&mut self, address: u32, payload : &[u8; 16]) -> Result<()>;

  fn end(&mut self) -> Result<()> {
    return Ok(());
  }
}
