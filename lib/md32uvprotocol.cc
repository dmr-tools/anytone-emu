#include "md32uvprotocol.hh"
#include <QtEndian>

#include "logger.hh"


/* ******************************************************************************************** *
 * Implementation of MD-32UV request
 * ******************************************************************************************** */
MD32UVRequest::MD32UVRequest()
{
  // pass...
}


MD32UVRequest *
MD32UVRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  ok = true;

  if ((buffer.size() >= 1) && (0x06 == buffer.front())) {
    logDebug() << "Ping.";
    buffer = buffer.sliced(1);
    return new MD32UVPingRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("PSEARCH")) {
    logDebug() << "Detect device.";
    buffer = buffer.sliced(7);
    return new MD32UVSearchRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("PASSSTA")) {
    logDebug() << "Check password.";
    buffer = buffer.sliced(7);
    return new MD32UVPasswordRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("SYSINFO")) {
    logDebug() << "Enter system info mode.";
    buffer = buffer.sliced(7);
    return new MD32UVStartSystemInfoRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("PROGRAM")) {
    logDebug() << "Enter programming mode.";
    buffer = buffer.sliced(7);
    return new MD32UVStartProgramRequest();
  } else if ((buffer.size() >= 1) && buffer.startsWith("\x02")) {
    logDebug() << "Some unknown 02h request.";
    buffer = buffer.sliced(1);
    return new MD32UVUnknown02Request();
  } else if ((buffer.size() >= 5) && buffer.startsWith("V")) {
    auto flags = qFromLittleEndian(*(uint16_t *)(buffer.constData()+1));
    auto len   = qFromLittleEndian(*(uint8_t *)(buffer.constData()+3));
    auto field = qFromLittleEndian(*(uint8_t *)(buffer.constData()+4));
    logDebug() << "Request " << len << "b from value " << field << ".";
    buffer = buffer.sliced(5);
    return new MD32UVValueRequest(flags, len, field);
  } else if ((buffer.size() >= 6) && buffer.startsWith('G')) {
    auto addr  = ((uint32_t)((uint8_t)buffer.at(1)) << 0) +
        ((uint32_t)((uint8_t)buffer.at(2)) << 8) +
        ((uint32_t)((uint8_t)buffer.at(3)) << 16);
    auto len   = qFromLittleEndian(*(uint16_t *)(buffer.constData()+4));
    logDebug() << "Read " << len << "b from value at address "
               << Qt::hex << addr << ".";
    buffer = buffer.sliced(6);
    return new MD32UVReadInfoRequest(addr, len);
  } else if ((buffer.size() >= 6) && buffer.startsWith('R')) {
    auto address = ((uint32_t)((uint8_t)buffer.at(1)) << 0) +
        ((uint32_t)((uint8_t)buffer.at(2)) << 8) +
        ((uint32_t)((uint8_t)buffer.at(3)) << 16);
    auto len = qFromLittleEndian(*(uint16_t *)(buffer.constData()+4));
    logDebug() << "Read " << len << "b from memory at address "
               << Qt::hex << address << ".";
    buffer = buffer.sliced(6);
    return new MD32UVReadRequest(address, len);
  } else if ((buffer.size() >= 6) && buffer.startsWith('W')) {
    uint32_t address = uint32_t(uint8_t(buffer.at(1)))
        + (uint32_t(uint8_t(buffer.at(2))) << 8)
        + (uint32_t(uint8_t(buffer.at(3))) << 16);
    auto len = qFromLittleEndian(*(uint16_t *)(buffer.constData()+4));
    if (buffer.size() < (len+6)) {
      logDebug() << "Incomplete write request...";
      return nullptr;
    }
    logDebug() << "Complete write " << len << "b to " << Qt::hex << address << ".";
    auto payload = buffer.mid(6, len);
    buffer = buffer.sliced(6+len);
    return new MD32UVWriteRequest(address, payload);
  } else if ((buffer.size() >= 5) && buffer.startsWith(QByteArray("\xff\xff\xff\xff\x0c", 5))) {
    logDebug() << "Ignore unknown data " << buffer.first(5).toHex(' ') << ".";
    buffer = buffer.sliced(5);
  }

  if (0 != buffer.size()) {
    logDebug() << "Some left-over bytes: " << buffer.toHex(' ') << ".";
  }

  return nullptr;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV ping request
 * ******************************************************************************************** */
MD32UVPingRequest::MD32UVPingRequest()
  : MD32UVRequest()
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV ACK response
 * ******************************************************************************************** */
MD32UVACK::MD32UVACK()
  : GenericResponse()
{
  // pass...
}

