use crate::interface::create_pty_interface;
use crate::device::Device;
use crate::devicehandler::DMR6X2UV;

use std::io::{Read,Write};
use log::{info, debug};
use stderrlog;

pub mod device;
pub mod interface;
pub mod request;
pub mod response;
pub mod devicehandler;

fn main() {
  stderrlog::new().module(module_path!()).verbosity(4).init().unwrap();

  let mut interface = create_pty_interface().unwrap();
  let mut dev = Device::new(interface, DMR6X2UV::new());
  info!("Wait for commands.");
  loop {
    let req = dev.read_request().unwrap();
    let resp = dev.process(&req).unwrap();
    dev.send_response(&resp).unwrap();
  }
}
