#include "deviceclass.hh"
#include "logger.hh"
#include "pattern.hh"

#include "definition.hh"
#include "device.hh"


RadtelDeviceClassPlugin::RadtelDeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
}


ModelDefinition *
RadtelDeviceClassPlugin::modelDefinition(const QString &id, QObject *parent, const ErrorStack &err) {
  return new RadtelModelDefinition(id, parent);
}

Device *
RadtelDeviceClassPlugin::device(QIODevice *interface, const ModelFirmwareDefinition *firmware,
                                ImageCollector *handler, QObject *parent, const ErrorStack &err) {
  CodeplugPattern *codeplug = CodeplugPattern::load(firmware->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << firmware->codeplug() << "'.";
    return nullptr;
  }

  return new RadtelDevice(interface, codeplug, handler, parent);
}

