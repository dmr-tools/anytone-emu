#include "modelrom.hh"
#include <algorithm>


/* ********************************************************************************************** *
 * Implementation of ModelRom::Segment
 * ********************************************************************************************** */
bool
ModelRom::Segment::operator <(const Segment &rhs) const {
  return address < rhs.address;
}

bool
ModelRom::Segment::operator <(uint32_t rhs_address) const {
  return address < rhs_address;
}

bool
ModelRom::Segment::contains(uint32_t address, uint8_t size) const {
  return (this->address <= address)
      && ((this->address+content.size()) >= (address+size));
}



/* ********************************************************************************************** *
 * Implementation of ModelRom
 * ********************************************************************************************** */
ModelRom::ModelRom()
  : _content()
{
  // pass...
}


ModelRom
ModelRom::operator +(const ModelRom &rhs) const {
  ModelRom res(*this);
  foreach (const Segment &segment, rhs) {
    res.write(segment.address, segment.content);
  }
  return res;
}

const ModelRom &
ModelRom::operator +=(const ModelRom &rhs) {
  foreach (const Segment &segment, rhs) {
    write(segment.address, segment.content);
  }
  return *this;
}

unsigned int
ModelRom::segmentCount() const {
  return _content.size();
}

void
ModelRom::write(uint32_t address, const QByteArray &data) {
  auto next = std::lower_bound(begin(), end(), address);

  if ((end() == next) || (next->address > address)) {
    _content.insert(next, {address, data});
  } else if (next->contains(address, data.size())) {
    uint32_t offset = address - next->address;
    memcpy(next->content.data()+offset, data.data(), data.size());
  } else if ((next->address+next->content.size()) == address) {
    next->content.append(data);
  } else {
    _content.insert(++next, {address, data});
  }
}


bool
ModelRom::read(uint32_t address, uint8_t length, QByteArray &data) const {
  auto next = std::lower_bound(begin(), end(), address);

  if (end() == next)
    return false;

  if (! next->contains(address, length))
    return false;

  uint32_t offset = address - next->address;
  data = next->content.mid(offset, length);
  return true;
}


ModelRom::iterator
ModelRom::begin() {
  return _content.begin();
}

ModelRom::iterator
ModelRom::end() {
  return _content.end();
}


ModelRom::const_iterator
ModelRom::begin() const {
  return _content.begin();
}

ModelRom::const_iterator
ModelRom::end() const {
  return _content.end();
}

