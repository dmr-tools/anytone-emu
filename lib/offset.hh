#ifndef OFFSET_HH
#define OFFSET_HH

#include <QString>
class Address;
class Offset;
class Size;


struct Address
{
protected:
  Address(unsigned long bits);

public:
  Address();

  inline Address(const Address &other): _value(other._value) {}
  inline Address &operator= (const Address &other) { _value = other._value; return *this; }

  bool isValid() const;
  bool byteAligned() const;

  static Address zero();
  static Address fromByte(unsigned int n, unsigned bit=7);
  static Address fromString(const QString &str);

  unsigned int byte() const;
  unsigned int bit() const;

  QString toString() const;

  inline bool operator==(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value == other._value) : false;
  }
  inline bool operator!=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value != other._value) : false;
  }
  inline bool operator<=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value <= other._value) : false;
  }
  inline bool operator<(const Address &other) const  {
    return (isValid() && other.isValid()) ? (_value < other._value) : false;
  }
  inline bool operator>(const Address &other) const  {
    return (isValid() && other.isValid()) ? (_value > other._value) : false;
  }
  inline bool operator>=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value >= other._value) : false;
  }

  Address &operator+=(const Offset &rhs);
  Address operator+(const Offset &rhs) const;
  Address &operator-=(const Offset &rhs);
  Address operator-(const Offset &rhs) const;
  Offset  operator-(const Address &rhs) const;

protected:
  uint64_t _value;
};


struct Offset
{
protected:
  Offset(int64_t bits);

public:
  Offset();
  Offset(const Size &other);

  inline Offset(const Offset &other): _value(other._value) {}
  inline Offset &operator= (const Offset &other) { _value = other._value; return *this; }

  bool isValid() const;

  static Offset zero();
  static Offset fromByte(unsigned int n, unsigned bit=0);
  static Offset fromBits(unsigned long n);
  static Offset fromString(const QString &str);

  QString toString() const;

  inline unsigned int byte() const { return _value/8; }
  inline unsigned int bit() const { return _value%8; }
  inline int64_t bits() const { return _value; }

  inline bool operator==(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value == other._value) : false;
  }
  inline bool operator!=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value != other._value) : false;
  }
  inline bool operator<=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value <= other._value) : false;
  }
  inline bool operator<(const Offset &other) const  {
    return (isValid() && other.isValid()) ? (_value < other._value) : false;
  }
  inline bool operator>(const Offset &other) const  {
    return (isValid() && other.isValid()) ? (_value > other._value) : false;
  }
  inline bool operator>=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value >= other._value) : false;
  }

  inline Offset &operator+=(const Offset &rhs) { _value += rhs._value; return *this; }
  inline Offset operator+(const Offset &rhs) const { return Offset(_value + rhs._value); }
  inline Offset &operator-=(unsigned int rhs) {
    _value = (rhs*8>_value) ? std::numeric_limits<unsigned long>::max() : (_value-(rhs*8));
    return *this;
  }
  inline Offset operator-(unsigned int rhs) const {
    return (rhs*8>_value) ? Offset() : Offset(_value - rhs*8);
  }

  inline Offset &operator*=(unsigned int n) { _value *= n; return *this; }
  inline Offset operator*(unsigned int n) const { return Offset(_value * n); }

protected:
  int64_t _value;
};


struct Size
{
protected:
  Size(unsigned long bits);

public:
  Size();
  Size(const Offset &other);

  inline Size(const Size &other): _value(other._value) {}

  bool isValid() const;

  static Size zero();
  static Size fromByte(unsigned int n, unsigned bit=0);
  static Size fromBits(unsigned long n);
  static Size fromString(const QString &str);

  QString toString() const;

  inline unsigned int byte() const { return _value/8; }
  inline unsigned int bit() const { return _value%8; }
  inline uint64_t bits() const { return _value; }

  inline Size &operator= (const Size &other) { _value = other._value; return *this; }
  inline bool operator==(const Size &other) const { return _value == other._value; }
  inline bool operator!=(const Size &other) const { return _value != other._value; }
  inline bool operator<=(const Size &other) const { return _value <= other._value; }
  inline bool operator<(const Size &other) const  { return _value <  other._value; }
  inline bool operator>(const Size &other) const  { return _value >  other._value; }
  inline bool operator>=(const Size &other) const { return _value >= other._value; }

  inline Size &operator+=(const Size &rhs) { _value += rhs._value; return *this; }
  inline Size operator+(const Size &rhs) const { return Size(_value + rhs._value); }
  Size &operator-=(const Size &rhs);
  Size operator-(const Size &rhs) const;

  inline Size &operator*=(unsigned int n) { _value *= n; return *this; }
  inline Size operator*(unsigned int n) const { return Size(_value * n); }

protected:
  uint64_t _value;
};

#endif // OFFSET_HH
