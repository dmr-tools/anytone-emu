#include "codeplugpattern.hh"
#include "logger.hh"
#include "image.hh"
#include <QVariant>
#include <QtEndian>


/* ********************************************************************************************* *
 * Implementation of PatternMeta
 * ********************************************************************************************* */
PatternMeta::PatternMeta(QObject *parent)
  : QObject{parent}, _name(), _description(), _fwVersion()
{
  // pass...
}

PatternMeta &
PatternMeta::operator =(const PatternMeta &other) {
  _name = other._name;
  _description = other._description;
  _fwVersion = other._fwVersion;
  return *this;
}

const QString &
PatternMeta::name() const {
  return _name;
}

void
PatternMeta::setName(const QString &name) {
  _name = name;
}

bool
PatternMeta::hasDescription() const {
  return ! _description.isEmpty();
}

const QString &
PatternMeta::description() const {
  return _description;
}

void
PatternMeta::setDescription(const QString &description) {
  _description = description;
}

bool
PatternMeta::hasFirmwareVersion() const {
  return ! _fwVersion.isEmpty();
}

const QString &
PatternMeta::firmwareVersion() const {
  return _fwVersion;
}

void
PatternMeta::setFirmwareVersion(const QString &version) {
  _fwVersion = version;
}



/* ********************************************************************************************* *
 * Implementation of AbstractPattern
 * ********************************************************************************************* */
AbstractPattern::AbstractPattern(QObject *parent)
  : QObject{parent}, _meta(), _offset(), _size()
{
  // pass...
}

const PatternMeta &
AbstractPattern::meta() const {
  return _meta;
}

PatternMeta &
AbstractPattern::meta() {
  return _meta;
}

bool
AbstractPattern::hasOffset() const {
  return _offset.isValid();
}

const Offset &
AbstractPattern::offset() const {
  return _offset;
}

void
AbstractPattern::setOffset(const Offset &offset) {
  _offset = offset;
}

bool
AbstractPattern::hasSize() const {
  return _size.isValid();
}

const Offset &
AbstractPattern::size() const {
  return _size;
}



/* ********************************************************************************************* *
 * Implementation of StructuredPattern
 * ********************************************************************************************* */
StructuredPattern::StructuredPattern()
{
  // pass...
}

StructuredPattern::~StructuredPattern() {
  // pass...
}



/* ********************************************************************************************* *
 * Implementation of GroupPattern
 * ********************************************************************************************* */
GroupPattern::GroupPattern(QObject *parent)
  : AbstractPattern{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodeplugPattern
 * ********************************************************************************************* */
CodeplugPattern::CodeplugPattern(QObject *parent)
  : GroupPattern(parent), _content()
{
  // pass...
}

bool
CodeplugPattern::verify() const {
  // A codeplug cannot be empty
  if (0 == _content.size())
    return false;

  return true;
}

bool
CodeplugPattern::addChildPattern(AbstractPattern *pattern) {
  // If the pattern is the first element and has no offset within the codeplug, I do not know,
  // where to put it.
  if (! pattern->hasOffset())
    return false;

  pattern->setParent(this);
  _content.append(pattern);

  return true;
}

unsigned int
CodeplugPattern::numChildPattern() const {
  return _content.size();
}

AbstractPattern *
CodeplugPattern::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}


/* ********************************************************************************************* *
 * Implementation of RepeatPattern
 * ********************************************************************************************* */
RepeatPattern::RepeatPattern(QObject *parent)
  : GroupPattern{parent}, _minRepetition(0), _maxRepetition(0), _step(), _subpattern(nullptr)
{
  // pass...
}

bool
RepeatPattern::verify() const {
  if (nullptr == _subpattern)
    return false;
  return _subpattern->verify();
}

unsigned int
RepeatPattern::minRepetition() const {
  return _minRepetition;
}
void
RepeatPattern::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

unsigned int
RepeatPattern::maxRepetition() const {
  return _maxRepetition;
}
void
RepeatPattern::setMaxRepetition(unsigned int rep) {
  _maxRepetition = rep;
}

AbstractPattern *
RepeatPattern::subpattern() const {
  return _subpattern;
}
bool
RepeatPattern::addChildPattern(AbstractPattern *subpattern) {
  if (_subpattern)
    return false;

  _subpattern = subpattern;
  _subpattern->setParent(this);

  // Update stepsize, if not set
  if ((!_step.isValid()) && (_subpattern->size().isValid()))
    _step = _subpattern->size();

  return true;
}

