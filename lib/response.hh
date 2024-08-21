#ifndef RESPONSE_HH
#define RESPONSE_HH

#include <QByteArray>


/** Base class of all AnyTone device responses.
 * @ingroup interface */
class Response
{
protected:
  /** Hidden constructor. */
  Response();

public:
  /** Destructor. */
  virtual ~Response();

  /** Serializes the response into the given buffer */
  virtual bool serialize(QByteArray &buffer) = 0;
};


/** Response to a program request (ACK). */
class ProgramResponse: public Response
{
public:
  /** Constructor. */
  ProgramResponse();

  bool serialize(QByteArray &buffer);
};


/** Response to a device info request. Contains the model ID and HW version. */
class DeviceInfoResponse: public Response
{
public:
  /** Constructor from model ID and hardware version number. */
  DeviceInfoResponse(const QByteArray &model, const QByteArray &hwVersion);

  bool serialize(QByteArray &buffer);

protected:
  /** The model ID. */
  QByteArray _model;
  /** The hardware verison number. */
  QByteArray _hwVersion;
};


/** Response to a read reqeust. Contains the data read. */
class ReadResponse: public Response
{
public:
  /** Contructor from address and payload. */
  ReadResponse(uint32_t addr, const QByteArray payload);

  bool serialize(QByteArray &buffer);

protected:
  /** Holds the address. */
  uint32_t   _address;
  /** Holds the payload. */
  QByteArray _payload;
};


/** Response to a write request. Contains the address and amount written. */
class WriteResponse: public Response
{
public:
  /** Constructor from address and length. */
  WriteResponse(uint32_t addr, uint8_t len);

  bool serialize(QByteArray &buffer);

protected:
  /** Holds the address. */
  uint32_t _address;
  /** Holds the amount of data written. */
  uint8_t  _length;
};


#endif // RESPONSE_HH
