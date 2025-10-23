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
class AnytoneRequest
{
protected:
  /** Hidden default constructor. */
  AnytoneRequest();

public:
  /** Destructor. */
  virtual ~AnytoneRequest();

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
  static AnytoneRequest *fromBuffer(QByteArray &buffer);
};


/** Represents a program request. */
class AnytoneProgramRequest: public AnytoneRequest
{
public:
  /** Default constructor. */
  AnytoneProgramRequest();
};


/** Represents an end-program request. */
class AnytoneEndRequest: public AnytoneRequest
{
public:
  /** Default constructor. */
  AnytoneEndRequest();
};


/** Represents a device-info request. */
class AnytoneDeviceInfoRequest: public AnytoneRequest
{
public:
  /** Default constructor. */
  AnytoneDeviceInfoRequest();
};


/** Represents a read request. */
class AnytoneReadRequest: public AnytoneRequest
{
public:
  /** Default from address and length to read. */
  AnytoneReadRequest(uint32_t addr, uint8_t len);

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
class AnytoneWriteRequest: public AnytoneRequest
{
public:
  /** Constructor to write @c payload to address @c addr. */
  AnytoneWriteRequest(uint32_t addr, const QByteArray &payload);

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



/** Base class of all AnyTone device responses.
 * @ingroup interface */
class AnytoneResponse
{
protected:
  /** Hidden constructor. */
  AnytoneResponse();

public:
  /** Destructor. */
  virtual ~AnytoneResponse();

  /** Serializes the response into the given buffer */
  virtual bool serialize(QByteArray &buffer) = 0;
};


/** Response to a program request (ACK). */
class AnytoneProgramResponse: public AnytoneResponse
{
public:
  /** Constructor. */
  AnytoneProgramResponse();

  bool serialize(QByteArray &buffer);
};


/** Response to a device info request. Contains the model ID and HW version. */
class AnytoneDeviceInfoResponse: public AnytoneResponse
{
public:
  /** Constructor from model ID and hardware version number. */
  AnytoneDeviceInfoResponse(const QByteArray &model, uint8_t band, const QByteArray &hwVersion);

  bool serialize(QByteArray &buffer);

protected:
  /** The model ID. */
  QByteArray _model;
  /** The band enum value. */
  uint8_t _band;
  /** The hardware verison number. */
  QByteArray _hwVersion;
};


/** Response to a read reqeust. Contains the data read. */
class AnytoneReadResponse: public AnytoneResponse
{
public:
  /** Contructor from address and payload. */
  AnytoneReadResponse(uint32_t addr, const QByteArray payload);

  bool serialize(QByteArray &buffer);

protected:
  /** Holds the address. */
  uint32_t   _address;
  /** Holds the payload. */
  QByteArray _payload;
};


/** Response to a write request. Contains the address and amount written. */
class AnytoneWriteResponse: public AnytoneResponse
{
public:
  /** Constructor from address and length. */
  AnytoneWriteResponse(uint32_t addr, uint8_t len);

  bool serialize(QByteArray &buffer);

protected:
  /** Holds the address. */
  uint32_t _address;
  /** Holds the amount of data written. */
  uint8_t  _length;
};


#endif // REQUEST_HH
