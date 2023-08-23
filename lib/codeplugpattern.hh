#ifndef CODEPLUGPATTERN_HH
#define CODEPLUGPATTERN_HH

#include <QObject>

class Image;


struct Offset
{
protected:
  Offset(unsigned long bits);

public:
  Offset();

  inline Offset(const Offset &other): _bitOffset(other._bitOffset) {}

  bool isValid() const;

  static Offset zero();
  static Offset fromByte(unsigned int n, unsigned bit=0);
  static Offset fromBits(unsigned long n);
  static Offset fromString(const QString &str);

  unsigned int byte() const;
  unsigned int bit() const;
  unsigned int bits() const;

  inline Offset &operator= (const Offset &other) { _bitOffset = other._bitOffset; return *this; }
  inline bool operator==(const Offset &other) const { return _bitOffset == other._bitOffset; }
  inline bool operator!=(const Offset &other) const { return _bitOffset != other._bitOffset; }
  inline bool operator<=(const Offset &other) const { return _bitOffset <= other._bitOffset; }
  inline bool operator<(const Offset &other) const  { return _bitOffset <  other._bitOffset; }
  inline bool operator>(const Offset &other) const  { return _bitOffset >  other._bitOffset; }
  inline bool operator>=(const Offset &other) const { return _bitOffset >= other._bitOffset; }

  inline Offset &operator+=(const Offset &rhs) { _bitOffset += rhs._bitOffset; return *this; }
  inline Offset operator+(const Offset &rhs) const { return Offset(_bitOffset + rhs._bitOffset); }

  inline Offset &operator*=(unsigned int n) { _bitOffset *= n; return *this; }
  inline Offset operator*(unsigned int n) const { return Offset(_bitOffset * n); }

protected:
  unsigned long _bitOffset;
};


class PatternMeta: public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QString firmwareVersion READ firmwareVersion WRITE setFirmwareVersion)

public:
  explicit PatternMeta(QObject *parent=nullptr);

  PatternMeta &operator=(const PatternMeta &other);

  const QString &name() const;
  void setName(const QString &name);

  bool hasDescription() const;
  const QString &description() const;
  void setDescription(const QString &description);

  bool hasFirmwareVersion() const;
  const QString &firmwareVersion() const;
  void setFirmwareVersion(const QString &version);

protected:
  QString _name;
  QString _description;
  QString _fwVersion;
};


class AbstractPattern : public QObject
{
  Q_OBJECT

protected:
  explicit AbstractPattern(QObject *parent = nullptr);

public:
  virtual bool verify() const = 0;
  virtual bool match(const Image *image, const Offset &offset) const = 0;

  bool hasOffset() const;
  const Offset &offset() const;
  void setOffset(const Offset &offset);

  bool hasSize() const;
  const Offset &size() const;

  const PatternMeta &meta() const;
  PatternMeta &meta();

  template <class T>
  bool is() const {
    return nullptr != qobject_cast<const T *>(this);
  }

  template <class T>
  const T* as() const {
    return qobject_cast<const T *>(this);
  }

  template <class T>
  T* as() {
    return qobject_cast<T *>(this);
  }

protected:
  PatternMeta _meta;
  Offset _offset;
  Offset _size;
};


class StructuredPattern
{
protected:
  StructuredPattern();

public:
  virtual ~StructuredPattern();

  virtual bool addChildPattern(AbstractPattern *pattern) = 0;
  virtual unsigned int numChildPattern() const = 0;
  virtual AbstractPattern *childPattern(unsigned int n) const = 0;
};


/** A group pattern groups several other pattern into logical groups.
 *
 * Each sub-pattern must have an explicit or implicit position, as groups are considered to be
 * sparse. */
class GroupPattern: public AbstractPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit GroupPattern(QObject *parent = nullptr);
};


class CodeplugPattern: public GroupPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit CodeplugPattern(QObject *parent = nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  bool addChildPattern(AbstractPattern *pattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  bool _sparse;
  QList<AbstractPattern *> _content;
};


class RepeatPattern: public GroupPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit RepeatPattern(QObject *parent = nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  const Offset &step() const;
  void setStep(const Offset &step);

  AbstractPattern *subpattern() const;
  bool addChildPattern(AbstractPattern *subpattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  unsigned int _minRepetition;
  unsigned int _maxRepetition;
  Offset _step;
  AbstractPattern *_subpattern;
};


/** Represents a pattern of a continuous piece of memory. */
class BlockPattern: public AbstractPattern
{
  Q_OBJECT

protected:
  explicit BlockPattern(QObject *parent=nullptr);
};


/** Represents a pattern of a continuous piece of memory of fixed size.
 * This is a specialization of the BlockPattern in that, the footprint of this pattern is known. */
class FixedPattern: public BlockPattern
{
  Q_OBJECT

protected:
  explicit FixedPattern(QObject *parent = nullptr);

public:
  bool verify() const;
};


class BlockRepeatPattern: public BlockPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit BlockRepeatPattern(QObject *parent=nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  FixedPattern *subpattern() const;
  bool addChildPattern(AbstractPattern *subpattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  unsigned int _minRepetition;
  unsigned int _maxRepetition;
  FixedPattern *_subpattern;
};


class ElementPattern : public FixedPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit ElementPattern(QObject *parent = nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  bool addChildPattern(AbstractPattern *pattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  QList<FixedPattern *> _content;
};


class FixedRepeatPattern: public FixedPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit FixedRepeatPattern(QObject *parent = nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  unsigned int repetition() const;
  void setRepetition(unsigned int n);

  FixedPattern *subpattern() const;
  bool addChildPattern(AbstractPattern *pattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  unsigned int _repetition;
  FixedPattern *_subpattern;
};


class FieldPattern: public FixedPattern
{
  Q_OBJECT

protected:
  explicit FieldPattern(QObject *parent=nullptr);
};


class UnknownFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnknownFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  void setSize(const Offset &size);
};


class UnusedFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnusedFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  const QByteArray &content() const;
  bool setContent(const QByteArray &content);

protected:
  QByteArray _content;
};


class IntegerFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  enum class Format {
    Signed, Unsigned, BCD
  };
  Q_ENUM(Format)

  enum class Endian {
    Little, Big
  };
  Q_ENUM(Endian)

public:
  explicit IntegerFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  void setWidth(const Offset &width);

  Format format() const;
  void setFormat(Format format);

  Endian endian() const;
  void setEndian(Endian endian);

  bool hasMinValue() const;
  long long minValue() const;
  void setMinValue(long long min);

  bool hasMaxValue() const;
  long long maxValue() const;
  void setMaxValue(long long max);

  bool hasDefaultValue() const;
  long long defaultValue() const;
  void setDefaultValue(long long value);

protected:
  Format _format;
  Endian _endian;
  long long _minValue;
  long long _maxValue;
  long long _defaultValue;
};


class EnumFieldPatternItem: public PatternMeta
{
  Q_OBJECT

public:
  explicit EnumFieldPatternItem(QObject *parent = nullptr);

  bool hasValue() const;
  unsigned int value() const;
  bool setValue(unsigned int value);

protected:
  unsigned int _value;
};


class EnumFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit EnumFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool match(const Image *image, const Offset &offset) const;

  bool addItem(EnumFieldPatternItem *item);
  unsigned int numItems() const;
  EnumFieldPatternItem *item(unsigned int n) const;

protected:
  QList<EnumFieldPatternItem *> _items;
};

#endif // CODEPLUGPATTERN_HH
