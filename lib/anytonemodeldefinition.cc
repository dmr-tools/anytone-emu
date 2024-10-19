#include "anytonemodeldefinition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "device.hh"
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
  // First, parse codeplug
  QFile codeplugFile(codeplug());
  if (! codeplugFile.open(QIODevice::ReadOnly)) {
    errMsg(err) << "Cannot open codeplug file '" << codeplugFile.fileName()
                << "': " << codeplugFile.errorString();
    return nullptr;
  }

  QXmlStreamReader reader(&codeplugFile);
  CodeplugPatternParser parser;
  if (! parser.parse(reader)) {
    errMsg(err) << "Cannot parse codeplug file '" << codeplugFile.fileName()
                << "' :" << parser.errorMessage();
    return nullptr;
  }

  if (! parser.topIs<CodeplugPattern>()) {
    errMsg(err) << "Internal error, unexpected pattern type!";
    return nullptr;
  }

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();

  return new AnyToneDevice(interface, codeplug, nullptr, modelId(), revision());
}




