use std::fs::DirBuilder;
use std::io::{Result, Error, ErrorKind};
use nix::pty::{posix_openpt, ptsname_r, PtyMaster, grantpt, unlockpt};
use nix::fcntl::OFlag;
use home;
use log::debug;


pub fn create_device() -> Result<PtyMaster> {
  // Open PTY
  let ptys = posix_openpt(OFlag::O_RDWR) ?;
  grantpt(&ptys)?;
  unlockpt(&ptys)?;
  let pty_path = ptsname_r(&ptys) ?;

  // Create symlink pty_path -> $HOME/.local/share/anytone-emu/anytoneport
  let mut port_dir = match home::home_dir()  {
    Some(path) => Ok(path),
    None => Err(Error::new(ErrorKind::Other, "Cannot obtain home directory."))
  } ?;
  port_dir.push(".local/share/anytone-emu");
  let mut port_path = port_dir.clone();
  port_path.push("anytoneport");
  DirBuilder::new().recursive(true).create(&port_dir) ?;

  let port_path_str = match port_path.to_str() {
    Some(path) => Ok(path),
    None => Err(Error::new(ErrorKind::Other, "Cannot convert path to string."))
  } ?;

  if port_path.exists() {
    std::fs::remove_file(port_path_str)?;
  }
  std::os::unix::fs::symlink(&pty_path, &port_path_str) ?;
  debug!("Linked pty {} to {}. You may need to edit wine registry.", pty_path, port_path_str);

  // Assemble device
  Ok(ptys)
}



