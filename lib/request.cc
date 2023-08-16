#include "request.hh"
#include <QtEndian>
#include "logger.hh"

inline uint8_t crc8(const QByteArray &data) {
  uint8_t crc = 0;
  for (int i=0; i<data.length(); i++)
    crc += data[i];
  return crc;
}


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
    uint32_t address = qFromBigEndian(*(uint32_t*)buffer.mid(1,4).constData());
    uint8_t  length  = buffer[5];
    buffer.remove(0,6);
    return new ReadRequest(address, length);
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
    return new WriteRequest(address, payload);
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
ReadRequest::ReadRequest(uint32_t addr, uint8_t len)
  : Request(), _addr(addr), _len(len)
{
  // pass...
}


/* ********************************************************************************************* *
 * WriteRequest implementation
 * ********************************************************************************************* */
WriteRequest::WriteRequest(uint32_t addr, const QByteArray &payload)
  : Request(), _addr(addr), _payload(payload)
{
  // pass...
}
