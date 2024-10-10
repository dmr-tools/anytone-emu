/** @defgroup pattern Codeplug structure and pattern definition.
 * This module collects all classes that form the pattern definitions defining the structure of a
 * codeplug. */
#ifndef CODEPLUGPATTERN_HH
#define CODEPLUGPATTERN_HH

#include <QObject>
#include <QFileInfo>
#include "offset.hh"

class Image;
class Element;
class QXmlStreamWriter;
class QFileInfo;


/** Holds some meta information about the pattern.
 * @ingroup pattern */
class PatternMeta: public QObject
{
  Q_OBJECT

  /** The name property. */
  Q_PROPERTY(QString name READ name WRITE setName)
  /** The description property. */
  Q_PROPERTY(QString description READ description WRITE setDescription)
  /** The firmware version property. */
  Q_PROPERTY(QString firmwareVersion READ firmwareVersion WRITE setFirmwareVersion)

public:
  /** It is possible to flag a pattern. */
  enum class Flags {
    None,        ///< Not flags set.
    Done,        ///< Pattern is considered complete.
    NeedsReview, ///< Pattern needs review.
    Incomplete   ///< Pattern needs work. I.e., contains unknwon memory sections.
  };
  Q_ENUM(Flags)

public:
  /** Default constructor. */
  explicit PatternMeta(QObject *parent=nullptr);

  /** Serializes itself to the XML stream. */
  virtual bool serialize(QXmlStreamWriter &writer) const;
  /** Copying assignment operator. */
  PatternMeta &operator=(const PatternMeta &other);

  /** Returns the name of the pattern. */
  const QString &name() const;
  /** Sets the name of the pattern. */
  void setName(const QString &name);

  /** Returns @c true if the pattern has a description. */
  bool hasDescription() const;
  /** Returns the description of the pattern. */
  const QString &description() const;
  /** Sets the description of the pattern. */
  void setDescription(const QString &description);

  /** Returns @c true if the pattern was verified against a specific firmware version. */
  bool hasFirmwareVersion() const;
  /** Returns the associated firmware version. */
  const QString &firmwareVersion() const;
  /** Sets the firmware version. */
  void setFirmwareVersion(const QString &version);

  /** Returns the flag of the pattern. */
  Flags flags() const;
  /** Sets the flag of the pattern. */
  void setFlags(Flags flags);

signals:
  /** Gets emitted, once the meta information changed. */
  void modified();

protected:
  /** Holds the name. */
  QString _name;
  /** Holds the description. */
  QString _description;
  /** Holds the firmware version. */
  QString _fwVersion;
  /** Holds the flags. */
  Flags   _flags;
};



/** Base class of all pattern definitions.
 * @ingroup pattern */
class AbstractPattern : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit AbstractPattern(QObject *parent = nullptr);

public:
  /** Verifies if the pattern is consistent.
   * What consistent means, depends heavily on the type of the pattern. Some need absolute or
   * relative addresses and/or sizes. */
  virtual bool verify() const = 0;
  /** Serialzes the pattern to the given XML stream. */
  virtual bool serialize(QXmlStreamWriter &writer) const = 0;

  /** Returns @c true, if the pattern has an absolute or relative address. */
  bool hasAddress() const;
  /** Returns @c true, if the pattern has an implicit address. That is, its address can be computed
   * from the context but is not set yet. */
  bool hasImplicitAddress() const;
  /** Returns the address of the pattern. */
  const Address &address() const;
  /** Sets the address of the pattern. */
  void setAddress(const Address &offset);

  /** Returns the meta information. */
  const PatternMeta &meta() const;
  /** Returns the meta information. */
  PatternMeta &meta();

  /** Returns @c true if the pattern can be casted to the template argument. */
  template <class T>
  bool is() const {
    return nullptr != dynamic_cast<const T *>(this);
  }

  /** Casts this pattern to the template argument. */
  template <class T>
  const T* as() const {
    return dynamic_cast<const T *>(this);
  }

  /** Casts this pattern to the template argument. */
  template <class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

