use std::io::{Read, Write};

use serialport::SerialPort;
use std::fs::DirBuilder;
use std::os::unix::io::AsRawFd;
use std::io::{Result, Error, ErrorKind};
use nix::pty::{posix_openpt, ptsname_r, PtyMaster, grantpt, unlockpt};
use nix::fcntl::{OFlag, fcntl, F_GETFD};
use home;
use log::debug;

enum InterfaceType {
  PseudoTerminal, SerialPort 
}

pub struct Interface {
  iftype : InterfaceType,
  pty : Option<PtyMaster> ,
  serial : Option<Box<dyn SerialPort>>
}


impl Interface {
  pub fn from_pty() -> Result<Box<Interface>> {
    // Open PTY
    let ptys = posix_openpt(OFlag::O_RDWR) ?;
    grantpt(&ptys)?;
    unlockpt(&ptys)?;
    let pty_path = ptsname_r(&ptys) ?;

    // Create symlink pty_path -> $HOME/.local/share/anytone-emu/anytoneport
    let mut port_dir = home::home_dir().ok_or(
    Error::new(ErrorKind::Other, "Cannot obtain home directory."))?;
    port_dir.push(".local");
    port_dir.push("share");
    port_dir.push("anytone-emu");
    if ! port_dir.is_dir() {
      debug!("Create directory '{}'.", &port_dir.to_str().ok_or(
        Error::new(ErrorKind::Other, "Cannot obtain port-directory path.")
      )?);
      DirBuilder::new().recursive(true).create(&port_dir) ?;
    }

    let mut port_path = port_dir.clone();
    port_path.push("anytoneport");
    if port_path.is_symlink() {
      debug!("File '{}' already exists, remove it.", &port_path.to_str().ok_or(
        Error::new(ErrorKind::Other, "Cannot obtain port path."))?);
      std::fs::remove_file(&port_path)?;
    }

    debug!("Link pty {} to {}. You may need to edit wine registry.", pty_path, 
      port_path.to_str().ok_or(Error::new(ErrorKind::Other, "Cannot obtain port path."))?);
    std::os::unix::fs::symlink(&pty_path, &port_path) ?;

    // Assemble device
    Ok(Box::new(Interface {
      iftype : InterfaceType::PseudoTerminal,
      pty : Some(ptys),
      serial : None
    }))
  }

  pub fn from_serial(port : &String) -> Result<Box<Interface>> {
    let interface : Box<dyn SerialPort> = 
    match serialport::new(port, 115000).open() {
      Ok(port) => Ok(port),
      Err(e) => Err(Error::new(ErrorKind::Other, e.to_string()))
    }?;
    
    Ok(Box::new(Interface { 
      iftype : InterfaceType::SerialPort,
      pty : None,
      serial : Some(interface)
    }))
  }

  pub fn read(&mut self, buf: &mut [u8]) -> Result<usize> {
    match self.iftype {
      InterfaceType::PseudoTerminal => self.pty.as_mut().expect("PTY not set!").read(buf),
      InterfaceType::SerialPort => self.serial.as_mut().expect("Serial port not set!").read(buf)
    }
  }

  pub fn write(&mut self, buf: &[u8]) -> Result<usize> {
    match self.iftype {
      InterfaceType::PseudoTerminal => self.pty.as_mut().expect("PTY not set!").write(buf),
      InterfaceType::SerialPort => self.serial.as_mut().expect("Serial port not set!").write(buf)
    }
  }
}