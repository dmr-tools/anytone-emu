#ifndef PATTERNDEFINITION_HH
#define PATTERNDEFINITION_HH

#include <QObject>
#include "codeplugpattern.hh"

class AbstractPatternDefinition;


class PatternDefinitionLibrary: public QObject
{
  Q_OBJECT

public:
  explicit PatternDefinitionLibrary(QObject *parent=nullptr);

  bool add(const QString &qname, AbstractPatternDefinition *definition);
  bool has(const QString &qname) const;
  AbstractPatternDefinition *get(const QString &qname) const;

  QHash<QString, AbstractPatternDefinition *> pattern() const;

protected:
  QHash<QString, QObject *> _elements;
};


class AbstractPatternDefinition: public QObject
{
  Q_OBJECT

protected:
  explicit AbstractPatternDefinition(QObject *parent=nullptr);

public:
  virtual bool verify() const = 0;
  virtual AbstractPattern *instantiate() const = 0;
  virtual bool serialize(QXmlStreamWriter &writer) const = 0;

  bool hasAddress() const;
  bool hasImplicitAddress() const;
  const Address &address() const;
  void setAddress(const Address &offset);

  virtual bool hasSize() const = 0;
  virtual Size size() const = 0;

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
  void modified(const AbstractPatternDefinition *pattern);
  void added(AbstractPatternDefinition *parent, unsigned int idx);
  void removing(AbstractPatternDefinition *parent, unsigned int idx);
  void removed(AbstractPatternDefinition *parent, unsigned int idx);
  void resized(const AbstractPatternDefinition *pattern, const Size &newSize);

private slots:
  void onMetaModified();

private:
  PatternMeta _meta;
  Address _address;
};


class StructuredPatternDefinition
{
protected:
  StructuredPatternDefinition();

public:
  virtual ~StructuredPatternDefinition();

  virtual int indexOf(const AbstractPatternDefinition *pattern) const = 0;
  virtual unsigned int numChildPattern() const = 0;
  virtual bool addChildPattern(AbstractPatternDefinition *pattern) = 0;
  virtual AbstractPatternDefinition *childPattern(unsigned int n) const = 0;
  virtual bool deleteChild(unsigned int n) = 0;

};


/** A group pattern groups several other pattern into logical groups.
 *
 * Each sub-pattern must have an explicit position, as groups are considered to be
 * sparse. */
class GroupPatternDefinition: public AbstractPatternDefinition, public StructuredPatternDefinition
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit GroupPatternDefinition(QObject *parent = nullptr);
};


class CodeplugPatternDefinition: public GroupPatternDefinition
{
  Q_OBJECT

public:
  explicit CodeplugPatternDefinition(QObject *parent = nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  int indexOf(const AbstractPatternDefinition *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPatternDefinition *pattern);
  AbstractPatternDefinition *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

  bool hasSize() const;
  Size size() const;

  bool isModified() const;
  static CodeplugPatternDefinition *load(const QString &filename);
  bool save();
  bool save(const QString &filename);
  bool save(QIODevice *device);

  const QFileInfo &source() const;

protected:
  void setSource(const QString &filename);

private slots:
  void onModified();

protected:
  bool _modified;
  QFileInfo _source;
  QList<AbstractPatternDefinition *> _content;
};


class RepeatPatternDefinition: public GroupPatternDefinition
{
  Q_OBJECT

public:
  explicit RepeatPatternDefinition(QObject *parent = nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool hasSize() const;
  Size size() const;

  bool hasMinRepetition() const;
  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  bool hasMaxRepetition() const;
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  const Offset &step() const;
  void setStep(const Offset &step);

  int indexOf(const AbstractPatternDefinition *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPatternDefinition *subpattern);
  AbstractPatternDefinition *subpattern() const;
  AbstractPatternDefinition *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

protected:
  unsigned int _minRepetition;
  unsigned int _maxRepetition;
  Offset _step;
  AbstractPatternDefinition *_subpattern;
};


/** Represents a pattern of a continuous piece of memory. */
class BlockPatternDefinition: public AbstractPatternDefinition
{
  Q_OBJECT

protected:
  explicit BlockPatternDefinition(QObject *parent=nullptr);
};


/** Represents a pattern of a continuous piece of memory of fixed size.
 * This is a specialization of the BlockPattern in that, the footprint of this pattern is known. */
class FixedPatternDefinition: public BlockPatternDefinition
{
  Q_OBJECT

protected:
  explicit FixedPatternDefinition(QObject *parent = nullptr);

public:
  bool verify() const;

