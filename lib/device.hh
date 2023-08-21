#ifndef DEVICE_HH
#define DEVICE_HH

#include <QObject>
#include <QHash>
#include <QPair>

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

  virtual bool elementKnown(uint32_t address) const;
  virtual QString elementName(uint32_t address) const;
  virtual QString elementDescription(uint32_t address) const;

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

  /** Lookuptable for element descriptions. */
  QHash<uint32_t, QPair<QString, QString>> _elementDescriptions;
};

#endif // DEVICE_HH
