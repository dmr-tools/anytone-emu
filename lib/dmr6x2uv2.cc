#include "dmr6x2uv2.hh"

DMR6X2UV2::DMR6X2UV2(QIODevice *interface, Model *model, QObject *parent)
  : Device{interface, model, parent}
{
  // pass...
}

QByteArray
DMR6X2UV2::model() const {
  return QByteArray::fromRawData("6X2UV2", 6);
}

QByteArray
DMR6X2UV2::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

