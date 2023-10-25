#include "patterndefinition.hh"
#include "logger.hh"
#include "image.hh"

#include <QXmlStreamWriter>
#include <QtEndian>


/* ********************************************************************************************* *
 * PatternDefinitionLibrary
 * ********************************************************************************************* */
PatternDefinitionLibrary::PatternDefinitionLibrary(QObject *parent)
  : QObject(parent), _elements()
{
  // pass...
}

bool
PatternDefinitionLibrary::add(const QString &qname, AbstractPatternDefinition *def) {
  QStringList path = qname.split(".",Qt::SkipEmptyParts);
  PatternDefinitionLibrary *lib = this;
  while (path.size() > 1) {
    QString libName = path.first(); path.pop_front();
    if (! lib->_elements.contains(libName))
      lib->_elements[libName] = new PatternDefinitionLibrary(lib);
    if (nullptr == (lib = qobject_cast<PatternDefinitionLibrary*>(_elements[libName]))) {
      logError() << "Cannot store pattern definition " << def->meta().name()
                 << " as " << qname << ": '" << libName << "' is not a library.";
      return false;
    }
  }
  QString elName = path.front(); path.pop_front();
  if (lib->_elements.contains(elName)) {
    logError() << "Cannot stroe pattern definition " << def->meta().name()
               << " as " << qname << ": " << elName << " already defined";
    return false;
  }

  def->setParent(lib);
  lib->_elements[elName] = def;

  return true;
}

bool
PatternDefinitionLibrary::has(const QString &qname) const {
  QStringList path = qname.split(".", Qt::SkipEmptyParts);
  const PatternDefinitionLibrary *lib = this;
  while (path.size() > 1) {
    QString libName = path.first(); path.pop_front();
    if (! lib->_elements.contains(libName))
      return false;
    if (nullptr == (lib = qobject_cast<PatternDefinitionLibrary*>(_elements[libName])))
      return false;
  }

  QString elName = path.front(); path.pop_front();
  if (! lib->_elements.contains(elName))
    return false;
  return nullptr != qobject_cast<AbstractPatternDefinition*>(lib->_elements[elName]);
}

AbstractPatternDefinition *
PatternDefinitionLibrary::get(const QString &qname) const {
  QStringList path = qname.split(".", Qt::SkipEmptyParts);
  const PatternDefinitionLibrary *lib = this;
  while (path.size() > 1) {
    QString libName = path.first(); path.pop_front();
    if (! lib->_elements.contains(libName))
      return nullptr;
    if (nullptr == (lib = qobject_cast<PatternDefinitionLibrary*>(_elements[libName])))
      return nullptr;
  }

  QString elName = path.front(); path.pop_front();
  if (! lib->_elements.contains(elName))
    return nullptr;
  return qobject_cast<AbstractPatternDefinition*>(lib->_elements[elName]);
}

QHash<QString, AbstractPatternDefinition*>
PatternDefinitionLibrary::pattern() const {
  QHash<QString, AbstractPatternDefinition *> res;
  QHashIterator<QString, QObject *> iter(_elements);
  while (iter.hasNext()) {
    iter.next();
    if (auto lib = qobject_cast<PatternDefinitionLibrary*>(iter.value())) {
      QHash<QString, AbstractPatternDefinition *> content = lib->pattern();
      QHashIterator<QString, AbstractPatternDefinition *> citer(content);
      while(citer.hasNext()) {
        citer.next();
        res[iter.key() + "." + citer.key()] = citer.value();
      }
    } else if (auto pattern = qobject_cast<AbstractPatternDefinition *>(iter.value())) {
      res[iter.key()] = pattern;
    }
  }

  return res;
}


/* ********************************************************************************************* *
 * AbstractPatternDefinition
 * ********************************************************************************************* */
AbstractPatternDefinition::AbstractPatternDefinition(QObject *parent)
  : QObject{parent}, _meta(), _address()
{
  connect(&_meta, &PatternMeta::modified, this, &AbstractPatternDefinition::onMetaModified);
}

const PatternMeta &
AbstractPatternDefinition::meta() const {
  return _meta;
}

PatternMeta &
AbstractPatternDefinition::meta() {
  return _meta;
}

bool
AbstractPatternDefinition::hasImplicitAddress() const {
  return parent() && (nullptr==qobject_cast<GroupPatternDefinition *>(parent()));
}
bool
AbstractPatternDefinition::hasAddress() const {
  return _address.isValid();
}

