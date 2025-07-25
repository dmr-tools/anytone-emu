#include "deviceclass.hh"
#include "logger.hh"
#include "pattern.hh"

#include "definition.hh"
#include "device.hh"


MD32UVDeviceClassPlugin::MD32UVDeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
}


ModelDefinition *
MD32UVDeviceClassPlugin::modelDefinition(const QString &id, QObject *parent, const ErrorStack &err) {
  return new MD32UVModelDefinition(id, parent);
}

Device *
MD32UVDeviceClassPlugin::device(QIODevice *interface, const ModelFirmwareDefinition *firmware,
                                ImageCollector *handler, QObject *parent, const ErrorStack &err) {
  CodeplugPattern *codeplug = CodeplugPattern::load(firmware->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << firmware->codeplug() << "'.";
    return nullptr;
  }

  return new MD32UVDevice(interface, codeplug, handler, parent);
}

