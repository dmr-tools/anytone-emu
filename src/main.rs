use crate::device::create_device;
use std::collections::VecDeque;
use std::io::{Read,Write};
use log::{info, debug};
use stderrlog;
pub mod device;


fn buffer_match_string(buffer: &VecDeque<u8>, string: &[u8]) -> bool {
    if buffer.len()<string.len() {
        return false;
    }
    for i in 0..string.len() {
        if buffer[i] != string[i] {
            return false;
        }
    }
    return true;
}

fn main() {
  stderrlog::new().module(module_path!()).verbosity(4).init().unwrap();

  let mut dev = create_device().unwrap();

  let mut buffer: VecDeque<u8> = VecDeque::new();
  info!("Wait for commands.");
  loop {
    let mut data = [0u8; 256];
    let n = dev.read(&mut data).unwrap();
    buffer.extend(data[..n].iter());
    if buffer_match_string(&buffer, "PROGRAM".as_ref()) {
        debug!("Enter program mode.");
    }
  }
}
