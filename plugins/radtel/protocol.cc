#include "protocol.hh"
#include <QtEndian>
#include "logger.hh"


// CRC
inline bool checkCRC(const QByteArray &buffer) {
  uint8_t b = 0;
  for (qsizetype i=0; i<(buffer.size()-1); i++)
    b += (uint8_t)buffer.at(i);
  return b == (uint8_t)buffer.at(buffer.size()-1);
}


/* ********************************************************************************************* *
 * Implementation of RadTel request
 * ********************************************************************************************* */
RadtelRequest::RadtelRequest() {
  // pass...
}


RadtelRequest::~RadtelRequest() {
  // pass...
}


RadtelRequest *
RadtelRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  ok = true;
  if (1 > buffer.size())
    return nullptr;

  if (0x34 == buffer.at(0)) {
    if (5 > buffer.size())
      return nullptr;
    if (! checkCRC(buffer.first(5))) {
      errMsg(err) << "Cannot parse request: invalid CRC.";
      ok = false;
      return nullptr;
    }
    uint8_t f1 = buffer.at(2), f2 = buffer.at(3);
    if ((0x05 == f1) && (0x10==f2)) {
      buffer = buffer.sliced(5);
      return new RadtelCommandRequest(RadtelCommandRequest::EnterProgrammingMode);
    } else if ((0x05 == f1) && (0xee==f2)) {
      buffer = buffer.sliced(5);
      return new RadtelCommandRequest(RadtelCommandRequest::LeaveProgrammingMode);
    }
  } else if (0x52 == buffer.at(0)) {
    if (4 > buffer.size())
      return nullptr;
    if (! checkCRC(buffer.first(4))) {
      errMsg(err) << "Cannot parse request: invalid CRC.";
      ok = false;
      return nullptr;
    }
    auto page = qFromBigEndian(*(uint16_t *)(buffer.constData()+1));
    buffer = buffer.sliced(4);
    return new RadtelReadRequest(page);
  } else if (0x90 == (0xf0 & buffer.at(0))) {
    if (1028 > buffer.size())
      return nullptr;
    if (! checkCRC(buffer.first(1028))) {
      errMsg(err) << "Cannot parse request: invalid CRC.";
      ok = false;
      return nullptr;
    }
    auto segment = 0x0f & (uint8_t)buffer.at(0);
    auto page = qFromBigEndian(*(uint16_t *)(buffer.constData()+1));
    auto payload = buffer.mid(3, 1024);
    buffer = buffer.sliced(1028);
    return new RadtelWriteRequest(segment, page, payload);
  }

  errMsg(err) << "Unexpected request: " << buffer << ".";
  ok = false;
  buffer.clear();

  return nullptr;
}



/* ********************************************************************************************* *
 * Implementation of RadTel response
 * ********************************************************************************************* */
RadtelResponse::RadtelResponse()
{
  // pass...
}

RadtelResponse::~RadtelResponse()
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of RadTel command request
 * ********************************************************************************************* */
RadtelCommandRequest::RadtelCommandRequest(Command command)
  : RadtelRequest(), _command(command)
{
  // pass...
}

RadtelCommandRequest::Command
RadtelCommandRequest::command() const {
  return _command;
}



/* ********************************************************************************************* *
 * Implementation of RadTel read request
 * ********************************************************************************************* */
RadtelReadRequest::RadtelReadRequest(uint16_t page)
  : RadtelRequest(), _page(page)
{
  // pass...
}


uint16_t
RadtelReadRequest::page() const {
  return _page;
}

uint32_t
RadtelReadRequest::address() const {
  return 1024*((uint32_t)_page);
}



/* ********************************************************************************************* *
 * Implementation of RadTel write request
 * ********************************************************************************************* */
RadtelWriteRequest::RadtelWriteRequest(uint8_t segment, uint16_t page, const QByteArray &payload)
  : RadtelRequest(), _segment(segment), _page(page), _payload(payload)
{
  // pass...
}


uint8_t
RadtelWriteRequest::segment() const {
  return _segment;
}

uint16_t
RadtelWriteRequest::page() const {
  return _page;
}

uint32_t
RadtelWriteRequest::address() const {
  return (((uint32_t)_segment) << 24) + (((uint32_t)_page)<<10);
}

const QByteArray &
RadtelWriteRequest::payload() const {
  return _payload;
}



/* ********************************************************************************************* *
 * Implementation of RadTel ACK response
 * ********************************************************************************************* */
RadtelACK::RadtelACK()
  : RadtelResponse()
{
  // pass...
}

bool
RadtelACK::serialize(QByteArray &buffer) {
  buffer.append(0x06);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of RadTel read response
 * ********************************************************************************************* */
RadtelReadResonse::RadtelReadResonse(uint16_t page, const QByteArray &payload)
  : _page(page), _payload(payload)
{
  if (1024 > _payload.size())
    _payload.append(1024 - _payload.size(), 0x00);
  else if (1024 < _payload.size())
    _payload = _payload.first(1024);
}

bool
RadtelReadResonse::serialize(QByteArray &buffer) {
  uint16_t page = qToBigEndian(_page);
  auto startIdx = buffer.size(), endIdx = startIdx+1027;
  buffer.append('R'); buffer.append((char*)&page, 2);
  buffer.append(_payload);
  uint8_t crc = 0; for (auto i=startIdx; i<endIdx; i++) crc += (uint8_t)buffer.at(i);
  buffer.append(crc);
  return true;
 }