bool
MD32UVACK::serialize(QByteArray &buffer) {
  buffer.append('\x06');
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV search request
 * ******************************************************************************************** */
MD32UVSearchRequest::MD32UVSearchRequest()
  : MD32UVRequest()
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV search response
 * ******************************************************************************************** */
MD32UVSearchResponse::MD32UVSearchResponse(const QByteArray &model)
  : GenericResponse(), _model(model)
{
  if (_model.size() < 7)
    _model.append(7-_model.size(), '\0');
  else if (_model.size() > 7)
    _model = _model.first(7);
}

bool
MD32UVSearchResponse::serialize(QByteArray &buffer) {
  buffer.append('\x06');
  buffer.append(_model);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV password request
 * ******************************************************************************************** */
MD32UVPasswordRequest::MD32UVPasswordRequest()
  : MD32UVRequest()
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV password response
 * ******************************************************************************************** */
MD32UVPasswordResponse::MD32UVPasswordResponse()
  : GenericResponse()
{
  // pass...
}

bool
MD32UVPasswordResponse::serialize(QByteArray &buffer) {
  buffer.append("P\x00\x00", 3);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV start system info request
 * ******************************************************************************************** */
MD32UVStartSystemInfoRequest::MD32UVStartSystemInfoRequest()
  : MD32UVRequest()
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV value request
 * ******************************************************************************************** */
MD32UVValueRequest::MD32UVValueRequest(uint16_t flags, uint8_t len, uint8_t field)
  : MD32UVRequest(), _flags(flags), _length(len), _field(field)
{
  // pass...
}

uint16_t
MD32UVValueRequest::flags() const {
  return _flags;
}

uint8_t
MD32UVValueRequest::length() const {
  return _length;
}

uint8_t
MD32UVValueRequest::field() const {
  return _field;
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV value response
 * ******************************************************************************************** */
MD32UVValueResponse::MD32UVValueResponse(uint8_t field, const QByteArray &payload)
  : GenericResponse(), _field(field), _payload(payload)
{
  if (_payload.size() > 255)
    _payload = _payload.first(255);
}

bool
MD32UVValueResponse::serialize(QByteArray &buffer) {
  buffer.append('V'); buffer.append((char) _field);
  buffer.append((char) _payload.size());
  buffer.append(_payload);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV read info request
 * ******************************************************************************************** */
MD32UVReadInfoRequest::MD32UVReadInfoRequest(uint32_t address, uint16_t len)
  : MD32UVRequest(), _address(address), _length(len)
{
  // pass...
}

uint32_t
MD32UVReadInfoRequest::address() const {
  return _address;
}

uint16_t
MD32UVReadInfoRequest::length() const {
  return _length;
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV read info response
 * ******************************************************************************************** */
MD32UVReadInfoResponse::MD32UVReadInfoResponse(uint32_t address, const QByteArray &payload)
  : GenericResponse(), _address(address), _payload(payload)
{
  if (_payload.size() > 65535)
    _payload = _payload.first(65535);
}

bool
MD32UVReadInfoResponse::serialize(QByteArray &buffer) {
  buffer.append('S');
  buffer.append(0xff & (_address >>  0));
  buffer.append(0xff & (_address >>  8));
  buffer.append(0xff & (_address >> 16));
  uint16_t v = qToLittleEndian((uint16_t)_payload.size());
  buffer.append((char *)&v, sizeof(uint16_t));
  buffer.append(_payload);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV start program request
 * ******************************************************************************************** */
MD32UVStartProgramRequest::MD32UVStartProgramRequest()
  : MD32UVRequest()
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV unknown 02h request
 * ******************************************************************************************** */
MD32UVUnknown02Request::MD32UVUnknown02Request()
  : MD32UVRequest()
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV unknown 02h response
 * ******************************************************************************************** */
MD32UVUnknown02Response::MD32UVUnknown02Response()
  : GenericResponse()
{
  // pass...
}

bool
MD32UVUnknown02Response::serialize(QByteArray &buffer) {
  buffer.append(8, '\xff');
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV read request
 * ******************************************************************************************** */
MD32UVReadRequest::MD32UVReadRequest(uint32_t address, uint16_t len)
  : MD32UVRequest(), _address(address), _length(len)
{
  // pass...
}

uint32_t
MD32UVReadRequest::address() const {
  return _address;
}

uint16_t
MD32UVReadRequest::length() const {
  return _length;
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV read response
 * ******************************************************************************************** */
MD32UVReadResponse::MD32UVReadResponse(uint32_t address, const QByteArray &payload)
  : GenericResponse(), _address(address), _payload(payload)
{
  if (_payload.size() > 65535)
    _payload = _payload.first(65535);
}

bool
MD32UVReadResponse::serialize(QByteArray &buffer) {
  buffer.append('W');
  buffer.append((char) 0xff & (_address >>  0));
  buffer.append((char) 0xff & (_address >>  8));
  buffer.append((char) 0xff & (_address >> 16));
  uint16_t v = qToLittleEndian((uint16_t)_payload.size());
  buffer.append((char *)&v, sizeof(uint16_t));
  buffer.append(_payload);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV write request
 * ******************************************************************************************** */
MD32UVWriteRequest::MD32UVWriteRequest(uint32_t address, const QByteArray &payload)
  : MD32UVRequest(), _address(address), _payload(payload)
{
  // pass...
}

uint32_t
MD32UVWriteRequest::address() const {
  return _address;
}

const QByteArray &
MD32UVWriteRequest::payload() const {
  return _payload;
}
