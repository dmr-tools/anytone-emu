#include "d868uv.hh"

D868UV::D868UV(QIODevice *interface, QObject *parent)
  : Device{interface, parent}
{
  // pass...
}

QByteArray
D868UV::model() const {
  return QByteArray::fromRawData("868UV\x00", 6);
}

QByteArray
D868UV::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

