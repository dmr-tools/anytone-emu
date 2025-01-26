#include "annotation.hh"
#include "image.hh"
#include "logger.hh"


/* ********************************************************************************************* *
 * Implementation of AnnotationIssue
 * ********************************************************************************************* */
AnnotationIssue::AnnotationIssue(const Address &address, Severity severity, const QString &message)
  : QTextStream(), _address(address), _severity(severity), _message(message)
{
  this->setString(&_message, QIODeviceBase::ReadWrite);
}

AnnotationIssue::AnnotationIssue(const AnnotationIssue &other)
  : QTextStream(), _address(other._address), _severity(other._severity), _message(other._message)
{
  this->setString(&_message, QIODeviceBase::ReadWrite);
}

AnnotationIssue &
AnnotationIssue::operator =(const AnnotationIssue &other) {
  _address = other._address;
  _severity = other._severity;
  _message = other._message;
  return *this;
}

const Address &
AnnotationIssue::address() const {
  return _address;
}

AnnotationIssue::Severity
AnnotationIssue::severity() const {
  return _severity;
}

const QString &
AnnotationIssue::message() const {
  return _message;
}


/* ********************************************************************************************* *
 * Implementation of AnnotationIssues
 * ********************************************************************************************* */
AnnotationIssues::AnnotationIssues()
  : _issues()
{
  // pass...
}

void
AnnotationIssues::add(const AnnotationIssue &issue) {
  _issues.append(issue);
}

unsigned int
AnnotationIssues::numIssues() const {
  return _issues.size();
}

const AnnotationIssue &
AnnotationIssues::issue(unsigned int n) const {
  return _issues[n];
}

bool
AnnotationIssues::has(AnnotationIssue::Severity severity) const {
  foreach (const AnnotationIssue &issue, _issues)
    if (issue.severity() == severity)
      return true;
  return false;
}

AnnotationIssues::iterator
AnnotationIssues::begin() const {
  return _issues.begin();
}

AnnotationIssues::iterator
AnnotationIssues::end() const {
  return _issues.end();
}


/* ********************************************************************************************* *
 * Implementation of AnnotationCollection
 * ********************************************************************************************* */
AnnotationCollection::AnnotationCollection()
  : _annotations()
{
  // pass...
}

AnnotationCollection::~AnnotationCollection() {
}

bool
AnnotationCollection::unAnnotated() const {
  return _annotations.isEmpty();
}

unsigned int
AnnotationCollection::numAnnotations() const {
  return _annotations.size();
}

const AbstractAnnotation *
AnnotationCollection::annotation(unsigned int n) const {
  if (n >= numAnnotations())
    return nullptr;
  return _annotations[n];
}

void
AnnotationCollection::addAnnotation(AbstractAnnotation *child) {
  _annotations.append(child);
}

void
AnnotationCollection::clearAnnotations() {
  for (auto annotation: _annotations)
    delete annotation;
  _annotations.clear();
}

