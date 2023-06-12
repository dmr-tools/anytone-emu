use crate::writer::Writer;
use std::fs::File;
use std::io::{Result, Error, ErrorKind, Write};

pub struct HexWriter {
  prefix : String,
  counter : u32,
  current_address : u32,
  filename : String,
  outfile : File
}

impl HexWriter {
  pub fn new(prefix : &String) -> Result<HexWriter> {
    let filename = format!("{}{:04}.hex", prefix, 0);
    let file = File::create(&filename)?;
    Ok(HexWriter {
      prefix : prefix.clone(),
      counter : 0,
      current_address : 0,
      filename : filename,
      outfile : file
    })
  }
}

impl Writer for HexWriter {
  fn reset(&mut self) -> Result<()> {
    self.counter += 1;
    let filename = format!("{}{:04}.hex", self.prefix, self.counter);
    self.outfile = File::create(&filename)?;
    self.filename = filename;
    return Ok(());
  }

  fn write(&mut self, addr : u32, data : &[u8; 16]) -> Result<()> {
    if self.current_address != addr {
      self.outfile.write(b"--------------------------------------------------------\n")?;
    }
    let line = data.iter().map(|x| format!("{:02x}", x)).fold(String::new(), |a, b| a+" "+&b);
    self.outfile.write(format!("{:08x}: {}\n",addr, line).as_bytes())?;
    self.current_address = addr + 16;
    Ok(())
  }
}

impl Drop for HexWriter {
  fn drop(&mut self) { 
    self.outfile.flush().unwrap();
  }
}