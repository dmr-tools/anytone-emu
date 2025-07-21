#ifndef RADTELPROTOCOL_HH
#define RADTELPROTOCOL_HH

#include "errorstack.hh"


/** Base class of all requests. */
class RadtelRequest
{
protected:
  /** Hidden constructor. */
  RadtelRequest();

public:
  /** Destructor. */
  virtual ~RadtelRequest();

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
  static RadtelRequest *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err = ErrorStack());
};



/** Base class of all Radtel device responses.
 * @ingroup interface */
class RadtelResponse
{
protected:
  /** Hidden constructor. */
  RadtelResponse();

public:
  /** Destructor. */
  virtual ~RadtelResponse();

  /** Serializes the response into the given buffer */
  virtual bool serialize(QByteArray &buffer) = 0;
};



class RadtelCommandRequest: public RadtelRequest
{
public:
  enum Command {
    EnterProgrammingMode, LeaveProgrammingMode
  };

public:
  RadtelCommandRequest(Command command);

  Command command() const;

protected:
  Command _command;
};



class RadtelReadRequest: public RadtelRequest
{
public:
  RadtelReadRequest(uint16_t page);

  uint16_t page() const;
  uint32_t address() const;

protected:
  uint16_t _page;
};



class RadtelWriteRequest: public RadtelRequest
{
public:
  RadtelWriteRequest(uint8_t segment, uint16_t page, const QByteArray &payload);

public:
  uint8_t segment() const;
  uint16_t page() const;
  uint32_t address() const;
  const QByteArray &payload() const;

protected:
  uint8_t _segment;
  uint16_t _page;
  QByteArray _payload;
};



class RadtelACK: public RadtelResponse
{
public:
  RadtelACK();

  bool serialize(QByteArray &buffer) override;
};



class RadtelReadResonse: public RadtelResponse
{
public:
  RadtelReadResonse(uint16_t page, const QByteArray &payload);

  bool serialize(QByteArray &buffer) override;

protected:
  uint16_t _page;
  QByteArray _payload;
};



#endif // RADTELPROTOCOL_HH
