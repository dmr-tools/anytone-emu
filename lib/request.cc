#include "request.hh"
#include <QtEndian>

/* ********************************************************************************************* *
 * Request implementation
 * ********************************************************************************************* */
Request::Request()
{
  // pass...
}

Request::~Request()
{
  // pass...
}

Request *
Request::fromBuffer(QByteArray &buffer) {
  if (buffer.startsWith("PROGRAM")) {
    buffer.remove(0, 7); // Remove request
    return new ProgramRequest();
  } else if (buffer.startsWith("END")) {
    buffer.remove(0, 3); // Remove request
    return new EndRequest();
  } else if ((buffer.size()>=1) && (0x02 == buffer.front())) {
    buffer.remove(0, 7); // Remove request
    return new DeviceInfoRequest();
  } else if ((buffer.size()>=6) && ('R' == buffer.front())) {
    uint32_t address = qFromBigEndian(buffer.mid(1,4).toUInt());
    uint8_t  length  = buffer[5];
    buffer.remove(0,6);
    return new ReadRequest(address, length);
  } else if ((buffer.size()>=8) && ('W' == buffer.front())) {
    uint32_t address = qFromBigEndian(buffer.mid(1,4).toUInt());
    uint8_t  length  = buffer[5];
    if (buffer.size()<(length+8))
      return nullptr;
    QByteArray payload = buffer.mid(6,length);
    uint16_t crc = qFromBigEndian(buffer.mid(6+length,2).toUShort());
    return new WriteRequest(address, payload, crc);
  }

  return nullptr;
}


/* ********************************************************************************************* *
 * ProgramRequest implementation
 * ********************************************************************************************* */
ProgramRequest::ProgramRequest()
  : Request()
{
  // pass...
}


/* ********************************************************************************************* *
 * EndRequest implementation
 * ********************************************************************************************* */
EndRequest::EndRequest()
  : Request()
{
  // pass...
}


/* ********************************************************************************************* *
 * DeviceInfoRequest implementation
 * ********************************************************************************************* */
DeviceInfoRequest::DeviceInfoRequest()
  : Request()
{
  // pass...
}


/* ********************************************************************************************* *
 * ReadRequest implementation
 * ********************************************************************************************* */

/* ********************************************************************************************* *
 * WriteRequest implementation
 * ********************************************************************************************* */
