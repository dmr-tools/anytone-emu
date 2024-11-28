#include "opengd77modeldefinition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "opengd77device.hh"
#include "codeplugpatternparser.hh"


/* ********************************************************************************************* *
 * Implementation of OpenGD77ModelDefinition (nothing to do)
 * ********************************************************************************************* */
OpenGD77ModelDefinition::OpenGD77ModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition(id, parent)
{
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77ModelFirmwareDefinition
 * ********************************************************************************************* */
OpenGD77ModelFirmwareDefinition::OpenGD77ModelFirmwareDefinition(const QString &context, OpenGD77ModelDefinition *parent)
  : ModelFirmwareDefinition{context, parent}
{
  // pass...
}


Device *
OpenGD77ModelFirmwareDefinition::createDevice(QIODevice *interface, const ErrorStack &err) const {
  CodeplugPattern *codeplug = CodeplugPattern::load(this->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << this->codeplug() << "'.";
    return nullptr;
  }

  Device *dev = new OpenGD77Device(interface, codeplug, nullptr);
  dev->rom() += qobject_cast<OpenGD77ModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}
