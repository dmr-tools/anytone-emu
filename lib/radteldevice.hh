#ifndef RADTELDEVICE_HH
#define RADTELDEVICE_HH

#include "device.hh"

class RadtelRequest;
class RadtelResponse;


class RadtelDevice : public Device
{
  Q_OBJECT

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit RadtelDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                        QObject *parent = nullptr);

protected:
  /** Handles a request and constructs an appropriate response. */
  virtual RadtelResponse *handle(RadtelRequest *request);

protected slots:
  /** Internal callback to handle incomming data. */
  void onBytesAvailable();
  /** Internal callback to handle data being written. */
  void onBytesWritten();

protected:
  QIODevice *_interface;
  QByteArray _in_buffer;
  QByteArray _out_buffer;
};

#endif // RADTELDEVICE_HH
