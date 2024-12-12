#include "offset.hh"
#include <QRegularExpression>


/* ********************************************************************************************* *
 * Implementation of Address
 * ********************************************************************************************* */
Address::Address(uint64_t bits)
  : _value(bits)
{
  // pass...
}

Address::Address()
  : _value(std::numeric_limits<uint64_t>().max())
{
  // pass...
}

Address::Address(const Offset &offset)
  : _value(offset.bits())
{
  // pass...
}

bool
Address::isValid() const {
  return std::numeric_limits<uint64_t>().max() != _value;
}

bool
Address::byteAligned() const {
  return 0 == (_value%8);
}

Address
Address::zero() {
  return Address(0);
}

Address
Address::fromByte(unsigned int byte, unsigned int bit)
{
  return Address(8*(byte + bit/8) + (7-bit%8));
}

Address
Address::fromString(const QString &str) {
  QRegularExpression regex("([0-9A-Fa-f]*)h?:([0-7]?)|([0-9A-Fa-f]+)h?");

  QRegularExpressionMatch match = regex.match(str);
  if (! match.isValid())
    return Address();

  unsigned int byte = 0, bit = 7;
  if (match.capturedLength(1))
    byte = match.captured(1).toUInt(nullptr, 16);
  else if (match.capturedLength(3))
    byte = match.captured(3).toUInt(nullptr, 16);
  if (match.capturedLength(2))
    bit = match.captured(2).toUInt(nullptr, 8);

  return Address::fromByte(byte, bit);
}

unsigned int
Address::byte() const {
  return _value / 8;
}

unsigned int
Address::bit() const {
  return 7 - _value%8;
}

QString
Address::toString() const {
  if (7 == bit())
    return QString("%1h").arg(byte(), 0, 16);
  return QString("%1h:%2").arg(byte(), 0, 16).arg(bit(),0,8);
}

Address &
Address::operator+=(const Offset &other) {
  if ((0 > other.bits()) && (std::abs(other.bits())>_value))
    return (*this = Address());
  _value += other.bits();
  return *this;
}

Address
Address::operator+(const Offset &other) const {
  if ((0 > other.bits()) && (std::abs(other.bits())>_value))
    return Address();
  return Address(_value + other.bits());
}

Address &
Address::operator-=(const Offset &other) {
  if ((0 < other.bits()) && (std::abs(other.bits())>_value))
    return (*this = Address());
  _value -= other.bits();
  return *this;
}

Address
Address::operator-(const Offset &other) const {
  if ((0 < other.bits()) && (std::abs(other.bits())>_value))
    return Address();
  return Address(_value - other.bits());
}

Offset
Address::operator-(const Address &other) const {
  return Offset::fromBits((int64_t)_value - (int64_t)other._value);
}


/* ********************************************************************************************* *
 * Implementation of Offset
 * ********************************************************************************************* */
Offset::Offset(int64_t bits)
  : _value(bits)
{
  // pass...
}

Offset::Offset()
  : _value(std::numeric_limits<int64_t>::max())
{
  // pass...
}

Offset::Offset(const Size &size)
  : _value(size.bits())
{
  // pass...
}

Offset::Offset(const Address &addr)
  : _value(addr._value)
{
  // pass...
}

bool
Offset::isValid() const {
  return std::numeric_limits<int64_t>::max() != _value;
}

Offset
Offset::zero() {
  return { 0 };
}

Offset
Offset::fromByte(int n, int bit) {
  return Offset(8*((int64_t)n) + bit);
}

Offset
Offset::fromBits(unsigned long n) {
  return Offset(n);
}

Offset
Offset::fromString(const QString &str) {
  QRegularExpression regex("([0-9A-Fa-f]*)h?:([0-7]*)|([0-9A-Fa-f]+)h?");

  QRegularExpressionMatch match = regex.match(str);
  if (! match.isValid())
    return Offset();

  unsigned int byte = 0, bit = 0;
  if (match.capturedLength(1))
    byte = match.captured(1).toUInt(nullptr, 16);
  else if (match.capturedLength(3))
    byte = match.captured(3).toUInt(nullptr, 16);
  if (match.capturedLength(2))
    bit = match.captured(2).toUInt(nullptr, 8);

  return Offset::fromByte(byte, bit);
}

QString
Offset::toString() const {
  if (0 == bit())
    return QString("%1h").arg(byte(), 0, 16);
  return QString("%1h:%2").arg(byte(), 0, 16).arg(bit(),0,8);
}


/* ********************************************************************************************* *
 * Implementation of Size
 * ********************************************************************************************* */
Size::Size(unsigned long bits)
  : _value(bits)
{
  // pass...
}

Size::Size()
  : _value(std::numeric_limits<uint64_t>::max())
{
  // pass...
}

Size::Size(const Offset &offset)
  : _value((offset.bits()>0) ? offset.bits() : std::numeric_limits<uint64_t>::max())
{
  // pass...
}

bool
Size::isValid() const {
  return std::numeric_limits<uint64_t>::max() != _value;
}

Size
Size::zero() {
  return { 0 };
}

Size
Size::fromByte(unsigned int n, unsigned int bit) {
  return Size(8*((unsigned long)n) + bit);
}

Size
Size::fromBits(unsigned long n) {
  return Size(n);
}

Size
Size::fromString(const QString &str) {
  QRegularExpression regex("([0-9A-Fa-f]*)h?:([0-7]?)|([0-9A-Fa-f]+)h?");

  QRegularExpressionMatch match = regex.match(str);
  if (! match.isValid())
    return Size();

  unsigned int byte = 0, bit = 0;
  if (match.capturedLength(1))
    byte = match.captured(1).toUInt(nullptr, 16);
  else if (match.capturedLength(3))
    byte = match.captured(3).toUInt(nullptr, 16);
  if (match.capturedLength(2))
    bit = match.captured(2).toUInt(nullptr, 8);

  return Size::fromByte(byte, bit);
}

QString
Size::toString() const {
  if (0 == bit())
    return QString("%1h").arg(byte(), 0, 16);
  return QString("%1h:%2").arg(byte(), 0, 16).arg(bit(),0,8);
}

Size &
Size::operator -=(const Size &other) {
  if (other._value > _value)
    _value = std::numeric_limits<uint64_t>::max();
  else
    _value -= other._value;
  return *this;
}

