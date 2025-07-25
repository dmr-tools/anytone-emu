#include "deviceclass.hh"
#include "logger.hh"
#include "pattern.hh"

#include "device.hh"
#include "modeldefinition.hh"


OpenGD77DeviceClassPlugin::OpenGD77DeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
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

