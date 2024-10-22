#include "d578uv2.hh"

D578UV2::D578UV2(QIODevice *interface, Model* model, QObject *parent)
  : AnyToneDevice{interface, model, parent}
{
  // pass...
}

QByteArray
D578UV2::model() const {
  return QByteArray::fromRawData("578UV2", 6);
}

QByteArray
D578UV2::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

