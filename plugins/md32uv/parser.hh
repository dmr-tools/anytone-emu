#ifndef PARSER_HH
#define PARSER_HH

#include "modelparser.hh"
#include "definition.hh"

class DM32UVFirmwareDefinition;


class DM32UVFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  explicit DM32UVFirmwareDefinitionHandler(
    const QString &name, const QString &codeplug, const QDate &released,
    DeviceClassPluginInterface *plugin, ModelDefinitionHandler *parent);

  ~DM32UVFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

private slots:
  virtual bool beginFirmwareVersionElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareVersionElement();
  virtual bool beginBuildDateElement(const QXmlStreamAttributes &attributes);
  virtual bool endBuildDateElement();
  virtual bool beginDSPVersionElement(const QXmlStreamAttributes &attributes);
  virtual bool endDSPVersionElement();
  virtual bool beginRadioVersionElement(const QXmlStreamAttributes &attributes);
  virtual bool endRadioVersionElement();
  virtual bool beginCodeplugVersionElement(const QXmlStreamAttributes &attributes);
  virtual bool endCodeplugVersionElement();

  virtual bool beginMainConfigElement(const QXmlStreamAttributes &attributes);
  virtual bool endMainConfigElement();
  virtual bool beginCompactItemTableElement(const QXmlStreamAttributes &attributes);
  virtual bool endCompactItemTableElement();
  virtual bool beginListsElement(const QXmlStreamAttributes &attributes);
  virtual bool endListsElement();
  virtual bool beginZonesElement(const QXmlStreamAttributes &attributes);
  virtual bool endZonesElement();
  virtual bool beginEmergencyElement(const QXmlStreamAttributes &attributes);
  virtual bool endEmergencyElement();
  virtual bool beginContactsElement(const QXmlStreamAttributes &attributes);
  virtual bool endContactsElement();
  virtual bool beginAudioResourceElement(const QXmlStreamAttributes &attributes);
  virtual bool endAudioResourceElement();
  virtual bool beginContactCountElement(const QXmlStreamAttributes &attributes);
  virtual bool endContactCountElement();


protected:
  bool getAddressRange(const QXmlStreamAttributes &attributes,
                       DM32UVFirmwareProperties::AddressRange &range);

protected:
  DM32UVFirmwareDefinition *_definition;
};

#endif // PARSER_HH
