#ifndef HEXDUMP_HH
#define HEXDUMP_HH

#include <QByteArray>
#include <QVector>

class Element;
class Image;
class ElementDifference;
class ImageDifference;
class QTextStream;


class HexLine
{
public:
  struct Byte
  {
  public:
    enum Type {
      Keep, Remove, Add, Modified, Unused
    };

  public:
    Type type;
    uint8_t value;
  };

public:
  explicit HexLine(uint32_t address, const QByteArray &left);
  explicit HexLine(uint32_t address, const QByteArray &left, const QByteArray &right);
  HexLine(const HexLine &other);

  HexLine &operator= (const HexLine &other);

  unsigned int consumed() const;
  uint32_t address() const;

  const QVector<Byte> &left() const;
  const Byte &left(unsigned int i) const;
  Byte &left(unsigned int i);

  const QVector<Byte> &right() const;
  const Byte &right(unsigned int i) const;
  Byte &right(unsigned int i);

  bool isDiff() const;
  bool hasDiff() const;

protected:
  uint32_t _address;
  unsigned int _consumed;
  QVector<Byte> _left;
  QVector<Byte> _right;
  bool _hasDiff;
};


class HexElement
{
public:
  explicit HexElement(const Element *element);
  explicit HexElement(const Element *left, const Element *right);
  HexElement(const HexElement &other);

  HexElement &operator= (const HexElement &other);

  uint32_t address() const;

  unsigned int size() const;
  const HexLine &line(unsigned int n) const;

  bool isDiff() const;
  bool hasDiff() const;

protected:
  QVector<HexLine> _lines;
  uint32_t _address;
  bool _isDiff;
  bool _hasDiff;
};


class HexImage
{
public:
  explicit HexImage(const Image *image);
  explicit HexImage(const Image *left, const Image *right);

  HexImage(const HexImage &other) = default;
  HexImage &operator= (const HexImage &other) = default;

  unsigned int size() const;
  const HexElement &element(unsigned int i) const;

  bool isDiff() const;
  bool hasDiff() const;

protected:
  QVector<HexElement> _elements;
  bool _isDiff;
  bool _hasDiff;
};

void hexdump(const HexImage &hex, QTextStream &stream);

#endif // HEXDUMP_HH
