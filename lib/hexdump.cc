#include "hexdump.hh"
#include "image.hh"
#include "imagedifference.hh"
#include <QTextStream>


QString
hexFormat(uint32_t addr, const QByteArray &buffer) {
  QString res; res.reserve(75);

  res += QString("%1 | ").arg(addr, 8, 16, QChar('0'));

  for (int i=0; i<16; i++) {
    if (i<buffer.size())
      res += QString("%1 ").arg((uint8_t)buffer.at(i), 2, 16, QChar('0'));
    else
      res += "   ";
    if (8 == i)
      res += " ";
  }
  res += "| ";

  for (int i=0; i<16; i++) {
    if ((i<buffer.size()) && (buffer.at(i)>=0x20) && (buffer.at(i)<0x7f))
      res += buffer.at(i);
    else
      res += ' ';
    if (8 == i)
      res += " ";
  }

  return res;
}


void
hexdump(const Element *element, QTextStream &stream) {
  uint32_t start = (element->address()/0x10)*0x10;
  uint32_t offset = 0;
  uint32_t n = element->size();

  if (start < element->address()) {
    QByteArray buffer(element->address()-start, 0);
    offset = start + 0x10 - element->address();
    buffer.append(element->data().left(offset));
    n -= offset;
    stream << hexFormat(start, buffer) << Qt::endl;
    start += 0x10;
  }

  for (; n>=0x10; n-=0x10, start+=0x10, offset+=0x10) {
    stream << hexFormat(start, element->data().mid(offset, 0x10)) << Qt::endl;
  }
}


void
hexdump(const Image *image, QTextStream &stream) {
  for (unsigned int i=0; i<image->count(); i++) {
    hexdump(image->element(i), stream); stream << Qt::endl;
  }
}

void
hexdump(const ImageDifference *diff, QTextStream &stream) {
  for (unsigned int i=0; i<diff->count(); i++) {
    hexdump(diff->diff(i), stream); stream << Qt::endl;
  }
}

void
hexdump(const ElementDifference *diff, QTextStream &stream) {
  if (diff->is<AddElement>()) {

  } else if (diff->is<DelElement>()) {

  } else {

  }
}
