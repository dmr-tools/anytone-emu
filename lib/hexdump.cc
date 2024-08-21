#include "hexdump.hh"
#include "image.hh"
#include "annotation.hh"
#include <QTextStream>


/* ********************************************************************************************* *
 * Implementation of HexLine
 * ********************************************************************************************* */
HexLine::HexLine(uint32_t address, const QByteArray &left)
  : _address((address>>4)<<4), _consumed(0), _left(), _right(), _hasDiff(false)
{
  // If address does not align with 16 bytes, prepend with unused bytes to ensure alignment
  if (_address < address)
    _left.fill({Byte::Unused, 0}, address-_address);


  // Copy consumed bytes as "kept"
  unsigned int bytes_left = (0x10+_address-address); // < bytes left to fill the line
  _consumed = std::min(bytes_left, (unsigned int)left.size());
  for (unsigned i=0; i<_consumed; i++) {
    _left.append({Byte::Keep, (uint8_t)left.at(i)});
  }

  // Fill remaining line with unused bytes
  if ((unsigned int)left.size()<bytes_left) {
    for (unsigned int i=0; i<(unsigned int)(bytes_left-left.size()); i++)
      _left.append({Byte::Unused, 0});
  }
}

HexLine::HexLine(uint32_t address, const QByteArray &left, const QByteArray &right)
  : _address((address>>4)<<4), _consumed(0), _left(), _right(), _hasDiff(false)
{
  // If address does not align with 16 bytes, prepend with unused bytes to ensure alignment
  if (_address < address) {
    _left.fill({Byte::Unused, 0}, address-_address);
    _right.fill({Byte::Unused, 0}, address-_address);
  }

  unsigned int bytes_left = (0x10+_address-address); // < bytes left to fill the line
  unsigned int left_consumed = std::min(bytes_left, (unsigned int)left.size());
  unsigned int right_consumed = std::min(bytes_left, (unsigned int)right.size());
  _consumed = std::max(left_consumed, right_consumed);
  for (unsigned i=0; i<_consumed; i++) {
    if ((i < left_consumed) && (i<right_consumed)) {
      if (left.at(i) == right.at(i)) {
        _left.append({Byte::Keep, (uint8_t)left.at(i)});
        _right.append({Byte::Keep, (uint8_t)right.at(i)});
      } else {
        _left.append({Byte::Remove, (uint8_t)left.at(i)});
        _right.append({Byte::Add, (uint8_t)right.at(i)});
        _hasDiff = true;
      }
    } else if (i<left_consumed) {
      _left.append({Byte::Remove, (uint8_t)left.at(i)});
      _right.append({Byte::Unused, 0});
      _hasDiff = true;
    } else if (i<right_consumed) {
      _left.append({Byte::Unused, 0});
      _right.append({Byte::Add, (uint8_t)right.at(i)});
      _hasDiff = true;
    }
  }
}

HexLine::HexLine(const HexLine &other)
  : _address(other._address), _consumed(other._consumed), _left(other._left), _right(other._right),
    _hasDiff(other._hasDiff)
{
  // pass...
}

HexLine &
HexLine::operator =(const HexLine &other) {
  _address = other._address;
  _consumed = other._consumed;
  _left = other._left;
  _right = other._right;
  _hasDiff = other._hasDiff;
  return *this;
}

uint32_t
HexLine::address() const {
  return _address;
}

unsigned int
HexLine::consumed() const {
  return _consumed;
}

const QVector<HexLine::Byte> &
HexLine::left() const {
  return _left;
}

const HexLine::Byte &
HexLine::left(unsigned int i) const {
  return _left[i];
}

HexLine::Byte &
HexLine::left(unsigned int i) {
  return _left[i];
}

const QVector<HexLine::Byte> &
HexLine::right() const {
  return _right;
}

const HexLine::Byte &
HexLine::right(unsigned int i) const {
  return _right[i];
}

HexLine::Byte &
HexLine::right(unsigned int i) {
  return _right[i];
}

bool
HexLine::isDiff() const {
  return ! _right.isEmpty();
}

bool
HexLine::hasDiff() const {
  return _hasDiff;
}


/* ********************************************************************************************* *
 * Implementation of HexElement
 * ********************************************************************************************* */
HexElement::HexElement(const Element *element)
  : _lines(), _address(element->address().byte()), _isDiff(false), _hasDiff(false)
{
  uint32_t address = _address;
  QByteArray data = element->data();

  for (int offset=0; offset<data.size();) {
    HexLine line(address, data.right(data.size()-offset));
    address += line.consumed();
    offset  += line.consumed();
    _lines.append(line);
  }
}

HexElement::HexElement(const Element *left, const Element *right)
  : _lines(), _address(0), _isDiff(true), _hasDiff(false)
{
  uint32_t address;
  QByteArray left_data, right_data;

  if (left) {
    address = _address = left->address().byte();
    left_data = left->data();
  }
  if (right) {
    address = _address = right->address().byte();
    right_data = right->data();
  }

  for (int offset=0; offset<std::max(left_data.size(), right_data.size());) {
    QByteArray left_line, right_line;
    if (offset < left_data.size())
      left_line = left_data.mid(offset, 16);
    if (offset < right_data.size())
      right_line = right_data.mid(offset, 16);
    HexLine line(address, left_line, right_line);
    address += line.consumed();
    offset  += line.consumed();
    _lines.append(line);
    _hasDiff |= line.hasDiff();
  }
}

HexElement::HexElement(const HexElement &other)
  : _lines(other._lines), _address(other._address), _isDiff(other._isDiff), _hasDiff(other._hasDiff)
{
  // pass...
}

HexElement &
HexElement::operator =(const HexElement &other) {
  _lines = other._lines;
  _address = other._address;
  _isDiff = other._isDiff;
  _hasDiff = other._hasDiff;
  return *this;
}

