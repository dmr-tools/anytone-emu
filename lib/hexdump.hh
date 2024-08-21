#ifndef HEXDUMP_HH
#define HEXDUMP_HH

#include <QByteArray>
#include <QVector>

class Element;
class Image;
class ElementDifference;
class ImageDifference;
class QTextStream;


/** A single line in a hex-dump or hex-diff.
 * Starts with the address, followed by the hex data and the printable chars of that data.
 * If the line in a hex-difference, the address is followed by both hex blocks and both printable
 * chars. */
class HexLine
{
public:
  /** Represents a single byte in a hex dump or difference.
   * That is, the value itself as well as a type indicating the difference between two bytes. */
  struct Byte
  {
  public:
    /** Possible difference types. */
    enum Type {
      Keep,     ///< Default, also indicates no difference in a diff.
      Remove,   ///< This byte is removed.
      Add,      ///< This byte is added.
      Modified, ///< This byte is modified
      Unused    ///< This byte is marked unused.
    };

  public:
    /** The type of the byte difference. */
    Type type;
    /** The actual value of the byte. */
    uint8_t value;
  };

public:
  /** Constructs a simple hex-dump. */
  explicit HexLine(uint32_t address, const QByteArray &left);
  /** Constructs a hex difference, where @c right replaces @c left. */
  explicit HexLine(uint32_t address, const QByteArray &left, const QByteArray &right);
  /** Copy constructor. */
  HexLine(const HexLine &other);
  /** Copying assignment operator. */
  HexLine &operator= (const HexLine &other);

  /** Returns the (maximum) number of bytes in this line. */
  unsigned int consumed() const;
  /** Returns the start-address of this line. */
  uint32_t address() const;

  /** Returns the left bytes of a diff or just the bytes for a hex-dump. */
  const QVector<Byte> &left() const;
  /** Returns the i-th byte on the left side. */
  const Byte &left(unsigned int i) const;
  /** Returns the i-th byte on the left side. */
  Byte &left(unsigned int i);

  /** Returns the right bytes of a diff.
   * This is empty for simple hex-dumps. */
  const QVector<Byte> &right() const;
  /** Returns the i-th byte on the right side. */
  const Byte &right(unsigned int i) const;
  /** Returns the i-th byte on the right side. */
  Byte &right(unsigned int i);

  /** Returns @c true if the line is a difference. That is, if there is a left and right side. */
  bool isDiff() const;
  /** Returns @c true if the line is a difference and has any differences. That is, */
  bool hasDiff() const;

protected:
  /** Start-address of the line. */
  uint32_t _address;
  /** Number of consumed bytes (length of the longer line). */
  unsigned int _consumed;
  /** Left bytes. */
  QVector<Byte> _left;
  /** Right bytes, empty if only hex dump. */
  QVector<Byte> _right;
  /** Is @c true, if this line is a hex-difference and there is a difference. */
  bool _hasDiff;
};


/** Represents a hex dump or hex difference between entire @c Element instances.
 * To this end, this is just a collection of @c HexLine instances.
 * @ingroup utils */
class HexElement
{
public:
  /** Hex dump constructor for the given element. */
  explicit HexElement(const Element *element);
  /** Hex diff constructor for the two elements. */
  explicit HexElement(const Element *left, const Element *right);
  /** Copy constructor. */
  HexElement(const HexElement &other);
  /** Copying assignment operator. */
  HexElement &operator= (const HexElement &other);

  /** Returns the start-address of the dump or diff. */
  uint32_t address() const;
  /** Returns the number of lines. */
  unsigned int size() const;
  /** Returns the n-th line. */
  const HexLine &line(unsigned int n) const;

  /** Returns @c true if this is a difference. That is, there is a left and a right side. */
  bool isDiff() const;
  /** Returns @c true if this is a difference and there are any differences. */
  bool hasDiff() const;

protected:
  /** The lines. */
  QVector<HexLine> _lines;
  /** Starting address. */
  uint32_t _address;
  /** If @c true, any line is a hex difference. */
  bool _isDiff;
  /** If @c true, any line is a hex difference and has some difference. */
  bool _hasDiff;
};


/** Represents the hex dump of a single @c Image or the hex-difference between two images.
 * @ingroup utils */
class HexImage
{
public:
  /** Constructs a hex dump of the image. */
  explicit HexImage(const Image *image);
  /** Constructs a hex-difference between the given images. */
  explicit HexImage(const Image *left, const Image *right);
  /** Copy constructor. */
  HexImage(const HexImage &other) = default;
  /** Copying assignment operator. */
  HexImage &operator= (const HexImage &other) = default;

  /** Returns the number of elements in the hex image. */
  unsigned int size() const;
  /** Returns the i-th element. */
  const HexElement &element(unsigned int i) const;

  /** Returns @c true, if this hex image is a difference.
   * That is, there is a left and right side. */
  bool isDiff() const;
  /** Returns @c true, if this image is a difference and there are differences. */
  bool hasDiff() const;

protected:
  /** The elements. */
  QVector<HexElement> _elements;
  /** Is @c true if any element is a difference. */
  bool _isDiff;
  /** Is @c true if any element is a difference and has any differences. */
  bool _hasDiff;
};


/** Formats the hex-image and serializes it into the given text stream.
 * You may also use the @c << operator.
 * @ingroup utils */
void hexdump(const HexImage &hex, QTextStream &stream);

/** Stream operator for formatting hex-dumps into a @c QTextStream.
 * @ingroup utils */
inline QTextStream &
operator<<(QTextStream &stream, const HexImage &hex) {
  hexdump(hex, stream);
  return stream;
}

#endif // HEXDUMP_HH
