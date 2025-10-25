#include "deviceclassplugininterface.hh"

#include "modeldefinition.hh"
#include "modelparser.hh"



/* ********************************************************************************************* *
 * Implementation of device class plugin interface
 * ********************************************************************************************* */
DeviceClassPluginInterface::~DeviceClassPluginInterface() {
  // pass...
}

ModelDefinition *
DeviceClassPluginInterface::modelDefinition(const QString &id, QObject *parent, const ErrorStack &err) {
  return new ModelDefinition(id, parent);
}


ModelDefinitionHandler *
DeviceClassPluginInterface::definitionHandler(const QString &id, ModelDefinitionParser *parent) {
  return new GenericModelDefinitionHandler(this, id, parent);
}


ModelFirmwareDefinitionHandler *
DeviceClassPluginInterface::firmwareDefinitionHandler(const QString &name, const QString &codeplug,
                                                      const QDate &released, ModelDefinitionHandler *parent) {
  return new GenericModelFirmwareDefinitionHandler(this, name, codeplug, released, parent);
}
