#include "device.hh"


#include <QIODevice>

#include "logger.hh"
#include "protocol.hh"


/* ********************************************************************************************* *
 * Implementation of RadtelDevice
 * ********************************************************************************************* */
RadtelDevice::RadtelDevice(
    QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : Device{pattern, handler, parent}, _interface(interface), _in_buffer(), _out_buffer()
{
  _interface->setParent(this);
  connect(_interface, &QIODevice::readyRead, this, &RadtelDevice::onBytesAvailable);
  connect(_interface, &QIODevice::bytesWritten, this, &RadtelDevice::onBytesWritten);

  if (! _interface->open(QIODevice::ReadWrite)) {
    logError() << "Cannot open interface: " << _interface->errorString();
    disconnect(_interface, &QIODevice::readyRead, this, &RadtelDevice::onBytesAvailable);
    disconnect(_interface, &QIODevice::bytesWritten, this, &RadtelDevice::onBytesWritten);
  }
}


void
RadtelDevice::onBytesAvailable() {
  _in_buffer.append(_interface->readAll());

  bool ok = true;
  ErrorStack err;
  while (auto req = RadtelRequest::fromBuffer(_in_buffer, ok, err)) {
    auto resp = this->handle(req);
    delete req;
    if (resp) {
      if (resp->serialize(_out_buffer)) {
        onBytesWritten();
      }
      delete resp;
    }
  }

  if (! ok) {
    logError() << "Cannot receive/handle request: " << err.format();
    return;
  }
}


void
RadtelDevice::onBytesWritten() {
  if (0 == _out_buffer.size())
    return;

  qint64 nbytes = _interface->write(_out_buffer);
  _out_buffer.remove(0, nbytes);
  logDebug() << "Send " << Qt::hex << nbytes
             << "h bytes," << Qt::hex << _out_buffer.size() << "h bytes left.";
}


RadtelResponse *
RadtelDevice::handle(RadtelRequest *request) {
  if (nullptr == request) {
    logWarn() << "Got a null-request.";
    return new RadtelACK();
  }

  if (request->is<RadtelCommandRequest>()) {
    if (RadtelCommandRequest::EnterProgrammingMode == request->as<RadtelCommandRequest>()->command())
      emit startProgram();
    else if (RadtelCommandRequest::LeaveProgrammingMode == request->as<RadtelCommandRequest>()->command())
      emit endProgram();
    return new RadtelACK();
  }

  if (request->is<RadtelReadRequest>()) {
    auto rr = request->as<RadtelReadRequest>();
    QByteArray buffer;
    logDebug() << "Read 1024b from " << Qt::hex << rr->address() << ".";
    if (! read(rr->address(), 1024, buffer))
      return new RadtelACK();
    return new RadtelReadResonse(rr->page(), buffer);
  }

  if (request->is<RadtelWriteRequest>()) {
    auto wr = request->as<RadtelWriteRequest>();
    if (! write(wr->address(), wr->payload()))
        return new RadtelACK();
    return new RadtelACK();
  }

  logWarn() << "Got unkown request type.";
  return new RadtelACK();
}
