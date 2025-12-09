#include "device.hh"

#include <QIODevice>
#include <QtEndian>

#include "logger.hh"
#include "protocol.hh"

using namespace std::chrono_literals;
// Block meta flags starting at address 0x00000000, each block is 0x1000b wide.
/*constexpr char meta_flags[] =
  "\xff\x07\x20\x34\x09\x0a\x54\x06\x42\x67\x43\x44\x10\x30\x0d\x0f"  // 00000000 - 0000ffff
  "\x41\x03\x59\x5d\x00\x13\x55\x14\x65\x57\x50\x50\x5b\x66\x6a\x53"  // 00010000 - 0001ffff
  "\x1f\x4f\x5f\x00\x58\x6d\x04\x00\x3b\x00\x1c\x00\x00\x00\x00\x00"  // 00020000 - 0002ffff
  "\x00\x37\x00\x00\x00\x5c\xff\xff\xff\x05\xff\xff\xff\xff\xff\x32"  // 00030000 - 0003ffff
  "\xff\x22\xff\xff\xff\xff\xff\xff\xff\x31\xff\xff\xff\x5a\xff\xff"  // 00040000 - 0004ffff
  "\xff\x6c\xff\xff\xff\xff\xff\xff\x0e\x75\xff\xff\xff\xff\xff\xff"  // 00050000 - 0005ffff
  "\xff\xff\xff\xff\xff\x74\xff\xff\xff\x64\x02\x11\xff\xff\xff\xff"  // 00060000 - 0006ffff
  "\xff\xff\x1a\x18\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x5e\x6e"  // 00070000 - 0007ffff
  "\xff\x56\x60\xff\x3d\xff\xff\xff\xff\xff\xff\xff\xff\x0b\xff\xff"  // 00080000 - 0008ffff
  "\xff\xff\xff\xff\xff\xff\x6b\x01\x7c\xff\xff\xff\xff\xff\xff\xff"  // 00090000 - 0009ffff
  "\xff\xff\x23\x00\xff\xff\xff\xff\xff\xff\x12\xff\x51\x1d\xff\xff"  // 000a0000 - 000affff
  "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x69\xff\xff\xff"  // 000b0000 - 000bffff
  "\xff\xff\xff\x08\xff\x4b\xff\x00\x0c";                             // 000c0000 - 000c8fff */

constexpr char meta_flags_sorted[] =
  "\xff\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"  // 00000000 - 0000ffff
  "\x10\x11\x12\x13\x14\xff\xff\x00\x18\xff\x1a\xff\x1c\x1d\xff\x1f"  // 00010000 - 0001ffff
  "\x20\xff\x22\x23\x24\xff\x00\x00\xff\x00\xff\x00\x00\x00\x00\x00"  // 00020000 - 0002ffff
  "\x30\x31\x32\x00\x34\xff\xff\x37\xff\xff\xff\x3b\xff\x3d\xff\x00"  // 00030000 - 0003ffff
  "\xff\x41\x42\x43\x44\xff\xff\xff\xff\xff\xff\x4b\xff\xff\xff\x4f"  // 00040000 - 0004ffff
  "\x50\x51\xff\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f"  // 00050000 - 0005ffff
  "\x60\xff\xff\xff\x64\x65\x66\x67\xff\x69\x6a\x6b\x6c\x6d\x6e\xff"  // 00060000 - 0006ffff
  "\xff\xff\xff\xff\x74\x75\xff\xff\xff\xff\xff\xff\x7c\xff\xff\xff"  // 00070000 - 0007ffff
  "\xff\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"  // 00080000 - 0008ffff
  "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"  // 00090000 - 0009ffff
  "\xff\xff\x00\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"  // 000a0000 - 000affff
  "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff"  // 000b0000 - 000bffff
  "\xff\xff\xff\xff\xff\xff\xff\x00\xff";                             // 000c0000 - 000c8fff


