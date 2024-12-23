#ifndef OPENGD77DEVICE_HH
#define OPENGD77DEVICE_HH

#include "device.hh"

class OpenGD77Request;
class OpenGD77Response;

class OpenGD77Device : public Device
{
  Q_OBJECT

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit OpenGD77Device(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                         QObject *parent = nullptr);

protected:
  /** Handles a request and constructs an appropriate response. */
  virtual OpenGD77Response *handle(OpenGD77Request *request);

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

#endif // OPENGD77DEVICE_HH