unsigned int
RepeatPattern::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPattern *
RepeatPattern::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

const Offset &
RepeatPattern::step() const {
  return _step;
}
void
RepeatPattern::setStep(const Offset &step) {
  _step = step;
}



/* ********************************************************************************************* *
 * Implementation of BlockPattern
 * ********************************************************************************************* */
BlockPattern::BlockPattern(QObject *parent)
  : AbstractPattern{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of BlockRepeatPattern
 * ********************************************************************************************* */
BlockRepeatPattern::BlockRepeatPattern(QObject *parent)
  : BlockPattern{parent}, _minRepetition(0), _maxRepetition(0), _subpattern(nullptr)
{
  // pass...
}

bool
BlockRepeatPattern::verify() const {
  if (! _subpattern)
    return false;
  return _subpattern->verify();
}

unsigned int
BlockRepeatPattern::minRepetition() const {
  return _minRepetition;
}
void
BlockRepeatPattern::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

unsigned int
BlockRepeatPattern::maxRepetition() const {
  return _maxRepetition;
}
void
BlockRepeatPattern::setMaxRepetition(unsigned int rep) {
  _maxRepetition = rep;
}

FixedPattern *
BlockRepeatPattern::subpattern() const {
  return _subpattern;
}
bool
BlockRepeatPattern::addChildPattern(AbstractPattern *subpattern) {
  if (! subpattern->is<FixedPattern>())
    return false;

  if (_subpattern)
    return false;

  _subpattern = subpattern->as<FixedPattern>();
  _subpattern->setParent(this);

  return true;
}

unsigned int
BlockRepeatPattern::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPattern *
BlockRepeatPattern::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}


/* ********************************************************************************************* *
 * Implementation of FixedPattern
 * ********************************************************************************************* */
FixedPattern::FixedPattern(QObject *parent)
  : BlockPattern(parent)
{
  // pass...
}

bool
FixedPattern::verify() const {
  return hasSize();
}



/* ********************************************************************************************* *
 * Implementation of ElementPattern
 * ********************************************************************************************* */
ElementPattern::ElementPattern(QObject *parent)
  : FixedPattern(parent), _content()
{
  // pass...
}

bool
ElementPattern::verify() const {
  if (! FixedPattern::verify())
    return false;

  foreach(FixedPattern *pattern, _content)
    if (! pattern->verify())
      return false;

  return true;
}

bool
ElementPattern::addChildPattern(AbstractPattern *pattern) {
  if (! pattern->is<FixedPattern>())
    return false;

  // Compute offset, where to put pattern
  Offset offset = Offset::fromByte(0);
  if (! _content.isEmpty())
    offset = _content.back()->offset() + _content.back()->size();

  // If a offset is set -> check it
  if (pattern->hasOffset() && (pattern->offset() != offset))
    return false;

  // Set/update offset
  pattern->setOffset(offset);
  // update own size
  _size += pattern->size();
  // add to content
  pattern->setParent(this);
  _content.append(pattern->as<FixedPattern>());

  return true;
}

unsigned int
ElementPattern::numChildPattern() const {
  return _content.size();
}

AbstractPattern *
ElementPattern::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}


/* ********************************************************************************************* *
 * Implementation of FixedRepeatPattern
 * ********************************************************************************************* */
FixedRepeatPattern::FixedRepeatPattern(QObject *parent)
  : FixedPattern(parent), _repetition(0), _subpattern(nullptr)
{
  // pass...
}

bool
FixedRepeatPattern::verify() const {
  if (! FixedPattern::verify())
    return false;

  if ((nullptr == _subpattern) || (0 == _repetition))
    return false;

  return true;
}

unsigned int
FixedRepeatPattern::repetition() const {
  return _repetition;
}
void
FixedRepeatPattern::setRepetition(unsigned int n) {
  _repetition = n;
  if (_subpattern)
    _size = _subpattern->size()*_repetition;
}

FixedPattern *
FixedRepeatPattern::subpattern() const {
  return _subpattern;
}
bool
FixedRepeatPattern::addChildPattern(AbstractPattern *pattern) {
  if (! pattern->is<FixedPattern>())
    return false;

  if (_subpattern)
    return false;

  _subpattern = pattern->as<FixedPattern>();
  _subpattern->setParent(this);

  _size = _subpattern->size()*_repetition;
  return true;
}

unsigned int
FixedRepeatPattern::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPattern *
FixedRepeatPattern::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}


