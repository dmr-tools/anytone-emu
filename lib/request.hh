/** @defgroup interface Interface to the manufacturer CPS.
 * @ingroup device
 *
 * This module collects classes to interface the manufacturer CPS and communicate with it.
 * Obviously, this is part of the device emulation but is shared between all devices of a single
 * manufactuert or family of devices. */
#ifndef REQUEST_HH
#define REQUEST_HH

#include <QByteArray>


/** Baseclass of a request to an AnyTone device.
 * @ingroup interface */
class Request
{
protected:
  /** Hidden default constructor. */
  Request();

public:
  /** Destructor. */
  virtual ~Request();

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

public:
  /** Decodes the given request. */
  static Request *fromBuffer(QByteArray &buffer);
};


/** Represents a program request. */
class ProgramRequest: public Request
{
public:
  /** Default constructor. */
  ProgramRequest();
};


/** Represents an end-program request. */
class EndRequest: public Request
{
public:
  /** Default constructor. */
  EndRequest();
};


/** Represents a device-info request. */
class DeviceInfoRequest: public Request
{
public:
  /** Default constructor. */
  DeviceInfoRequest();
};


/** Represents a read request. */
class ReadRequest: public Request
{
public:
  /** Default from address and length to read. */
  ReadRequest(uint32_t addr, uint8_t len);

  /** Returns the address to read from. */
  inline uint32_t address() const { return _addr; }
  /** Returns the amount to read. */
  inline uint8_t length() const { return _len; }

protected:
  /** The address to read from. */
  uint32_t _addr;
  /** The amount to read. */
  uint8_t  _len;
};


/** Represents a write request. */
class WriteRequest: public Request
{
public:
  /** Constructor to write @c payload to address @c addr. */
  WriteRequest(uint32_t addr, const QByteArray &payload);

  /** Returns the address to write to. */
  inline uint32_t address() const { return _addr; }
  /** Returns the payload to be written. */
  inline const QByteArray &payload() const { return _payload; }

protected:
  /** The address to write to. */
  uint32_t   _addr;
  /** The payload to be written. */
  QByteArray _payload;
};


#endif // REQUEST_HH
