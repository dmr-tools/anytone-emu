#include "modelrom.hh"
#include <algorithm>
#include "logger.hh"


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
ModelRom::Segment::contains(uint32_t address, uint16_t size) const {
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
  auto left = std::lower_bound(begin(), end(), address);
  // we use end() as an invalid iterator
  auto prev = (begin() != left) ? left-1 : end(),
      next = (end() != left) ? left+1 : end();

  if ((end() != prev) && ((prev->address + prev->content.size()) == address)) {
    prev->content.append(data);
  } else if ((end() != prev) && prev->contains(address, data.size())) {
    uint32_t offset = address - prev->address;
    memcpy(prev->content.data()+offset, data.data(), data.size());
  } else if ((end() != left) && left->contains(address, data.size())) {
    uint32_t offset = address - left->address;
    memcpy(left->content.data()+offset, data.data(), data.size());
  } else if ((end() == left) || ((address+data.size()) < left->address)) {
    _content.insert(left, {address, data});
  } else {
    _content.insert(next, {address, data});
  }
}


bool
ModelRom::read(uint32_t address, uint16_t length, QByteArray &data) const {
  auto next = std::lower_bound(begin(), end(), address);

  if (begin() == next) {
    if ((end() == next) || (next->address != address) || (! next->contains(address, length))) {
      logDebug() << "Cannot read from rom at address " << QString::number(address, 16)
                 << "h: No segment containing this address found.";
      return false;
    }

    uint32_t offset = address - next->address;
    data = next->content.mid(offset, length);
    return true;
  }

  auto prev = next-1;
  if (! prev->contains(address, length)) {
    logDebug() << "Cannot read from rom at address " << QString::number(address, 16)
               << "h: No segment containing this address found.";
    return false;
  }

  uint32_t offset = address - prev->address;
  data = prev->content.mid(offset, length);
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

