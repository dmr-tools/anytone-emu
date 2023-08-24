#ifndef CODEPLUGPATTERN_HH
#define CODEPLUGPATTERN_HH

#include <QObject>
#include "offset.hh"

class Image;
class Element;

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
class GroupPattern: public AbstractPattern, public StructuredPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit GroupPattern(QObject *parent = nullptr);
};


class CodeplugPattern: public GroupPattern
{
  Q_OBJECT

public:
  explicit CodeplugPattern(QObject *parent = nullptr);

  bool verify() const;

  bool addChildPattern(AbstractPattern *pattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;

protected:
  bool _sparse;
  QList<AbstractPattern *> _content;
};


class RepeatPattern: public GroupPattern
{
  Q_OBJECT

public:
  explicit RepeatPattern(QObject *parent = nullptr);

  bool verify() const;

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

public:
  virtual QVariant value(const Element *element, const Offset &offset) const = 0;
};


class UnknownFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnknownFieldPattern(QObject *parent=nullptr);

  bool verify() const;

  void setSize(const Offset &size);

  QVariant value(const Element *element, const Offset &offset) const;
};


class UnusedFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnusedFieldPattern(QObject *parent=nullptr);

  bool verify() const;

  const QByteArray &content() const;
  bool setContent(const QByteArray &content);

  QVariant value(const Element *element, const Offset &offset) const;

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

  QVariant value(const Element *element, const Offset &offset) const;

protected:
  static uint16_t fromBCD4le(uint16_t bcd);
  static uint16_t fromBCD4be(uint16_t bcd);
  static uint32_t fromBCD8le(uint32_t bcd);
  static uint32_t fromBCD8be(uint32_t bcd);

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

  QVariant value(const Element *element, const Offset &offset) const;

protected:
  unsigned int _value;
};


class EnumFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit EnumFieldPattern(QObject *parent=nullptr);

  bool verify() const;

  bool addItem(EnumFieldPatternItem *item);
  unsigned int numItems() const;
  EnumFieldPatternItem *item(unsigned int n) const;

  QVariant value(const Element *element, const Offset &offset) const;

protected:
  QList<EnumFieldPatternItem *> _items;
};

#endif // CODEPLUGPATTERN_HH
