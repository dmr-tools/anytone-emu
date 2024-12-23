#include "anytoneprotocol.hh"
#include <QtEndian>
#include "logger.hh"


inline uint8_t crc8(const QByteArray &data) {
  uint8_t crc = 0;
  for (int i=0; i<data.length(); i++)
    crc += data[i];
  return crc;
}


/* ********************************************************************************************* *
 * AnytoneRequest implementation
 * ********************************************************************************************* */
AnytoneRequest::AnytoneRequest()
{
  // pass...
}

AnytoneRequest::~AnytoneRequest()
{
  // pass...
}

AnytoneRequest *
AnytoneRequest::fromBuffer(QByteArray &buffer) {
  if (buffer.startsWith("PROGRAM")) {
    buffer.remove(0, 7); // Remove request
    return new AnytoneProgramRequest();
  } else if (buffer.startsWith("END")) {
    buffer.remove(0, 3); // Remove request
    return new AnytoneEndRequest();
  } else if ((buffer.size()>=1) && (0x02 == buffer.front())) {
    buffer.remove(0, 7); // Remove request
    return new AnytoneDeviceInfoRequest();
  } else if ((buffer.size()>=6) && ('R' == buffer.front())) {
    uint32_t address = qFromBigEndian(*(uint32_t*)buffer.mid(1,4).constData());
    uint8_t  length  = buffer[5];
    buffer.remove(0,6);
    return new AnytoneReadRequest(address, length);
  } else if ((buffer.size()>=8) && ('W' == buffer.front())) {
    uint32_t address = qFromBigEndian(*(uint32_t*)buffer.mid(1,4).constData());
    uint8_t  length  = buffer[5];
    // Continue, if sufficient data is there
    if (buffer.size()<(length+8))
      return nullptr;
    // Get payload
    QByteArray payload = buffer.mid(6, length);
    // check CRC
    if ((uint8_t)buffer[6+length] != crc8(buffer.mid(1, 5+length))) {
      logWarn() << "CRC mismatch!";
    }
    buffer.remove(0,8+length);
    return new AnytoneWriteRequest(address, payload);
  }

  return nullptr;
}


/* ********************************************************************************************* *
 * AnytoneProgramRequest implementation
 * ********************************************************************************************* */
AnytoneProgramRequest::AnytoneProgramRequest()
  : AnytoneRequest()
{
  // pass...
}


/* ********************************************************************************************* *
 * AnytoneEndRequest implementation
 * ********************************************************************************************* */
AnytoneEndRequest::AnytoneEndRequest()
  : AnytoneRequest()
{
  // pass...
}


/* ********************************************************************************************* *
 * AnytoneDeviceInfoRequest implementation
 * ********************************************************************************************* */
AnytoneDeviceInfoRequest::AnytoneDeviceInfoRequest()
  : AnytoneRequest()
{
  // pass...
}


/* ********************************************************************************************* *
 * AnytoneReadRequest implementation
 * ********************************************************************************************* */
AnytoneReadRequest::AnytoneReadRequest(uint32_t addr, uint8_t len)
  : AnytoneRequest(), _addr(addr), _len(len)
{
  // pass...
}


/* ********************************************************************************************* *
 * AnytoneWriteRequest implementation
 * ********************************************************************************************* */
AnytoneWriteRequest::AnytoneWriteRequest(uint32_t addr, const QByteArray &payload)
  : AnytoneRequest(), _addr(addr), _payload(payload)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneResponse
 * ********************************************************************************************* */
AnytoneResponse::AnytoneResponse()
{
  // pass...
}

AnytoneResponse::~AnytoneResponse() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of AnytoneProgramResponse
 * ********************************************************************************************* */
AnytoneProgramResponse::AnytoneProgramResponse()
  : AnytoneResponse()
{
  // pass...
}

bool
AnytoneProgramResponse::serialize(QByteArray &buffer) {
  buffer.append("QX\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneDeviceInfoResponse
 * ********************************************************************************************* */
AnytoneDeviceInfoResponse::AnytoneDeviceInfoResponse(const QByteArray &model, const QByteArray &hwVersion)
  : AnytoneResponse(), _model(model), _hwVersion(hwVersion)
{
  // pass...
}

bool
AnytoneDeviceInfoResponse::serialize(QByteArray &buffer) {
  buffer.append("ID");
  buffer.append(_model.left(6));
  buffer.append('\x00');
  buffer.append(_hwVersion.left(6));
  buffer.append("\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneReadResponse
 * ********************************************************************************************* */
AnytoneReadResponse::AnytoneReadResponse(uint32_t addr, const QByteArray payload)
  : AnytoneResponse(), _address(addr), _payload(payload)
{
  // pass...
}

bool
AnytoneReadResponse::serialize(QByteArray &buffer) {
  uint32_t addr = qToBigEndian(_address);
  buffer.append("W");
  buffer.append(QByteArray::fromRawData((char*) &addr, 4));
  buffer.append((char)_payload.length());
  buffer.append(_payload);
  buffer.append(crc8(buffer.right(5+_payload.length())));
  buffer.append("\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnytoneWriteResponse
 * ********************************************************************************************* */
AnytoneWriteResponse::AnytoneWriteResponse(uint32_t addr, uint8_t len)
  : AnytoneResponse(), _address(addr), _length(len)
{
  // pass...
}

bool
AnytoneWriteResponse::serialize(QByteArray &buffer) {
  buffer.append("\x06");
  return true;
}

