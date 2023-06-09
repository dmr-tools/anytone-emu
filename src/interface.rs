use std::fs::DirBuilder;
use std::io::{Result, Error, ErrorKind};
use nix::pty::{posix_openpt, ptsname_r, PtyMaster, grantpt, unlockpt};
use nix::fcntl::OFlag;
use home;
use log::debug;


pub fn create_pty_interface() -> Result<PtyMaster> {
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
  Ok(ptys)
}
