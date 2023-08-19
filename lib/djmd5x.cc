#include "djmd5x.hh"

DJMD5X::DJMD5X(QIODevice *interface, Model* model, QObject *parent)
  : Device{interface, model, parent}
{
  // pass...
}

QByteArray
DJMD5X::model() const {
  return QByteArray::fromRawData("JMD5X\x00", 6);
}

QByteArray
DJMD5X::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

