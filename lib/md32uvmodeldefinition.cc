#include "md32uvmodeldefinition.hh"


#include <QXmlStreamReader>
#include "pattern.hh"
#include "md32uvdevice.hh"


/* ********************************************************************************************* *
 * Implementation of MD32UVModelDefinition (nothing to do)
 * ********************************************************************************************* */
MD32UVModelDefinition::MD32UVModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition(id, parent)
{
}



/* ********************************************************************************************* *
 * Implementation of MD32UVModelFirmwareDefinition
 * ********************************************************************************************* */
MD32UVModelFirmwareDefinition::MD32UVModelFirmwareDefinition(const QString &context, MD32UVModelDefinition *parent)
  : ModelFirmwareDefinition{context, parent}
{
  // pass...
}


Device *
MD32UVModelFirmwareDefinition::createDevice(QIODevice *interface, const ErrorStack &err) const {
  CodeplugPattern *codeplug = CodeplugPattern::load(this->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << this->codeplug() << "'.";
    return nullptr;
  }

  Device *dev = new MD32UVDevice(interface, codeplug, nullptr);
  dev->rom() += qobject_cast<MD32UVModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}
