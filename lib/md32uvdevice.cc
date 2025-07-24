#include "md32uvdevice.hh"

#include <QIODevice>

#include "logger.hh"
#include "md32uvprotocol.hh"

using namespace std::chrono_literals;

/* ********************************************************************************************* *
 * Implementation of MD32UV Device
 * ********************************************************************************************* */
MD32UVDevice::MD32UVDevice(
    QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : GenericDevice{interface, pattern, handler, parent}, _timer(2s)
{
  _timer.setTimerType(Qt::CoarseTimer);
  _timer.setSingleShot(true);
  connect(&_timer, &QChronoTimer::timeout, [this]() {
    emit endProgram();
  });
}


GenericRequest *
MD32UVDevice::parse(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  return MD32UVRequest::fromBuffer(buffer, ok, err);
}


GenericResponse *
MD32UVDevice::handle(GenericRequest *request) {
  if (nullptr == request) {
    logWarn() << "Got a null-request.";
    return new MD32UVACK();
  }

  if (request->is<MD32UVPingRequest>())
    return new MD32UVACK();
  else if (request->is<MD32UVSearchRequest>())
    return new MD32UVSearchResponse("DP570UV");
  else if (request->is<MD32UVPasswordRequest>())
    return new MD32UVPasswordResponse();
  else if (request->is<MD32UVStartSystemInfoRequest>())
    return new MD32UVACK();
  else if (request->is<MD32UVStartProgramRequest>()) {
    _timer.stop(); _timer.start();
    emit startProgram();
    return new MD32UVACK();
  } else if (request->is<MD32UVUnknown02Request>())
    return new MD32UVUnknown02Response();
  else if (request->is<MD32UVValueRequest>()) {
    QByteArray payload;
    getValue(request->as<MD32UVValueRequest>()->field(),
             request->as<MD32UVValueRequest>()->length(),
             payload);
    return new MD32UVValueResponse(request->as<MD32UVValueRequest>()->field(), payload);
  } else if (request->is<MD32UVReadInfoRequest>()) {
    uint32_t address = request->as<MD32UVReadInfoRequest>()->address();
    uint16_t length = request->as<MD32UVReadInfoRequest>()->length();
    QByteArray payload;
    if (! readInfo(address, length, payload))
      payload = QByteArray(length, '\xff');
    return new MD32UVReadInfoResponse(request->as<MD32UVReadInfoRequest>()->address(), payload);
  } else if (request->is<MD32UVReadRequest>()) {
    _timer.setInterval(2s);
    auto address  = request->as<MD32UVReadRequest>()->address();
    auto length = request->as<MD32UVReadRequest>()->length();
    QByteArray payload;
    if (! read(address, length, payload))
      payload = QByteArray(length, '\xff');
    return new MD32UVReadResponse(address, payload);
  } else if (request->is<MD32UVWriteRequest>()) {
    _timer.setInterval(2s);
    auto address = request->as<MD32UVWriteRequest>()->address();
    auto payload = request->as<MD32UVWriteRequest>()->payload();
    write(address, payload);
    return new MD32UVACK();
  }

  logWarn() << "Got unkown request type.";
  return new MD32UVACK();
}


bool
MD32UVDevice::getValue(uint8_t field, uint8_t length, QByteArray &payload) {
  switch (field) {
  case 0x01:
    payload = QByteArray::fromHex("33 32 2e 30 31 2e 30 31 2e 30 34");
    return true;
  case 0x02:
    payload = QByteArray::fromHex("00 00 00 00 00 00 1c 4c 00 00 1c 4c");
    return true;
  case 0x03:
    payload = QByteArray::fromHex("32 30 32 32 2d 30 36 2d 32 37");
    return true;
  case 0x04:
    payload = QByteArray::fromHex("44 31 2e 30 31 2e 30 31 2e 30 30 34");
    return true;
  case 0x05:
    payload = QByteArray::fromHex("52 31 2e 30 30 2e 30 31 2e 30 30 31");
    return true;
  case 0x06:
    payload = QByteArray::fromHex("00 10 20 00 ff 4f 26 00");
    return true;
  case 0x07:
    payload = QByteArray::fromHex("00 90 0c 00 ff 9f 14 00");
    return true;
  case 0x08:
    payload = QByteArray::fromHex("00 00 18 00 ff 0f 20 00");
    return true;
  case 0x09:
    payload = QByteArray::fromHex("00 c0 6d 00 ff ff ff 00");
    return true;
  case 0x0a:
    payload = QByteArray::fromHex("00 10 00 00 ff 8f 0c 00");
    return true;
  case 0x0b:
    payload = QByteArray::fromHex("43 31 2e 30 30 2e 30 31 2e 30 30 31");
    return true;
  case 0x0d:
    payload = QByteArray::fromHex("03 4e 2d 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "3f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
    payload = payload.first(length);
    return true;
  case 0x0e:
    payload = QByteArray::fromHex("00 00 15 00 ff 5f 17 00");
    return true;
  case 0x0f:
    payload = QByteArray::fromHex("00 80 27 00 ff bf 6d 00");
    return true;
  case 0x10:
    payload = QByteArray::fromHex("50 c3 00");
    return true;
  }
  return false;
}

bool
MD32UVDevice::readInfo(uint32_t address, uint16_t length, QByteArray &payload) {
  if (0 == address)
    payload = QByteArray::fromHex("ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "36 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "00 ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "df ff df ff ff ff be f7 79 ce d6 b5 79 ce be f7 "
                                  "ff ff ef de 1c e7 3d e7 9a d6 f4 9c 55 a5 fb d6 "
                                  "5e df 5d e7 5d e7 5d e7 7d ef 92 94 c7 39 49 4a "
                                  "69 a5 69 4a aa 52 aa 52 8a 52 28 42 65 29 08 43 "
                                  "60 13 40 17 60 13 40 17 00 40 00 52 00 40 00 52 "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff "
                                  "ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff");
  payload = payload.first(length);
  return true;
}


