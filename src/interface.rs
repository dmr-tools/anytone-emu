use std::io;


enum InterfaceState {
  S_OPEN, S_PROGRAM, S_READ, S_WRITE, S_ERROR
}

pub struct Interface {
  state: InterfaceState,
  interface: &(Read + Write),
  buffer: VecDeque<u8>,
  device: Device
}

impl Interface {
  pub fn new(interface: &mut (Read + Write), device : &mut Device) -> Interface {
    Interface {
      state: InterfaceState::OPEN,
      interface: interface,
      buffer: VecDeque::new()
    };
  }

  pub fn loop() -> bool {
    let buffer : u8[256]; 
    let n = read(self.interface, buffer);
    self.buffer.append(buffer.into());
    return self.process_buffer();
  }

  fn process_buffer() -> bool {
    if (InterfaceState::S_OPEN == self.state) {
      if ((7 <= self.buffer.len()) and ("PROGRAM" == String::from_utf8_lossy(self.buffer[..8]))) {
        self.state = S_PROGRAM;
        self.buffer.drain(..6);
      }
    }
  }
}
