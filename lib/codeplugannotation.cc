#include "codeplugannotation.hh"
#include "codeplugpattern.hh"
#include "image.hh"


/* ********************************************************************************************* *
 * Implementation of FieldAnnotation
 * ********************************************************************************************* */
FieldAnnotation::FieldAnnotation(const FieldPattern *pattern, const Offset &offset, const QVariant &value, QObject *parent)
  : QObject{parent}, _offset(offset), _pattern(pattern), _value(value)
{
  connect(_pattern, &QObject::destroyed, this, &FieldAnnotation::onPatternDeleted);
}

const Offset &
FieldAnnotation::offset() const {
  return _offset;
}

const Offset &
FieldAnnotation::size() const {
  return _pattern->size();
}

bool
FieldAnnotation::contains(const Offset &offset) const {
  if (offset < _offset)
    return false;
  return offset < (_offset+size());
}

const QVariant &
FieldAnnotation::value() const {
  return _value;
}

const FieldPattern *
FieldAnnotation::pattern() const {
  return _pattern;
}

QStringList
FieldAnnotation::path() const {
  QStringList path;
  const AbstractPattern *pattern = _pattern;
  while (nullptr != pattern) {
    path.append(pattern->meta().name());
    pattern = qobject_cast<AbstractPattern*>(pattern->parent());
  }
  return path;
}

void
FieldAnnotation::onPatternDeleted() {
  delete this;
}


/* ********************************************************************************************* *
 * Implementation of ImageAnnotation
 * ********************************************************************************************* */
ImageAnnotation::ImageAnnotation(const Image *image, const CodeplugPattern *pattern, QObject *parent)
  : QObject{parent}, _image(image), _pattern(pattern)
{
  annotate(_annotations, _image, _pattern);
}

bool
ImageAnnotation::isEmpty() const {
  return _annotations.isEmpty();
}

FieldAnnotation *
ImageAnnotation::at(const Offset &offset) const {
  if (_annotations.isEmpty())
    return nullptr;
  unsigned int a=0, b=_annotations.size();
  while (a<b) {
    unsigned int mid = (a+b)/2;
    if (_annotations[mid]->contains(offset))
      return _annotations[mid];
    if (_annotations[mid]->offset() > offset)
      b = mid;
    else
      a = mid;
  }
  if ((a == b) && (_annotations[a]->contains(offset)))
    return _annotations[a];
  return nullptr;
}

bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Image *image, const CodeplugPattern *pattern) {
  for (unsigned int i=0; i<pattern->numChildPattern(); i++) {
    AbstractPattern *child = pattern->childPattern(i);
    if (child->is<RepeatPattern>()) {
      if (! annotate(annotations, image, child->as<RepeatPattern>(), child->offset()))
        return false;
    } else if (child->is<BlockPattern>()) {
      const Element *el = image->findPred(child->offset().byte());
      if (nullptr == el)
        return false;
      annotate(annotations, el, child->as<BlockPattern>(), child->offset());
    }
  }
  return true;
}


bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Image *image, const RepeatPattern *pattern, const Offset &offset) {
  Offset addr = offset ;
  for (unsigned int i=0; i<pattern->maxRepetition(); i++) {
    AbstractPattern *child = pattern->subpattern();
    QVector<FieldAnnotation *> tmp;
    if (child->is<RepeatPattern>()) {
      if (! annotate(tmp, image, child->as<RepeatPattern>(), addr)) {
        if ((i+1) < pattern->minRepetition()) {
          foreach(auto a, tmp)
            delete a;
          return false;
        }
      }
    } else if (child->is<BlockPattern>()) {
      const Element *el = image->findPred(child->offset().byte());
      if ((nullptr == el) && ((i+1) < pattern->minRepetition()))
        return false;
      if (! annotate(tmp, el, child->as<BlockPattern>(), addr))
        return false;
    }
    annotations += tmp;
    addr += pattern->step();
  }

  return true;
}

bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const BlockPattern *pattern, const Offset &offset) {
  // Dispatch by type
  if (pattern->is<BlockRepeatPattern>())
    return annotate(annotations, element, pattern->as<BlockRepeatPattern>(), offset);
  else if (pattern->is<FixedRepeatPattern>())
    return annotate(annotations, element, pattern->as<FixedRepeatPattern>(), offset);
  else if (pattern->is<Element>())
    return annotate(annotations, element, pattern->as<ElementPattern>(), offset);
  else if (pattern->is<FieldPattern>())
    return annotate(annotations, element, pattern->as<FieldPattern>(), offset);
  return false;
}

bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const BlockRepeatPattern *pattern, const Offset &offset) {
  Offset addr = offset;
  Offset end = Offset::fromByte(element->address()+element->size());
  FixedPattern *child = pattern->subpattern();
  for (unsigned int i=0; i<pattern->maxRepetition(); i++, addr+=child->size()) {
    if ((addr >= end) && ((i+1) < pattern->minRepetition()))
      return false;
    if (! annotate(annotations, element, child, addr))
      return false;
  }

  return true;
}


bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const FixedRepeatPattern *pattern, const Offset &offset) {
  Offset addr = offset;
  Offset end = Offset::fromByte(element->address() + element->size());
  FixedPattern *child = pattern->subpattern();
  for (unsigned int i=0; i<pattern->repetition(); i++, addr+=child->size()) {
    if (addr >= end)
      return false;
    if (! annotate(annotations, element, child, addr))
      return false;
  }

  return true;
}

bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const ElementPattern *pattern, const Offset &offset) {
  Offset addr = offset;
  Offset end = Offset::fromByte(element->address() + element->size());
  for (unsigned int i=0; i<pattern->numChildPattern(); i++) {
    if (addr >= end)
      return false;
    FixedPattern *child = pattern->childPattern(i)->as<FixedPattern>();
    if (! annotate(annotations, element, child, addr))
      return false;
    addr += child->size();
  }

  return true;
}

bool
ImageAnnotation::annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const FieldPattern *pattern, const Offset &offset) {
  Offset end = Offset::fromByte(element->address() + element->size());
  if ((offset + pattern->size()) > end)
    return false;
  annotations.append(new FieldAnnotation(pattern, offset, pattern->value(element, offset)));
  return true;
}
