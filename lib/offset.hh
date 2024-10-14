/** @defgroup address Addressing things.
 * Within codplugs, a slot of stuff needs to be addressed and sized in bit resolution. To this end,
 * it is worth to perfom these computations using separate classes. */
#ifndef OFFSET_HH
#define OFFSET_HH

#include <QString>
class Address;
class Offset;
class Size;


/** Represents an address with bit resolution.
 * @ingroup address */
struct Address
{
protected:
  /** Internal constructor from number of bits. */
  Address(uint64_t bits);

public:
  /** Default constructor, creates an invalid address. To create a valid address,
   * use one of the factory methods @c fromByte() or @c zero(). */
  Address();

  /** Copy constructor. */
  inline Address(const Address &other): _value(other._value) {}
  /** Copying assignment operator. */
  inline Address &operator= (const Address &other) { _value = other._value; return *this; }

  /** Returns @c true, if address is valid. */
  bool isValid() const;
  /** Returns @c true, if the address aligned with a byte boundary. */
  bool byteAligned() const;

  /** Constructs a zero address. */
  static Address zero();
  /** Constructs an address from the given byte and bit. */
  static Address fromByte(unsigned int n, unsigned bit=7);
  /** Parses the given address. */
  static Address fromString(const QString &str);

  /** Returns the byte-address. */
  unsigned int byte() const;
  /** Retunrs the bit offset. */
  unsigned int bit() const;
  /** Serializes the address. */
  QString toString() const;

