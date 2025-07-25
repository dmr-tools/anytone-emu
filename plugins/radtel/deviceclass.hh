#ifndef RADTEL_DEVICECLASS_HH
#define RADTEL_DEVICECLASS_HH

#include <QObject>
#include "deviceclassplugininterface.hh"


class RadtelDeviceClassPlugin: public QObject, public DeviceClassPluginInterface
{
  Q_OBJECT

  Q_CLASSINFO("deviceClass", "Radtel")

  Q_PLUGIN_METADATA(IID "io.github.dmr-tools.anytone-emu.DeviceClassPluginInterface/1.0"
                    FILE "deviceclass.json")

  Q_INTERFACES(DeviceClassPluginInterface)


public:
  Q_INVOKABLE explicit RadtelDeviceClassPlugin(QObject *parent = nullptr);

  Device *device(QIODevice *interface, const ModelFirmwareDefinition *firmware, ImageCollector *handler,
                 QObject *parent = nullptr, const ErrorStack &err=ErrorStack()) override;

};


#endif // RADTEL_DEVICECLASS_HH
