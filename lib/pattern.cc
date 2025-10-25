#include "pattern.hh"
#include "logger.hh"
#include "image.hh"
#include "annotation.hh"
#include "codeplugpatternparser.hh"

#include <QVariant>
#include <QtEndian>
#include <QXmlStreamWriter>


/* ********************************************************************************************* *
 * Implementation of PatternMeta
 * ********************************************************************************************* */
PatternMeta::PatternMeta(QObject *parent)
  : QObject{parent}, _name(), _shortName(), _brief(), _description(),
    _fwVersion(), _flags(Flags::None)
{
  // pass...
}

bool
PatternMeta::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("meta");

  writer.writeStartElement("name");
  writer.writeCharacters(name());
  writer.writeEndElement();

  if (hasShortName()) {
    writer.writeStartElement("short-name");
    writer.writeCharacters(shortName());
    writer.writeEndElement();
  }

  if (hasBriefDescription()) {
    writer.writeStartElement("brief");
    writer.writeCharacters(briefDescription());
    writer.writeEndElement();
  }

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
  _shortName = other._shortName;
  _brief = other._brief;
  _description = other._description;
  _fwVersion = other._fwVersion;
  _flags = other._flags;
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
PatternMeta::hasShortName() const {
  return !_shortName.isEmpty();
}

const QString &
PatternMeta::shortName() const {
  return _shortName;
}

void
PatternMeta::setShortName(const QString &name) {
  _shortName = name;
  emit modified();
}

bool
PatternMeta::hasBriefDescription() const {
  return ! _brief.isEmpty();
}

const QString &
PatternMeta::briefDescription() const {
  return _brief;
}

