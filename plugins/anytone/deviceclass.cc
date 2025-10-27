#include "deviceclass.hh"
#include "pattern.hh"

#include "definition.hh"
#include "device.hh"
#include "parser.hh"


AnytoneDeviceClassPlugin::AnytoneDeviceClassPlugin(QObject *parent)
  : QObject{parent}, DeviceClassPluginInterface()
{
  // pass...
}


ModelDefinition *
AnytoneDeviceClassPlugin::modelDefinition(const QString &id, QObject *parent, const ErrorStack &err) {
  return new AnyToneModelDefinition(id, parent);
}

Device *
AnytoneDeviceClassPlugin::device(QIODevice *interface, const ModelFirmwareDefinition *firmware,
                                 ImageCollector *handler, QObject *parent, const ErrorStack &err) {
  auto fw = qobject_cast<const AnyToneModelFirmwareDefinition*>(firmware);
  if (nullptr == fw) {
    errMsg(err) << "Expected instance of " << AnyToneModelFirmwareDefinition::staticMetaObject.className()
                << " got " << firmware->metaObject()->className() << ".";
    errMsg(err) << "Canont create device, invalid firmware definition given.";
    return nullptr;
  }

  CodeplugPattern *codeplug = CodeplugPattern::load(fw->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << fw->codeplug() << "'.";
    return nullptr;
  }

  return new AnyToneDevice(interface, codeplug, handler,
                           fw->modelId(), fw->band(), fw->revision(), parent);
}


ModelDefinitionHandler *
AnytoneDeviceClassPlugin::definitionHandler(const QString &id, ModelDefinitionParser *parent) {
  return new AnyToneModelDefinitionHandler(id, parent);
}


ModelFirmwareDefinitionHandler *
AnytoneDeviceClassPlugin::firmwareDefinitionHandler(const QString &name, const QString &codeplug,
                                                    const QDate &released, ModelDefinitionHandler *parent) {
  return new AnyToneModelFirmwareDefinitionHandler(name, codeplug, released, parent);
}
