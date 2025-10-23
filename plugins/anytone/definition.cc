#include "definition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "device.hh"


/* ********************************************************************************************* *
 * Implementation of AnyToneModelDefinition
 * ********************************************************************************************* */
AnyToneModelDefinition::AnyToneModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition{id, parent}, _modelId(), _band(0), _revision()
{
  // pass...
}


const QByteArray &
AnyToneModelDefinition::modelId() const {
  return _modelId;
}

void
AnyToneModelDefinition::setModelId(const QByteArray &id) {
  _modelId = id;
}

uint8_t
AnyToneModelDefinition::band() const {
  return _band;
}

void
AnyToneModelDefinition::setBand(uint8_t band) {
  _band = band;
}


const QByteArray &
AnyToneModelDefinition::revision() const {
  return _revision;
}

void
AnyToneModelDefinition::setRevision(const QByteArray &rev) {
  _revision = rev;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneModelFirmwareDefinition
 * ********************************************************************************************* */
AnyToneModelFirmwareDefinition::AnyToneModelFirmwareDefinition(const QString& context, AnyToneModelDefinition *parent)
  : ModelFirmwareDefinition{context, parent}, _modelId(), _band(std::numeric_limits<uint8_t>::max()), _revision()
{
  // pass...
}

const QByteArray &
AnyToneModelFirmwareDefinition::modelId() const {
  if (! _modelId.isEmpty())
    return _modelId;
  return qobject_cast<AnyToneModelDefinition *>(parent())->modelId();
}

void
AnyToneModelFirmwareDefinition::setModelId(const QByteArray &id) {
  _modelId = id;
}


uint8_t
AnyToneModelFirmwareDefinition::band() const {
  if (std::numeric_limits<uint8_t>::max() != _band)
    return _band;
  return qobject_cast<AnyToneModelDefinition *>(parent())->band();
}

void
AnyToneModelFirmwareDefinition::setBand(uint8_t band) {
  _band = band;
}

const QByteArray &
AnyToneModelFirmwareDefinition::revision() const {
  if (! _revision.isEmpty())
    return _revision;
  return qobject_cast<AnyToneModelDefinition *>(parent())->revision();
}

void
AnyToneModelFirmwareDefinition::setRevision(const QByteArray &rev) {
  _revision = rev;
}


Device *
AnyToneModelFirmwareDefinition::createDevice(QIODevice *interface, const ErrorStack &err) const {
  CodeplugPattern *codeplug = CodeplugPattern::load(this->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << this->codeplug() << "'.";
    return nullptr;
  }

  Device *dev = new AnyToneDevice(interface, codeplug, nullptr, modelId(), band(), revision());
  dev->rom() += qobject_cast<AnyToneModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}




