#include "d878uv2.hh"

D878UV2::D878UV2(QIODevice *interface, Model* model, QObject *parent)
  : Device{interface, model, parent}
{
  // pass...
}

QByteArray
D878UV2::model() const {
  return QByteArray::fromRawData("878UV2", 6);
}

QByteArray
D878UV2::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