const Address &
AbstractPatternDefinition::address() const {
  return _address;
}

void
AbstractPatternDefinition::setAddress(const Address& offset) {
  _address = offset;
}

void
AbstractPatternDefinition::onMetaModified() {
  emit modified(this);
}


/* ********************************************************************************************* *
 * Implementation of StructuredPatternDefinition
 * ********************************************************************************************* */
StructuredPatternDefinition::StructuredPatternDefinition()
{
  // pass...
}

StructuredPatternDefinition::~StructuredPatternDefinition() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of GroupPatternDefinition
 * ********************************************************************************************* */
GroupPatternDefinition::GroupPatternDefinition(QObject *parent)
  : AbstractPatternDefinition{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of CodeplugPatternDefinition
 * ********************************************************************************************* */
CodeplugPatternDefinition::CodeplugPatternDefinition(QObject *parent)
  : GroupPatternDefinition{parent}, _modified(false), _source(), _content()
{
  //connect(this, &AbstractPattern::modified, this, &CodeplugPattern::onModified);
}

bool
CodeplugPatternDefinition::verify() const {
  // A codeplug cannot be empty
  if (0 == _content.size())
    return false;

  return true;
}

AbstractPattern *
CodeplugPatternDefinition::instantiate() const {
  return new CodeplugPattern(this);
}

bool
CodeplugPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("codeplug");

  if (! meta().serialize(writer))
    return false;

  foreach(AbstractPatternDefinition *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();

  return true;
}


int
CodeplugPatternDefinition::indexOf(const AbstractPatternDefinition *pattern) const {
  return _content.indexOf(pattern);
}

unsigned int
CodeplugPatternDefinition::numChildPattern() const {
  return _content.size();
}

bool
CodeplugPatternDefinition::addChildPattern(AbstractPatternDefinition *pattern) {
  // If the pattern is the first element and has no offset within the codeplug, I do not know,
  // where to put it.
  if (! pattern->hasAddress()) {
    logDebug() << "Cannot add child pattern: child has no explicit address.";
    return false;
  }

  unsigned int idx = _content.size();
  pattern->setParent(this);
  _content.append(pattern);
  connect(pattern, &AbstractPatternDefinition::modified, this, &AbstractPatternDefinition::modified);
  connect(pattern, &AbstractPatternDefinition::added, this, &AbstractPatternDefinition::added);
  connect(pattern, &AbstractPatternDefinition::removing, this, &AbstractPatternDefinition::removing);
  connect(pattern, &AbstractPatternDefinition::removed, this, &AbstractPatternDefinition::removed);

  emit added(this, idx);

  return true;
}

AbstractPatternDefinition *
CodeplugPatternDefinition::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}

bool
CodeplugPatternDefinition::deleteChild(unsigned int n) {
  if (n >= _content.size())
    return false;

  AbstractPatternDefinition *pattern = _content[n];

  emit removing(this, n);
  _content.remove(n);
  emit removed(this, n);

  pattern->deleteLater();
  return true;
}

bool
CodeplugPatternDefinition::hasSize() const {
  return false;
}
Size
CodeplugPatternDefinition::size() const {
  return Size();
}

bool
CodeplugPatternDefinition::isModified() const {
  return _modified;
}

CodeplugPatternDefinition *
CodeplugPatternDefinition::load(const QString &filename) {
  QFile file(filename);

  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "': " << file.errorString() << ".";
    return nullptr;
  }

  /*CodeplugPatternDefinitionParser parser;
  QXmlStreamReader reader(&file);
  if (! parser.parse(reader)) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "', cannot parse pattern: " << parser.errorMessage() << ".";
    return nullptr;
  }

  if (! parser.topIs<CodeplugPatternDefinition>()) {
    logError() << "Cannot load annotation pattern from '" << filename
               << "': Files does not contain a codeplug pattern.";
    return nullptr;
  }

  CodeplugPatternDefinition *pattern = parser.popAs<CodeplugPatternDefinition>();
  pattern->setSource(filename);
  return pattern;
  */
  return nullptr;
}

bool
CodeplugPatternDefinition::save() {
  if ((! _source.exists()) || (! _source.isWritable()))
    return false;
  return save(_source.absoluteFilePath());
}

