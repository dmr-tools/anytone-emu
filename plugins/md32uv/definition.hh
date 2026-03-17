#ifndef ANYTONEEMU_DM32UV_DEFINITION_HH
#define ANYTONEEMU_DM32UV_DEFINITION_HH

#include "modeldefinition.hh"


struct DM32UVFirmwareProperties
{
  struct AddressRange {
    uint32_t address;
    uint32_t size;

    QByteArray pack() const;
  };

  QByteArray firmwareVersion;
  QDate buildDate;
  QByteArray dspVersion;
  QByteArray radioVersion;
  QByteArray codeplugVersion;
  AddressRange mainConfigRange;
  AddressRange compactItemTableRange;
  AddressRange listsRange;
  AddressRange zonesRange;
  AddressRange audioResourceRange;
  AddressRange contactsRange;
  AddressRange emergencyRange;
  uint32_t contactsCount;

  DM32UVFirmwareProperties();
};



class DM32UVFirmwareDefinition : public GenericModelFirmwareDefinition
{
  Q_OBJECT

public:
  explicit DM32UVFirmwareDefinition(DeviceClassPluginInterface *plugin, ModelDefinition *parent=nullptr);

  const DM32UVFirmwareProperties &properties() const;
  DM32UVFirmwareProperties &properties();

protected:
  DM32UVFirmwareProperties _properties;
};

#endif // ANYTONEEMU_DM32UV_DEFINITION_HH
