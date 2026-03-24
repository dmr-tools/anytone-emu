#include "definition.hh"

#include <QtEndian>


QByteArray
DM32UVFirmwareProperties::AddressRange::pack() const {
  uint32_t from = qToLittleEndian((uint32_t)address);
  uint32_t to   = qToLittleEndian((uint32_t)(address+size-1));

  QByteArray buffer;
  buffer.append((char *)&from, sizeof(uint32_t));
  buffer.append((char *)&to, sizeof(uint32_t));

  return buffer;

}


DM32UVFirmwareProperties::DM32UVFirmwareProperties() :
  firmwareVersion(QByteArray::fromHex("33 32 2e 30 31 2e 30 31 2e 30 34")),
  buildDate(2022,06,27),
  dspVersion(QByteArray::fromHex("44 31 2e 30 31 2e 30 31 2e 30 30 34")),
  radioVersion(QByteArray::fromHex("52 31 2e 30 30 2e 30 31 2e 30 30 31")),
  codeplugVersion(QByteArray::fromHex("43 31 2e 30 30 2e 30 31 2e 30 30 31")),
  mainConfigRange{0x00001000, 0x000c8000}, compactItemTableRange{0x000c9000, 0x00081000},
  listsRange{0x00150000, 0x00026000}, zonesRange{0x00170000, 0x00081000},
  audioResourceRange{0x006dc000, 0x00924000}, contactsRange{0x00278000, 0x464000},
  emergencyRange{0x00201000, 0x00064000}, contactsCount(0x0000c350)
{
  // pass...
}




DM32UVFirmwareDefinition::DM32UVFirmwareDefinition(DeviceClassPluginInterface *plugin, ModelDefinition *parent)
  : GenericModelFirmwareDefinition{plugin, parent}, _properties()
{
  // pass...
}


const DM32UVFirmwareProperties &
DM32UVFirmwareDefinition::properties() const {
  return _properties;
}

DM32UVFirmwareProperties &
DM32UVFirmwareDefinition::properties() {
  return _properties;
}

