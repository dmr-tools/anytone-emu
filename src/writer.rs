use std::io::Result;

pub trait Writer {
  fn write(&mut self, addr : u32, data : &[u8;16] ) -> Result<()>;
  fn reset(&mut self) -> Result<()>;
}

