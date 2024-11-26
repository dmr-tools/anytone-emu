#ifndef OPENGD77PROTOCOL_HH
#define OPENGD77PROTOCOL_HH

#include <QByteArray>

#include "errorstack.hh"


class OpenGD77Request
{
protected:
  /** Hidden default constructor. */
  OpenGD77Request();

public:
  /** Destructor. */
  virtual ~OpenGD77Request();

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
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err = ErrorStack());
};



/** Base class of all OpenGD77 device responses.
 * @ingroup interface */
class OpenGD77Response
{
protected:
  /** Hidden constructor. */
  OpenGD77Response();

public:
  /** Destructor. */
  virtual ~OpenGD77Response();

  /** Serializes the response into the given buffer */
  virtual bool serialize(QByteArray &buffer) = 0;
};



class OpenGD77CommandRequest: public OpenGD77Request
{
public:
  /** Possible commands. */
  enum Command {
    SHOW_CPS_SCREEN  = 0,
    CLEAR_SCREEN     = 1,
    DISPLAY          = 2,
    RENDER_CPS       = 3,
    CLOSE_CPS_SCREEN = 5,
    COMMAND          = 6,
    PING             = 0xfe
  };

protected:
  OpenGD77CommandRequest();

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};


class OpenGD77PingRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77PingRequest();

public:
  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};


class OpenGD77ShowCPSScreenRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77ShowCPSScreenRequest();

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};


class OpenGD77ClearScreenRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77ClearScreenRequest();

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};


class OpenGD77DisplayRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77DisplayRequest(uint8_t x, uint8_t y, uint8_t font, uint8_t alignment, bool inverted,
                         const QByteArray &message);

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());

protected:
  uint8_t _x, _y;
  uint8_t _font;
  uint8_t _alignment;
  uint8_t _inverted;
  QByteArray _message;
};



class OpenGD77RenderScreenRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77RenderScreenRequest();

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};



class OpenGD77ResetScreenRequest: public OpenGD77CommandRequest
{
protected:
  OpenGD77ResetScreenRequest();

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());
};



class OpenGD77ControlRequest: public OpenGD77CommandRequest
{
public:
  /** Possible options. */
  enum Option {
    SAVE_SETTINGS_NOT_VFOS = 0,
    REBOOT = 1,
    SAVE_SETTINGS_AND_VFOS = 2,
    FLASH_GREEN_LED = 3,
    FLASH_RED_LED = 4
  };

protected:
  OpenGD77ControlRequest(Option option);

public:
  /** Decodes the given request. */
  static OpenGD77Request *fromBuffer(
      QByteArray &buffer, bool &ok, const ErrorStack &err=ErrorStack());

  Option option() const;
protected:
  Option _option;
};


class OpenGD77CommandResponse: public OpenGD77Response
{
public:
  OpenGD77CommandResponse(bool success);

  bool serialize(QByteArray &buffer);

protected:
  bool _success;
};


class OpenGD77ReadRequest: public OpenGD77Request
{
public:
  enum Section {
    READ_FLASH = 1,
    READ_EEPROM = 2,
    READ_MCU_ROM = 5,
    READ_DISPLAY_BUFFER = 6,
    READ_WAV_BUFFER = 7,
    READ_AMBE_BUFFER = 8,
    READ_FIRMWARE_INFO = 9
  };

protected:
  OpenGD77ReadRequest(Section sec, uint32_t address, uint16_t length);

public:
  Section section() const;
  uint32_t address() const;
  uint16_t length() const;

public:
  static constexpr unsigned int size() { return 0x0008; }

  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err = ErrorStack());

protected:
  Section _section;
  uint32_t _address;
  uint16_t _length;
};



class OpenGD77ReadResponse: public OpenGD77Response
{
public:
  OpenGD77ReadResponse(const QByteArray &data);

  bool serialize(QByteArray &buffer);

protected:
  QByteArray _data;
};


class OpenGD77WriteRequest: public OpenGD77Request
{
public:
  enum Section {
    SET_FLASH_SECTOR = 1,
    WRITE_SECTOR_BUFFER = 2,
    WRITE_FLASH_SECTOR = 3,
    WRITE_EEPROM = 4,
    WRITE_WAV_BUFFER = 7
  };

protected:
  OpenGD77WriteRequest(Section section);

public:
  Section section() const;

public:
  static constexpr unsigned int size() { return 0x0028; }

  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err = ErrorStack());

protected:
  Section _section;
};


class OpenGD77WriteResponse: public OpenGD77Response
{
public:
  OpenGD77WriteResponse(OpenGD77WriteRequest::Section section);

  bool serialize(QByteArray &buffer);

protected:
  OpenGD77WriteRequest::Section _section;
};


class OpenGD77SetSectorRequest: public OpenGD77WriteRequest
{
protected:
  OpenGD77SetSectorRequest(uint32_t sector);

public:
  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err);

protected:
  uint32_t _sector;
};


class OpenGD77WriteSectorRequest: public OpenGD77WriteRequest
{
protected:
  OpenGD77WriteSectorRequest();

public:
  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err);
};


class OpenGD77WriteDataRequest: public OpenGD77WriteRequest
{
protected:
  OpenGD77WriteDataRequest(Section section, uint32_t address, const QByteArray &data);

public:
  uint32_t address() const;
  const QByteArray &data() const;

public:
  static OpenGD77Request *fromBuffer(QByteArray &buffer, bool &ok, const ErrorStack &err);

protected:
  uint32_t _address;
  QByteArray _data;
};


#endif // OPENGD77PROTOCOL_HH
