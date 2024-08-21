/** @defgroup device Device emulation
 * This module collects all classes that implement the device emulation. This includes the interface
 * implementation. */

#ifndef DEVICE_HH
#define DEVICE_HH

#include <QObject>
#include <QHash>
#include <QPair>

class QIODevice;
class Request;
class Response;
class Model;
class CodeplugPattern;


/** Abstract base class for all emulated devices.
 * @ingroup device */
class Device : public QObject
{
  Q_OBJECT

protected:
  /** Possible device states. */
  enum class State {
    Initial, ///< Initial state, active.
    Program, ///< Programming state.
    Error    ///< Captive error state.
  };

protected:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit Device(QIODevice *interface, Model *model =nullptr, QObject *parent = nullptr);

public:
  /** Returns the model code for this device. */
  virtual QByteArray model() const = 0;
  /** Returns the hardware version code for this device. */
  virtual QByteArray hwVersion() const = 0;

  /** Reads some data from the device and stores it @c payload. */
  virtual bool read(uint32_t addr, uint8_t len, QByteArray &payload);
  /** Write some data. */
  virtual bool write(uint32_t addr, const QByteArray &data);

  /** Returns the pattern associated with this device. */
  CodeplugPattern *pattern() const;

signals:
  /** Gets emitted once the programming started. */
  void startProgram();
  /** Gets emitted once the programming ended. */
  void endProgram();

protected:
  /** Handles a request and constructs an appropriate response. */
  virtual Response *handle(Request *request);

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
  /** Holds and owns the model. */
  Model *_model;

  /** Internal receive buffer. */
  QByteArray _in_buffer;
  /** Internal transmit buffer. */
  QByteArray _out_buffer;
};

#endif // DEVICE_HH