bool
CodeplugPatternDefinition::save(const QString &filename) {
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
CodeplugPatternDefinition::save(QIODevice *device) {
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
CodeplugPatternDefinition::source() const {
  return _source;
}

void
CodeplugPatternDefinition::setSource(const QString &filename) {
  _source = QFileInfo(filename);
}

void
CodeplugPatternDefinition::onModified() {
  _modified = true;
}


/* ********************************************************************************************* *
 * Implementation of RepeatPatternDefinition
 * ********************************************************************************************* */
RepeatPatternDefinition::RepeatPatternDefinition(QObject *parent)
  : GroupPatternDefinition{parent}, _minRepetition(std::numeric_limits<unsigned int>::max()),
    _maxRepetition(std::numeric_limits<unsigned int>::max()), _step(), _subpattern(nullptr)
{
  // pass...
}

bool
RepeatPatternDefinition::verify() const {
  if (nullptr == _subpattern)
    return false;
  return _subpattern->verify();
}

AbstractPattern *
RepeatPatternDefinition::instantiate() const {
  return new RepeatPattern(this);
}

bool
RepeatPatternDefinition::serialize(QXmlStreamWriter &writer) const {
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

bool
RepeatPatternDefinition::hasSize() const {
  return false;
}
Size
RepeatPatternDefinition::size() const {
  return Size();
}

bool
RepeatPatternDefinition::hasMinRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _minRepetition;
}
unsigned int
RepeatPatternDefinition::minRepetition() const {
  return _minRepetition;
}
void
RepeatPatternDefinition::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

bool
RepeatPatternDefinition::hasMaxRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _maxRepetition;
}
unsigned int
RepeatPatternDefinition::maxRepetition() const {
  return _maxRepetition;
}
void
RepeatPatternDefinition::setMaxRepetition(unsigned int rep) {
  _maxRepetition = rep;
}

int
RepeatPatternDefinition::indexOf(const AbstractPatternDefinition *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

unsigned int
RepeatPatternDefinition::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPatternDefinition *
RepeatPatternDefinition::subpattern() const {
  return _subpattern;
}

bool
RepeatPatternDefinition::addChildPattern(AbstractPatternDefinition *subpattern) {
  if (_subpattern)
    return false;

  _subpattern = subpattern;
  _subpattern->setParent(this);

  connect(_subpattern, &AbstractPatternDefinition::modified, this, &AbstractPatternDefinition::modified);
  connect(_subpattern, &AbstractPatternDefinition::added, this, &AbstractPatternDefinition::added);
  connect(_subpattern, &AbstractPatternDefinition::removing, this, &AbstractPatternDefinition::removing);
  connect(_subpattern, &AbstractPatternDefinition::removed, this, &AbstractPatternDefinition::removed);

  emit added(this, 0);

  return true;
}

AbstractPatternDefinition *
RepeatPatternDefinition::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

bool
RepeatPatternDefinition::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  AbstractPatternDefinition *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  return true;
}

const Offset &
RepeatPatternDefinition::step() const {
  return _step;
}
void
RepeatPatternDefinition::setStep(const Offset &step) {
  _step = step;
}



/* ********************************************************************************************* *
 * Implementation of BlockPatternDefinition
 * ********************************************************************************************* */
BlockPatternDefinition::BlockPatternDefinition(QObject *parent)
  : AbstractPatternDefinition{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of BlockRepeatPatternDefinition
 * ********************************************************************************************* */
BlockRepeatPatternDefinition::BlockRepeatPatternDefinition(QObject *parent)
  : BlockPatternDefinition{parent}, _minRepetition(std::numeric_limits<unsigned int>::max()),
    _maxRepetition(std::numeric_limits<unsigned int>::max()), _subpattern(nullptr)
{
  // pass...
}

bool
BlockRepeatPatternDefinition::verify() const {
  if (! _subpattern)
    return false;
  return _subpattern->verify();
}

AbstractPattern *
BlockRepeatPatternDefinition::instantiate() const {
  return new BlockRepeatPattern(this);
}

bool
BlockRepeatPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("repeat");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());
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

bool
BlockRepeatPatternDefinition::hasSize() const {
  return false;
}

Size
BlockRepeatPatternDefinition::size() const {
  return Size();
}

bool
BlockRepeatPatternDefinition::hasMinRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _minRepetition;
}
unsigned int
BlockRepeatPatternDefinition::minRepetition() const {
  return _minRepetition;
}
void
BlockRepeatPatternDefinition::setMinRepetition(unsigned int rep) {
  _minRepetition = rep;
}

