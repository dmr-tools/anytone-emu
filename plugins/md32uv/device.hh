#ifndef MD32UVDEVICE_HH
#define MD32UVDEVICE_HH

#include <QTimer>
#include "genericdevice.hh"

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
    ContactsAddress = 15, Unknown10h = 16
  };

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit MD32UVDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                        QObject *parent = nullptr);

protected:
  GenericRequest *parse(QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack()) override;
  GenericResponse *handle(GenericRequest *request) override;

  virtual bool getValue(uint8_t field, uint8_t length, QByteArray &payload);
  virtual bool readInfo(uint32_t address, uint16_t length, QByteArray &payload);

protected:
  static QByteArray packAddressRange(uint32_t from, uint32_t to);

private:
  QTimer _timer;

protected:
  /** Some pre-defined addresses, defining the memory layout of the device. */
  struct Address{
    static constexpr unsigned int channelBank() { return 0x00A000; }
  };

  /** Some limits. */
  struct Limit {
    static constexpr unsigned int numChannels() { return 4080;}
    static constexpr unsigned int numChannelsPerBlock() { return 85;}
    static constexpr unsigned int numChannelBlocks() { return numChannels()/numChannelsPerBlock(); }
  };
};


#endif // MD32UVDEVICE_HH
