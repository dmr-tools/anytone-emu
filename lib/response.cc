#include "response.hh"
#include <QtEndian>

inline uint8_t crc8(const QByteArray &data) {
  uint8_t crc = 0;
  for (int i=0; i<data.length(); i++)
    crc += data[i];
  return crc;
}


/* ********************************************************************************************* *
 * Implementation of Response
 * ********************************************************************************************* */
Response::Response()
{
  // pass...
}

Response::~Response() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ProgramResponse
 * ********************************************************************************************* */
ProgramResponse::ProgramResponse()
  : Response()
{
  // pass...
}

bool
ProgramResponse::serialize(QByteArray &buffer) {
  buffer.append("QX\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of DeviceInfoResponse
 * ********************************************************************************************* */
DeviceInfoResponse::DeviceInfoResponse(const QByteArray &model, const QByteArray &hwVersion)
  : Response(), _model(model), _hwVersion(hwVersion)
{
  // pass...
}

bool
DeviceInfoResponse::serialize(QByteArray &buffer) {
  buffer.append("ID");
  buffer.append(_model.left(6));
  buffer.append(_hwVersion.left(6));
  buffer.append("\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of ReadResponse
 * ********************************************************************************************* */
ReadResponse::ReadResponse(uint32_t addr, const QByteArray payload)
  : Response(), _address(addr), _payload(payload)
{
  // pass...
}

bool
ReadResponse::serialize(QByteArray &buffer) {
  buffer.append("W");
  buffer.append(QByteArray::number(qToBigEndian(_address)));
  buffer.append((char)buffer.length());
  buffer.append(_payload);
  buffer.append(crc8(buffer.left(5+_payload.length())));
  buffer.append("\x06");
  return true;
}


/* ********************************************************************************************* *
 * Implementation of WriteResponse
 * ********************************************************************************************* */
WriteResponse::WriteResponse(uint32_t addr, uint8_t len)
  : Response(), _address(addr), _length(len)
{
  // pass...
}

bool
WriteResponse::serialize(QByteArray &buffer) {
  buffer.append("\x06");
  return true;
}