  /** Comparison operator. */
  inline bool operator==(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value == other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator!=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value != other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator<=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value <= other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator<(const Address &other) const  {
    return (isValid() && other.isValid()) ? (_value < other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator>(const Address &other) const  {
    return (isValid() && other.isValid()) ? (_value > other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator>=(const Address &other) const {
    return (isValid() && other.isValid()) ? (_value >= other._value) : false;
  }

  /** Shifts the address by the specified offset. */
  Address &operator+=(const Offset &rhs);
  /** Shifts the address by the specified offset. */
  Address operator+(const Offset &rhs) const;
  /** Shifts the address by the specified offset. */
  Address &operator-=(const Offset &rhs);
  /** Shifts the address by the specified offset. */
  Address operator-(const Offset &rhs) const;
  /** Computes the offset between two addresses. */
  Offset  operator-(const Address &rhs) const;

protected:
  /** The address in bits. */
  uint64_t _value;

  friend class Offset;
};


/** Represents the offset between two addresses with bit resulution.
 * @ingroup address */
struct Offset
{
protected:
  /** Hidden constructor from bits. */
  Offset(int64_t bits);

public:
  /** Default constructor, creates an invalid offset. */
  Offset();
  /** Copy/cast constructor from size. */
  Offset(const Size &other);
  /** Copy/cast constructor from address. */
  Offset(const Address &other);
  /** Copy constructor. */
  inline Offset(const Offset &other): _value(other._value) {}
  /** Copying assignment operator. */
  inline Offset &operator= (const Offset &other) { _value = other._value; return *this; }

  /** Returns @c true if the offset has a valid value. */
  bool isValid() const;

  /** Factorry method to construct a zero offset. */
  static Offset zero();
  /** Factorry method to construct an offset from bytes and bits. */
  static Offset fromByte(unsigned int n, unsigned bit=0);
  /** Factorry method to construct an offset from bits. */
  static Offset fromBits(unsigned long n);
  /** Parses an offset. */
  static Offset fromString(const QString &str);

  /** Serializes the offset. */
  QString toString() const;

  /** Returns the number of bytes. */
  inline unsigned int byte() const { return _value/8; }
  /** Returns the remaining bits. */
  inline unsigned int bit() const { return _value%8; }
  /** Returns the offset in bits (total). */
  inline int64_t bits() const { return _value; }

  /** Comparison operator. */
  inline bool operator==(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value == other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator!=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value != other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator<=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value <= other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator<(const Offset &other) const  {
    return (isValid() && other.isValid()) ? (_value < other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator>(const Offset &other) const  {
    return (isValid() && other.isValid()) ? (_value > other._value) : false;
  }
  /** Comparison operator. */
  inline bool operator>=(const Offset &other) const {
    return (isValid() && other.isValid()) ? (_value >= other._value) : false;
  }

  /** Offset arithmetic. */
  inline Offset &operator+=(const Offset &rhs) { _value += rhs._value; return *this; }
  /** Offset arithmetic. */
  inline Offset operator+(const Offset &rhs) const { return Offset(_value + rhs._value); }
  /** Shifts offsets by whole bytes. If the offset gets negative, it becomes invalid. */
  inline Offset &operator-=(unsigned int rhs) {
    _value = (rhs*8>_value) ? std::numeric_limits<unsigned long>::max() : (_value-(rhs*8));
    return *this;
  }
  /** Shifts offsets by whole bytes. If the offset gets negative, it becomes invalid. */
  inline Offset operator-(unsigned int rhs) const {
    return (rhs*8>_value) ? Offset() : Offset(_value - rhs*8);
  }
  /** Scales offset. */
  inline Offset &operator*=(unsigned int n) { _value *= n; return *this; }
  /** Scales offset. */
  inline Offset operator*(unsigned int n) const { return Offset(_value * n); }

protected:
  /** The offset in bits. */
  int64_t _value;
};


/** Specifies a size with bit resolution.
 *
 *  A size is closely related to an offset. Hence they can be converted into eachother. Addresses
 *  have a different semantic, hence there are no conversion between sizes/offsets and addresses.
 *
 * @ingroup address */
struct Size
{
protected:
  /** Hidden constructor from bits. */
  Size(unsigned long bits);

public:
  /** Default constructor. Creates an invalid size. */
  Size();
  /** Copy/cast constructor from offset. */
  Size(const Offset &other);

  /** Copy constructor. */
  inline Size(const Size &other): _value(other._value) {}
  /** Copying assignement operator. */
  inline Size &operator= (const Size &other) { _value = other._value; return *this; }

  /** Returns @c true if the size is valid. */
  bool isValid() const;

  /** Factory method to construct a zero size. */
  static Size zero();
  /** Factory method to construct a size from the given bytes and bits. */
  static Size fromByte(unsigned int n, unsigned bit=0);
  /** Factory method to construct a size from the given bits. */
  static Size fromBits(unsigned long n);
  /** Parses a size. */
  static Size fromString(const QString &str);
  /** Serializes this size. */
  QString toString() const;

  /** Retunrs the hole bytes of the size. */
  inline unsigned int byte() const { return _value/8; }
  /** Returns the remaining bits. */
  inline unsigned int bit() const { return _value%8; }
  /** Returns the size in (total) bits. */
  inline uint64_t bits() const { return _value; }

  /** Comparison opertor. */
  inline bool operator==(const Size &other) const { return _value == other._value; }
  /** Comparison opertor. */
  inline bool operator!=(const Size &other) const { return _value != other._value; }
  /** Comparison opertor. */
  inline bool operator<=(const Size &other) const { return _value <= other._value; }
  /** Comparison opertor. */
  inline bool operator<(const Size &other) const  { return _value <  other._value; }
  /** Comparison opertor. */
  inline bool operator>(const Size &other) const  { return _value >  other._value; }
  /** Comparison opertor. */
  inline bool operator>=(const Size &other) const { return _value >= other._value; }

  /** Size arithmetic. */
  inline Size &operator+=(const Size &rhs) { _value += rhs._value; return *this; }
  /** Size arithmetic. */
  inline Size operator+(const Size &rhs) const { return Size(_value + rhs._value); }
  /** Size arithmetic. */
  Size &operator-=(const Size &rhs);
  /** Size arithmetic. */
  Size operator-(const Size &rhs) const;
  /** Size arithmetic. */
  inline Size &operator*=(unsigned int n) { _value *= n; return *this; }
  /** Size arithmetic. */
  inline Size operator*(unsigned int n) const { return Size(_value * n); }

protected:
  /** The size in bits. */
  uint64_t _value;
};

#endif // OFFSET_HH
