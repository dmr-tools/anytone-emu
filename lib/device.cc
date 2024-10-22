#include "device.hh"
#include "request.hh"
#include "response.hh"
#include "pattern.hh"
#include "pattern.hh"
#include "model.hh"

#include <QIODevice>
#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of Device
 * ********************************************************************************************* */
Device::Device(CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : QObject{parent}, _pattern(pattern), _handler(handler), _rom()
{
  if (_pattern)
    _pattern->setParent(this);
  if (_handler) {
    _handler->setParent(this);
    connect(this, &AnyToneDevice::startProgram, _handler, &ImageCollector::startProgram);
    connect(this, &AnyToneDevice::endProgram, _handler, &ImageCollector::endProgram);
  }
}


bool
Device::read(uint32_t address, uint8_t len, QByteArray &buffer) {
  if (! rom().read(address, len, buffer)) {
    logError() << "Cannot read " << len
               << "b from ROM at address " << QString::number(address, 16) << "h.";
    return false;
  }

  return true;
}

bool
Device::write(uint32_t addr, const QByteArray &data) {
  if (nullptr != _handler)
    return _handler->write(addr, data);
  return false;
}


ImageCollector *
Device::handler() const {
  return _handler;
}

void
Device::setHandler(ImageCollector *handler) {
  if (nullptr != _handler)
    delete _handler;

  _handler = handler;

  if (_handler) {
    _handler->setParent(this);
    connect(this, &AnyToneDevice::startProgram, _handler, &ImageCollector::startProgram);
    connect(this, &AnyToneDevice::endProgram, _handler, &ImageCollector::endProgram);
  }
}

CodeplugPattern *
Device::pattern() const {
  return _pattern;
}


const ModelRom &
Device::rom() const {
  return _rom;
}

ModelRom &
Device::rom() {
  return _rom;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneDevice
 * ********************************************************************************************* */
AnyToneDevice::AnyToneDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                             const QByteArray &model, const QByteArray &revision, QObject *parent)
  : Device{pattern, handler, parent}, _state(State::Initial), _interface(interface),
    _in_buffer(), _out_buffer(), _model(model), _revision(revision)
{
  _interface->setParent(this);
  connect(_interface, SIGNAL(readyRead()), this, SLOT(onBytesAvailable()));
  connect(_interface, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));

  if (! _interface->open(QIODevice::ReadWrite)) {
    logError() << "Cannot open interface: " << _interface->errorString();
    disconnect(_interface, &QIODevice::readyRead, this, &AnyToneDevice::onBytesAvailable);
    disconnect(_interface, &QIODevice::bytesWritten, this, &AnyToneDevice::onBytesWritten);
  }
}


void
AnyToneDevice::onBytesAvailable() {
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
AnyToneDevice::onBytesWritten() {
  if (0 == _out_buffer.size())
    return;

  qint64 nbytes = _interface->write(_out_buffer);
  _out_buffer.remove(0, nbytes);
}


Response *
AnyToneDevice::handle(Request *request) {
  if (request->is<ProgramRequest>()) {
    if (State::Initial == _state)
      emit startProgram();
    logDebug() << "Enter progam mode.";
    _state = State::Program;
    return new ProgramResponse();
  } else if ((State::Program == _state) && request->is<DeviceInfoRequest>()) {
    logDebug() << "Get device info.";
    return new DeviceInfoResponse(this->model(), this->revision());
  } else if ((State::Program == _state) && request->is<ReadRequest>()) {
    ReadRequest *rreq = request->as<ReadRequest>();
    logDebug() << "Read " << (int)rreq->length() << "b from " << Qt::hex << rreq->address() << "h.";
    QByteArray payload; payload.reserve(rreq->length());
    if (! this->read(rreq->address(), rreq->length(), payload)) {
      logError() << "Cannot read from emulated device.";
      return nullptr;
    }
    return new ReadResponse(rreq->address(), payload);
  } else if ((State::Program == _state) && request->is<WriteRequest>()) {
    WriteRequest *wreq = request->as<WriteRequest>();
    if (! this->write(wreq->address(), wreq->payload()))
      return nullptr;
    return new WriteResponse(wreq->address(), wreq->payload().size());
  } else if (request->is<EndRequest>()) {
    if (State::Program == _state)
      emit endProgram();
    logDebug() << "Done.";
    _state = State::Initial;
    return nullptr;
  }

  logWarn() << "Uknown request.";
  return nullptr;
}


const QByteArray &
AnyToneDevice::model() const {
  return _model;
}

const QByteArray &
AnyToneDevice::revision() const {
  return _revision;
}
