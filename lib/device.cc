#include "device.hh"
#include "request.hh"
#include "response.hh"

#include <QIODevice>


Device::Device(QIODevice *interface, QObject *parent)
  : QObject{parent}, _state(State::Initial), _interface(interface), _in_buffer(), _out_buffer()
{
  if (nullptr == _interface)
    return;

  _interface->setParent(this);
  connect(_interface, SIGNAL(readyRead()), this, SLOT(onBytesAvailable()));
  connect(_interface, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));
}

void
Device::onBytesAvailable() {
  _in_buffer.append(_interface->readAll());

  while (Request *req = Request::fromBuffer(_in_buffer)) {
    Response *resp = this->handle(req);
    delete req;
    if (resp) {
      if (resp->serialize(_out_buffer))
        onBytesWritten();
      delete resp;
    }
  }
}

void
Device::onBytesWritten() {
  if (0 == _out_buffer.size())
    return;

  qint64 nbytes = _interface->write(_out_buffer);
  _out_buffer.remove(0, nbytes);
}

