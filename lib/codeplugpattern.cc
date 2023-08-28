#include "codeplugpattern.hh"
#include "logger.hh"
#include "image.hh"
#include "codeplugpatternparser.hh"

#include <QVariant>
#include <QtEndian>
#include <QXmlStreamWriter>


/* ********************************************************************************************* *
 * Implementation of PatternMeta
 * ********************************************************************************************* */
PatternMeta::PatternMeta(QObject *parent)
  : QObject{parent}, _name(), _description(), _fwVersion(), _flags(Flags::None)
{
  // pass...
}

bool
PatternMeta::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("meta");

  writer.writeStartElement("name");
  writer.writeCharacters(name());
  writer.writeEndElement();

  if (hasDescription()) {
    writer.writeStartElement("description");
    writer.writeCharacters(description());
    writer.writeEndElement();
  }

  if (hasFirmwareVersion()) {
    writer.writeStartElement("firmware");
    writer.writeCharacters(firmwareVersion());
    writer.writeEndElement();
  }

  switch(flags()) {
  case Flags::None: break;
  case Flags::Done: writer.writeEmptyElement("done"); break;
  case Flags::NeedsReview: writer.writeEmptyElement("needs-review"); break;
  case Flags::Incomplete: writer.writeEmptyElement("incomplete"); break;
  }

  writer.writeEndElement();
  return true;
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
  emit modified();
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
  emit modified();
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
  emit modified();
}

PatternMeta::Flags
PatternMeta::flags() const {
  return _flags;
}
void
PatternMeta::setFlags(Flags flags) {
  _flags = flags;
  emit modified();
}


/* ********************************************************************************************* *
 * Implementation of AbstractPattern
 * ********************************************************************************************* */
AbstractPattern::AbstractPattern(QObject *parent)
  : QObject{parent}, _meta(), _address(), _size()
{
  connect(&_meta, &PatternMeta::modified, this, &AbstractPattern::onMetaModified);
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
AbstractPattern::hasImplicitAddress() const {
  return parent() && (nullptr==qobject_cast<CodeplugPattern *>(parent()));
}
bool
AbstractPattern::hasAddress() const {
  return _address.isValid();
}

const Address &
AbstractPattern::address() const {
  return _address;
}

void
AbstractPattern::setAddress(const Address& offset) {
  _address = offset;
}

bool
AbstractPattern::hasSize() const {
  return _size.isValid();
}

const Size &
AbstractPattern::size() const {
  return _size;
}

void
AbstractPattern::onMetaModified() {
  emit modified(this);
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
CodeplugPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("codeplug");

  if (! _meta.serialize(writer))
    return false;

  foreach(AbstractPattern *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();

  return true;
}


int
CodeplugPattern::indexOf(const AbstractPattern *pattern) const {
  return _content.indexOf(pattern);
}

unsigned int
CodeplugPattern::numChildPattern() const {
  return _content.size();
}

bool
CodeplugPattern::addChildPattern(AbstractPattern *pattern) {
  // If the pattern is the first element and has no offset within the codeplug, I do not know,
  // where to put it.
  if (! pattern->hasAddress())
    return false;

  unsigned int idx = _content.size();
  pattern->setParent(this);
  _content.append(pattern);
  connect(pattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(pattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(pattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(pattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, idx);

  return true;
}

AbstractPattern *
CodeplugPattern::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}

bool
CodeplugPattern::deleteChild(unsigned int n) {
  if (n >= _content.size())
    return false;

  AbstractPattern *pattern = _content[n];

  emit removing(this, n);
  _content.remove(n);
  emit removed(this, n);

  pattern->deleteLater();
  return true;
}


CodeplugPattern *
CodeplugPattern::load(const QString &filename) {
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "': " << file.errorString() << ".";
    return nullptr;
  }

  CodeplugPatternParser parser;
  QXmlStreamReader reader(&file);
  if (! parser.parse(reader)) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "', cannot parse pattern: " << parser.errorMessage() << ".";
    return nullptr;
  }

  if (! parser.topIs<CodeplugPattern>()) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "': Files does not contain a codeplug pattern.";
    return nullptr;
  }

  return parser.popAs<CodeplugPattern>();
}

bool
CodeplugPattern::save(const QString &filename) {
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly)) {
    logError() << "Cannot save codeplug to '" << filename << "': " << file.errorString() << ".";
    return false;
  }

  bool ok = save(&file);
  file.flush();
  file.close();

  return ok;
}

