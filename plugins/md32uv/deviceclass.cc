#include "deviceclass.hh"
#include "pattern.hh"

#include "device.hh"
#include "modeldefinition.hh"
#include "definition.hh"
#include "parser.hh"


MD32UVDeviceClassPlugin::MD32UVDeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
}


Device *
MD32UVDeviceClassPlugin::device(QIODevice *interface, const ModelFirmwareDefinition *firmware,
                                ImageCollector *handler, QObject *parent, const ErrorStack &err) {
  auto fwDef = qobject_cast<const DM32UVFirmwareDefinition*>(firmware);

  CodeplugPattern *codeplug = CodeplugPattern::load(fwDef->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << fwDef->codeplug() << "'.";
    return nullptr;
  }

  return new MD32UVDevice(interface, fwDef->properties(), codeplug, handler, parent);
}


ModelFirmwareDefinitionHandler *
MD32UVDeviceClassPlugin::firmwareDefinitionHandler(const QString &name, const QString &codeplug,
                                                    const QDate &released, ModelDefinitionHandler *parent) {
  return new DM32UVFirmwareDefinitionHandler(name, codeplug, released, this, parent);
}