const AbstractAnnotation *
AnnotationCollection::annotationAt(const Address &addr) const {
  if (_annotations.isEmpty())
    return nullptr;

  unsigned int a=0, b=(_annotations.size()-1);

  if (_annotations[a]->contains(addr))
    return _annotations[a];
  if (_annotations[b]->contains(addr))
    return _annotations[b];

  while (a < b) {
    unsigned int mid = (a+b)/2;
    if (_annotations[mid]->contains(addr))
      return _annotations[mid];
    if (_annotations[mid]->address() > addr)
      b = mid;
    else
      a = mid;
    if (1 == (b-a))
      return nullptr;
  }

  if ((a == b) && (_annotations[a]->contains(addr)))
    return _annotations[a];

  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of AbstractAnnotation
 * ********************************************************************************************* */
AbstractAnnotation::AbstractAnnotation(const BlockPattern *pattern, const Address &addr, const Size& size, QObject *parent)
  : QObject{parent}, _address(addr), _size(size), _pattern(pattern), _issues()
{
  connect(_pattern, &QObject::destroyed, this, &AbstractAnnotation::onPatternDeleted);
}

const Address &
AbstractAnnotation::address() const {
  return _address;
}

const Size &
AbstractAnnotation::size() const {
  return _size;
}

bool
AbstractAnnotation::contains(const Address& addr) const {
  if (addr < _address)
    return false;
  return addr < (_address+size());
}

const BlockPattern *
AbstractAnnotation::pattern() const {
  return _pattern;
}

QStringList
AbstractAnnotation::path() const {
  QStringList path;
  const AbstractPattern *pattern = _pattern;
  while (nullptr != pattern) {
    path.append(pattern->meta().name());
    pattern = qobject_cast<AbstractPattern*>(pattern->parent());
  }
  return path;
}

const AnnotationIssues &
AbstractAnnotation::issues() const {
  return _issues;
}

AnnotationIssues &
AbstractAnnotation::issues() {
  return _issues;
}

void
AbstractAnnotation::onPatternDeleted() {
  delete this;
}


/* ********************************************************************************************* *
 * Implementation of FieldAnnotation
 * ********************************************************************************************* */
FieldAnnotation::FieldAnnotation(const FieldPattern *pattern, const Address& addr, const QVariant &value, QObject *parent)
  : AbstractAnnotation{pattern, addr, pattern->size(), parent}, _value(value)
{
  // pass...
}

const QVariant &
FieldAnnotation::value() const {
  return _value;
}

const FieldAnnotation *
FieldAnnotation::resolve(const Address &addr) const {
  if (this->contains(addr))
    return this;
  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of StructuredAnnotation
 * ********************************************************************************************* */
StructuredAnnotation::StructuredAnnotation(const BlockPattern *pattern, const Address &addr, QObject *parent)
  : AbstractAnnotation{pattern, addr, Size::zero(), parent}, AnnotationCollection()
{
  // pass...
}

void
StructuredAnnotation::addAnnotation(AbstractAnnotation *child) {
  child->setParent(this);
  _size += child->size();
  AnnotationCollection::addAnnotation(child);
}

const FieldAnnotation *
StructuredAnnotation::resolve(const Address &addr) const {
  const AbstractAnnotation *element = annotationAt(addr);
  if (nullptr != element)
    return element->resolve(addr);
  return nullptr;
}


/* ********************************************************************************************* *
 * Implementation of ImageAnnotator
 * ********************************************************************************************* */
bool
ImageAnnotator::annotate(const Image *image, const CodeplugPattern *pattern) {
  for (unsigned int i=0; i<pattern->numChildPattern(); i++) {
    AbstractPattern *child = pattern->childPattern(i);
    if (child->is<RepeatPattern>()) {
      if (! annotate(image, child->as<RepeatPattern>(), child->address()))
        return false;
    } else if (child->is<BlockRepeatPattern>()) {
      Element *el = image->find(child->address());
      if ((nullptr == el) && child->as<BlockRepeatPattern>()->minRepetition()) {
        AnnotationIssue issue(child->address(), AnnotationIssue::Error);
        issue << "Cannot annotate non-optional block-repeat pattern '" << child->meta().name()
              << "': No element found for address " << child->address().toString() << ".";
        logWarn() << "At " << issue.address().toString() << ": " << issue.message();
        return false;
      }
      if (nullptr != el)
        annotate(*el, el, child->as<BlockPattern>(), child->address());
    } else if (child->is<BlockPattern>()) {
      Element *el = image->find(child->address());
      if (nullptr == el) {
        AnnotationIssue issue(child->address(), AnnotationIssue::Error);
        issue << "Cannot annotate block pattern '" << child->meta().name()
              << "': No element found for address " << child->address().toString() << ".";
        logWarn() << "At " << issue.address().toString() << ": " << issue.message();
        return false;
      }
      annotate(*el, el, child->as<BlockPattern>(), child->address());
    }
  }
  return true;
}


bool
ImageAnnotator::annotate(const Image *image, const RepeatPattern *pattern, const Address& address) {
  Address addr = address ;
  for (unsigned int i=0; i<pattern->maxRepetition(); i++) {
    AbstractPattern *child = pattern->subpattern();
    if (child->is<RepeatPattern>()) {
      if (! annotate(image, child->as<RepeatPattern>(), addr)) {
        if ((i+1) <= pattern->minRepetition()) {
          logWarn() << "Cannot annotate pattern '" << pattern->meta().name()
                    << "': Minimum repetition not met.";
          return false;
        }
        logDebug() << "Processed pattern repeat pattern '" << pattern->meta().name() << "'.";
        return true;
      }
    } else if (child->is<BlockPattern>()) {
      Element *el = image->find(addr);
      if (nullptr == el) {
        if ((i+1) <= pattern->minRepetition())
          logWarn() << "Cannot annotate block pattern '" << child->meta().name()
                    << "': No element found for address " << addr.toString() << ".";
        if ((i+1) > pattern->minRepetition())
          logDebug() << "Processed pattern repeat pattern '" << pattern->meta().name() << "'.";
        return ((i+1) > pattern->minRepetition());
      }
      if (! annotate(*el, el, child->as<BlockPattern>(), addr)) {
        if ((i+1) <= pattern->minRepetition()) {
          logWarn() << "Cannot annotate pattern '" << pattern->meta().name()
                    << "': Minimum repetition not met.";
          return false;
        }
        logDebug() << "Processed pattern repeat pattern '" << pattern->meta().name() << "'.";
        return true;
      }
    }
    addr += pattern->step();
  }

  logDebug() << "Processed pattern repeat pattern '" << pattern->meta().name() << "'.";
  return true;
}

bool
ImageAnnotator::annotate(AnnotationCollection &parent, const Element *element, const BlockPattern *pattern, const Address &addr) {
  // Dispatch by type
  if (pattern->is<BlockRepeatPattern>())
    return annotate(parent, element, pattern->as<BlockRepeatPattern>(), addr);
  else if (pattern->is<FixedRepeatPattern>())
    return annotate(parent, element, pattern->as<FixedRepeatPattern>(), addr);
  else if (pattern->is<ElementPattern>())
    return annotate(parent, element, pattern->as<ElementPattern>(), addr);
  else if (pattern->is<FieldPattern>())
    return annotate(parent, element, pattern->as<FieldPattern>(), addr);
  return false;
}

bool
ImageAnnotator::annotate(AnnotationCollection &parent, const Element *element, const BlockRepeatPattern *pattern, const Address& address) {
  Address addr = address;
  Address end = element->address() + element->size();
  FixedPattern *child = pattern->subpattern();
  StructuredAnnotation *annotation = new StructuredAnnotation(pattern, address);
  for (unsigned int i=0; i<pattern->maxRepetition(); i++, addr+=child->size()) {
    if ((addr >= end) && ((i+1) >= pattern->minRepetition()))
      break;
    if (addr >= end) {
      logWarn() << "Min repetition of block repeat '" << pattern->meta().name() << "' not reached.";
      delete annotation;
      return false;
    }
    if (! annotate(*annotation, element, child, addr)) {
      delete annotation;
      return false;
    }
  }

  parent.addAnnotation(annotation);
  return true;
}


bool
ImageAnnotator::annotate(AnnotationCollection &parent, const Element *element, const FixedRepeatPattern *pattern, const Address& address) {
  Address addr = address;
  Address end = element->address() + element->size();
  FixedPattern *child = pattern->subpattern();
  StructuredAnnotation *annotation = new StructuredAnnotation(pattern, address);
  for (unsigned int i=0; i<pattern->repetition(); i++, addr += child->size()) {
    if (addr >= end) {
      logWarn() << "Repetition of fixed repeat '" << pattern->meta().name() << "' not reached.";
      delete annotation;
      return false;
    }
    if (! annotate(*annotation, element, child, addr)) {
      delete annotation;
      return false;
    }
  }

  parent.addAnnotation(annotation);
  return true;
}

bool
ImageAnnotator::annotate(AnnotationCollection &parent, const Element *element, const ElementPattern *pattern, const Address& address) {
  Address addr = address;
  Address end = element->address() + element->size();

  StructuredAnnotation *annotation = new StructuredAnnotation(pattern, address);
  for (unsigned int i=0; i<pattern->numChildPattern(); i++) {
    if (addr >= end) {
      logWarn() << "Cannot match element '" << pattern->meta().name() << "' end of data.";
      delete annotation;
      return false;
    }
    FixedPattern *child = pattern->childPattern(i)->as<FixedPattern>();
    if (! annotate(*annotation, element, child, addr)) {
      delete annotation;
      return false;
    }
    addr += child->size();
  }

  parent.addAnnotation(annotation);
  logDebug() << "Processed pattern '" << pattern->meta().name() << "' at " << address.toString() << ".";

  return true;
}

bool
ImageAnnotator::annotate(AnnotationCollection &parent, const Element *element, const FieldPattern *pattern, const Address& address) {
  Address end = element->address() + element->size();

  if ((address + pattern->size()) > end) {
    logWarn() << "Field pattern '" << pattern->meta().name() << " at " << address.toString()
              << " of size " << pattern->size().toString()
              << " does not fit into element at " << element->address().toString()
              << " of size " << element->size().toString() << ".";
    return false;
  }

  parent.addAnnotation(new FieldAnnotation(pattern, address, pattern->value(element, address)));
  return true;
}
