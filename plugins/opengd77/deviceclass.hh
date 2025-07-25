#ifndef OPENGD77_DEVICECLASS_HH
#define OPENGD77_DEVICECLASS_HH

#include <QObject>
#include "deviceclassplugininterface.hh"


class OpenGD77DeviceClassPlugin: public QObject, public DeviceClassPluginInterface
{
  Q_OBJECT

  Q_CLASSINFO("deviceClass", "OpenGD77")

  Q_PLUGIN_METADATA(IID "io.github.dmr-tools.anytone-emu.DeviceClassPluginInterface/1.0"
                    FILE "deviceclass.json")

  Q_INTERFACES(DeviceClassPluginInterface)


public:
  Q_INVOKABLE explicit OpenGD77DeviceClassPlugin(QObject *parent = nullptr);

  ModelDefinition *modelDefinition(const QString &id, QObject *parent, const ErrorStack &err=ErrorStack()) override;
  Device *device(QIODevice *interface, const ModelFirmwareDefinition *firmware, ImageCollector *handler,
                 QObject *parent = nullptr, const ErrorStack &err=ErrorStack()) override;

};


#endif // OPENGD77_DEVICECLASS_HH
