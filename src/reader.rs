use std::io::Result;

pub trait Reader {
  fn read(&mut self, addr : u32) -> Result<[u8;16]>;
}

