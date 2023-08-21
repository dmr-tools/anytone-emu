#include "djmd5.hh"

DJMD5::DJMD5(QIODevice *interface, Model* model, QObject *parent)
  : Device{interface, model, parent}
{
  // pass...
}

QByteArray
DJMD5::model() const {
  return QByteArray::fromRawData("JMD5\x00\x00", 6);
}

QByteArray
DJMD5::hwVersion() const {
  return QByteArray::fromRawData("V100\x00\x00", 6);
}

