use crate::devicehandler::DeviceHandler;
use crate::interface::Interface;
use crate::{device::Device};
use crate::{d868uve::D868UVE, d878uv::D878UV, d878uv2::D878UV2, d578uv::D578UV, d578uv2::D578UV2};
use crate::{dmr6x2uv::DMR6X2UV, dmr6x2uv2::DMR6X2UV2};
use crate::{writer::Writer, hexwriter::HexWriter, reader::Reader};

use log::{info, debug};
use stderrlog;
use clap::Parser;

pub mod device;
pub mod interface;
pub mod request;
pub mod response;
pub mod devicehandler;
pub mod writer;
pub mod hexwriter;
pub mod reader;
pub mod d868uve;
pub mod d878uv;
pub mod d878uv2;
pub mod d578uv;
pub mod d578uv2;
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
  /// Specifies the radio to emulate. Must be one of "d868uve", "d878uv", "d878uv2", "d578uv", 
  /// "d578uv2", "dmr6x2uv", "dmr6x2uv2".
  #[arg(long)]
  radio: String,
  /// Specifies the way, the received codeplug is stored. Must be either "hex" or "dfu".
  #[arg(long, default_value="hex")]
  output: String,
  /// Specifies the file-prefix for the generated files.
  #[arg(long, default_value="codeplug_")]
  prefix: String,
  /// If present, the emulator does exit, once the connection is closed by the CPS.
  #[arg(long, default_value_t=false)]
  single: bool,
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

  let mut writer: Option<Box<dyn Writer + 'static>> = None;
  let mut reader: Option<Box<dyn Reader + 'static>> = None;

  // Dispatch by output type
  if "hex" == args.output {
    debug!("Use hex-dump output");
    writer = Some(Box::new(HexWriter::new(&args.prefix).unwrap()));
  } 

  // TODO: Dispatch by source
  
  // Dispatch by device ID
  let handler: Box<dyn DeviceHandler>; 
  if "d578uv" == radio_id {
    debug!("Emulate AnyTone D578UV.");
    handler = Box::new(D578UV::new(writer));
  } else if "d578uv2" == radio_id {
    debug!("Emulate AnyTone D578UV II.");
    handler = Box::new(D578UV2::new(writer));
  } else if "d868uve" == radio_id {
    debug!("Emulate AnyTone D868UVE.");
    handler = Box::new(D868UVE::new(writer));
  } else if "d878uv" == radio_id {
    debug!("Emulate AnyTone D878UV.");
    handler = Box::new(D878UV::new(writer));
  } else if "d878uv2" == radio_id {
    debug!("Emulate AnyTone D878UV II.");
    handler = Box::new(D878UV2::new(writer));
  } else if "dmr6x2uv" == radio_id {
    debug!("Emulate BTECH DMR-6X2UV.");
    handler = Box::new(DMR6X2UV::new(writer));
  } else if "dmr6x2uv2" == radio_id {
    debug!("Emulate BTECH DMR-6X2UV PRO.");
    handler = Box::new(DMR6X2UV2::new(writer));
  } else {
    panic!("Unknown radio ID '{}'", args.radio);
  }
  
  // Create device, open with interface later
  let mut device = Device::new(handler);

  loop {
    // Dispatch by interface. Each transfer requires a new interface.
    if "wine" == args.interface {
      debug!("Create pty device.");
      device.open(Interface::from_pty().unwrap())
    } else if args.interface.starts_with("com") {
      debug!("Open serial device {}.", args.interface);
      device.open(Interface::from_serial(&args.interface).unwrap())
    } else {
      panic!("Unknown interface {}", args.interface);
    }
  
    info!("Wait for commands.");
  
    // as long as the PTY/COM port is open -> handle requests
    while device.is_ready() {
      if let Some(req) = device.read_request().unwrap() {
        if let Some(resp) = device.process(&req).unwrap() {
          device.send_response(&resp).unwrap();
        }
      }
    }

    // If only one codeplug is written/read -> exit
    if args.single {
      break;
    }
  }
}