void
PatternMeta::setBriefDescription(const QString &description) {
  _brief = description;
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


PatternMeta::Flags
operator+(PatternMeta::Flags a, PatternMeta::Flags b) {
  if ((int)a < (int)b)
    return b;
  return a;
}

PatternMeta::Flags &
operator+=(PatternMeta::Flags &a, PatternMeta::Flags b) {
  return (a = a+b);
}



/* ********************************************************************************************* *
 * Implementation of AbstractPattern
 * ********************************************************************************************* */
AbstractPattern::AbstractPattern(QObject *parent)
  : QObject{parent}, _meta(), _address()
{
  connect(&_meta, &PatternMeta::modified, this, &AbstractPattern::onMetaModified);
}

AbstractPattern *
AbstractPattern::clone() const {
  AbstractPattern *pattern = qobject_cast<AbstractPattern *>(this->metaObject()->newInstance());
  pattern->_address = _address;
  pattern->_meta = _meta;
  return pattern;
}


PatternMeta::Flags
AbstractPattern::combinedFlags() const {
  return meta().flags();
}

const PatternMeta &
AbstractPattern::meta() const {
  return _meta;
}

PatternMeta &
AbstractPattern::meta() {
  return _meta;
}


const CodeplugPattern *
AbstractPattern::codeplug() const {
  if (parent() && qobject_cast<AbstractPattern *>(parent()))
    return qobject_cast<AbstractPattern *>(parent())->codeplug();
  return nullptr;
}


bool
AbstractPattern::hasImplicitAddress() const {
  return (nullptr == parent()) || (parent() && (nullptr==qobject_cast<CodeplugPattern *>(parent())));
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

bool
StructuredPattern::deleteChild(unsigned int n) {
  auto pattern = this->takeChild(n);
  if (nullptr == pattern)
    return false;
  delete pattern;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of PatternFragment
 * ********************************************************************************************* */
PatternFragment::PatternFragment(QObject *parent)
  : QObject{parent}, StructuredPattern(), _pattern(nullptr)
{
  // pass...
}

bool
PatternFragment::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartDocument();
  writer.writeStartElement("fragment");

  if (_pattern && (! _pattern->serialize(writer)))
    return false;

  writer.writeEndElement();
  writer.writeEndDocument();

  return true;
}

int
PatternFragment::indexOf(const AbstractPattern *pattern) const {
  if (nullptr == pattern)
    return -1;
  if (_pattern == pattern)
    return 0;
  return -1;
}

unsigned int
PatternFragment::numChildPattern() const {
  return (nullptr == _pattern) ? 0 : 1;
}

bool
PatternFragment::addChildPattern(AbstractPattern *pattern) {
  if (nullptr != _pattern)
    return false;

  _pattern = pattern;

  if (_pattern)
    _pattern->setParent(this);

  return true;
}

AbstractPattern *
PatternFragment::childPattern(unsigned int n) const {
  if (0 == n)
    return _pattern;
  return nullptr;
}

AbstractPattern *
PatternFragment::takeChild(unsigned int n) {
  if ((0 != n) || (nullptr ==_pattern))
    return nullptr;

  auto *ptr = _pattern;
  ptr->setParent(nullptr);
  _pattern = nullptr;

  return ptr;
}


/* ********************************************************************************************* *
 * Implementation of GroupPattern
 * ********************************************************************************************* */
GroupPattern::GroupPattern(QObject *parent)
  : AbstractPattern{parent}
{
  // pass...
}

PatternMeta::Flags
GroupPattern::combinedFlags() const {
  PatternMeta::Flags flags = AbstractPattern::combinedFlags();
  for (unsigned int i=0; i<numChildPattern(); i++)
    flags += childPattern(i)->combinedFlags();
  return flags;
}


/* ********************************************************************************************* *
 * Implementation of CodeplugPattern
 * ********************************************************************************************* */
CodeplugPattern::CodeplugPattern(QObject *parent)
  : GroupPattern(parent), _modified(false), _source(), _content()
{
  connect(this, &AbstractPattern::modified,
          this, [this](const AbstractPattern *pattern){ this->_modified = true; });
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

  if (! meta().serialize(writer))
    return false;

  foreach(AbstractPattern *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();

  return true;
}

AbstractPattern *
CodeplugPattern::clone() const {
  auto *pattern = GroupPattern::clone()->as<CodeplugPattern>();

  pattern->_modified = _modified;
  pattern->_source = _source;

  foreach (AbstractPattern *child, _content)
    pattern->addChildPattern(child->clone());

  return pattern;
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

  Address addr = pattern->address();

  // Find index, where to add item
  unsigned int idx = 0;
  QList<AbstractPattern*>::const_iterator iter = _content.constBegin();
  for (; (iter != _content.constEnd()) && (addr > (*iter)->address()); iter++, idx++);

  pattern->setParent(this);
  _content.insert(iter, pattern);

  connect(pattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(pattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(pattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(pattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, idx);
  emit modified(this);

  return true;
}

AbstractPattern *
CodeplugPattern::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}

AbstractPattern *
CodeplugPattern::takeChild(unsigned int n) {
  if (n >= _content.size())
    return nullptr;

  AbstractPattern *pattern = _content[n];
  emit removing(this, n);
  pattern->setParent(nullptr);
  _content.remove(n);
  emit removed(this, n);

  emit modified(this);

  return pattern;
}

const CodeplugPattern *
CodeplugPattern::codeplug() const {
  return this;
}

bool
CodeplugPattern::isModified() const {
  return _modified;
}

CodeplugPattern *
CodeplugPattern::load(const QString &filename, const ErrorStack &err) {
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly)) {
    errMsg(err) << "Cannot load annotation pattern from '" << filename
                << "': " << file.errorString() << ".";
    return nullptr;
  }

  CodeplugPatternParser parser;
  QXmlStreamReader reader(&file);
  if (! parser.parse(reader, XmlParser::Context(QFileInfo(file).absolutePath(), 0,0))) {
    errMsg(err) << "Cannot load annotation pattern from '" << filename
                << "', cannot parse pattern: " << parser.errorMessage() << ".";
    return nullptr;
  }

  if (! parser.topIs<CodeplugPattern>()) {
    errMsg(err) << "Cannot load annotation pattern from '" << filename
                << "': Files does not contain a codeplug pattern.";
    return nullptr;
  }

  CodeplugPattern *pattern = parser.popAs<CodeplugPattern>();
  pattern->setSource(filename);
  pattern->_modified = false;
  return pattern;
}

bool
CodeplugPattern::save() {
  if ((! _source.exists()) || (! _source.isWritable()))
    return false;
  return save(_source.absoluteFilePath());
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

  if (ok) logDebug() << "Saved pattern in '" << file.fileName() << "'.";

  return ok;
}

bool
CodeplugPattern::save(QIODevice *device) {
  QXmlStreamWriter writer(device);

  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);

  writer.writeStartDocument();
  writer.writeComment("vi: set ts=2 sw=2:");
  if (! this->serialize(writer)) {
    logError() << "Cannot serialize codeplug.";
    return false;
  }
  writer.writeEndDocument();

  _modified = false;
  return true;
}

const QFileInfo &
CodeplugPattern::source() const {
  return _source;
}

void
CodeplugPattern::setSource(const QString &filename) {
  _source = QFileInfo(filename);
}

void
CodeplugPattern::onModified() {
  _modified = true;
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

  if (! meta().serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
  return true;
}

AbstractPattern *
RepeatPattern::clone() const {
  auto pattern = GroupPattern::clone()->as<RepeatPattern>();

  if (nullptr != _subpattern)
    pattern->addChildPattern(_subpattern->clone());

  pattern->setMinRepetition(_minRepetition);
  pattern->setMaxRepetition(_maxRepetition);
  pattern->setStep(_step);

  return pattern;
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

  connect(_subpattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(_subpattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(_subpattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(_subpattern, &AbstractPattern::removed, this, &AbstractPattern::removed);

  emit added(this, 0);
  emit modified(this);

  return true;
}

AbstractPattern *
RepeatPattern::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

AbstractPattern *
RepeatPattern::takeChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return nullptr;

  AbstractPattern *pattern = _subpattern;

  emit removing(this, 0);
  _subpattern = nullptr;
  pattern->setParent(nullptr);
  emit removed(this, 0);
  emit modified(this);

  return pattern;
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
  : BlockPattern{parent}, _minRepetition(0), _maxRepetition(1), _subpattern(nullptr)
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
  writer.writeAttribute("min", QString::number(minRepetition()));
  writer.writeAttribute("max", QString::number(maxRepetition()));

  if (! meta().serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
  return true;
}

AbstractPattern *
BlockRepeatPattern::clone() const {
  auto pattern = BlockPattern::clone()->as<BlockRepeatPattern>();

  pattern->_minRepetition = _minRepetition;
  pattern->_maxRepetition = _maxRepetition;

  if (nullptr != _subpattern)
    pattern->addChildPattern(_subpattern->clone());

  return pattern;
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


PatternMeta::Flags
BlockRepeatPattern::combinedFlags() const {
  PatternMeta::Flags flags = AbstractPattern::combinedFlags();
  for (unsigned int i=0; i<numChildPattern(); i++)
    flags += childPattern(i)->combinedFlags();
  return flags;
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
  emit modified(this);

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

AbstractPattern *
BlockRepeatPattern::takeChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return nullptr;

  AbstractPattern *pattern = _subpattern;

  emit removing(this, 0);
  _subpattern = nullptr;
  pattern->setParent(nullptr);
  emit removed(this, 0);
  emit modified(this);

  return pattern;
}


/* ********************************************************************************************* *
 * Implementation of FixedPattern
 * ********************************************************************************************* */
FixedPattern::FixedPattern(QObject *parent)
  : BlockPattern(parent), _size()
{
  // pass...
}

bool
FixedPattern::verify() const {
  return hasSize();
}

AbstractPattern *
FixedPattern::clone() const {
  auto *pattern = BlockPattern::clone()->as<FixedPattern>();

  pattern->setSize(_size);

  return pattern;
}

bool
FixedPattern::hasSize() const {
  return _size.isValid();
}

const Size &
FixedPattern::size() const {
  return _size;
}

void
FixedPattern::resetSize() {
  setSize(Size());
}

void
FixedPattern::setSize(const Size &size) {
  _size = size;
  emit resized(this, _size);
  emit modified(this);
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

  if (! meta().serialize(writer))
    return false;

  foreach(FixedPattern *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();
  return true;
}

AbstractPattern *
ElementPattern::clone() const {
  auto pattern = FixedPattern::clone()->as<ElementPattern>();

  pattern->resetSize();
  foreach(FixedPattern *child, _content)
    pattern->addChildPattern(child->clone());

  return pattern;
}

PatternMeta::Flags
ElementPattern::combinedFlags() const {
  PatternMeta::Flags flags = AbstractPattern::combinedFlags();
  for (unsigned int i=0; i<numChildPattern(); i++)
    flags += childPattern(i)->combinedFlags();
  return flags;
}

bool
ElementPattern::addChildPattern(AbstractPattern *pattern) {
  if (! pattern->is<FixedPattern>()) {
    logInfo() << "Only fixed pattern can be added to ElementPattern.";
    return false;
  }

  // Compute offset, where to put pattern
  Address addr = Address::zero();
  if (! _content.isEmpty())
    addr = _content.back()->address() + _content.back()->size();
  // If a offset is set -> check it
  if (pattern->hasAddress() && (pattern->address() != addr)) {
    logInfo() << "Cannot append pattern at address " << pattern->address().toString()
              << ", must be " << addr.toString() << ".";
    return false;
  }

  // Set/update offset
  pattern->setAddress(addr);

  // add to content
  unsigned int idx = _content.size();
  pattern->setParent(this);
  _content.append(pattern->as<FixedPattern>());
  connect(pattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(pattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(pattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(pattern, &AbstractPattern::removed, this, &AbstractPattern::removed);
  connect(pattern->as<FixedPattern>(), &FixedPattern::resized,
          this, &ElementPattern::onChildResized);

  // update own size
  if (size().isValid())
    setSize(size() + pattern->as<FixedPattern>()->size());
  else
    setSize(pattern->as<FixedPattern>()->size());

  emit added(this, idx);
  emit modified(this);

  return true;
}

bool
ElementPattern::insertChildPattern(FixedPattern *pattern, unsigned int idx) {
  if (idx > _content.size())
    return false;
  // Compute offset, where to put pattern
  Address addr = Address::zero();
  if (_content.size() && (idx == _content.size()))
    addr = _content.back()->address() + _content.back()->size();
  else if (_content.size())
    addr = _content.at(idx)->address();

  // If a offset is set -> check it
  if (pattern->hasAddress() && (pattern->address() != addr))
    return false;

  // Set/update offset
  pattern->setAddress(addr);

  // add to content
  pattern->setParent(this);
  _content.insert(idx, pattern->as<FixedPattern>());
  connect(pattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(pattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(pattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(pattern, &AbstractPattern::removed, this, &AbstractPattern::removed);
  connect(pattern->as<FixedPattern>(), &FixedPattern::resized,
          this, &ElementPattern::onChildResized);

  // update own size
  if (size().isValid())
    setSize(size() + pattern->size());
  else
    setSize(pattern->size());

  emit added(this, idx);
  emit modified(this);

  // Update addresses of all subsequent patterns:
  for (idx++; idx < _content.size(); idx++) {
    _content[idx]->setAddress(_content[idx-1]->address() + _content[idx-1]->size());
  }

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

AbstractPattern *
ElementPattern::takeChild(unsigned int n) {
  if (n >= _content.size())
    return nullptr;

  FixedPattern *pattern = _content[n];
  emit removing(this, n);
  _content.remove(n);
  pattern->setParent(nullptr);
  emit removed(this, n);

  Size mySize = size();
  if (pattern->hasSize())
      mySize -= pattern->size();

  // Adjust own size and addresses of childen
  Address addr = (0 != n) ? (_content[n-1]->address()+_content[n-1]->size()) : Address::zero();
  for (int i=n; i<_content.size(); i++) {
    _content[i]->setAddress(addr);
    addr += _content[i]->size();
  }

  setSize(mySize);

  emit modified(this);

  return pattern;
}

void
ElementPattern::onChildResized(const FixedPattern *child, const Size &size) {
  int idx = indexOf(child);
  if (0 > idx)
    return;

  Address addr = child->address() + child->size();
  for (unsigned int i = (idx+1); i<numChildPattern(); i++) {
    _content[i]->setAddress(addr); addr += _content[i]->size();
  }

  setSize(addr-Address::zero());
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

  if (! meta().serialize(writer))
    return false;

  if ((nullptr != subpattern()) && (!subpattern()->serialize(writer)))
    return false;

  writer.writeEndElement();
  return true;
}

AbstractPattern *
FixedRepeatPattern::clone() const {
  auto pattern = FixedPattern::clone()->as<FixedRepeatPattern>();

  if (_subpattern)
    pattern->addChildPattern(_subpattern->clone());
  pattern->setRepetition(_repetition);

  return pattern;
}

unsigned int
FixedRepeatPattern::repetition() const {
  return _repetition;
}
void
FixedRepeatPattern::setRepetition(unsigned int n) {
  _repetition = n;
  if (_subpattern)
    setSize(_subpattern->size()*_repetition);
}

PatternMeta::Flags
FixedRepeatPattern::combinedFlags() const {
  PatternMeta::Flags flags = FixedPattern::combinedFlags();
  for (unsigned int i=0; i<numChildPattern(); i++)
    flags += childPattern(i)->combinedFlags();
  return flags;
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

  setSize(_subpattern->size()*_repetition);
  connect(_subpattern, &AbstractPattern::modified, this, &AbstractPattern::modified);
  connect(_subpattern, &AbstractPattern::added, this, &AbstractPattern::added);
  connect(_subpattern, &AbstractPattern::removing, this, &AbstractPattern::removing);
  connect(_subpattern, &AbstractPattern::removed, this, &AbstractPattern::removed);
  connect(_subpattern, &FixedPattern::resized, this, &FixedRepeatPattern::onChildResized);

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

AbstractPattern *
FixedRepeatPattern::takeChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return nullptr;

  FixedPattern *pattern = _subpattern;

  emit removing(this, 0);
  _subpattern = nullptr;
  pattern->setParent(nullptr);
  emit removed(this, 0);

  setSize(Size::zero());

  return pattern;
}

void
FixedRepeatPattern::onChildResized(const FixedPattern *pattern, const Size &size) {
  if (_subpattern != pattern)
    return;
  setSize(_subpattern->size()*_repetition);
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

  if (! meta().serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}

void
UnknownFieldPattern::setWidth(const Size &size) {
  setSize(size);
}

QVariant
UnknownFieldPattern::value(const Element *element, const Address& address, FieldAnnotation *annotation) const {
  Q_UNUSED(annotation);

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

  if (! meta().serialize(writer))
    return false;

  if (! content().isEmpty())
    writer.writeCharacters(content().toHex());

  writer.writeEndElement();
  return true;
}

AbstractPattern *
UnusedFieldPattern::clone() const {
  auto pattern = FieldPattern::clone()->as<UnusedFieldPattern>();

  pattern->setContent(_content);

  return pattern;
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
    setSize(Offset::fromByte(content.size()));

  return true;
}

void
UnusedFieldPattern::setWidth(const Size &size) {
  setSize(size);
}

QVariant
UnusedFieldPattern::value(const Element *element, const Address& address, FieldAnnotation *annotation) const {
  Q_UNUSED(annotation);

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

  if (! meta().serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}


AbstractPattern *
IntegerFieldPattern::clone() const {
  auto pattern = FieldPattern::clone()->as<IntegerFieldPattern>();

  pattern->setFormat(_format);
  pattern->setEndian(_endian);
  pattern->setMinValue(_minValue);
  pattern->setMaxValue(_maxValue);
  pattern->setDefaultValue(_defaultValue);

  return pattern;
}


void
IntegerFieldPattern::setWidth(const Offset &width) {
  setSize(width);
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
void
IntegerFieldPattern::clearMinValue() {
  _minValue = std::numeric_limits<long long>().max();
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
void
IntegerFieldPattern::clearMaxValue() {
  _maxValue = std::numeric_limits<long long>().max();
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


long long
IntegerFieldPattern::decode(const Element *element, const Address &address, AnnotationIssue &errmsg) const {
  Offset within = address - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {   // int8_t or smaller
    if ((address.bit()+1)<size().bits()) {
      errmsg << "Cannot decode integer, bitpattern extens across bytes.";
      return std::numeric_limits<long long>::max();
    }
    unsigned int shift = (address.bit()+1)-size().bits();
    unsigned int mask  = (1<<size().bits())-1;
    uint8_t value = (uint8_t(element->data().at(within.byte())) >> shift) & mask;
    return value;
  }

  if (size().bits() == 16) {
    if (! address.byteAligned()) {
      errmsg << "Cannot decode int16, values does not align with bytes.";
      return std::numeric_limits<long long>::max();
    }

    const char *ptr = element->data().mid(within.byte(),2).constData();
    if (Format::Signed == _format) {
      if (Endian::Little == _endian)
        return qFromLittleEndian(*((int16_t *)ptr));
      return qFromBigEndian(*((int16_t *)ptr));
    } else if (Format::Unsigned == _format) {
      if (Endian::Little == _endian)
        return qFromLittleEndian(*((uint16_t *)ptr));
      return qFromBigEndian(*((uint16_t *)ptr));
    } else if (Format::BCD == _format) {
      if (Endian::Little == _endian)
        return fromBCD4(qFromLittleEndian(*((uint16_t *)ptr)));
      return fromBCD4(qFromBigEndian(*((uint16_t *)ptr)));
    }

    errmsg << "Cannot decode integer. Unkown format " << (int)_format << ".";
    return std::numeric_limits<long long>::max();
  }

  if (size().bits() <= 32) {
    if (!address.byteAligned()) {
      errmsg << "Cannot decode int32, values does not align with bytes.";
      return std::numeric_limits<long long>::max();
    }
    const char *ptr = element->data().mid(within.byte(),4).constData();
    if (Format::Signed == _format) {
      if (Endian::Little == _endian)
        return qFromLittleEndian(*((int32_t *)ptr));
      return qFromBigEndian(*((int32_t *)ptr));
    } else if (Format::Unsigned == _format) {
      if (Endian::Little == _endian)
        return qFromLittleEndian(*((uint32_t *)ptr));
      return qFromBigEndian(*((uint32_t *)ptr));
    } else if (Format::BCD == _format) {
      if (Endian::Little == _endian)
        return fromBCD8(qFromLittleEndian(*((uint32_t *)ptr)));
      return fromBCD8(qFromBigEndian(*((uint32_t *)ptr)));
    }

    errmsg << "Cannot decode integer. Unkown format " << (int)_format << ".";
    return std::numeric_limits<long long>::max();
  }

  errmsg << "Cannot decode integer. Unhandle size " << size().bits() << "b.";
  return std::numeric_limits<long long>::max();
}


QVariant
IntegerFieldPattern::value(const Element *element, const Address& address, FieldAnnotation *annotation) const {
  if ((address+size()) >  element->address() + element->size()) {
    logError() << "Cannot decode integer, extends beyond element bounds.";
    return QVariant();
  }

  AnnotationIssue issue(address, AnnotationIssue::Error);
  auto val = decode(element, address, issue);
  if (std::numeric_limits<long long>::max() == val) {
    annotation->issues().add(issue);
    return QVariant();
  }

  if (annotation && hasMinValue() && (val < minValue())) {
    AnnotationIssue issue(address, AnnotationIssue::Warning);
    issue << "Value " << val << " of integer " << meta().name()
          << " exceeds lower bound " << minValue() << ".";
    annotation->issues().add(issue);
  }

  if (annotation && hasMaxValue() && (val > maxValue())) {
    AnnotationIssue issue(address, AnnotationIssue::Warning);
    issue << "Value " << val << " of integer " << meta().name()
          << " exceeds upper bound " << maxValue() << ".";
    annotation->issues().add(issue);
  }

  return QVariant(val);
}


uint16_t
IntegerFieldPattern::fromBCD4(uint16_t bcd) {
  uint16_t res = 0;
  for (int i=0; i<4; i++) {
    res *= 10;
    res += (bcd & 0xf000)>>12;
    bcd <<= 4;
  }
  return res;
}


uint32_t
IntegerFieldPattern::fromBCD8(uint32_t bcd) {
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

EnumFieldPatternItem *
EnumFieldPatternItem::clone() const {
  auto item = new EnumFieldPatternItem();
  (*item) = *this;
  item->setValue(this->value());
  return item;
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

  if (! meta().serialize(writer))
    return false;

  foreach(EnumFieldPatternItem *item, _items) {
    if (! item->serialize(writer))
      return false;
  }

  writer.writeEndElement();
  return true;
}

AbstractPattern *
EnumFieldPattern::clone() const {
  auto pattern = FieldPattern::clone()->as<EnumFieldPattern>();

  foreach(auto item, _items)
    pattern->addItem(item->clone());

  return pattern;
}

void
EnumFieldPattern::setWidth(const Size &size) {
  setSize(size);
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

EnumFieldPatternItem *
EnumFieldPattern::itemByValue(unsigned int val) const {
  foreach(auto item, _items) {
    if (item->hasValue() && (val == item->value()))
      return item;
  }
  return nullptr;
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


unsigned int
EnumFieldPattern::decode(const Element *element, const Address &address, AnnotationIssue &errmsg) const {
  if (! element->contains(address, this->size())) {
    errmsg << "Cannot decode enum '" << meta().name() << "': Outside of element bounds.";
    return std::numeric_limits<unsigned int>::max();
  }

  if (size().bits() > 8) {
    errmsg << "Enums are limited to 8 bits in size, got " << size().bits();
    return std::numeric_limits<unsigned int>::max();
  }

  Offset within = address - element->address();

  if ((address.bit()+1)<size().bits()) {
    errmsg << "Cannot decode integer, bitpattern extens across bytes.";
    return std::numeric_limits<unsigned int>::max();
  }
  unsigned int shift = (address.bit()+1)-size().bits();
  unsigned int mask  = (1<<size().bits())-1;
  return (uint8_t(element->data().at(within.byte())) >> shift) & mask;
}


QVariant
EnumFieldPattern::value(const Element *element, const Address& address, FieldAnnotation *annotation) const {
  AnnotationIssue issue(address, AnnotationIssue::Error);
  unsigned int val = decode(element, address, issue);
  if (std::numeric_limits<unsigned int>::max() == val) {
    annotation->issues().add(issue);
    return QVariant();
  }

  auto item = itemByValue(val);
  if (nullptr == item) {
    AnnotationIssue issue(address, AnnotationIssue::Warning);
    issue << "Unknown enum value " << val << " for enum '" << meta().name() << "'.";
    annotation->issues().add(issue);
  }

  return QVariant(val);
}



/* ********************************************************************************************* *
 * Implementation of StringFieldPattern
 * ********************************************************************************************* */
StringFieldPattern::StringFieldPattern(QObject *parent)
  : FieldPattern{parent}, _format(Format::ASCII), _numChars(0), _padValue(0)
{
  // pass...
}

bool
StringFieldPattern::verify() const {
  return true;
}

bool
StringFieldPattern::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("string");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  switch(format()) {
  case Format::ASCII: writer.writeAttribute("format", "ascii"); break;
  case Format::Unicode: writer.writeAttribute("format", "unicode"); break;
  }

  writer.writeAttribute("width", QString::number(_numChars));
  writer.writeAttribute("pad", QString::number(_padValue));

  if (! meta().serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}


AbstractPattern *
StringFieldPattern::clone() const {
  auto pattern = FieldPattern::clone()->as<StringFieldPattern>();
  pattern->setFormat(this->format());
  pattern->setNumChars(this->numChars());
  pattern->setPadValue(this->padValue());
  return pattern;
}


QVariant
StringFieldPattern::value(const Element *element, const Address &address, FieldAnnotation *annotation) const {
  if (! element->contains(address, size())) {
    AnnotationIssue issue(address, AnnotationIssue::Error);
    issue << "Cannot decode string: string not contained within element.";
    annotation->issues().add(issue);
    return QVariant();
  }

  Offset offset = address-element->address();
  QByteArray mid = element->data().mid(offset.byte(), size().byte());

  if (Format::ASCII == format()) {
    bool printable = true;
    unsigned int len = 0;
    for (; (len<numChars()) && (mid[len] != padValue()); len++)
      printable &= ((mid[len]>=32) && (mid[len]<=126));

    if (! printable) {
      AnnotationIssue issue(address, AnnotationIssue::Warning);
      issue << "ASCII string contains non-printable characters.";
      annotation->issues().add(issue);
    }

    return QString::fromLatin1(mid.first(len));
  } else if (Format::Unicode == format()) {
    return QString(mid);
  }

  AnnotationIssue issue(address, AnnotationIssue::Error);
  issue << "Cannot decode string: unknown format " << (int)format() << ".";
  annotation->issues().add(issue);
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
  case Format::ASCII: setSize(Size::fromByte(_numChars)); break;
  case Format::Unicode: setSize(Size::fromByte(_numChars*2)); break;
  }
}


unsigned int
StringFieldPattern::padValue() const {
  return _padValue;
}
void
StringFieldPattern::setPadValue(unsigned int pad) {
  _padValue = pad;
}