/* ********************************************************************************************* *
 * Implementation of FieldPattern
 * ********************************************************************************************* */
FieldPattern::FieldPattern(QObject *parent)
  : FixedPattern{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of UnknownFieldPattern
 * ********************************************************************************************* */
UnknownFieldPattern::UnknownFieldPattern(QObject *parent)
  : FieldPattern{parent}
{
  // pass...
}

bool
UnknownFieldPattern::verify() const {
  return FixedPattern::verify();
}

void
UnknownFieldPattern::setSize(const Offset &size) {
  _size = size;
}

QVariant
UnknownFieldPattern::value(const Element *element, const Offset &offset) const {
  if ((offset+size())>=Offset::fromByte(element->address()+element->size()))
    return QVariant();
  Offset within = offset - element->address();
  return element->data().mid(within.byte(), size().byte());
}


/* ********************************************************************************************* *
 * Implementation of UnusedFieldPattern
 * ********************************************************************************************* */
UnusedFieldPattern::UnusedFieldPattern(QObject *parent)
  : FieldPattern(parent), _content()
{
  // pass...
}

bool
UnusedFieldPattern::verify() const {
  if (! FieldPattern::verify())
    return false;
  if (_content.isEmpty())
    return false;
  return true;
}

const QByteArray &
UnusedFieldPattern::content() const {
  return _content;
}

bool
UnusedFieldPattern::setContent(const QByteArray &content) {
  if (! _content.isEmpty())
    return false;

  _content = content;

  if (! hasSize())
    _size = Offset::fromByte(content.size());

  return true;
}

QVariant
UnusedFieldPattern::value(const Element *element, const Offset &offset) const {
  Offset within = offset - element->address();
  return element->data().mid(within.byte(), size().byte());
}



/* ********************************************************************************************* *
 * Implementation of IntegerFieldPattern
 * ********************************************************************************************* */
IntegerFieldPattern::IntegerFieldPattern(QObject *parent)
  : FieldPattern{parent}, _format(Format::Unsigned), _endian(Endian::Little),
    _minValue(std::numeric_limits<long long>().max()),
    _maxValue(std::numeric_limits<long long>().max()),
    _defaultValue(std::numeric_limits<long long>().max())
{
  // pass...
}

bool
IntegerFieldPattern::verify() const {
  return FieldPattern::verify();
}

void
IntegerFieldPattern::setWidth(const Offset &width) {
  _size = width;
}

IntegerFieldPattern::Format
IntegerFieldPattern::format() const {
  return _format;
}
void
IntegerFieldPattern::setFormat(Format format) {
  _format = format;
}

IntegerFieldPattern::Endian
IntegerFieldPattern::endian() const {
  return _endian;
}
void
IntegerFieldPattern::setEndian(Endian endian) {
  _endian = endian;
}

bool
IntegerFieldPattern::hasMinValue() const {
  return std::numeric_limits<long long>().max() != _minValue;
}
long long
IntegerFieldPattern::minValue() const {
  return _minValue;
}
void
IntegerFieldPattern::setMinValue(long long min) {
  _minValue = min;
}

bool
IntegerFieldPattern::hasMaxValue() const {
  return std::numeric_limits<long long>().max() != _maxValue;
}
long long
IntegerFieldPattern::maxValue() const {
  return _maxValue;
}
void
IntegerFieldPattern::setMaxValue(long long max) {
  _maxValue = max;
}

bool
IntegerFieldPattern::hasDefaultValue() const {
  return std::numeric_limits<long long>().max() != _defaultValue;
}
long long
IntegerFieldPattern::defaultValue() const {
  return _defaultValue;
}
void
IntegerFieldPattern::setDefaultValue(long long value) {
  _defaultValue = value;
}

QVariant
IntegerFieldPattern::value(const Element *element, const Offset &offset) const {
  if ((offset+size())>=Offset::fromByte(element->address()+element->size())) {
    logError() << "Cannot decode integer, extends the element bounds.";
    return QVariant();
  }

  Offset within = offset - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {
    if (8 < (offset.bit()+size().bits())) {
      logWarn() << "Cannot decode integer, bitpattern extens across bytes.";
      return QVariant();
    }
    unsigned int shift = 8 - (offset.bit()+size().bits());
    unsigned int mask  = (1<<size().bits())-1;
    uint8_t value = (uint8_t(element->data().at(within.byte())) >> shift) & mask;
    return QVariant::fromValue(value) ;
  }

  if (size().bits() == 16) {
    if (offset.bit()) {
      logWarn() << "Cannot decode int16, values does not align with bytes.";
      return QVariant();
    }
    const char *ptr = element->data().mid(within.byte(),2).constData();
    if (Format::Signed == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((int16_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((int16_t *)ptr)));
    } else if (Format::Unsigned == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((uint16_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((uint16_t *)ptr)));
    } else if (Format::BCD == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(fromBCD4le(*((uint16_t *)ptr)));
      return QVariant::fromValue(fromBCD4be(*((uint16_t *)ptr)));
    }
    return QVariant();
  }

  if (size().bits() <= 32) {
    if (offset.bit()) {
      logWarn() << "Cannot decode int16, values does not align with bytes.";
      return QVariant();
    }
    const char *ptr = element->data().mid(within.byte(),4).constData();
    if (Format::Signed == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((int32_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((int32_t *)ptr)));
    } else if (Format::Unsigned == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((uint32_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((uint32_t *)ptr)));
    } else if (Format::BCD == _format) {
      if (Endian::Little == _endian)
        return QVariant::fromValue(fromBCD8le(*((uint32_t *)ptr)));
      return QVariant::fromValue(fromBCD8be(*((uint32_t *)ptr)));
    }
    return QVariant();
  }

  return QVariant();
}

uint16_t
IntegerFieldPattern::fromBCD4le(uint16_t bcd) {
  uint16_t res = 0;
  for (int i=0; i<4; i++) {
    res *= 10;
    res += bcd & 0xf;
    bcd >>= 4;
  }
  return res;
}

uint16_t
IntegerFieldPattern::fromBCD4be(uint16_t bcd) {
  uint16_t res = 0;
  for (int i=0; i<4; i++) {
    res *= 10;
    res += (bcd & 0xf000)>>12;
    bcd <<= 4;
  }
  return res;
}

uint32_t
IntegerFieldPattern::fromBCD8le(uint32_t bcd) {
  uint32_t res = 0;
  for (int i=0; i<8; i++) {
    res *= 10;
    res += bcd & 0xf;
    bcd >>= 4;
  }
  return res;
}

uint32_t
IntegerFieldPattern::fromBCD8be(uint32_t bcd) {
  uint32_t res = 0;
  for (int i=0; i<8; i++) {
    res *= 10;
    res += (bcd & 0xf0000000)>>28;
    bcd <<= 4;
  }
  return res;
}


/* ********************************************************************************************* *
 * Implementation of EnumFieldPatternItem
 * ********************************************************************************************* */
EnumFieldPatternItem::EnumFieldPatternItem(QObject *parent)
  : PatternMeta(parent), _value(std::numeric_limits<unsigned int>().max())
{
  // pass...
}

bool
EnumFieldPatternItem::hasValue() const {
  return std::numeric_limits<unsigned int>().max() != _value;
}

unsigned int
EnumFieldPatternItem::value() const {
  return _value;
}

bool
EnumFieldPatternItem::setValue(unsigned int value) {
  if (hasValue())
    return false;
  _value = value;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of EnumFieldPattern
 * ********************************************************************************************* */
EnumFieldPattern::EnumFieldPattern(QObject *parent)
  : FieldPattern(parent), _items()
{
  // pass...
}

bool
EnumFieldPattern::verify() const {
  if (! FieldPattern::verify())
    return false;
  return 0 != _items.size();
}

bool
EnumFieldPattern::addItem(EnumFieldPatternItem *item) {
  item->setParent(this);
  _items.append(item);
  return true;
}

unsigned int
EnumFieldPattern::numItems() const {
  return _items.size();
}

EnumFieldPatternItem *
EnumFieldPattern::item(unsigned int n) const {
  if (n >= _items.size())
    return nullptr;
  return _items[n];
}

QVariant
EnumFieldPattern::value(const Element *element, const Offset &offset) const {
  if ((offset+size())>=Offset::fromByte(element->address()+element->size())) {
    logError() << "Cannot decode enum, extends the element bounds.";
    return QVariant();
  }

  Offset within = offset - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {
    if (8 < (offset.bit()+size().bits())) {
      logWarn() << "Cannot decode enum, bitpattern extens across bytes.";
      return QVariant();
    }
    unsigned int shift = 8 - (offset.bit()+size().bits());
    unsigned int mask  = (1<<size().bits())-1;
    return QVariant((unsigned int) (uint8_t(element->data().at(within.byte())) & mask) >> shift);
  }

  return QVariant();
}