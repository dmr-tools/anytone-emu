#ifndef RESPONSE_HH
#define RESPONSE_HH

#include <QByteArray>


class Response
{
protected:
  Response();

public:
  virtual ~Response();

  virtual bool serialize(QByteArray &buffer) = 0;
};


class ProgramResponse: public Response
{
public:
  ProgramResponse();

  bool serialize(QByteArray &buffer);
};


class DeviceInfoResponse: public Response
{
public:
  DeviceInfoResponse(const QByteArray &model, const QByteArray &hwVersion);

  bool serialize(QByteArray &buffer);

protected:
  QByteArray _model;
  QByteArray _hwVersion;
};


class ReadResponse: public Response
{
public:
  ReadResponse(uint32_t addr, const QByteArray payload);

  bool serialize(QByteArray &buffer);

protected:
  uint32_t   _address;
  QByteArray _payload;
};


class WriteResponse: public Response
{
public:
  WriteResponse(uint32_t addr, uint8_t len);

  bool serialize(QByteArray &buffer);

protected:
  uint32_t _address;
  uint8_t  _length;
};


#endif // RESPONSE_HH
