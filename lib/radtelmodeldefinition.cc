#include "radtelmodeldefinition.hh"


#include <QXmlStreamReader>
#include "pattern.hh"
#include "radteldevice.hh"


/* ********************************************************************************************* *
 * Implementation of RadtelModelDefinition (nothing to do)
 * ********************************************************************************************* */
RadtelModelDefinition::RadtelModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition(id, parent)
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of RadtelModelFirmwareDefinition
 * ********************************************************************************************* */
RadtelModelFirmwareDefinition::RadtelModelFirmwareDefinition(const QString &context, RadtelModelDefinition *parent)
  : ModelFirmwareDefinition{context, parent}
{
  // pass...
}


Device *
RadtelModelFirmwareDefinition::createDevice(QIODevice *interface, const ErrorStack &err) const {
  CodeplugPattern *codeplug = CodeplugPattern::load(this->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << this->codeplug() << "'.";
    return nullptr;
  }

  Device *dev = new RadtelDevice(interface, codeplug, nullptr);
  dev->rom() += qobject_cast<RadtelModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}
