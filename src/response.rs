
use std::io::{Result, Error, ErrorKind};
use std::num::Wrapping;
use core::cmp::min;

use nix::NixPath;

fn crc(data: &[u8]) -> u8 {
  let mut crc = Wrapping::<u8>(0);
  for i in 0..data.len() {
    crc += data[i];
  }
  return crc.0
}


#[derive(PartialEq)]
pub enum ResponseType {
  ProgramOk, DeviceInfo, Read, Write
}

pub struct Response {
  response_type : ResponseType,
  pub packet : Vec<u8>
}

impl Response {
  pub fn program_ok() -> Self {
    Response {
      response_type : ResponseType::ProgramOk,
      packet : b"QX\x06".to_vec()
    }
  }

  pub fn device_info(model: &[u8], hwversion: &[u8]) -> Self {
    let n = min(model.len(),6);
    let m = min(hwversion.len(),6);
    let mut packet : Vec<u8> = Vec::new();
    packet.resize(16, 0);
    packet[..2].copy_from_slice(b"ID");
    packet[2..(2+n)].copy_from_slice(&model[..n]);
    packet[9..(9+m)].copy_from_slice(&hwversion[..m]);
    packet[15] = 0x06;
    Response {
      response_type : ResponseType::DeviceInfo,
      packet : packet
    }
  }

  pub fn read(address : u32, payload : &[u8]) -> Self {
    let n = min(16, payload.len());
    let mut packet : Vec<u8> = Vec::new();
    packet.resize(n+8, 0);
    packet[0] = b'W';
    packet[1..5].copy_from_slice(&(address.to_be_bytes()));
    packet[5] = payload.len().try_into().unwrap();
    packet[6..(6+n)].copy_from_slice(&payload[..n]);
    packet[6+n] = crc(&packet[1..(5+n)]);
    packet[7+n] = 0x06;

    Response {
      response_type : ResponseType::Read,
      packet : packet
    }
  }

  pub fn write() -> Self {
    let mut packet : Vec<u8> = Vec::new();
    packet.push(0x06); // <- just ACK
    Response {
      response_type : ResponseType::Write,
      packet : packet
    }
  } 

}