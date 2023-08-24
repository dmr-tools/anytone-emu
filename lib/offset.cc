#include "offset.hh"
#include <QRegularExpression>


/* ********************************************************************************************* *
 * Implementation of Offset
 * ********************************************************************************************* */
Offset::Offset(unsigned long bits)
  : _value(bits)
{
  // pass...
}

Offset::Offset()
  : _value(std::numeric_limits<unsigned long>::max())
{
  // pass...
}

bool
Offset::isValid() const {
  return std::numeric_limits<unsigned long>::max() != _value;
}

Offset
Offset::zero() {
  return { 0 };
}

Offset
Offset::fromByte(unsigned int n, unsigned int bit) {
  return Offset(8*((unsigned long)n) + bit);
}

Offset
Offset::fromBits(unsigned long n) {
  return Offset(n);
}

Offset
Offset::fromString(const QString &str) {
  QRegularExpression regex("([0-9A-Fa-f]*):([0-7]+)|([0-9A-Fa-f]+)");

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


