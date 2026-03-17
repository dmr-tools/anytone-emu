#include "parser.hh"
#include "definition.hh"

#include <QXmlStreamAttributes>


/* ********************************************************************************************* *
 * Implementation of DM32UVFirmwareDefinitionHandler
 * ********************************************************************************************* */
DM32UVFirmwareDefinitionHandler::DM32UVFirmwareDefinitionHandler(
  const QString &name, const QString &codeplug, const QDate &released,
  DeviceClassPluginInterface *plugin, ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
  _definition(new DM32UVFirmwareDefinition{plugin})
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(parser()->context().directory().absoluteFilePath(codeplug));
}

DM32UVFirmwareDefinitionHandler::~DM32UVFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
DM32UVFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
DM32UVFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
DM32UVFirmwareDefinitionHandler::beginFirmwareVersionElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endFirmwareVersionElement() {
  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties().firmwareVersion
    = _textBuffer.toLatin1();
  _textBuffer.clear();
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginBuildDateElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endBuildDateElement() {
  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties().buildDate
    = QDate::fromString(_textBuffer, Qt::ISODate);
  _textBuffer.clear();
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginDSPVersionElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endDSPVersionElement() {
  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties().dspVersion
    = _textBuffer.toLatin1();
  _textBuffer.clear();
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginRadioVersionElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endRadioVersionElement() {
  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties().radioVersion
    = _textBuffer.toLatin1();
  _textBuffer.clear();
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginCodeplugVersionElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endCodeplugVersionElement() {
  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties().codeplugVersion
    = _textBuffer.toLatin1();
  _textBuffer.clear();
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::getAddressRange(const QXmlStreamAttributes &attributes,
                                                 DM32UVFirmwareProperties::AddressRange &range) {
  if (! attributes.hasAttribute("address")) {
    raiseError("Attribute 'address' missing");
    return false;
  }
  if (! attributes.hasAttribute("size")) {
    raiseError("Attribute 'size' missing");
    return false;
  }

  bool ok;
  uint32_t address = attributes.value("address").toUInt(&ok, 16);
  if (! ok) {
    raiseError(QString("Invalid value for 'address': '%1'")
                 .arg(attributes.value("address")));
    return false;
  }

  uint32_t size = attributes.value("size").toUInt(&ok, 16);
  if (! ok) {
    raiseError(QString("Invalid value for 'size': '%1'")
                 .arg(attributes.value("size")));
    return false;
  }

  range = {address, size};
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginMainConfigElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
                  ->properties().mainConfigRange);
}

bool
DM32UVFirmwareDefinitionHandler::endMainConfigElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginCompactItemTableElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().compactItemTableRange);
}

bool
DM32UVFirmwareDefinitionHandler::endCompactItemTableElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginListsElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().listsRange);
}

bool
DM32UVFirmwareDefinitionHandler::endListsElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginZonesElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().zonesRange);
}

bool
DM32UVFirmwareDefinitionHandler::endZonesElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginEmergencyElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().emergencyRange);
}

bool
DM32UVFirmwareDefinitionHandler::endEmergencyElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginContactsElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().contactsRange);
}

bool
DM32UVFirmwareDefinitionHandler::endContactsElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginAudioResourceElement(const QXmlStreamAttributes &attributes) {
  return getAddressRange(
    attributes, qobject_cast<DM32UVFirmwareDefinition*>(definition())
      ->properties().audioResourceRange);
}

bool
DM32UVFirmwareDefinitionHandler::endAudioResourceElement() {
  return true;
}


bool
DM32UVFirmwareDefinitionHandler::beginContactCountElement(const QXmlStreamAttributes &attributes) {
  Q_UNUSED(attributes);
  _textBuffer.clear();
  return true;
}

bool
DM32UVFirmwareDefinitionHandler::endContactCountElement() {
  bool ok;
  unsigned int count = _textBuffer.toUInt(&ok);
  if (! ok) {
    raiseError(QString("Invalid value for contact counts: %1").arg(_textBuffer));
    return false;
  }

  qobject_cast<DM32UVFirmwareDefinition*>(definition())->properties()
    .contactsCount = count;
  _textBuffer.clear();
  return true;
}