signals:
  /** Gets emitted, if the pattern, its meta information or one of its sub-pattern is modified. */
  void modified(const AbstractPattern *pattern);
  /** Gets emitted if a pattern was added to this or one of its sub-pattern. */
  void added(AbstractPattern *parent, unsigned int idx);
  /** Gets emitted before a pattern is removed from this or one of its sub-pattern. */
  void removing(AbstractPattern *parent, unsigned int idx);
  /** Gets emitted after a pattern was removed from this or one of its sub-pattern. */
  void removed(AbstractPattern *parent, unsigned int idx);

private slots:
  /** Internal callback to forward modified signals. */
  void onMetaModified();

private:
  /** The pattern meta information. */
  PatternMeta _meta;
  /** The addewss of the pattern (if there is one). */
  Address _address;
};


/** Interface of all pattern, that contain other pattern.
 * @ingroup pattern */
class StructuredPattern
{
protected:
  /** Hidden constructor. */
  StructuredPattern();

public:
  /** Destructor. */
  virtual ~StructuredPattern();

  /** Returns the index of the given sub-pattern within this one or -1. */
  virtual int indexOf(const AbstractPattern *pattern) const = 0;
  /** Returns the number of sub-pattern. */
  virtual unsigned int numChildPattern() const = 0;
  /** Appends a sub-pattern. */
  virtual bool addChildPattern(AbstractPattern *pattern) = 0;
  /** Returns the n-th sub-pattern. */
  virtual AbstractPattern *childPattern(unsigned int n) const = 0;
  /** Removes the n-th sub-pattern. */
  virtual bool deleteChild(unsigned int n) = 0;
};


/** A group pattern groups several other pattern into logical groups.
 *
 * Each sub-pattern must have an explicit or implicit position, as groups are considered to be
 * sparse.
 *
 * @ingroup pattern */
class GroupPattern: public AbstractPattern, public StructuredPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit GroupPattern(QObject *parent = nullptr);
};



/** Root pattern for all codeplugs.
 * The codeplug is usually considered a sparse group of patterns. If the codeplug is a single
 * continous blob of memory, just add a single element here.
 * @ingroup pattern */
class CodeplugPattern: public GroupPattern
{
  Q_OBJECT

public:
  /** Desfault constructor. */
  explicit CodeplugPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPattern *pattern);
  AbstractPattern *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

  /** Retruns @c true if the codeplug was modified since the last save. */
  bool isModified() const;
  /** Loads a codeplug from the given file. */
  static CodeplugPattern *load(const QString &filename);
  /** Saves the codeplug into the last used file. */
  bool save();
  /** Saves the codeplug into the given file. */
  bool save(const QString &filename);
  /** Saves the codeplug into the given IO device. */
  bool save(QIODevice *device);

  /** Returns the last used file. */
  const QFileInfo &source() const;

protected:
  /** Sets the current file. */
  void setSource(const QString &filename);

private slots:
  /** Handles modified signals from its content. */
  void onModified();

protected:
  /** Is @c true if any pattern was modified since the last save. */
  bool _modified;
  /** The current file. */
  QFileInfo _source;
  /** The sub-patterns, that form the structure of the codeplug. */
  QList<AbstractPattern *> _content;
};



/** Sparse repeat pattern.
 * The sub-pattern of this repeat pattern can be spaced by a step offset.
 * @ingroup pattern */
class RepeatPattern: public GroupPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit RepeatPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Retunrs @c true, if a minimum repetition number is specified. */
  bool hasMinRepetition() const;
  /** Returns the minimum number of repetitions of the sub-pattern. */
  unsigned int minRepetition() const;
  /** Sets the minimum number of repetitions of the sub-pattern. */
  void setMinRepetition(unsigned int rep);
  /** Retunrs @c true, if a maximum repetition number is specified. */
  bool hasMaxRepetition() const;
  /** Returns the maximum number of repetitions of the sub-pattern. */
  unsigned int maxRepetition() const;
  /** Sets the maximum number of repetitions of the sub-pattern. */
  void setMaxRepetition(unsigned int rep);

  /** Returns the offset between repetition of the sub-pattern. */
  const Offset &step() const;
  /** Sets the offset between repetition of the sub-pattern. */
  void setStep(const Offset &step);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  bool addChildPattern(AbstractPattern *subpattern);
  /** Retunrs the one child pattern, the sub pattern. */
  AbstractPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool deleteChild(unsigned int n);

