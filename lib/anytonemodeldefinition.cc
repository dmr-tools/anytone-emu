#include "anytonemodeldefinition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "anytonedevice.hh"
#include "codeplugpatternparser.hh"


/* ********************************************************************************************* *
 * Implementation of AnyToneModelDefinition
 * ********************************************************************************************* */
AnyToneModelDefinition::AnyToneModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition{id, parent}, _modelId(), _revision()
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
  : ModelFirmwareDefinition{context, parent}, _modelId(), _revision()
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

  Device *dev = new AnyToneDevice(interface, codeplug, nullptr, modelId(), revision());
  dev->rom() += qobject_cast<AnyToneModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}




