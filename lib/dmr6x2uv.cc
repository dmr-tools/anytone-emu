#include "dmr6x2uv.hh"

DMR6X2UV::DMR6X2UV(QIODevice *interface, Model *model, QObject *parent)
  : AnyToneDevice{interface, model, parent}
{
  // pass...
}

QByteArray
DMR6X2UV::model() const {
  return QByteArray::fromRawData("6X2UV\x00", 6);
}

QByteArray
DMR6X2UV::hwVersion() const {
  return QByteArray::fromRawData("V102\x00\x00", 6);
}