protected:
  /** The minimum number of prepetitions, @c std::numeric_limits<unsigned int>::max() if not set. */
  unsigned int _minRepetition;
  /** The maximum number of prepetitions, @c std::numeric_limits<unsigned int>::max() if not set. */
  unsigned int _maxRepetition;
  /** The offset between repetitions of the sub pattern. */
  Offset _step;
  /** The sub pattern. */
  AbstractPattern *_subpattern;
};


/** Represents a pattern of a continuous piece of memory. */
class BlockPattern: public AbstractPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit BlockPattern(QObject *parent=nullptr);
};



/** Represents a pattern of a continuous piece of memory of fixed size.
 * This is a specialization of the @c BlockPattern in that, the footprint of this pattern is
 * known. */
class FixedPattern: public BlockPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit FixedPattern(QObject *parent = nullptr);

public:
  bool verify() const;

  /** Returns @c true, if a size is set. */
  bool hasSize() const;
  /** Returns the size. */
  const Size &size() const;

signals:
  /** Gets emitted, if the size of the pattern changes. */
  void resized(const FixedPattern *pattern, const Size &newSize);

protected:
  /** Sets the size. */
  virtual void setSize(const Size &size);

private:
  /** The size of the pattern. */
  Size _size;
};



/** Dense repetition of a fixed sub-pattern. The number of reprtitions is varible, hence this
 *  pattern itself is not fixed in size.
 * @ingroup pattern */
class BlockRepeatPattern: public BlockPattern, public StructuredPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit BlockRepeatPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Retunrs @c true, if a minimum repetition number is specified. */
  bool hasMinRepetition() const;
  /** Returns the minimum number of repetitions of the sub-pattern. */
  unsigned int minRepetition() const;
  /** Sets the minimum number of repetitions of the sub-pattern. */
  void setMinRepetition(unsigned int rep);
  /** Retunrs @c true, if a maximum repetition number is specified. */
  bool hasMaxRepetition() const;
  /** Returns the maximum number of repetitions of the sub-pattern. */
  unsigned int maxRepetition() const;
  /** Sets the maximum number of repetitions of the sub-pattern. */
  void setMaxRepetition(unsigned int rep);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  /** Retunrs the one child pattern, the sub pattern. */
  FixedPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPattern *subpattern);
  bool deleteChild(unsigned int n);

protected:
  /** The minimum number of prepetitions, @c std::numeric_limits<unsigned int>::max() if not set. */
  unsigned int _minRepetition;
  /** The maximum number of prepetitions, @c std::numeric_limits<unsigned int>::max() if not set. */
  unsigned int _maxRepetition;
  /** The sub-pattern. */
  FixedPattern *_subpattern;
};



/** A fixed sized and dense collection of other fixed sized patterns.
 * @ingroup pattern */
class ElementPattern : public FixedPattern, public StructuredPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit ElementPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  bool addChildPattern(AbstractPattern *pattern);
  /** Insert the given pattern at the specified index.
   * Also updates all addresses. */
  bool insertChildPattern(FixedPattern *pattern, unsigned int idx);
  unsigned int numChildPattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  int indexOf(const AbstractPattern *pattern) const;
  bool deleteChild(unsigned int n);

private slots:
  /** Gets called, if a sub-patern is resized. Then updates the relative addresses of all
   * sub-patterns. */
  void onChildResized(const FixedPattern *child, const Size &size);

