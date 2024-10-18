#include "device.hh"
#include "request.hh"
#include "response.hh"
#include "model.hh"

#include <QIODevice>
#include "logger.hh"


AnyToneDevice::AnyToneDevice(QIODevice *interface, AnyToneModel* model, QObject *parent)
  : QObject{parent}, _state(State::Initial), _interface(interface), _model(model),
    _in_buffer(), _out_buffer()
{
  _interface->setParent(this);
  connect(_interface, SIGNAL(readyRead()), this, SLOT(onBytesAvailable()));
  connect(_interface, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));

  if (! _interface->open(QIODevice::ReadWrite)) {
    logError() << "Cannot open interface: " << _interface->errorString();
    disconnect(_interface, &QIODevice::readyRead, this, &AnyToneDevice::onBytesAvailable);
    disconnect(_interface, &QIODevice::bytesWritten, this, &AnyToneDevice::onBytesWritten);
  }

  if (nullptr != _model) {
    _model->setParent(this);
    connect(this, &AnyToneDevice::startProgram, _model, &Model::startProgram);
    connect(this, &AnyToneDevice::endProgram, _model, &Model::endProgram);
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
    return new DeviceInfoResponse(this->_model->model(), this->_model->revision());
  } else if ((State::Program == _state) && request->is<ReadRequest>()) {
    ReadRequest *rreq = request->as<ReadRequest>();
    logDebug() << "Read " << (int)rreq->length() << "b from " << Qt::hex << rreq->address() << "h.";
    QByteArray payload; payload.reserve(rreq->length());
    if (! this->read(rreq->address(), rreq->length(), payload))
      return nullptr;
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


bool
AnyToneDevice::read(uint32_t address, uint8_t len, QByteArray &buffer) {
  int n = std::min(16u, std::min((uint)len, (uint)buffer.capacity()));

  if (0x02fa0000 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x01\x01\x01\x00\x00\x01\x01\x20\x20\x20\x20\xff", n));
  else if (0x02fa0010 == address)
    buffer.append(QByteArray::fromRawData("\x44\x38\x37\x38\x55\x56\x00\x01\x00\xff\xff\xff\xff\xff\xff\xff", n));
  else if (0x02fa0020 == address)
    buffer.append(QByteArray::fromRawData("\xff\xff\xff\xff\x00\x00\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0030 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0040 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0050 == address)
    buffer.append(QByteArray::fromRawData("\x31\x32\x33\x34\x35\x36\x37\x38\xff\xff\xff\xff\xff\xff\xff\xff", n));
  else if (0x02fa0060 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0070 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0080 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa0090 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00a0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00b0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00c0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00d0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00e0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (0x02fa00f0 == address)
    buffer.append(QByteArray::fromRawData("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", n));
  else if (nullptr != _model)
    return _model->read(address, len, buffer);
  else
    return false;

  return true;
}

ImageCollector *
AnyToneDevice::model() const {
  return _model;
}

bool
AnyToneDevice::write(uint32_t addr, const QByteArray &data) {
  if (nullptr != _model)
    return _model->write(addr, data);
  return false;
}

CodeplugPattern *
AnyToneDevice::pattern() const {
  return _model->pattern();
}