  bool hasSize() const;
  Size size() const;

protected:
  virtual void setSize(const Size &size);

private:
  Size _size;
};


class BlockRepeatPatternDefinition: public BlockPatternDefinition, public StructuredPatternDefinition
{
  Q_OBJECT

public:
  explicit BlockRepeatPatternDefinition(QObject *parent=nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool hasSize() const;
  Size size() const;

  bool hasMinRepetition() const;
  unsigned int minRepetition() const;
  void setMinRepetition(unsigned int rep);
  bool hasMaxRepetition() const;
  unsigned int maxRepetition() const;
  void setMaxRepetition(unsigned int rep);

  int indexOf(const AbstractPatternDefinition *pattern) const;
  unsigned int numChildPattern() const;
  FixedPatternDefinition *subpattern() const;
  AbstractPatternDefinition *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPatternDefinition *subpattern);
  bool deleteChild(unsigned int n);

protected:
  unsigned int _minRepetition;
  unsigned int _maxRepetition;
  FixedPatternDefinition *_subpattern;
};


class ElementPatternDefinition: public FixedPatternDefinition, public StructuredPatternDefinition
{
  Q_OBJECT

public:
  explicit ElementPatternDefinition(QObject *parent = nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool addChildPattern(AbstractPatternDefinition *pattern);
  unsigned int numChildPattern() const;
  AbstractPatternDefinition *childPattern(unsigned int n) const;
  int indexOf(const AbstractPatternDefinition *pattern) const;
  bool deleteChild(unsigned int n);

private slots:
  void onChildResized(const AbstractPatternDefinition* child, const Size &size);

protected:
  QList<FixedPatternDefinition *> _content;
};


class FixedRepeatPatternDefinition: public FixedPatternDefinition, public StructuredPatternDefinition
{
  Q_OBJECT

public:
  explicit FixedRepeatPatternDefinition(QObject *parent = nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  unsigned int repetition() const;
  void setRepetition(unsigned int n);

  int indexOf(const AbstractPatternDefinition *pattern) const;
  unsigned int numChildPattern() const;
  FixedPatternDefinition *subpattern() const;
  AbstractPatternDefinition *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPatternDefinition *pattern);
  bool deleteChild(unsigned int n);

private slots:
  void onChildResized(const AbstractPatternDefinition *pattern, const Size &size);

protected:
  unsigned int _repetition;
  FixedPatternDefinition *_subpattern;
};


class FieldPatternDefinition: public FixedPatternDefinition
{
  Q_OBJECT

protected:
  explicit FieldPatternDefinition(QObject *parent=nullptr);

public:
  virtual QVariant value(const Element *element, const Address &address) const = 0;
};


class UnknownFieldPatternDefinition: public FieldPatternDefinition
{
  Q_OBJECT

public:
  explicit UnknownFieldPatternDefinition(QObject *parent=nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  void setWidth(const Size &size);

  QVariant value(const Element *element, const Address &address) const;
};


class UnusedFieldPatternDefinition: public FieldPatternDefinition
{
  Q_OBJECT

public:
  explicit UnusedFieldPatternDefinition(QObject *parent=nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  const QByteArray &content() const;
  bool setContent(const QByteArray &content);
  void setWidth(const Size &size);

  QVariant value(const Element *element, const Address &address) const;

protected:
  QByteArray _content;
};


class IntegerFieldPatternDefinition: public FieldPatternDefinition
{
  Q_OBJECT

public:
  explicit IntegerFieldPatternDefinition(QObject *parent=nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  void setWidth(const Offset &width);

  IntegerFieldPattern::Format format() const;
  void setFormat(IntegerFieldPattern::Format format);

  IntegerFieldPattern::Endian endian() const;
  void setEndian(IntegerFieldPattern::Endian endian);

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
  IntegerFieldPattern::Format _format;
  IntegerFieldPattern::Endian _endian;
  long long _minValue;
  long long _maxValue;
  long long _defaultValue;
};


class EnumFieldPatternItemDefinition: public PatternMeta
{
  Q_OBJECT

public:
  explicit EnumFieldPatternItemDefinition(QObject *parent = nullptr);

  bool serialize(QXmlStreamWriter &writer) const;

  bool hasValue() const;
  unsigned int value() const;
  bool setValue(unsigned int value);

  QVariant value(const Element *element, const Offset &offset) const;

protected:
  unsigned int _value;
};


class EnumFieldPatternDefinition: public FieldPatternDefinition
{
  Q_OBJECT

public:
  explicit EnumFieldPatternDefinition(QObject *parent=nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool addItem(EnumFieldPatternItemDefinition *item);
  unsigned int numItems() const;
  EnumFieldPatternItemDefinition *item(unsigned int n) const;
  bool deleteItem(unsigned int n);

  void setWidth(const Size &size);

  QVariant value(const Element *element, const Address &address) const;

signals:
  void itemAdded(unsigned int idx);
  void itemDeleted(unsigned int idx);

protected:
  QList<EnumFieldPatternItemDefinition *> _items;
};


class StringFieldPatternDefinition: public FieldPatternDefinition
{
  Q_OBJECT

public:
  explicit StringFieldPatternDefinition(QObject *parent = nullptr);

  bool verify() const;
  AbstractPattern *instantiate() const;
  bool serialize(QXmlStreamWriter &writer) const;

  QVariant value(const Element *element, const Address &address) const;

  StringFieldPattern::Format format() const;
  void setFormat(StringFieldPattern::Format format);

  unsigned int numChars() const;
  void setNumChars(unsigned int n);

  unsigned int padValue() const;
  void setPadValue(unsigned int pad);

protected:
  StringFieldPattern::Format _format;
  unsigned int _numChars;
  unsigned int _padValue;
};


#endif // PATTERNDEFINITION_HH