/* ********************************************************************************************* *
 * Implementation of MD32UV Device
 * ********************************************************************************************* */
MD32UVDevice::MD32UVDevice(
    QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : GenericDevice{interface, pattern, handler, parent}, _timer()
{
  // IO time-out handling
  _timer.setInterval(2000);
  _timer.setSingleShot(true);
  connect(&_timer, &QTimer::timeout, [this]() {
    emit endProgram();
  });

  for (uint addr=0; addr<0xc9; addr++) {
    rom().write(addr*0x1000+0xfff, QByteArray(1, meta_flags_sorted[addr]));
  }
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
    _timer.start();
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
    if (0xfff001 == address) // Allocate new memory depending on flag
      address = (uint32_t)((uint8_t)payload.back())<<12;
    write(address, payload);
    return new MD32UVACK();
  }

  logWarn() << "Got unkown request type.";
  return new MD32UVACK();
}


bool
MD32UVDevice::getValue(uint8_t field, uint8_t length, QByteArray &payload) {
  switch ((Value)field) {
  case Value::FirmwareVersion:
    payload = QByteArray::fromHex("33 32 2e 30 31 2e 30 31 2e 30 34");
    return true;
  case Value::Unknown02h:
    payload = QByteArray::fromHex("00 00 00 00 00 00 1c 4c 00 00 1c 4c");
    return true;
  case Value::BuildDate:
    payload = QByteArray::fromHex("32 30 32 32 2d 30 36 2d 32 37");
    return true;
  case Value::DSPVersion:
    payload = QByteArray::fromHex("44 31 2e 30 31 2e 30 31 2e 30 30 34");
    return true;
  case Value::RadioVersion:
    payload = QByteArray::fromHex("52 31 2e 30 30 2e 30 31 2e 30 30 31");
    return true;
  case Value::AudioResourceAddress:
    //payload = QByteArray::fromHex("00 10 20 00 ff 4f 26 00");
    payload = packAddressRange(0x00201000, 0x00264fff);
    return true;
  case Value::CompactItemTable:
    //payload = QByteArray::fromHex("00 90 0c 00 ff 9f 14 00");
    payload = packAddressRange(0x000c9000, 0x00149fff);
    return true;
  case Value::ZonesAddress:
    //payload = QByteArray::fromHex("00 00 18 00 ff 0f 20 00");
    payload = packAddressRange(0x00180000, 0x00200fff);
    return true;
  case Value::EmergencyAddress:
    //payload = QByteArray::fromHex("00 c0 6d 00 ff ff ff 00");
    payload = packAddressRange(0x006dc000, 0x00ffffff);
    return true;
  case Value::MainConfigAddress:
    //payload = QByteArray::fromHex("00 10 00 00 ff 8f 0c 00");
    payload = packAddressRange(0x00001000, 0x000c8fff);
    return true;
  case Value::CodeplugVersion:
    payload = QByteArray::fromHex("43 31 2e 30 30 2e 30 31 2e 30 30 31");
    return true;
  case Value::Capabilities:
    payload = QByteArray::fromHex("03 4e 2d 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "3f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 "
                                  "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
    payload = payload.first(length);
    return true;
  case Value::ListsAddress:
    //payload = QByteArray::fromHex("00 00 15 00 ff 5f 17 00");
    payload = packAddressRange(0x00150000, 0x00175fff);
    return true;
  case Value::ContactsAddress:
    //payload = QByteArray::fromHex("00 80 27 00 ff bf 6d 00");
    payload = packAddressRange(0x00278000, 0x006dbfff);
    return true;
  case Value::Unknown10h:
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


QByteArray
MD32UVDevice::packAddressRange(uint32_t from, uint32_t to) {
  from = qToLittleEndian(from);
  to = qToLittleEndian(to);

  QByteArray buffer;
  buffer.append((char *)&from, sizeof(uint32_t));
  buffer.append((char *)&to, sizeof(uint32_t));

  return buffer;
}
