#ifndef MD32UV_DEVICECLASS_HH
#define MD32UV_DEVICECLASS_HH

#include <QObject>
#include "deviceclassplugininterface.hh"


class MD32UVDeviceClassPlugin: public QObject, public DeviceClassPluginInterface
{
  Q_OBJECT

  Q_CLASSINFO("deviceClass", "MD32UV")

  Q_PLUGIN_METADATA(IID "io.github.dmr-tools.anytone-emu.DeviceClassPluginInterface/1.0"
                    FILE "deviceclass.json")

  Q_INTERFACES(DeviceClassPluginInterface)


public:
  Q_INVOKABLE explicit MD32UVDeviceClassPlugin(QObject *parent = nullptr);

  Device *device(QIODevice *interface, const ModelFirmwareDefinition *firmware, ImageCollector *handler,
                 QObject *parent = nullptr, const ErrorStack &err=ErrorStack()) override;

};


#endif // MD32UV_DEVICECLASS_HH