bool
BlockRepeatPatternDefinition::hasMaxRepetition() const {
  return std::numeric_limits<unsigned int>::max() != _maxRepetition;
}
unsigned int
BlockRepeatPatternDefinition::maxRepetition() const {
  return _maxRepetition;
}
void
BlockRepeatPatternDefinition::setMaxRepetition(unsigned int rep) {
  _maxRepetition = rep;
}

FixedPatternDefinition *
BlockRepeatPatternDefinition::subpattern() const {
  return _subpattern;
}
bool
BlockRepeatPatternDefinition::addChildPattern(AbstractPatternDefinition *subpattern) {
  if (! subpattern->is<FixedPattern>())
    return false;

  if (_subpattern)
    return false;

  _subpattern = subpattern->as<FixedPatternDefinition>();
  _subpattern->setParent(this);
  connect(_subpattern, &AbstractPatternDefinition::modified, this, &AbstractPatternDefinition::modified);
  connect(_subpattern, &AbstractPatternDefinition::added, this, &AbstractPatternDefinition::added);
  connect(_subpattern, &AbstractPatternDefinition::removing, this, &AbstractPatternDefinition::removing);
  connect(_subpattern, &AbstractPatternDefinition::removed, this, &AbstractPatternDefinition::removed);

  emit added(this, 0);

  return true;
}

unsigned int
BlockRepeatPatternDefinition::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPatternDefinition *
BlockRepeatPatternDefinition::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

int
BlockRepeatPatternDefinition::indexOf(const AbstractPatternDefinition *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

bool
BlockRepeatPatternDefinition::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  AbstractPatternDefinition *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  return true;
}

/* ********************************************************************************************* *
 * Implementation of FixedPatternDefinition
 * ********************************************************************************************* */
FixedPatternDefinition::FixedPatternDefinition(QObject *parent)
  : BlockPatternDefinition(parent), _size()
{
  // pass...
}

bool
FixedPatternDefinition::verify() const {
  return hasSize();
}

bool
FixedPatternDefinition::hasSize() const {
  return _size.isValid();
}

Size
FixedPatternDefinition::size() const {
  return _size;
}

void
FixedPatternDefinition::setSize(const Size &size) {
  _size = size;
  emit resized(this, _size);
}


/* ********************************************************************************************* *
 * Implementation of ElementPattern
 * ********************************************************************************************* */
ElementPatternDefinition::ElementPatternDefinition(QObject *parent)
  : FixedPatternDefinition(parent), _content()
{
  // pass...
}

bool
ElementPatternDefinition::verify() const {
  if (! FixedPatternDefinition::verify())
    return false;

  foreach(FixedPatternDefinition *pattern, _content)
    if (! pattern->verify())
      return false;

  return true;
}

AbstractPattern *
ElementPatternDefinition::instantiate() const {
  return new ElementPattern(this);
}

bool
ElementPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("element");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  if (! meta().serialize(writer))
    return false;

  foreach(FixedPatternDefinition *pattern, _content)
    if (! pattern->serialize(writer))
      return false;

  writer.writeEndElement();
  return true;
}

bool
ElementPatternDefinition::addChildPattern(AbstractPatternDefinition *pattern) {
  if (! pattern->is<FixedPatternDefinition>())
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

  // add to content
  unsigned int idx = _content.size();
  pattern->setParent(this);
  _content.append(pattern->as<FixedPatternDefinition>());
  connect(pattern, &AbstractPatternDefinition::modified, this, &AbstractPatternDefinition::modified);
  connect(pattern, &AbstractPatternDefinition::added, this, &AbstractPatternDefinition::added);
  connect(pattern, &AbstractPatternDefinition::removing, this, &AbstractPatternDefinition::removing);
  connect(pattern, &AbstractPatternDefinition::removed, this, &AbstractPatternDefinition::removed);
  connect(pattern, &AbstractPatternDefinition::resized,
          this, &ElementPatternDefinition::onChildResized);

  // update own size
  if (size().isValid())
    setSize(size() + pattern->as<FixedPatternDefinition>()->size());
  else
    setSize(pattern->as<FixedPatternDefinition>()->size());

  emit added(this, idx);

  return true;
}

unsigned int
ElementPatternDefinition::numChildPattern() const {
  return _content.size();
}

AbstractPatternDefinition *
ElementPatternDefinition::childPattern(unsigned int n) const {
  if (n >= _content.size())
    return nullptr;
  return _content[n];
}

