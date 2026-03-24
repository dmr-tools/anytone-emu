#ifndef MD32UVDEVICE_HH
#define MD32UVDEVICE_HH

#include <QTimer>
#include "genericdevice.hh"
#include "definition.hh"

class MD32UVRequest;
class MD32UVResponse;


class MD32UVDevice : public GenericDevice
{
  Q_OBJECT

protected:
  enum class Value {
    FirmwareVersion = 1, Unknown02h = 2, BuildDate = 3, DSPVersion = 4, RadioVersion = 5,
    AudioResourceAddress = 6, CompactItemTable = 7, ZonesAddress = 8, EmergencyAddress = 9,
    MainConfigAddress = 10, CodeplugVersion = 11, Capabilities = 13, ListsAddress = 14,
    ContactsAddress = 15, ContactCount = 16
  };

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit MD32UVDevice(QIODevice *interface, const DM32UVFirmwareProperties &properties,
                        CodeplugPattern *pattern, ImageCollector *handler,
                        QObject *parent = nullptr);

protected:
  GenericRequest *parse(QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack()) override;
  GenericResponse *handle(GenericRequest *request) override;

  virtual bool getValue(uint8_t field, uint8_t length, QByteArray &payload);
  virtual bool readInfo(uint32_t address, uint16_t length, QByteArray &payload);

private:
  QTimer _timer;
  DM32UVFirmwareProperties _properties;

protected:
  /** Some pre-defined addresses, defining the memory layout of the device. */
  struct Address{
    static constexpr unsigned int channelBank() { return 0x0000A000; }
    static constexpr unsigned int firmwareVersion() { return 0x80000000; }
    static constexpr unsigned int buildDate()       { return 0x80000010; }
    static constexpr unsigned int dspVersion()      { return 0x80000020; }
    static constexpr unsigned int radioVersion()    { return 0x80000030; }
    static constexpr unsigned int codeplugVersion() { return 0x80000040; }
  };

  /** Some limits. */
  struct Limit {
    static constexpr unsigned int numChannels() { return 4080;}
    static constexpr unsigned int numChannelsPerBlock() { return 85;}
    static constexpr unsigned int numChannelBlocks() { return numChannels()/numChannelsPerBlock(); }
  };
};


#endif // MD32UVDEVICE_HH
