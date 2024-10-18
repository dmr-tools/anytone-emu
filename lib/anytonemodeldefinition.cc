#include "anytonemodeldefinition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "device.hh"
#include "model.hh"
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


AnyToneDevice *
AnyToneModelFirmwareDefinition::createDevice(QIODevice *interface) const {
  // First, parse codeplug
  QFile codeplugFile(codeplug());
  if (! codeplugFile.open(QIODevice::ReadOnly))
    return nullptr;

  QXmlStreamReader reader(&codeplugFile);
  CodeplugPatternParser parser;
  if (! parser.parse(reader))
    return nullptr;

  if (! parser.topIs<CodeplugPattern>())
    return nullptr;

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();

  return new AnyToneDevice(interface, new AnyToneModel(codeplug, modelId(), revision()));
}