int
ElementPatternDefinition::indexOf(const AbstractPatternDefinition *pattern) const {
  return _content.indexOf(pattern);
}

bool
ElementPatternDefinition::deleteChild(unsigned int n) {
  if (n >= _content.size())
    return false;

  FixedPatternDefinition *pattern = _content[n];
  emit removing(this, n);
  _content.remove(n);
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
  return true;
}

void
ElementPatternDefinition::onChildResized(const AbstractPatternDefinition *child, const Size &size) {
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
 * Implementation of FixedRepeatPatternDefinition
 * ********************************************************************************************* */
FixedRepeatPatternDefinition::FixedRepeatPatternDefinition(QObject *parent)
  : FixedPatternDefinition(parent), _repetition(0), _subpattern(nullptr)
{
  // pass...
}

bool
FixedRepeatPatternDefinition::verify() const {
  if (! FixedPatternDefinition::verify())
    return false;

  if ((nullptr == _subpattern) || (0 == _repetition))
    return false;

  return true;
}

AbstractPattern *
FixedRepeatPatternDefinition::instantiate() const {
  return new FixedRepeatPattern(this);
}

bool
FixedRepeatPatternDefinition::serialize(QXmlStreamWriter &writer) const {
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

unsigned int
FixedRepeatPatternDefinition::repetition() const {
  return _repetition;
}
void
FixedRepeatPatternDefinition::setRepetition(unsigned int n) {
  _repetition = n;
  if (_subpattern)
    setSize(_subpattern->size()*_repetition);
}

FixedPatternDefinition *
FixedRepeatPatternDefinition::subpattern() const {
  return _subpattern;
}
bool
FixedRepeatPatternDefinition::addChildPattern(AbstractPatternDefinition *pattern) {
  if (! pattern->is<FixedPattern>())
    return false;

  if (_subpattern)
    return false;

  _subpattern = pattern->as<FixedPatternDefinition>();
  _subpattern->setParent(this);

  setSize(_subpattern->size()*_repetition);
  connect(_subpattern, &AbstractPatternDefinition::modified, this, &AbstractPatternDefinition::modified);
  connect(_subpattern, &AbstractPatternDefinition::added, this, &AbstractPatternDefinition::added);
  connect(_subpattern, &AbstractPatternDefinition::removing, this, &AbstractPatternDefinition::removing);
  connect(_subpattern, &AbstractPatternDefinition::removed, this, &AbstractPatternDefinition::removed);
  connect(_subpattern, &AbstractPatternDefinition::resized, this, &FixedRepeatPatternDefinition::onChildResized);

  emit added(this, 0);

  return true;
}

unsigned int
FixedRepeatPatternDefinition::numChildPattern() const {
  return (nullptr == _subpattern) ? 0 : 1;
}

AbstractPatternDefinition *
FixedRepeatPatternDefinition::childPattern(unsigned int n) const {
  if (0 != n)
    return nullptr;
  return _subpattern;
}

int
FixedRepeatPatternDefinition::indexOf(const AbstractPatternDefinition *pattern) const {
  return (_subpattern == pattern) ? 0 : -1;
}

bool
FixedRepeatPatternDefinition::deleteChild(unsigned int n) {
  if ((0 != n) || (nullptr == _subpattern))
    return false;

  FixedPatternDefinition *pattern = _subpattern;
  emit removing(this, 0);
  _subpattern = nullptr;
  emit removed(this, 0);
  pattern->deleteLater();

  setSize(Size::zero());

  return true;
}

void
FixedRepeatPatternDefinition::onChildResized(const AbstractPatternDefinition *pattern, const Size &size) {
  if (_subpattern != pattern)
    return;
  setSize(_subpattern->size()*_repetition);
}


/* ********************************************************************************************* *
 * Implementation of FieldPatternDefinition
 * ********************************************************************************************* */
FieldPatternDefinition::FieldPatternDefinition(QObject *parent)
  : FixedPatternDefinition{parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of UnknownFieldPatternDefinition
 * ********************************************************************************************* */
UnknownFieldPatternDefinition::UnknownFieldPatternDefinition(QObject *parent)
  : FieldPatternDefinition{parent}
{
  meta().setName("Unknown data");
}

bool
UnknownFieldPatternDefinition::verify() const {
  return FixedPatternDefinition::verify();
}

AbstractPattern *
UnknownFieldPatternDefinition::instantiate() const {
  return new UnknownFieldPattern(this);
}

bool
UnknownFieldPatternDefinition::serialize(QXmlStreamWriter &writer) const {
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
UnknownFieldPatternDefinition::setWidth(const Size &size) {
  setSize(size);
}

QVariant
UnknownFieldPatternDefinition::value(const Element *element, const Address& address) const {
  if ((address+size()) > (element->address()+element->size()))
    return QVariant();
  Offset within = address - element->address();
  return element->data().mid(within.byte(), size().byte());
}


/* ********************************************************************************************* *
 * Implementation of UnusedFieldPatternDefinition
 * ********************************************************************************************* */
UnusedFieldPatternDefinition::UnusedFieldPatternDefinition(QObject *parent)
  : FieldPatternDefinition{parent}, _content()
{
  meta().setName("Unused data");
}

bool
UnusedFieldPatternDefinition::verify() const {
  if (! FieldPatternDefinition::verify())
    return false;
  if (_content.isEmpty())
    return false;
  return true;
}

AbstractPattern *
UnusedFieldPatternDefinition::instantiate() const {
  return new UnusedFieldPattern(this);
}

bool
UnusedFieldPatternDefinition::serialize(QXmlStreamWriter &writer) const {
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

const QByteArray &
UnusedFieldPatternDefinition::content() const {
  return _content;
}

bool
UnusedFieldPatternDefinition::setContent(const QByteArray &content) {
  if (! _content.isEmpty())
    return false;

  _content = content;

  if (! hasSize())
    setSize(Offset::fromByte(content.size()));

  return true;
}

void
UnusedFieldPatternDefinition::setWidth(const Size &size) {
  setSize(size);
}

QVariant
UnusedFieldPatternDefinition::value(const Element *element, const Address& address) const {
  Offset within = address - element->address();
  return element->data().mid(within.byte(), size().byte());
}



/* ********************************************************************************************* *
 * Implementation of IntegerFieldPatternDefinition
 * ********************************************************************************************* */
IntegerFieldPatternDefinition::IntegerFieldPatternDefinition(QObject *parent)
  : FieldPatternDefinition{parent}, _format(IntegerFieldPattern::Format::Unsigned),
    _endian(IntegerFieldPattern::Endian::Little),
    _minValue(std::numeric_limits<long long>().max()),
    _maxValue(std::numeric_limits<long long>().max()),
    _defaultValue(std::numeric_limits<long long>().max())
{
  // pass...
}

bool
IntegerFieldPatternDefinition::verify() const {
  return FieldPatternDefinition::verify();
}

AbstractPattern *
IntegerFieldPatternDefinition::instantiate() const {
  return new IntegerFieldPattern(this);
}

bool
IntegerFieldPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("int");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  writer.writeAttribute("width", size().toString());

  switch(format()) {
  case IntegerFieldPattern::Format::Unsigned: writer.writeAttribute("format", "unsigned"); break;
  case IntegerFieldPattern::Format::Signed: writer.writeAttribute("format", "signed"); break;
  case IntegerFieldPattern::Format::BCD: writer.writeAttribute("format", "bcd"); break;
  }

  if (size().bits() > 8) {
    switch(endian()) {
    case IntegerFieldPattern::Endian::Little: writer.writeAttribute("endian", "little"); break;
    case IntegerFieldPattern::Endian::Big: writer.writeAttribute("endian", "big"); break;
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

void
IntegerFieldPatternDefinition::setWidth(const Offset &width) {
  setSize(width);
}

IntegerFieldPattern::Format
IntegerFieldPatternDefinition::format() const {
  return _format;
}
void
IntegerFieldPatternDefinition::setFormat(IntegerFieldPattern::Format format) {
  _format = format;
}

IntegerFieldPattern::Endian
IntegerFieldPatternDefinition::endian() const {
  return _endian;
}
void
IntegerFieldPatternDefinition::setEndian(IntegerFieldPattern::Endian endian) {
  _endian = endian;
}

bool
IntegerFieldPatternDefinition::hasMinValue() const {
  return std::numeric_limits<long long>().max() != _minValue;
}
long long
IntegerFieldPatternDefinition::minValue() const {
  return _minValue;
}
void
IntegerFieldPatternDefinition::setMinValue(long long min) {
  _minValue = min;
}

bool
IntegerFieldPatternDefinition::hasMaxValue() const {
  return std::numeric_limits<long long>().max() != _maxValue;
}
long long
IntegerFieldPatternDefinition::maxValue() const {
  return _maxValue;
}
void
IntegerFieldPatternDefinition::setMaxValue(long long max) {
  _maxValue = max;
}

bool
IntegerFieldPatternDefinition::hasDefaultValue() const {
  return std::numeric_limits<long long>().max() != _defaultValue;
}
long long
IntegerFieldPatternDefinition::defaultValue() const {
  return _defaultValue;
}
void
IntegerFieldPatternDefinition::setDefaultValue(long long value) {
  _defaultValue = value;
}
void
IntegerFieldPatternDefinition::clearDefaultValue() {
  _defaultValue = std::numeric_limits<long long>().max();
}

QVariant
IntegerFieldPatternDefinition::value(const Element *element, const Address& address) const {
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
    if (IntegerFieldPattern::Format::Signed == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((int16_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((int16_t *)ptr)));
    } else if (IntegerFieldPattern::Format::Unsigned == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((uint16_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((uint16_t *)ptr)));
    } else if (IntegerFieldPattern::Format::BCD == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
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
    if (IntegerFieldPattern::Format::Signed == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((int32_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((int32_t *)ptr)));
    } else if (IntegerFieldPattern::Format::Unsigned == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
        return QVariant::fromValue(qFromLittleEndian(*((uint32_t *)ptr)));
      return QVariant::fromValue(qFromBigEndian(*((uint32_t *)ptr)));
    } else if (IntegerFieldPattern::Format::BCD == _format) {
      if (IntegerFieldPattern::Endian::Little == _endian)
        return QVariant::fromValue(fromBCD8le(*((uint32_t *)ptr)));
      return QVariant::fromValue(fromBCD8be(*((uint32_t *)ptr)));
    }
    return QVariant();
  }

  return QVariant();
}

uint16_t
IntegerFieldPatternDefinition::fromBCD4le(uint16_t bcd) {
  uint16_t res = 0;
  for (int i=0; i<4; i++) {
    res *= 10;
    res += bcd & 0xf;
    bcd >>= 4;
  }
  return res;
}

uint16_t
IntegerFieldPatternDefinition::fromBCD4be(uint16_t bcd) {
  uint16_t res = 0;
  for (int i=0; i<4; i++) {
    res *= 10;
    res += (bcd & 0xf000)>>12;
    bcd <<= 4;
  }
  return res;
}

uint32_t
IntegerFieldPatternDefinition::fromBCD8le(uint32_t bcd) {
  uint32_t res = 0;
  for (int i=0; i<8; i++) {
    res *= 10;
    res += bcd & 0xf;
    bcd >>= 4;
  }
  return res;
}

uint32_t
IntegerFieldPatternDefinition::fromBCD8be(uint32_t bcd) {
  uint32_t res = 0;
  for (int i=0; i<8; i++) {
    res *= 10;
    res += (bcd & 0xf0000000)>>28;
    bcd <<= 4;
  }
  return res;
}


/* ********************************************************************************************* *
 * Implementation of EnumFieldPatternItemDefinition
 * ********************************************************************************************* */
EnumFieldPatternItemDefinition::EnumFieldPatternItemDefinition(QObject *parent)
  : PatternMeta{parent}, _value(std::numeric_limits<unsigned int>().max())
{
  // pass...
}

bool
EnumFieldPatternItemDefinition::serialize(QXmlStreamWriter &writer) const {
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
EnumFieldPatternItemDefinition::hasValue() const {
  return std::numeric_limits<unsigned int>().max() != _value;
}

unsigned int
EnumFieldPatternItemDefinition::value() const {
  return _value;
}

bool
EnumFieldPatternItemDefinition::setValue(unsigned int value) {
  if (hasValue())
    return false;
  _value = value;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of EnumFieldPatternDefinition
 * ********************************************************************************************* */
EnumFieldPatternDefinition::EnumFieldPatternDefinition(QObject *parent)
  : FieldPatternDefinition{parent}, _items()
{
  // pass...
}

bool
EnumFieldPatternDefinition::verify() const {
  if (! FieldPatternDefinition::verify())
    return false;
  return 0 != _items.size();
}

AbstractPattern *
EnumFieldPatternDefinition::instantiate() const {
  return new EnumFieldPattern(this);
}

bool
EnumFieldPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("enum");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  writer.writeAttribute("width", size().toString());

  if (! meta().serialize(writer))
    return false;

  foreach(EnumFieldPatternItemDefinition *item, _items) {
    if (! item->serialize(writer))
      return false;
  }

  writer.writeEndElement();
  return true;
}

void
EnumFieldPatternDefinition::setWidth(const Size &size) {
  setSize(size);
}

bool
EnumFieldPatternDefinition::addItem(EnumFieldPatternItemDefinition *item) {
  item->setParent(this);
  unsigned int idx = _items.size();
  _items.append(item);
  emit itemAdded(idx);
  return true;
}

unsigned int
EnumFieldPatternDefinition::numItems() const {
  return _items.size();
}

EnumFieldPatternItemDefinition *
EnumFieldPatternDefinition::item(unsigned int n) const {
  if (n >= _items.size())
    return nullptr;
  return _items[n];
}

bool
EnumFieldPatternDefinition::deleteItem(unsigned int n) {
  if (n >= _items.size())
    return false;
  EnumFieldPatternItemDefinition *item = _items[n];
  emit itemDeleted(n);
  _items.remove(n);
  item->deleteLater();
  return true;
}

QVariant
EnumFieldPatternDefinition::value(const Element *element, const Address& address) const {
  logDebug() << "Decode enum '" << meta().name() << "' @" << address.toString()
             << " in element starting at " << element->address().toString()
             << " of size " << element->size().toString() << ".";
  if (! element->contains(address, this->size())) {
    logError() << "Cannot decode enum '" << meta().name() << "': Outside of element bounds.";
    return QVariant();
  }

  Offset within = address - element->address();

  // sub-byte integers should not span multiple bytes
  // larger integers must align with bytes

  if (size().bits() <= 8) {
    if ((address.bit()+1)<size().bits()) {
      logWarn() << "Cannot decode enum, bitpattern extens across bytes.";
      return QVariant();
    }
    unsigned int shift = (address.bit()+1)-size().bits();
    unsigned int mask  = (1<<size().bits())-1;
    return QVariant((unsigned int) (uint8_t(element->data().at(within.byte())) & mask) >> shift);
  }

  return QVariant();
}



/* ********************************************************************************************* *
 * Implementation of StringFieldPatternDefinition
 * ********************************************************************************************* */
StringFieldPatternDefinition::StringFieldPatternDefinition(QObject *parent)
  : FieldPatternDefinition{parent}, _format(StringFieldPattern::Format::ASCII), _numChars(0), _padValue(0)
{
  // pass...
}

bool
StringFieldPatternDefinition::verify() const {
  return true;
}

AbstractPattern *
StringFieldPatternDefinition::instantiate() const {
  return new StringFieldPattern(this);
}

bool
StringFieldPatternDefinition::serialize(QXmlStreamWriter &writer) const {
  writer.writeStartElement("string");

  if (! hasImplicitAddress())
    writer.writeAttribute("at", address().toString());

  switch(format()) {
  case StringFieldPattern::Format::ASCII: writer.writeAttribute("format", "ascii"); break;
  case StringFieldPattern::Format::Unicode: writer.writeAttribute("format", "unicode"); break;
  }

  writer.writeAttribute("width", QString::number(_numChars));
  writer.writeAttribute("pad", QString::number(_padValue));

  if (! meta().serialize(writer))
    return false;

  writer.writeEndElement();
  return true;
}

QVariant
StringFieldPatternDefinition::value(const Element *element, const Address &address) const {
  QByteArray mid = element->data().mid(address.byte(), size().byte());
  switch (format()) {
  case StringFieldPattern::Format::ASCII: return QString(mid);
  case StringFieldPattern::Format::Unicode: return QString(mid);
  }
  return QVariant();
}

StringFieldPattern::Format
StringFieldPatternDefinition::format() const {
  return _format;
}
void
StringFieldPatternDefinition::setFormat(StringFieldPattern::Format format) {
  _format = format;
  setNumChars(_numChars);
}

unsigned int
StringFieldPatternDefinition::numChars() const {
  return _numChars;
}
void
StringFieldPatternDefinition::setNumChars(unsigned int n) {
  _numChars = n;
  switch (format()) {
  case StringFieldPattern::Format::ASCII: setSize(Size::fromByte(_numChars)); break;
  case StringFieldPattern::Format::Unicode: setSize(Size::fromByte(_numChars*2)); break;
  }
}

unsigned int
StringFieldPatternDefinition::padValue() const {
  return _padValue;
}
void
StringFieldPatternDefinition::setPadValue(unsigned int pad) {
  _padValue = pad;
}

