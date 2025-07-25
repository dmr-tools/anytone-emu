#include "deviceclass.hh"
#include "logger.hh"
#include "pattern.hh"

#include "definition.hh"
#include "device.hh"


OpenGD77DeviceClassPlugin::OpenGD77DeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
}


ModelDefinition *
OpenGD77DeviceClassPlugin::modelDefinition(const QString &id, QObject *parent, const ErrorStack &err) {
  return new OpenGD77ModelDefinition(id, parent);
}

Device *
OpenGD77DeviceClassPlugin::device(QIODevice *interface, const ModelFirmwareDefinition *firmware,
                                  ImageCollector *handler, QObject *parent, const ErrorStack &err) {
  CodeplugPattern *codeplug = CodeplugPattern::load(firmware->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << firmware->codeplug() << "'.";
    return nullptr;
  }

  return new OpenGD77Device(interface, codeplug, handler, parent);
}

