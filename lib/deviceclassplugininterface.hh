#ifndef DEVICECLASSPLUGININTERFACE_HH
#define DEVICECLASSPLUGININTERFACE_HH

#include <QString>
#include <QtPlugin>

class ModelDefinitionParser;
class ModelDefinitionHandler;
class ModelFirmwareDefinitionHandler;
class ModelDefinition;
class CodeplugPattern;
class ImageCollector;
class Device;


class DeviceClassPluginInterface
{
public:
  virtual ~DeviceClassPluginInterface();

  virtual const QString &deviceClass() = 0;
  virtual ModelDefinition *modelDefinition(const QString &id, QObject *parent) = 0;
  virtual Device *device(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                         QObject *parent = nullptr) = 0;

  /** By default, returns a GenericModelDefinitionHandler for this plugin.
   * That is, it does not handle additional elements and calls @c firmwareDefinitionHandler
   * to construct firmware definition parsers. */
  virtual ModelDefinitionHandler *definitionHandler(
      const QString &context, const QString& id, ModelDefinitionParser *parent);

  /** By default, returns a GenericModelFirmwareDefinitionHandler for this plugin. */
  virtual ModelFirmwareDefinitionHandler *firmwareDefinitionHandler(
      const QString& context, const QString &name, const QDate &released, const QString &codeplug,
      ModelDefinitionHandler *parent);

};


Q_DECLARE_INTERFACE(DeviceClassPluginInterface, "io.github.dmr-tools.anytone-emu.DeviceClassPluginInterface/1.0")

#endif // DEVICECLASSPLUGININTERFACE_HH