bool
CodeplugPattern::save(QIODevice *device) {
  QXmlStreamWriter writer(device);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  if (! this->serialize(writer)) {
    logError() << "Cannot serialize codeplug.";
    return false;
  }
  writer.writeEndDocument();

  return true;
}


/* ********************************************************************************************* *
 * Implementation of RepeatPattern
 * ********************************************************************************************* */
RepeatPattern::RepeatPattern(QObject *parent)
  : GroupPattern{parent}, _minRepetition(std::numeric_limits<unsigned int>::max()),
    _maxRepetition(std::numeric_limits<unsigned int>::max()), _step(), _subpattern(nullptr)
{
  // pass...
}

bool
RepeatPattern::verify() const {
  if (nullptr == _subpattern)
    return false;
  return _subpattern->verify();
}

bool
RepeatPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("repeat");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());
  writer.writeAttribute("step", _step.toString());
  if (hasMinRepetition())
    writer.writeAttribute("min", QString::number(minRepetition()));
  if (hasMaxRepetition())
    writer.writeAttribute("max", QString::number(maxRepetition()));

  if (! _meta.serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
  return true;
}


bool
RepeatPattern::hasMinRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _minRepetition;
}
unsigned int
RepeatPattern::minRepetition() const {
  return _minRepetition;
}
void
RepeatPattern::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

bool
RepeatPattern::hasMaxRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _maxRepetition;
}
unsigned int
RepeatPattern::maxRepetition() const {
  return _maxRepetition;
}
void
RepeatPattern::setMaxRepetition(unsigned int rep) {
  _maxRepetition = rep;
}


