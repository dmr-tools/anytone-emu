#ifndef MD32UVPROTOCOL_HH
#define MD32UVPROTOCOL_HH


#include "genericdevice.hh"
#include "errorstack.hh"


/** Base class of all MD-32UV requests. */
class MD32UVRequest: public GenericRequest
{
protected:
  /** Hidden constructor. */
  MD32UVRequest();

public:
  /** Decodes the given request. */
  static MD32UVRequest *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err = ErrorStack());
};



class MD32UVPingRequest: public MD32UVRequest
{
public:
  MD32UVPingRequest();
};


class MD32UVACK: public GenericResponse
{
public:
  MD32UVACK();

  bool serialize(QByteArray &buffer) override;
};


/** Device detection request. */
class MD32UVSearchRequest: public MD32UVRequest
{
public:
  MD32UVSearchRequest();
};


/** Device identification response. */
class MD32UVSearchResponse: public GenericResponse
{
public:
  MD32UVSearchResponse(const QByteArray &model);

  bool serialize(QByteArray &buffer) override;

protected:
  QByteArray _model;
};



/** Password request. */
class MD32UVPasswordRequest: public MD32UVRequest
{
public:
  MD32UVPasswordRequest();
};


/** Password response. This response indicates,
 *  that there is no programming password.*/
class MD32UVPasswordResponse: public GenericResponse
{
public:
  MD32UVPasswordResponse();

  bool serialize(QByteArray &buffer) override;
};



/** Start system info request. */
class MD32UVStartSystemInfoRequest: public MD32UVRequest
{
public:
  MD32UVStartSystemInfoRequest();
};



class MD32UVValueRequest: public MD32UVRequest
{
public:
  MD32UVValueRequest(uint16_t flags, uint8_t len, uint8_t field);

  uint16_t flags() const;
  uint8_t length() const;
  uint8_t field() const;

protected:
  uint16_t _flags;
  uint8_t  _length;
  uint8_t  _field;
};


class MD32UVValueResponse: public GenericResponse
{
public:
  MD32UVValueResponse(uint8_t field, const QByteArray &payload);

  bool serialize(QByteArray &buffer);

protected:
  uint8_t _field;
  QByteArray _payload;
};



class MD32UVReadInfoRequest: public MD32UVRequest
{
public:
  MD32UVReadInfoRequest(uint32_t address, uint16_t len);

  uint32_t address() const;
  uint16_t length() const;

protected:
  uint32_t  _address;
  uint16_t _length;
};


class MD32UVReadInfoResponse: public GenericResponse
{
public:
  MD32UVReadInfoResponse(uint32_t address, const QByteArray &payload);

  bool serialize(QByteArray &buffer);

protected:
  uint32_t   _address;
  QByteArray _payload;
};



/** Start program request. */
class MD32UVStartProgramRequest: public MD32UVRequest
{
public:
  MD32UVStartProgramRequest();
};



class MD32UVReadRequest: public MD32UVRequest
{
public:
  MD32UVReadRequest(uint32_t address, uint16_t len);

  uint32_t address() const;
  uint16_t length() const;

protected:
  uint32_t _address;
  uint16_t _length;
};


class MD32UVReadResponse: public GenericResponse
{
public:
  MD32UVReadResponse(uint32_t address, const QByteArray &payload);

  bool serialize(QByteArray &buffer);

protected:
  uint32_t   _address;
  QByteArray _payload;
};



class MD32UVWriteRequest: public MD32UVRequest
{
public:
  MD32UVWriteRequest(uint8_t flags, uint16_t field, const QByteArray &payload);

  uint8_t  flags() const;
  uint16_t field() const;
  const QByteArray &payload() const;

protected:
  uint8_t  _flags;
  uint16_t _field;
  QByteArray _payload;
};



#endif // MD32UVPROTOCOL_HH
