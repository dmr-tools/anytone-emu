#include "patternmimedata.hh"
#include "pattern.hh"


PatternMimeData::PatternMimeData(AbstractPattern *pattern)
  : QMimeData{}, _pattern(pattern)
{
  if (_pattern)
    _pattern->setParent(this);
}

QStringList
PatternMimeData::formats() const {
  return QStringList{"application/anytone-emu-pattern-object"};
}

bool
PatternMimeData::hasFormat(const QString &mimetype) const {
  return ("application/anytone-emu-pattern-object" == mimetype) && _pattern;
}

AbstractPattern *
PatternMimeData::pattern() const {
  return _pattern;
}

AbstractPattern *
PatternMimeData::takePattern() {
  AbstractPattern *ptr = _pattern;
  _pattern->setParent(nullptr);
  _pattern = nullptr;
  return ptr;
}

QVariant
PatternMimeData::retrieveData(const QString &mimetype, QMetaType preferredType) const {
  if (("application/anytone-emu-pattern-object" != mimetype) || (nullptr == _pattern))
    return QVariant();
  if (! preferredType.metaObject()->inherits(&AbstractPattern::staticMetaObject))
    return QVariant();
  return QVariant::fromValue(_pattern);
}


