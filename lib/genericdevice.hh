#ifndef GENERICDEVICE_H
#define GENERICDEVICE_H

#include <QByteArray>

#include "errorstack.hh"
#include "device.hh"



class GenericRequest
{
protected:
  GenericRequest();

public:
  virtual ~GenericRequest();

  /** Returns @c true if the request can be cast to the template argument. */
  template<class T>
  bool is() const {
    return nullptr != dynamic_cast<const T*>(this);
  }

  /** Casts this request to the template argument. */
  template<class T>
  const T* as() const {
    return dynamic_cast<const T*>(this);
  }

  /** Casts this request to the template argument. */
  template<class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }
};



class GenericResponse
{
protected:
  GenericResponse();

public:
  /** Destructor. */
  virtual ~GenericResponse();

  /** Serializes the response into the given buffer */
  virtual bool serialize(QByteArray &buffer) = 0;
};



class GenericDevice : public Device
{
  Q_OBJECT

public:
  /** Constructs a new device for the specifies interface using the given memory model.
   * Takes ownership of @c interface and @c model. */
  explicit GenericDevice(QIODevice *interface, CodeplugPattern *pattern, ImageCollector *handler,
                         QObject *parent = nullptr);

protected:
  /** Parses the given buffer and returns the parsed request. Also removes the consumed data from
   * the buffer. Retruns @c null, if not sufficient data is held in the buffer. If null is
   * returned and ok is false, the an error occured. */
  virtual GenericRequest *parse(QByteArray &buffer, bool &ok, const ErrorStack &errP=ErrorStack()) = 0;
  /** Handles a request and constructs an appropriate response. */
  virtual GenericResponse *handle(GenericRequest *request) = 0;

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


#endif // GENERICDEVICE_H
