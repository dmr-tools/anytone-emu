#include "d578uv.hh"

D578UV::D578UV(QIODevice *interface, Model* model, QObject *parent)
  : AnyToneDevice{interface, model, parent}
{
  // pass...
}

QByteArray
D578UV::model() const {
  return QByteArray::fromRawData("578UV\x00", 6);
}

QByteArray
D578UV::hwVersion() const {
  return QByteArray::fromRawData("V110\x00\x00", 6);
}

