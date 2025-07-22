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
    buffer = buffer.sliced(1);
    return new MD32UVPingRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("PSEARCH")) {
    buffer = buffer.sliced(7);
    return new MD32UVSearchRequest();
  } else if ((buffer.size() >= 7) && buffer.startsWith("PASSSTA")) {
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
  } else if ((buffer.size() >= 5) && buffer.startsWith("V")) {
    auto flags = qFromLittleEndian(*(uint16_t *)(buffer.constData()+1));
    auto len   = qFromLittleEndian(*(uint8_t *)(buffer.constData()+3));
    auto field = qFromLittleEndian(*(uint8_t *)(buffer.constData()+4));
    logDebug() << "Request " << len << "b from value " << field << ".";
    buffer = buffer.sliced(5);
    return new MD32UVValueRequest(flags, len, field);
  } else if ((buffer.size() >= 6) && buffer.startsWith('G')) {
    auto flags = qFromLittleEndian(*(uint8_t *)(buffer.constData()+1));
    auto field = qFromLittleEndian(*(uint16_t *)(buffer.constData()+2));
    auto len   = qFromLittleEndian(*(uint16_t *)(buffer.constData()+3));
    logDebug() << "Read " << len << "b from value at address "
               << Qt::hex << ((((uint32_t)field)<<8) | flags) << ".";
    buffer = buffer.sliced(6);
    return new MD32UVReadInfoRequest(flags, field, len);
  } else if ((buffer.size() >= 6) && buffer.startsWith('R')) {
    auto flags = qFromLittleEndian(*(uint8_t *)(buffer.constData()+1));
    auto field = qFromLittleEndian(*(uint16_t *)(buffer.constData()+2));
    auto len   = qFromLittleEndian(*(uint16_t *)(buffer.constData()+3));
    logDebug() << "Read " << len << "b from memory at address "
               << Qt::hex << ((((uint32_t)field)<<8) | flags) << ".";
    buffer = buffer.sliced(6);
    return new MD32UVReadRequest(flags, field, len);
  } else if ((buffer.size() >= 6) && buffer.startsWith('W')) {
    auto flags = qFromLittleEndian(*(uint8_t *)(buffer.constData()+1));
    auto field = qFromLittleEndian(*(uint16_t *)(buffer.constData()+2));
    auto len   = qFromLittleEndian(*(uint16_t *)(buffer.constData()+3));
    if (buffer.size() < (len+6))
      return nullptr;
    auto payload = buffer.mid(6,len);
    buffer = buffer.sliced(6+len);
    return new MD32UVWriteRequest(flags, field, payload);
  } else if ((buffer.size() >= 1) && (-1 == buffer.at(0))) {
    buffer = buffer.sliced(1);
  } else if (buffer.size()) {
    logDebug() << "Left over: " << buffer.toHex(' ');
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
MD32UVReadInfoRequest::MD32UVReadInfoRequest(uint8_t flags, uint16_t field, uint16_t len)
  : MD32UVRequest(), _flags(flags), _field(field), _length(len)
{
  // pass...
}

uint8_t
MD32UVReadInfoRequest::flags() const {
  return _flags;
}

uint16_t
MD32UVReadInfoRequest::field() const {
  return _field;
}

uint16_t
MD32UVReadInfoRequest::length() const {
  return _length;
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV read info response
 * ******************************************************************************************** */
MD32UVReadInfoResponse::MD32UVReadInfoResponse(uint8_t flags, uint16_t field, const QByteArray &payload)
  : GenericResponse(), _flags(flags), _field(field), _payload(payload)
{
  if (_payload.size() > 65535)
    _payload = _payload.first(65535);
}

bool
MD32UVReadInfoResponse::serialize(QByteArray &buffer) {
  buffer.append('S');
  buffer.append((char) _flags);
  uint16_t v = qToLittleEndian(_field);
  buffer.append((char *)&v, sizeof(uint16_t));
  v = qToLittleEndian((uint16_t)_payload.size());
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
 * Implementation of MD-32UV read request
 * ******************************************************************************************** */
MD32UVReadRequest::MD32UVReadRequest(uint8_t flags, uint16_t field, uint16_t len)
  : MD32UVRequest(), _flags(flags), _field(field), _length(len)
{
  // pass...
}

uint8_t
MD32UVReadRequest::flags() const {
  return _flags;
}

uint16_t
MD32UVReadRequest::field() const {
  return _field;
}

uint16_t
MD32UVReadRequest::length() const {
  return _length;
}


/* ******************************************************************************************** *
 * Implementation of MD-32UV read response
 * ******************************************************************************************** */
MD32UVReadResponse::MD32UVReadResponse(uint8_t flags, uint16_t field, const QByteArray &payload)
  : GenericResponse(), _flags(flags), _field(field), _payload(payload)
{
  if (_payload.size() > 65535)
    _payload = _payload.first(65535);
}

bool
MD32UVReadResponse::serialize(QByteArray &buffer) {
  buffer.append('W');
  buffer.append((char) _flags);
  uint16_t v = qToLittleEndian(_field);
  buffer.append((char *)&v, sizeof(uint16_t));
  v = qToLittleEndian((uint16_t)_payload.size());
  buffer.append((char *)&v, sizeof(uint16_t));
  buffer.append(_payload);
  return true;
}



/* ******************************************************************************************** *
 * Implementation of MD-32UV write request
 * ******************************************************************************************** */
MD32UVWriteRequest::MD32UVWriteRequest(uint8_t flags, uint16_t field, const QByteArray &payload)
  : MD32UVRequest(), _flags(flags), _field(field), _payload(payload)
{
  // pass...
}

uint8_t
MD32UVWriteRequest::flags() const {
  return _flags;
}

uint16_t
MD32UVWriteRequest::field() const {
  return _field;
}

const QByteArray &
MD32UVWriteRequest::payload() const {
  return _payload;
}
