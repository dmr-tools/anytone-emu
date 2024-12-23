#ifndef ANYTONEDEVICE_HH
#define ANYTONEDEVICE_HH

#include "device.hh"

class AnytoneRequest;
class AnytoneResponse;

/** Abstract base class for all emulated devices.
 * @ingroup device */
class AnyToneDevice : public Device
{
  Q_OBJECT

protected:
  /** Possible device states. */
  enum class State {
    Initial, ///< Initial state, active.
    Program, ///< Programming state.
    Error    ///< Captive error state.
  };

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit AnyToneDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                         const QByteArray &model, const QByteArray &revision,
                         QObject *parent = nullptr);

  /** Returns the model code for this device. */
  const QByteArray &model() const;
  /** Returns the revision code for this device. */
  const QByteArray &revision() const;

protected:
  /** Handles a request and constructs an appropriate response. */
  virtual AnytoneResponse *handle(AnytoneRequest *request);

protected slots:
  /** Internal callback to handle incomming data. */
  void onBytesAvailable();
  /** Internal callback to handle data being written. */
  void onBytesWritten();

protected:
  /** The device state. */
  State _state;
  /** Holds and owns the interface. */
  QIODevice *_interface;

  /** Internal receive buffer. */
  QByteArray _in_buffer;
  /** Internal transmit buffer. */
  QByteArray _out_buffer;

  QByteArray _model;
  QByteArray _revision;
};


#endif // ANYTONEDEVICE_HH
