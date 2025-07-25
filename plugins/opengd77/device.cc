#include "device.hh"

#include <QIODevice>

#include "logger.hh"
#include "protocol.hh"



/* ********************************************************************************* *
 * Implementation of OpenGD77Device
 * ********************************************************************************************* */
OpenGD77Device::OpenGD77Device(
    QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : Device{pattern, handler, parent}, _interface(interface), _in_buffer(), _out_buffer()
{
  _interface->setParent(this);
  connect(_interface, &QIODevice::readyRead, this, &OpenGD77Device::onBytesAvailable);
  connect(_interface, &QIODevice::bytesWritten, this, &OpenGD77Device::onBytesWritten);

  if (! _interface->open(QIODevice::ReadWrite)) {
    logError() << "Cannot open interface: " << _interface->errorString();
    disconnect(_interface, &QIODevice::readyRead, this, &OpenGD77Device::onBytesAvailable);
    disconnect(_interface, &QIODevice::bytesWritten, this, &OpenGD77Device::onBytesWritten);
  }
}


void
OpenGD77Device::onBytesAvailable() {
  _in_buffer.append(_interface->readAll());

  bool ok = true;
  ErrorStack err;
  while (auto req = OpenGD77Request::fromBuffer(_in_buffer, ok, err)) {
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
OpenGD77Device::onBytesWritten() {
  if (0 == _out_buffer.size())
    return;

  qint64 nbytes = _interface->write(_out_buffer);
  _out_buffer.remove(0, nbytes);
}


OpenGD77Response *
OpenGD77Device::handle(OpenGD77Request *request) {
  if (nullptr == request) {
    logWarn() << "Got a null-request.";
    return new OpenGD77CommandResponse(true);
  }

  if (request->is<OpenGD77CommandRequest>()) {
    if (request->is<OpenGD77ShowCPSScreenRequest>())
      emit startProgram();
    else if (request->is<OpenGD77ControlRequest>() &&
             ( (OpenGD77ControlRequest::REBOOT == request->as<OpenGD77ControlRequest>()->option()) |
               (OpenGD77ControlRequest::SAVE_SETTINGS_NOT_VFOS == request->as<OpenGD77ControlRequest>()->option()) |
               (OpenGD77ControlRequest::SET_DATETIME == request->as<OpenGD77ControlRequest>()->option())) )
      emit endProgram();

    return new OpenGD77CommandResponse(true);
  }

  if (request->is<OpenGD77ReadRequest>()) {
    auto rr = request->as<OpenGD77ReadRequest>();
    QByteArray buffer;
    if (OpenGD77ReadRequest::READ_EEPROM == rr->section()) {
      if (! read(rr->address(), rr->length(), buffer))
        return new OpenGD77CommandResponse(true);
    } else if (OpenGD77ReadRequest::READ_FLASH == rr->section()) {
      if (! read(0x01000000 + rr->address(), rr->length(), buffer))
        return new OpenGD77CommandResponse(true);
    } else if (OpenGD77ReadRequest::READ_FIRMWARE_INFO == rr->section()) {
      if (! read(0x02000000, 46, buffer))
        return new OpenGD77CommandResponse(true);
    } else {
      return new OpenGD77CommandResponse(true);
    }
    return new OpenGD77ReadResponse(buffer);
  }

  if (request->is<OpenGD77WriteRequest>()) {
    auto wr = request->as<OpenGD77WriteRequest>();
    if (OpenGD77WriteRequest::WRITE_EEPROM == wr->section()) {
      auto wd = wr->as<OpenGD77WriteDataRequest>();
      logDebug() << "Write " << wd->data().size() << "b to " << Qt::hex << wd->address() << "h.";
      if (! write(wd->address(), wd->data()))
        return new OpenGD77CommandResponse(true);
    } else if (OpenGD77WriteRequest::WRITE_SECTOR_BUFFER == wr->section()) {
      auto wd = wr->as<OpenGD77WriteDataRequest>();
      logDebug() << "Write " << wd->data().size() << "b to " << Qt::hex << 0x01000000+wd->address() << "h.";
      if (! write(0x01000000 + wd->address(), wd->data()))
        return new OpenGD77CommandResponse(true);
    }
    return new OpenGD77WriteResponse(wr->type(), wr->section());
  }

  logWarn() << "Got unkown request type.";
  return new OpenGD77CommandResponse(true);
}