uint32_t
HexElement::address() const {
  return _address;
}

unsigned int
HexElement::size() const {
  return _lines.size();
}

const HexLine &
HexElement::line(unsigned int n) const {
  return _lines[n];
}

bool
HexElement::isDiff() const {
  return _isDiff;
}

bool
HexElement::hasDiff() const {
  return _hasDiff;
}

/* ********************************************************************************************* *
 * Implementation of HexImage
 * ********************************************************************************************* */
HexImage::HexImage(const Image *image)
  : _elements(), _isDiff(false), _hasDiff(false)
{
  for (unsigned int i=0; i<image->count(); i++) {
    _elements.append(HexElement(image->element(i)));
  }
}

HexImage::HexImage(const Image *left, const Image *right)
  : _elements(), _isDiff(false), _hasDiff(false)
{
  for (unsigned int i=0,j=0; (i<left->count()) || (j<right->count());) {
    if ((i<left->count()) && (j<right->count())) {
      if (left->element(i)->address() < right->element(j)->address()) {
        _elements.append(HexElement(left->element(i), nullptr));
        _hasDiff |= _elements.last().hasDiff();
        i++;
      } else if (left->element(i)->address() > right->element(j)->address()) {
        _elements.append(HexElement(nullptr, right->element(j)));
        _hasDiff |= _elements.last().hasDiff();
        j++;
      } else {
        _elements.append(HexElement(left->element(i), right->element(j)));
        _hasDiff |= _elements.last().hasDiff();
        i++; j++;
      }
    } else if (i<left->count()) {
      _elements.append(HexElement(left->element(i), nullptr));
      _hasDiff |= _elements.last().hasDiff();
      i++;
    } else if (j<right->count()) {
      _elements.append(HexElement(nullptr, right->element(j)));
      _hasDiff |= _elements.last().hasDiff();
      j++;
    }
  }
}

unsigned int
HexImage::size() const {
  return _elements.size();
}

const HexElement &
HexImage::element(unsigned int i) const {
  return _elements[i];
}

bool
HexImage::isDiff() const {
  return _isDiff;
}

bool
HexImage::hasDiff() const {
  return _hasDiff;
}

/* ********************************************************************************************* *
 * Implementation of hexdump(HexImage)
 * ********************************************************************************************* */
void hexdump(const HexImage &hex, QTextStream &stream) {
  stream << "\033[0m";
  for (unsigned int i=0,c=0; i<hex.size(); i++) {
    const HexElement element=hex.element(i);
    // If it is just a dump or the element contains a difference
    if ((!element.isDiff()) || element.hasDiff()) {
      // Just prepend an empty line before every element within the image,
      // just not before the first
      if (c++) stream << Qt::endl;

      for (unsigned j=0; j<element.size(); j++) {
        const HexLine line = element.line(j);
        if (! line.isDiff()) {
          // If just a dump -> dump every line
          stream << Qt::hex << qSetFieldWidth(8) << qSetPadChar(QChar('0')) << line.address()
                 << qSetFieldWidth(0) << " :  ";
          for (int k=0; k<16; k++) {
            if (HexLine::Byte::Unused == line.left(k).type)
              stream << "   ";
            else
              stream << qSetFieldWidth(2) << line.left(k).value
                     << qSetFieldWidth(0) << " ";

          }
          stream << " | ";
          for (int k=0; k<16; k++) {
            if ((line.left(k).value >=0x20) && (line.left(k).value<0x7f))
              stream << (char)line.left(k).value;
            else
              stream << " ";
          }
          stream << Qt::endl;
        } else if (line.hasDiff()) {
          // If line contains a difference
          stream << Qt::hex << qSetFieldWidth(8) << qSetPadChar(QChar('0')) << line.address()
                 << qSetFieldWidth(0) << ": ";
          for (int k=0; k<16; k++) {
            if (HexLine::Byte::Unused == line.left(k).type)
              stream << "  ";
            else if (HexLine::Byte::Keep == line.left(k).type)
              stream << "\033[2m"
                     << qSetFieldWidth(2) << line.left(k).value
                     << qSetFieldWidth(0) << "\033[0m ";
            else
              stream << "\033[1m"
                     << qSetFieldWidth(2) << line.left(k).value
                     << qSetFieldWidth(0) << "\033[0m ";
          }
          stream << "> ";
          for (int k=0; k<16; k++) {
            if (HexLine::Byte::Unused == line.right(k).type)
              stream << "   ";
            else if (HexLine::Byte::Keep == line.right(k).type)
              stream << "\033[2m"
                     << qSetFieldWidth(2) << line.right(k).value
                     << qSetFieldWidth(0) << "\033[0m ";
            else
              stream << "\033[1m"
                     << qSetFieldWidth(2) << line.right(k).value
                     << qSetFieldWidth(0) << "\033[0m ";
          }
          stream << "| ";
          for (int k=0; k<16; k++) {
            char c = ((line.left(k).value>=0x20) && (line.left(k).value<0x7f)) ?
                  line.left(k).value : ' ';
            if (HexLine::Byte::Keep == line.left(k).type)
              stream << "\033[2m" << c << "\033[0m";
            else
              stream << "\033[1m" << c << "\033[0m";
          }
          stream << " > ";
          for (int k=0; k<16; k++) {
            char c = ((line.right(k).value>=0x20) && (line.right(k).value<0x7f)) ?
                  line.right(k).value : ' ';
            if (HexLine::Byte::Keep == line.right(k).type)
              stream << "\033[2m" << c << "\033[0m";
            else
              stream << "\033[1m" << c << "\033[0m";
          }
          stream << Qt::endl;
        }
      }
    }
  }
}
