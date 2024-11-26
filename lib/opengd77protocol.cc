#include "opengd77protocol.hh"
#include <QtEndian>
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of OpenGD77Request
 * ********************************************************************************************* */
OpenGD77Request::OpenGD77Request()
{
  // pass...
}

OpenGD77Request::~OpenGD77Request()
{
  // pass...
}

OpenGD77Request *
OpenGD77Request::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  if (0 == buffer.size())
    return nullptr;

  if ('C' == buffer.at(0))
    return OpenGD77CommandRequest::fromBuffer(buffer, ok, err);

  if ('R' == buffer.at(0))
    return OpenGD77ReadRequest::fromBuffer(buffer, ok, err);

  if ('W' == buffer.at(0))
    return OpenGD77WriteRequest::fromBuffer(buffer, ok, err);

  ok = false;
  errMsg(err) << "Unknown command type '" << buffer.at(0)
              << "' (" << (int)buffer.at(0) << ").";

  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77CommandRequest
 * ********************************************************************************************* */
OpenGD77CommandRequest::OpenGD77CommandRequest()
  : OpenGD77Request()
{
  // pass...
}

OpenGD77Request *
OpenGD77CommandRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {

  if (buffer.size() < 2) {
    ok = true;
    return nullptr;
  }

  logDebug() << "Got command request for command " << (uint8_t)buffer.at(1);

  uint8_t cmd = (uint8_t)buffer.at(1);
  switch (cmd) {
  case SHOW_CPS_SCREEN: return OpenGD77ShowCPSScreenRequest::fromBuffer(buffer, ok, err);
  case CLEAR_SCREEN: return OpenGD77ClearScreenRequest::fromBuffer(buffer, ok, err);
  case DISPLAY: return OpenGD77DisplayRequest::fromBuffer(buffer, ok, err);
  case RENDER_CPS: return OpenGD77RenderScreenRequest::fromBuffer(buffer, ok, err);
  case CLOSE_CPS_SCREEN: return OpenGD77ResetScreenRequest::fromBuffer(buffer, ok, err);
  case COMMAND: return OpenGD77ControlRequest::fromBuffer(buffer, ok, err);
  case PING: return OpenGD77PingRequest::fromBuffer(buffer, ok, err);
  default:
    break;
  }

  ok = false;
  buffer.clear();
  errMsg(err) << "Unkown command type " << (uint) cmd << ".";
  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77PingRequest
 * ********************************************************************************************* */
OpenGD77PingRequest::OpenGD77PingRequest()
  : OpenGD77CommandRequest()
{
  // pass...
}

OpenGD77Request *
OpenGD77PingRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  buffer.remove(0, 2);
  return new OpenGD77PingRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77ShowCPSScreenRequest
 * ********************************************************************************************* */
OpenGD77ShowCPSScreenRequest::OpenGD77ShowCPSScreenRequest()
  : OpenGD77CommandRequest()
{
  // pass...
}

OpenGD77Request *
OpenGD77ShowCPSScreenRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  buffer.remove(0, 2);
  return new OpenGD77ShowCPSScreenRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77ClearScreenRequest
 * ********************************************************************************************* */
OpenGD77ClearScreenRequest::OpenGD77ClearScreenRequest()
  : OpenGD77CommandRequest()
{
  // pass...
}

OpenGD77Request *
OpenGD77ClearScreenRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  buffer.remove(0, 2);
  return new OpenGD77ClearScreenRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77DisplayRequest
 * ********************************************************************************************* */
OpenGD77DisplayRequest::OpenGD77DisplayRequest(uint8_t x, uint8_t y, uint8_t font, uint8_t alignment, bool inverted, const QByteArray &text)
  : OpenGD77CommandRequest(),
    _x(x), _y(y), _font(font), _alignment(alignment), _inverted(inverted), _message(text)
{
  // pass...
}

OpenGD77Request *
OpenGD77DisplayRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  uint8_t x = buffer.at(2), y = buffer.at(3), font = buffer.at(4), alignment = buffer.at(5);
  bool inverted = buffer.at(6);
  QByteArray text = buffer.mid(7);

  buffer.clear();

  return new OpenGD77DisplayRequest{x,y, font, alignment, inverted, text};
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77RenderScreenRequest
 * ********************************************************************************************* */
OpenGD77RenderScreenRequest::OpenGD77RenderScreenRequest()
  : OpenGD77CommandRequest()
{
  // pass...
}

OpenGD77Request *
OpenGD77RenderScreenRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  buffer.remove(0, 2);
  return new OpenGD77RenderScreenRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77ResetScreenRequest
 * ********************************************************************************************* */
OpenGD77ResetScreenRequest::OpenGD77ResetScreenRequest()
  : OpenGD77CommandRequest()
{
  // pass...
}

OpenGD77Request *
OpenGD77ResetScreenRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  buffer.remove(0, 2);
  return new OpenGD77ResetScreenRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77ControlRequest
 * ********************************************************************************************* */
OpenGD77ControlRequest::OpenGD77ControlRequest(Option option)
  : OpenGD77CommandRequest(), _option(option)
{
  // pass...
}

OpenGD77ControlRequest::Option
OpenGD77ControlRequest::option() const {
  return _option;
}

OpenGD77Request *
OpenGD77ControlRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Q_UNUSED(err);
  Option opt = (Option) buffer.at(2);
  buffer.remove(0, 3);
  return new OpenGD77ControlRequest(opt);
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77ReadRequest
 * ********************************************************************************************* */
OpenGD77ReadRequest::OpenGD77ReadRequest(Section sec, uint32_t address, uint16_t length)
  : OpenGD77Request(), _section(sec), _address(address), _length(length)
{
  // pass...
}

OpenGD77ReadRequest::Section
OpenGD77ReadRequest::section() const {
  return _section;
}

uint32_t
OpenGD77ReadRequest::address() const {
  return _address;
}

uint16_t
OpenGD77ReadRequest::length() const {
  return _length;
}

OpenGD77Request *
OpenGD77ReadRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  if (buffer.size() < size()) {
    ok = true;
    return nullptr;
  }

  ok = true;

  Section sec      = (Section) buffer.at(1);
  uint32_t address = qFromBigEndian(*(uint32_t *)buffer.mid(2,4).constData());
  uint16_t length  = qFromBigEndian(*(uint16_t *)buffer.mid(6,2).constData());

  buffer.remove(0, size());

  return new OpenGD77ReadRequest{sec, address, length};
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77WriteRequest
 * ********************************************************************************************* */
OpenGD77WriteRequest::OpenGD77WriteRequest(Section section)
  : OpenGD77Request(), _section(section)
{
  // pass...
}

OpenGD77WriteRequest::Section
OpenGD77WriteRequest::section() const {
  return _section;
}

OpenGD77Request *
OpenGD77WriteRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  if (buffer.size() < size()) {
    ok = true;
    return nullptr;
  }

  Section sec = (Section)buffer.at(1);
  switch (sec) {
  case SET_FLASH_SECTOR:
    return OpenGD77SetSectorRequest::fromBuffer(buffer, ok, err);
  case WRITE_FLASH_SECTOR:
    return OpenGD77WriteSectorRequest::fromBuffer(buffer, ok, err);
  case WRITE_SECTOR_BUFFER:
  case WRITE_EEPROM:
  case WRITE_WAV_BUFFER:
    return OpenGD77WriteDataRequest::fromBuffer(buffer, ok, err);
  default:
    break;
  }

  ok = false;
  buffer.remove(0, size());
  errMsg(err) << "Unknown section " << (int) sec << " in write request";

  return nullptr;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77SetSectorRequest
 * ********************************************************************************************* */
OpenGD77SetSectorRequest::OpenGD77SetSectorRequest(uint32_t sector)
  : OpenGD77WriteRequest(OpenGD77WriteRequest::SET_FLASH_SECTOR), _sector(sector)
{
  // pass...
}


OpenGD77Request *
OpenGD77SetSectorRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {

  ok = true;

  uint32_t sector = ( (((uint32_t)buffer.at(2)) << 16) +
                      (((uint32_t)buffer.at(3)) <<  8) +
                      (((uint32_t)buffer.at(4)) <<  0) );

  buffer.remove(0, size());

  return new OpenGD77SetSectorRequest(sector);
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77WriteSectorRequest
 * ********************************************************************************************* */
OpenGD77WriteSectorRequest::OpenGD77WriteSectorRequest()
  : OpenGD77WriteRequest(OpenGD77WriteRequest::WRITE_FLASH_SECTOR)
{
  // pass...
}

OpenGD77Request *
OpenGD77WriteSectorRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  ok = true;
  buffer.remove(0, size());
  return new OpenGD77WriteSectorRequest();
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77WriteDataRequest
 * ********************************************************************************************* */
OpenGD77WriteDataRequest::OpenGD77WriteDataRequest(
    OpenGD77WriteRequest::Section section, uint32_t address, const QByteArray &data)
  : OpenGD77WriteRequest(section), _address(address), _data(data)
{
  // pass...
}

uint32_t
OpenGD77WriteDataRequest::address() const {
  return _address;
}

const QByteArray &
OpenGD77WriteDataRequest::data() const {
  return _data;
}

OpenGD77Request *
OpenGD77WriteDataRequest::fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err) {
  Section section = (Section) buffer.at(1);
  uint32_t address = qFromBigEndian(*((uint32_t *)buffer.mid(2,4).constData()));
  uint16_t length  = qFromBigEndian(*((uint16_t *)buffer.mid(6,2).constData()));
  QByteArray data  = buffer.mid(8, length);

  ok = true;
  buffer.remove(0, size());
  return new OpenGD77WriteDataRequest(section, address, data);
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77Response
 * ********************************************************************************************* */
OpenGD77Response::OpenGD77Response()
{
  // pass...
}

OpenGD77Response::~OpenGD77Response()
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77CommandResponse
 * ********************************************************************************************* */
OpenGD77CommandResponse::OpenGD77CommandResponse(bool success)
  : OpenGD77Response(), _success(success)
{
  // pass...
}

bool
OpenGD77CommandResponse::serialize(QByteArray &buffer) {
  if (_success)
    buffer.append('-');
  else
    buffer.append('E');
  return true;
}


/* ********************************************************************************************* *
 * Implementation of OpenGD77WriteResponse
 * ********************************************************************************************* */
OpenGD77WriteResponse::OpenGD77WriteResponse(OpenGD77WriteRequest::Section section)
  : OpenGD77Response(), _section(section)
{
  // pass...
}

bool
OpenGD77WriteResponse::serialize(QByteArray &buffer) {
  buffer.append('W');
  buffer.append((char)_section);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77ReadResponse
 * ********************************************************************************************* */
OpenGD77ReadResponse::OpenGD77ReadResponse(const QByteArray &data)
  : OpenGD77Response(), _data(data)
{
  // pass...
}

bool
OpenGD77ReadResponse::serialize(QByteArray &buffer) {
  buffer.append('R');
  uint16_t length = qToBigEndian((uint16_t)buffer.size());
  buffer.append((char *)&length, 2);
  buffer.append(_data);
  return true;
}