protected:
  /** The list of sub-patterns. */
  QList<FixedPattern *> _content;
};


/** Exactly n repetitions of a fixed sub-pattern.
 * @ingroup pattern */
class FixedRepeatPattern: public FixedPattern, public StructuredPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit FixedRepeatPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Returns the number of repetition. */
  unsigned int repetition() const;
  /** Sets the number of repetition. */
  void setRepetition(unsigned int n);

  int indexOf(const AbstractPattern *pattern) const;
  unsigned int numChildPattern() const;
  /** Returns the one sub-pattern. */
  FixedPattern *subpattern() const;
  AbstractPattern *childPattern(unsigned int n) const;
  bool addChildPattern(AbstractPattern *pattern);
  bool deleteChild(unsigned int n);

private slots:
  /** Gets called, if the sub-pattern resizes. This also updates the size of this pattern. */
  void onChildResized(const FixedPattern *pattern, const Size &size);

protected:
  /** The number of repetitions. */
  unsigned int _repetition;
  /** The sub-pattern to repeat. */
  FixedPattern *_subpattern;
};



/** Interface for all elementary field pattern, that is values. */
class FieldPattern: public FixedPattern
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit FieldPattern(QObject *parent=nullptr);

public:
  /** Decodes the given memeory at the specified address. */
  virtual QVariant value(const Element *element, const Address &address) const = 0;
};


/** Represents an unknown field.
 * @ingroup pattern */
class UnknownFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit UnknownFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Sets the size of the field. */
  void setWidth(const Size &size);

  QVariant value(const Element *element, const Address &address) const;
};



/** Represents an unused memory section.
 * @ingroup pattern */
class UnusedFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit UnusedFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Returns the expected content of the field. */
  const QByteArray &content() const;
  /** Sets the expected content of the field. */
  bool setContent(const QByteArray &content);
  /** Sets the precise width of the field. */
  void setWidth(const Size &size);

  /** Returns the section of the element corresponding to this field at the specified address.
   * This value should match the expected content. */
  QVariant value(const Element *element, const Address &address) const;

protected:
  /** The expected content. */
  QByteArray _content;
};


/** A field encoding an iteger.
 * The width, format and endianess can be set.
 * @ingroup pattern */
class IntegerFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  /** The format of the integer. */
  enum class Format {
    Signed,   ///< A signed integer (two-complement)
    Unsigned, ///< An unsigned integer.
    BCD       ///< An unsigned integer in BCD encoding.
  };
  Q_ENUM(Format)

  /** Possible endianess. */
  enum class Endian {
    Little,   ///< Little endian.
    Big       ///< Big endian.
  };
  Q_ENUM(Endian)

public:
  /** Default constructor. */
  explicit IntegerFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Sets the width of the integer field. */
  void setWidth(const Offset &width);

  /** Returns the format. */
  Format format() const;
  /** Sets the format. */
  void setFormat(Format format);

  /** Returns the endianess. */
  Endian endian() const;
  /** Sets the endianess. */
  void setEndian(Endian endian);

  /** Retuns @c true if the integer has a minimum value. */
  bool hasMinValue() const;
  /** Returns the minimum value. */
  long long minValue() const;
  /** Sets the minimum value. */
  void setMinValue(long long min);
  /** Clears the minimum value. */
  void clearMinValue();

  /** Retuns @c true if the integer has a maximum value. */
  bool hasMaxValue() const;
  /** Returns the maximum value. */
  long long maxValue() const;
  /** Sets the maximum value. */
  void setMaxValue(long long max);
  /** Clears the maximum value. */
  void clearMaxValue();

  /** Retuns @c true if the integer has a default value. */
  bool hasDefaultValue() const;
  /** Returns the default value. */
  long long defaultValue() const;
  /** Sets the default value. */
  void setDefaultValue(long long value);
  /** Clears the default value. */
  void clearDefaultValue();

  /** Decodes an iteger in the given element at the specified address. */
  QVariant value(const Element *element, const Address &address) const;

