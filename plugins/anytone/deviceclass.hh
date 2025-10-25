#ifndef ANYTONE_DEVICECLASS_HH
#define ANYTONE_DEVICECLASS_HH

#include <QObject>
#include "deviceclassplugininterface.hh"


class AnytoneDeviceClassPlugin: public QObject, public DeviceClassPluginInterface
{
  Q_OBJECT

  Q_CLASSINFO("deviceClass", "AnyTone")

  Q_PLUGIN_METADATA(IID "io.github.dmr-tools.anytone-emu.DeviceClassPluginInterface/1.0"
                    FILE "deviceclass.json")

  Q_INTERFACES(DeviceClassPluginInterface)


public:
  Q_INVOKABLE explicit AnytoneDeviceClassPlugin(QObject *parent = nullptr);

  ModelDefinition *modelDefinition(const QString &id, QObject *parent, const ErrorStack &err=ErrorStack()) override;
  Device *device(QIODevice *interface, const ModelFirmwareDefinition *firmware, ImageCollector *handler,
                 QObject *parent = nullptr, const ErrorStack &err=ErrorStack()) override;

  ModelDefinitionHandler *definitionHandler(
      const QString& id, ModelDefinitionParser *parent) override;

  ModelFirmwareDefinitionHandler *firmwareDefinitionHandler(
      const QString &name, const QString &codeplug, const QDate &released,
      ModelDefinitionHandler *parent) override;
};


#endif // ANYTONE_DEVICECLASS_HH
