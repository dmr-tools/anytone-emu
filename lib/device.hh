/** @defgroup device Device emulation
 * This module collects all classes that implement the device emulation. This includes the interface
 * implementation. */

#ifndef DEVICE_HH
#define DEVICE_HH

#include "modelrom.hh"
#include <QObject>
#include <QHash>
#include <QPair>

class Model;
class QIODevice;
class Request;
class Response;
class ImageCollector;
class CodeplugPattern;


class Device: public QObject
{
  Q_OBJECT

protected:
  explicit Device(CodeplugPattern *pattern, ImageCollector *handler, QObject *parent=nullptr);

public:
  /** Reads some data from the device and stores it @c payload. */
  virtual bool read(uint32_t addr, uint8_t len, QByteArray &payload);
  /** Write some data. */
  virtual bool write(uint32_t addr, const QByteArray &data);

  virtual ImageCollector *handler() const;
  void setHandler(ImageCollector *handler);

  /** Returns the pattern associated with this device. */
  virtual CodeplugPattern *pattern() const;

  const ModelRom &rom() const;
  ModelRom &rom();

signals:
  /** Gets emitted once the programming started. */
  void startProgram();
  /** Gets emitted once the programming ended. */
  void endProgram();

protected:
  CodeplugPattern *_pattern;
  ImageCollector *_handler;
  ModelRom _rom;
};



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

  /** Internal receive buffer. */
  QByteArray _in_buffer;
  /** Internal transmit buffer. */
  QByteArray _out_buffer;

  QByteArray _model;
  QByteArray _revision;
};

#endif // DEVICE_HH
