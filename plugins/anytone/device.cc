#include "device.hh"
#include <QIODevice>
#include "logger.hh"
#include "protocol.hh"


/* ********************************************************************************************* *
 * Implementation of AnyToneDevice
 * ********************************************************************************************* */
AnyToneDevice::AnyToneDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                             const QByteArray &model, uint8_t band, const QByteArray &revision,
                             QObject *parent)
  : Device{pattern, handler, parent}, _state(State::Initial), _interface(interface),
    _in_buffer(), _out_buffer(), _model(model), _band(band), _revision(revision)
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

  while (AnytoneRequest *req = AnytoneRequest::fromBuffer(_in_buffer)) {
    AnytoneResponse *resp = this->handle(req);
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


AnytoneResponse *
AnyToneDevice::handle(AnytoneRequest *request) {
  if (request->is<AnytoneProgramRequest>()) {
    if (State::Initial == _state)
      emit startProgram();
    logDebug() << "Enter progam mode.";
    _state = State::Program;
    return new AnytoneProgramResponse();
  } else if ((State::Program == _state) && request->is<AnytoneDeviceInfoRequest>()) {
    logDebug() << "Get device info.";
    return new AnytoneDeviceInfoResponse(this->model(), this->band(), this->revision());
  } else if ((State::Program == _state) && request->is<AnytoneReadRequest>()) {
    AnytoneReadRequest *rreq = request->as<AnytoneReadRequest>();
    logDebug() << "Read " << (int)rreq->length() << "b from " << Qt::hex << rreq->address() << "h.";
    QByteArray payload; payload.reserve(rreq->length());
    if (! this->read(rreq->address(), rreq->length(), payload)) {
      logError() << "Cannot read from emulated device.";
      return nullptr;
    }
    return new AnytoneReadResponse(rreq->address(), payload);
  } else if ((State::Program == _state) && request->is<AnytoneWriteRequest>()) {
    AnytoneWriteRequest *wreq = request->as<AnytoneWriteRequest>();
    if (! this->write(wreq->address(), wreq->payload()))
      return nullptr;
    return new AnytoneWriteResponse(wreq->address(), wreq->payload().size());
  } else if (request->is<AnytoneEndRequest>()) {
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

uint8_t
AnyToneDevice::band() const {
  return _band;
}

const QByteArray &
AnyToneDevice::revision() const {
  return _revision;
}
