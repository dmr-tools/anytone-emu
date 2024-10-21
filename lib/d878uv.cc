#include "d878uv.hh"

D878UV::D878UV(QIODevice *interface, Model* model, QObject *parent)
  : AnyToneDevice{interface, model, parent}
{
  // pass...
}

QByteArray
D878UV::model() const {
  return QByteArray::fromRawData("878UV\x00", 6);
}

QByteArray
D878UV::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

