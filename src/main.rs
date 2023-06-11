use crate::{interface::create_pty_interface, device::Device};
use crate::{dmr6x2uv::DMR6X2UV, dmr6x2uv2::DMR6X2UV2};

use log::{info, debug};
use stderrlog;
use clap::Parser;

pub mod device;
pub mod interface;
pub mod request;
pub mod response;
pub mod devicehandler;
pub mod dmr6x2uv;
pub mod dmr6x2uv2;

#[derive(Parser)]
#[command(author="Hannes Matuschek <dm3mat [at] darc [dot] de>", 
          version="0.1.0", about="Emulates AnyTone and similar devices.", 
          long_about = None)]
struct Args {
  /// Enables debug output (log to stderr).
  #[arg(long, default_value_t=false)]
  debug: bool,  
  /// Specifies the interface type. Must be either "pty" or "com".
  #[arg(long, default_value="wine")]
  interface: String,
  /// Specifies the radio to emulate. Must be one of "dmr6x2uv", "dmr6x2uv2".
  #[arg(long)]
  radio: String,
  /// Specifies the way, the received codeplug is stored. Must be either "hex" or "dfu".
  #[arg(long, default_value="hex")]
  output: String,
  /// Specifies the file-prefix for the generated files.
  #[arg(long, default_value="codeplug_")]
  prefix: String,
  /// Specifies the source DFU file for the emulator to provide to the CPS.
  #[arg(long)]
  source: Option<String>
}

fn main() {
  let args = Args::parse();

  let mut logger = stderrlog::new();
  logger.module(module_path!());
  if args.debug {
    logger.verbosity(4);
  } else {
    logger.verbosity(2);
  }
  logger.init().unwrap();

  let radio_id = args.radio.to_lowercase();
  let mut dev: Device; 
  if "dmr6x2uv" == radio_id {
    dev = Device::new(
      Box::new(create_pty_interface().unwrap()), 
      Box::new(DMR6X2UV::new()));
  } else if "dmr6x2uv2" == radio_id {
    dev = Device::new(
      Box::new(create_pty_interface().unwrap()), 
      Box::new(DMR6X2UV2::new()));
  } else {
    panic!("Unknown radio ID '{}'", args.radio);
  }
  
  info!("Wait for commands.");

  while dev.is_ready() {
    if let Some(req) = dev.read_request().unwrap() {
      if let Some(resp) = dev.process(&req).unwrap() {
        dev.send_response(&resp).unwrap();
      }
    }
  }
}
