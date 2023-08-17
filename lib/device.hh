#ifndef DEVICE_HH
#define DEVICE_HH

#include <QObject>

class QIODevice;
class Request;
class Response;
class Model;


/** Abstract base class for all emulated devices. */
class Device : public QObject
{
  Q_OBJECT

protected:
  enum class State {
    Initial, Program, Error
  };

public:
  explicit Device(QIODevice *interface, Model *model =nullptr, QObject *parent = nullptr);

  virtual QByteArray model() const = 0;
  virtual QByteArray hwVersion() const = 0;

  virtual bool read(uint32_t addr, uint8_t len, QByteArray &payload);
  virtual bool write(uint32_t addr, const QByteArray &data);

signals:
  void startProgram();
  void endProgram();

protected:
  virtual Response *handle(Request *request);

protected slots:
  void onBytesAvailable();
  void onBytesWritten();

protected:
  State _state;
  QIODevice *_interface;
  Model *_model;

  /** Internal receive buffer. */
  QByteArray _in_buffer;
  /** Internal transmit buffer. */
  QByteArray _out_buffer;
};

#endif // DEVICE_HH
