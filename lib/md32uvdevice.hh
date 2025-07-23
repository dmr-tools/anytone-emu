#ifndef MD32UVDEVICE_HH
#define MD32UVDEVICE_HH

#include "genericdevice.hh"

class MD32UVRequest;
class MD32UVResponse;


class MD32UVDevice : public GenericDevice
{
  Q_OBJECT

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
};


#endif // MD32UVDEVICE_HH
