#include "deviceclass.hh"
#include "logger.hh"
#include "pattern.hh"

#include "device.hh"
#include "modeldefinition.hh"


MD32UVDeviceClassPlugin::MD32UVDeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
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

