#ifndef REQUEST_HH
#define REQUEST_HH

#include <QByteArray>


class Request
{
protected:
  Request();

public:
  virtual ~Request();

  template<class T>
  bool is() const {
    return nullptr != dynamic_cast<const T*>(this);
  }

  template<class T>
  const T* as() const {
    return dynamic_cast<const T*>(this);
  }

  template<class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

public:
  static Request *fromBuffer(QByteArray &buffer);
};


class ProgramRequest: public Request
{
public:
  ProgramRequest();
};


class EndRequest: public Request
{
public:
  EndRequest();
};


class DeviceInfoRequest: public Request
{
public:
  DeviceInfoRequest();
};


class ReadRequest: public Request
{
public:
  ReadRequest(uint32_t addr, uint8_t len);

  inline uint32_t address() const { return _addr; }
  inline uint8_t length() const { return _len; }

protected:
  uint32_t _addr;
  uint8_t  _len;
};


class WriteRequest: public Request
{
public:
  WriteRequest(uint32_t addr, const QByteArray &payload);

  inline uint32_t address() const { return _addr; }
  inline const QByteArray &payload() const { return _payload; }

protected:
  uint32_t   _addr;
  QByteArray _payload;
};


#endif // REQUEST_HH
