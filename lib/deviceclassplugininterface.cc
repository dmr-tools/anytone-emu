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
DeviceClassPluginInterface::definitionHandler(const QString &context, const QString &id,
                                              ModelDefinitionParser *parent) {
  return new GenericModelDefinitionHandler(this, context, id, parent);
}


ModelFirmwareDefinitionHandler *
DeviceClassPluginInterface::firmwareDefinitionHandler(const QString &context, const QString &name,
                                                      const QDate &released, const QString &codeplug,
                                                      ModelDefinitionHandler *parent) {
  return new GenericModelFirmwareDefinitionHandler(this, context, name, released, codeplug, parent);
}
