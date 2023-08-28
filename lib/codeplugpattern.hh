#ifndef CODEPLUGPATTERN_HH
#define CODEPLUGPATTERN_HH

#include <QObject>
#include "offset.hh"

class Image;
class Element;
class QXmlStreamWriter;


class PatternMeta: public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QString firmwareVersion READ firmwareVersion WRITE setFirmwareVersion)

public:
  enum class Flags {
    None, Done, NeedsReview, Incomplete
  };
  Q_ENUM(Flags)

public:
  explicit PatternMeta(QObject *parent=nullptr);

  virtual bool serialize(QXmlStreamWriter &writer) const;

  PatternMeta &operator=(const PatternMeta &other);

  const QString &name() const;
  void setName(const QString &name);

  bool hasDescription() const;
  const QString &description() const;
  void setDescription(const QString &description);

  bool hasFirmwareVersion() const;
  const QString &firmwareVersion() const;
  void setFirmwareVersion(const QString &version);

  Flags flags() const;
  void setFlags(Flags flags);

signals:
  void modified();

protected:
  QString _name;
  QString _description;
  QString _fwVersion;
  Flags   _flags;
};


class AbstractPattern : public QObject
{
  Q_OBJECT

protected:
  explicit AbstractPattern(QObject *parent = nullptr);

public:
  virtual bool verify() const = 0;
  virtual bool serialize(QXmlStreamWriter &writer) const = 0;

  bool hasAddress() const;
  bool hasImplicitAddress() const;
  const Address &address() const;
  void setAddress(const Address &offset);

  bool hasSize() const;
  const Size &size() const;

  const PatternMeta &meta() const;
  PatternMeta &meta();

  template <class T>
  bool is() const {
    return nullptr != dynamic_cast<const T *>(this);
  }

  template <class T>
  const T* as() const {
    return dynamic_cast<const T *>(this);
  }

  template <class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

signals:
  void modified(const AbstractPattern *pattern);
  void added(AbstractPattern *parent, unsigned int idx);
  void removing(AbstractPattern *parent, unsigned int idx);
  void removed(AbstractPattern *parent, unsigned int idx);

private slots:
  void onMetaModified();

protected:
  PatternMeta _meta;
  Address _address;
  Size _size;
};


class StructuredPattern
{
protected:
  StructuredPattern();

public:
  virtual ~StructuredPattern();

  virtual int indexOf(const AbstractPattern *pattern) const = 0;
  virtual unsigned int numChildPattern() const = 0;
  virtual bool addChildPattern(AbstractPattern *pattern) = 0;
  virtual AbstractPattern *childPattern(unsigned int n) const = 0;
  virtual bool deleteChild(unsigned int n) = 0;
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
  bool serialize(QXmlStreamWriter &writer) const;

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPattern *pattern);
  AbstractPattern *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

  static CodeplugPattern *load(const QString &filename);
  bool save(const QString &filename);
  bool save(QIODevice *device);

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
  bool serialize(QXmlStreamWriter &writer) const;

  bool hasMinRepetition() const;
  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  bool hasMaxRepetition() const;
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  const Offset &step() const;
  void setStep(const Offset &step);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPattern *subpattern);
  AbstractPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

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

signals:
  void resized(const FixedPattern *pattern, const Size &newSize);
};


class BlockRepeatPattern: public BlockPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit BlockRepeatPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool hasMinRepetition() const;
  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  bool hasMaxRepetition() const;
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  FixedPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPattern *subpattern);
  bool deleteChild(unsigned int n);

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
  bool serialize(QXmlStreamWriter &writer) const;

  bool addChildPattern(AbstractPattern *pattern);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  int indexOf(const AbstractPattern *pattern) const;
  bool deleteChild(unsigned int n);

protected:
  QList<FixedPattern *> _content;
};


class FixedRepeatPattern: public FixedPattern, public StructuredPattern
{
  Q_OBJECT

public:
  explicit FixedRepeatPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  unsigned int repetition() const;
  void setRepetition(unsigned int n);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  FixedPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPattern *pattern);
  bool deleteChild(unsigned int n);

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
  virtual QVariant value(const Element *element, const Address &address) const = 0;
};


class UnknownFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnknownFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  void setSize(const Offset &size);

  QVariant value(const Element *element, const Address &address) const;
};


class UnusedFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  explicit UnusedFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  const QByteArray &content() const;
  bool setContent(const QByteArray &content);
  void setSize(const Size &size);

  QVariant value(const Element *element, const Address &address) const;

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
  bool serialize(QXmlStreamWriter &writer) const;

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
  void clearDefaultValue();

  QVariant value(const Element *element, const Address &address) const;

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

  bool serialize(QXmlStreamWriter &writer) const;

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
  bool serialize(QXmlStreamWriter &writer) const;

  bool addItem(EnumFieldPatternItem *item);
  unsigned int numItems() const;
  EnumFieldPatternItem *item(unsigned int n) const;
  bool deleteItem(unsigned int n);

  void setWidth(const Size &size);

  QVariant value(const Element *element, const Address &address) const;

signals:
  void itemAdded(unsigned int idx);
  void itemDeleted(unsigned int idx);

protected:
  QList<EnumFieldPatternItem *> _items;
};


class StringFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  enum class Format {
    ASCII, Unicode
  };
  Q_ENUM(Format)

public:
  explicit StringFieldPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  QVariant value(const Element *element, const Address &address) const;

  Format format() const;
  void setFormat(Format format);

  unsigned int numChars() const;
  void setNumChars(unsigned int n);

  unsigned int padValue() const;
  void setPadValue(unsigned int pad);

protected:
  Format _format;
  unsigned int _numChars;
  unsigned int _padValue;
};

#endif // CODEPLUGPATTERN_HH
