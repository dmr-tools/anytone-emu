#ifndef OFFSET_HH
#define OFFSET_HH

#include <QString>

struct Offset
{
protected:
  Offset(unsigned long bits);

public:
  Offset();

  inline Offset(const Offset &other): _value(other._value) {}

  bool isValid() const;

  static Offset zero();
  static Offset fromByte(unsigned int n, unsigned bit=0);
  static Offset fromBits(unsigned long n);
  static Offset fromString(const QString &str);

  inline unsigned int byte() const { return _value/8; }
  inline unsigned int bit() const { return _value%8; }
  inline unsigned int bits() const { return _value; }

  inline Offset &operator= (const Offset &other) { _value = other._value; return *this; }
  inline bool operator==(const Offset &other) const { return _value == other._value; }
  inline bool operator!=(const Offset &other) const { return _value != other._value; }
  inline bool operator<=(const Offset &other) const { return _value <= other._value; }
  inline bool operator<(const Offset &other) const  { return _value <  other._value; }
  inline bool operator>(const Offset &other) const  { return _value >  other._value; }
  inline bool operator>=(const Offset &other) const { return _value >= other._value; }

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
  unsigned long _value;
};


#endif // OFFSET_HH
