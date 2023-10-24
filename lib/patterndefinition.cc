#include "patterndefinition.hh"
#include "logger.hh"

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
  return parent() && (nullptr==qobject_cast<CodeplugPattern *>(parent()));
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