int
RepeatPattern::indexOf(const AbstractPattern *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

unsigned int
RepeatPattern::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
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
  connect(_subpattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(_subpattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(_subpattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(_subpattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, 0);

  return true;
}

AbstractPattern *
RepeatPattern::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

bool
RepeatPattern::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  AbstractPattern *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  return true;
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
  : BlockPattern{parent}, _minRepetition(std::numeric_limits<unsigned int>::max()),
    _maxRepetition(std::numeric_limits<unsigned int>::max()), _subpattern(nullptr)
{
  // pass...
}

bool
BlockRepeatPattern::verify() const {
  if (! _subpattern)
    return false;
  return _subpattern->verify();
}

bool
BlockRepeatPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("repeat");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());
  if (hasMinRepetition())
    writer.writeAttribute("min", QString::number(minRepetition()));
  if (hasMaxRepetition())
    writer.writeAttribute("max", QString::number(maxRepetition()));

  if (! _meta.serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
  return true;
}

bool
BlockRepeatPattern::hasMinRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _minRepetition;
}
unsigned int
BlockRepeatPattern::minRepetition() const {
  return _minRepetition;
}
void
BlockRepeatPattern::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

bool
BlockRepeatPattern::hasMaxRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _maxRepetition;
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
  connect(_subpattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(_subpattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(_subpattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(_subpattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, 0);

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

int
BlockRepeatPattern::indexOf(const AbstractPattern *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

bool
BlockRepeatPattern::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  AbstractPattern *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  return true;
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
ElementPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("element");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  if (! _meta.serialize(writer))
    return false;

  foreach(FixedPattern *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();
  return true;
}

bool
ElementPattern::addChildPattern(AbstractPattern *pattern) {
  if (! pattern->is<FixedPattern>())
    return false;

  // Compute offset, where to put pattern
  Address addr = Address::zero();
  if (! _content.isEmpty())
    addr = _content.back()->address() + _content.back()->size();
  // If a offset is set -> check it
  if (pattern->hasAddress() && (pattern->address() != addr))
    return false;

  // Set/update offset
  pattern->setAddress(addr);
  // update own size
  if (_size.isValid())
    _size += pattern->size();
  else
    _size = pattern->size();

  // add to content
  unsigned int idx = _content.size();
  pattern->setParent(this);
  _content.append(pattern->as<FixedPattern>());
  connect(pattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(pattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(pattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(pattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, idx);
  emit resized(this, _size);

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

int
ElementPattern::indexOf(const AbstractPattern *pattern) const {
  return _content.indexOf(pattern);
}

bool
ElementPattern::deleteChild(unsigned int n) {
  if (n >= _content.size())
    return false;

  FixedPattern *pattern = _content[n];
  emit removing(this, n);
  _content.remove(n);
  emit removed(this, n);

  if (pattern->hasSize())
      _size -= pattern->size();

  // Adjust own size and addresses of childen
  Address addr = (0 != n) ? (_content[n-1]->address()+_content[n-1]->size()) : Address::zero();
  for (int i=n; i<_content.size(); i++) {
    _content[i]->setAddress(addr);
    addr += _content[i]->size();
  }

  emit resized(this, _size);
  return true;
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

bool
FixedRepeatPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("repeat");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());
  writer.writeAttribute("n", QString::number(repetition()));

  if (! _meta.serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
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
  connect(_subpattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(_subpattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(_subpattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(_subpattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, 0);

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

int
FixedRepeatPattern::indexOf(const AbstractPattern *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

bool
FixedRepeatPattern::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  FixedPattern *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  _size = Size();
  emit resized(this, _size);

  return true;
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
  meta().setName("Unknown data");
}

bool
UnknownFieldPattern::verify() const {
  return FixedPattern::verify();
}

bool
UnknownFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("unknown");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());
  writer.writeAttribute("width", size().toString());

  if (! _meta.serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}

void
UnknownFieldPattern::setSize(const Offset &size) {
  _size = size;
}

QVariant
UnknownFieldPattern::value(const Element *element, const Address& address) const {
  if ((address+size()) > (element->address()+element->size()))
    return QVariant();
  Offset within = address - element->address();
  return element->data().mid(within.byte(), size().byte());
}


/* ********************************************************************************************* *
 * Implementation of UnusedFieldPattern
 * ********************************************************************************************* */
UnusedFieldPattern::UnusedFieldPattern(QObject *parent)
  : FieldPattern(parent), _content()
{
  meta().setName("Unused data");
}

bool
UnusedFieldPattern::verify() const {
  if (! FieldPattern::verify())
    return false;
  if (_content.isEmpty())
    return false;
  return true;
}

bool
UnusedFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("unused");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  writer.writeAttribute("width", size().toString());

  if (! _meta.serialize(writer))
    return false;

  if (! content().isEmpty())
    writer.writeCharacters(content().toHex());

  writer.writeEndElement();
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

void
UnusedFieldPattern::setSize(const Size &size) {
  _size = size;
}

QVariant
UnusedFieldPattern::value(const Element *element, const Address& address) const {
  Offset within = address - element->address();
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

bool
IntegerFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("int");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  writer.writeAttribute("width", size().toString());

  switch(format()) {
  case Format::Unsigned: writer.writeAttribute("format", "unsigned"); break;
  case Format::Signed: writer.writeAttribute("format", "signed"); break;
  case Format::BCD: writer.writeAttribute("format", "bcd"); break;
  }

  if (size().bits() > 8) {
    switch(endian()) {
    case Endian::Little: writer.writeAttribute("endian", "little"); break;
    case Endian::Big: writer.writeAttribute("endian", "big"); break;
    }
  }

  if (hasDefaultValue()) {
    writer.writeAttribute("default", QString::number(defaultValue()));
  }

  if (hasMinValue()) {
    writer.writeAttribute("min", QString::number(minValue()));
  }
  if (hasMaxValue()) {
    writer.writeAttribute("max", QString::number(maxValue()));
  }

  if (! _meta.serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}

void
IntegerFieldPattern::setWidth(const Offset &width) {
  _size = width;
  emit resized(this, _size);
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
void
IntegerFieldPattern::clearDefaultValue() {
  _defaultValue = std::numeric_limits<long long>().max();
}

QVariant
IntegerFieldPattern::value(const Element *element, const Address& address) const {
  if ((address+size()) >  element->address() + element->size()) {
    logError() << "Cannot decode integer, extends the element bounds.";
    return QVariant();
  }

  Offset within = address - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {   // int8_t or smaller
    if ((address.bit()+1)<size().bits()) {
      logWarn() << "Cannot decode integer, bitpattern extens across bytes.";
      return QVariant();
    }
    unsigned int shift = (address.bit()+1)-size().bits();
    unsigned int mask  = (1<<size().bits())-1;
    uint8_t value = (uint8_t(element->data().at(within.byte())) >> shift) & mask;
    return QVariant::fromValue(value) ;
  }

  if (size().bits() == 16) {
    if (!address.byteAligned()) {
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
    if (!address.byteAligned()) {
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
EnumFieldPatternItem::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("item");
  writer.writeAttribute("value", QString::number(value()));

  writer.writeStartElement("name");
  writer.writeCharacters(name());
  writer.writeEndElement();

  if (hasDescription()) {
    writer.writeStartElement("description");
    writer.writeCharacters(description());
    writer.writeEndElement();
  }

  if (hasFirmwareVersion()) {
    writer.writeStartElement("firmware");
    writer.writeCharacters(firmwareVersion());
    writer.writeEndElement();
  }

  switch(flags()) {
  case Flags::None: break;
  case Flags::Done: writer.writeEmptyElement("done"); break;
  case Flags::NeedsReview: writer.writeEmptyElement("needs-review"); break;
  case Flags::Incomplete: writer.writeEmptyElement("incomplete"); break;
  }

  writer.writeEndElement();
  return true;
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
EnumFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("enum");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  writer.writeAttribute("width", size().toString());

  if (! _meta.serialize(writer))
    return false;

  foreach(EnumFieldPatternItem *item, _items) {
    if (! item->serialize(writer))
      return false;
  }

  writer.writeEndElement();
  return true;
}

void
EnumFieldPattern::setWidth(const Size &size) {
  _size = size;
  emit resized(this, _size);
}

bool
EnumFieldPattern::addItem(EnumFieldPatternItem *item) {
  item->setParent(this);
  unsigned int idx = _items.size();
  _items.append(item);
  emit itemAdded(idx);
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

bool
EnumFieldPattern::deleteItem(unsigned int n) {
  if (n >= _items.size())
    return false;
  EnumFieldPatternItem *item = _items[n];
  emit itemDeleted(n);
  _items.remove(n);
  item->deleteLater();
  return true;
}

QVariant
EnumFieldPattern::value(const Element *element, const Address& address) const {
  if ((address+size()) > element->address()+element->size()) {
    logError() << "Cannot decode enum, extends the element bounds.";
    return QVariant();
  }

  Offset within = address - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {
    if (8 < (address.bit()+size().bits())) {
      logWarn() << "Cannot decode enum, bitpattern extens across bytes.";
      return QVariant();
    }
    unsigned int shift = 8 - (address.bit()+size().bits());
    unsigned int mask  = (1<<size().bits())-1;
    return QVariant((unsigned int) (uint8_t(element->data().at(within.byte())) & mask) >> shift);
  }

  return QVariant();
}



/* ********************************************************************************************* *
 * Implementation of StringFieldPattern
 * ********************************************************************************************* */
StringFieldPattern::StringFieldPattern(QObject *parent)
  : FieldPattern{parent}, _format(Format::ASCII), _numChars(0), _padValue(0)
{
  _size = Size::zero();
}

bool
StringFieldPattern::verify() const {
  return true;
}

bool
StringFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("enum");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  switch(format()) {
  case Format::ASCII: writer.writeAttribute("format", "ascii");
  case Format::Unicode: writer.writeAttribute("format", "unicode");
  }

  writer.writeAttribute("chars", QString::number(_numChars));
  writer.writeAttribute("pad", QString::number(_padValue));

  if (! _meta.serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}

QVariant
StringFieldPattern::value(const Element *element, const Address &address) const {
  QByteArray mid = element->data().mid(address.byte(), size().byte());
  switch (format()) {
  case Format::ASCII: return QString(mid);
  case Format::Unicode: return QString(mid);
  }
  return QVariant();
}

StringFieldPattern::Format
StringFieldPattern::format() const {
  return _format;
}
void
StringFieldPattern::setFormat(Format format) {
  _format = format;
  setNumChars(_numChars);
}

unsigned int
StringFieldPattern::numChars() const {
  return _numChars;
}
void
StringFieldPattern::setNumChars(unsigned int n) {
  _numChars = n;
  switch (format()) {
  case Format::ASCII: _size = Size::fromByte(_numChars); break;
  case Format::Unicode: _size = Size::fromByte(_numChars*2); break;
  }
  emit resized(this, _size);
}

unsigned int
StringFieldPattern::padValue() const {
  return _padValue;
}
void
StringFieldPattern::setPadValue(unsigned int pad) {
  _padValue = pad;
}