protected:
  /** Helper function to decode BCD numbers. */
  static uint16_t fromBCD4(uint16_t bcd);
  /** Helper function to decode BCD numbers. */
  static uint32_t fromBCD8(uint32_t bcd);

protected:
  /** Holds the format. */
  Format _format;
  /** Holds the endianess. */
  Endian _endian;
  /** Holds the minimum value, if @c std::numeric_limits<long long>().max() it is not set. */
  long long _minValue;
  /** Holds the maximum value, if @c std::numeric_limits<long long>().max() it is not set. */
  long long _maxValue;
  /** Holds the default value, if @c std::numeric_limits<long long>().max() it is not set. */
  long long _defaultValue;
};



/** Represents a possible value of an enum pattern. */
class EnumFieldPatternItem: public PatternMeta
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit EnumFieldPatternItem(QObject *parent = nullptr);

  bool serialize(QXmlStreamWriter &writer) const;

  /** Returns @c true, if a value is assigned to the enum entry. */
  bool hasValue() const;
  /** Returns the enum entry value. */
  unsigned int value() const;
  /** Sets the enum entry value. */
  bool setValue(unsigned int value);

  /** Decodes the enum value within the given element at the specified address. */
  QVariant value(const Element *element, const Offset &offset) const;

protected:
  /** The value (if set, @c std::numeric_limits<unsigned int>().max() otherwise). */
  unsigned int _value;
};


/** Represets a enum field pattern. This is just a unsigned integer field of variable width,
 * where some possible values are assigned to enum entries.
 * @ingroup pattern */
class EnumFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit EnumFieldPattern(QObject *parent=nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Adds an item to the enum. Takes ownership. */
  bool addItem(EnumFieldPatternItem *item);
  /** Returns the number of items. */
  unsigned int numItems() const;
  /** Returns the specified item by index. */
  EnumFieldPatternItem *item(unsigned int n) const;
  /** Returns the specified item by value if found and @c nullptr otherwise. */
  EnumFieldPatternItem *itemByValue(unsigned int value) const;
  /** Removes and deletes the n-th item. */
  bool deleteItem(unsigned int n);

  /** Specifies the exact width of the field. */
  void setWidth(const Size &size);

  /** Decodes the enum value within the given element at the specified address. */
  QVariant value(const Element *element, const Address &address) const;

signals:
  /** Gets emitted, if an item is added. */
  void itemAdded(unsigned int idx);
  /** Gets emitted, once an item is removed. */
  void itemDeleted(unsigned int idx);

protected:
  /** The list of enum entries. */
  QList<EnumFieldPatternItem *> _items;
};


/** Represents a string valued field.
 * This implements ASCII and Unicode strings.
 * @ingroup pattern */
class StringFieldPattern: public FieldPattern
{
  Q_OBJECT

public:
  /** Possible formats. */
  enum class Format {
    ASCII,    ///< ASCII string.
    Unicode   ///< Unicode string.
  };
  Q_ENUM(Format)

public:
  /** Default constructor. */
  explicit StringFieldPattern(QObject *parent = nullptr);

  bool verify() const;
  bool serialize(QXmlStreamWriter &writer) const;

  /** Decodes the string in the given element at the specified address. */
  QVariant value(const Element *element, const Address &address) const;

  /** Returns the string format. */
  Format format() const;
  /** Sets the string format. */
  void setFormat(Format format);

  /** Returns the (max) number of chars. I.e., the string length. */
  unsigned int numChars() const;
  /** Sets the (max) number of chars. */
  void setNumChars(unsigned int n);

  /** Returns the value to terminate/pad the string. */
  unsigned int padValue() const;
  /** Sets the value to terminate/pad the string. */
  void setPadValue(unsigned int pad);

protected:
  /** Holds the format. */
  Format _format;
  /** Holds the number of chars. */
  unsigned int _numChars;
  /** Holds the value to pad the string. */
  unsigned int _padValue;
};


#endif // CODEPLUGPATTERN_HH
