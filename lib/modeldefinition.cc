#include "modeldefinition.hh"
#include <algorithm>
#include <QXmlStreamAttributes>



/* ********************************************************************************************** *
 * Implementation of ModelRom::Segment
 * ********************************************************************************************** */
bool
Rom::Segment::operator <(const Segment &rhs) const {
  return address < rhs.address;
}

bool
Rom::Segment::operator <(uint32_t rhs_address) const {
  return address < rhs_address;
}

bool
Rom::Segment::contains(uint32_t address, uint8_t size) const {
  return (this->address <= address)
      && ((this->address+content.size()) >= (address+size));
}



/* ********************************************************************************************** *
 * Implementation of ModelRom
 * ********************************************************************************************** */
Rom::Rom()
  : _content()
{
  // pass...
}


Rom
Rom::operator +(const Rom &rhs) const {
  Rom res(*this);
  foreach (const Segment &segment, rhs) {
    res.write(segment.address, segment.content);
  }
  return res;
}

const Rom &
Rom::operator +=(const Rom &rhs) {
  foreach (const Segment &segment, rhs) {
    write(segment.address, segment.content);
  }
  return *this;
}


void
Rom::write(uint32_t address, const QByteArray &data) {
  auto next = std::lower_bound(begin(), end(), address);

  if ((end() == next) || (next->address > address)) {
    _content.insert(next, {address, data});
  } else if (next->contains(address, data.size())) {
    uint32_t offset = address - next->address;
    memcpy(next->content.data()+offset, data.data(), data.size());
  } else if ((next->address+next->content.size()) == address) {
    next->content.append(data);
  } else {
    _content.insert(++next, {address, data});
  }
}


bool
Rom::read(uint32_t address, uint8_t length, QByteArray &data) const {
  auto next = std::lower_bound(begin(), end(), address);

  if (end() == next)
    return false;

  if (! next->contains(address, length))
    return false;

  uint32_t offset = address - next->address;
  data = next->content.mid(offset, length);
  return true;
}


Rom::iterator
Rom::begin() {
  return _content.begin();
}

Rom::iterator
Rom::end() {
  return _content.end();
}


Rom::const_iterator
Rom::begin() const {
  return _content.begin();
}

Rom::const_iterator
Rom::end() const {
  return _content.end();
}



/* ********************************************************************************************** *
 * Implementation of ModelDefinition
 * ********************************************************************************************** */
ModelDefinition::ModelDefinition(QObject *parent)
  : QObject{parent}, _name(), _manufacturer(), _description(), _url(), _rom()
{
  // pass...
}


const QString &
ModelDefinition::name() const {
  return _name;
}

void
ModelDefinition::setName(const QString &name) {
  _name = name;
}


const QString &
ModelDefinition::manufacturer() const {
  return _manufacturer;
}

void
ModelDefinition::setManufacturer(const QString &manufacturer) {
  _manufacturer = manufacturer;
}


const QString &
ModelDefinition::description() const {
  return _description;
}

void
ModelDefinition::setDescription(const QString &description) {
  _description = description;
}


const QUrl &
ModelDefinition::url() const {
  return _url;
}

void
ModelDefinition::setUrl(const QUrl &url) {
  _url = url;
}


void
ModelDefinition::storeRom(uint32_t address, const QByteArray &data) {
  _rom.write(address, data);
}

const Rom &
ModelDefinition::rom() const {
  return _rom;
}


void
ModelDefinition::addFirmware(ModelFirmwareDefinition *firmware) {
  firmware->setParent(this);
  _firmwares.append(firmware);
}


/* ********************************************************************************************** *
 * Implementation of ModelFirmwareDefinition
 * ********************************************************************************************** */
ModelFirmwareDefinition::ModelFirmwareDefinition(ModelDefinition *parent)
  : QObject{parent}, _name(), _released(), _description(), _rom()
{
  // pass...
}


const QString &
ModelFirmwareDefinition::name() const {
  return _name;
}

void
ModelFirmwareDefinition::setName(const QString &name) {
  _name = name;
}


const QString &
ModelFirmwareDefinition::description() const {
  return _description;
}

void
ModelFirmwareDefinition::setDescription(const QString &description) {
  _description = description;
}


const QDate &
ModelFirmwareDefinition::released() const {
  return _released;
}

void
ModelFirmwareDefinition::setReleased(const QDate &released) {
  _released = released;
}


void
ModelFirmwareDefinition::storeRom(uint32_t address, const QByteArray &data) {
  _rom.write(address, data);
}

Rom
ModelFirmwareDefinition::rom() const {
  return qobject_cast<ModelDefinition*>(parent())->rom() + _rom;
}



/* ********************************************************************************************** *
 * Implementation of ModelDefinitionParser
 * ********************************************************************************************** */
ModelDefinitionParser::ModelDefinitionParser(QObject *parent)
  : XmlParser(parent), _modelDefinition(nullptr)
{
  // pass...
}


ModelDefinition *
ModelDefinitionParser::definition() const {
  return _modelDefinition;
}

ModelDefinition *
ModelDefinitionParser::takeDefinition() {
  if (nullptr == _modelDefinition)
    return nullptr;

  auto def = _modelDefinition;
  def->setParent(nullptr);
  _modelDefinition = nullptr;

  return def;
}


bool
ModelDefinitionParser::beginModelElement(const QXmlStreamAttributes &attributes) {
  if ((! attributes.hasAttribute("class")) || (attributes.value("class").isEmpty())) {
    raiseError("No 'class' attribute specified.");
    return false;
  }

  QString modelParserClass = QString("%1ModelDefinitionParser")
      .arg(attributes.value("class"));

  QMetaType typeId = QMetaType::fromName(modelParserClass.toLatin1());
  if (! typeId.isValid()) {
    raiseError(QString("Cannot find class specific parser '%1'.").arg(modelParserClass));
    return false;
  }

  QObject *instance = typeId.metaObject()->newInstance();
  if ((nullptr == instance) || (nullptr == qobject_cast<ModelDefinitionParser*>(instance))) {
    raiseError(QString("Cannot instantiate model specific parser '%1'.").arg(modelParserClass));
    return false;
  }

  pushHandler(qobject_cast<ModelDefinitionParser*>(instance));

  return true;
}


bool
ModelDefinitionParser::endModelElement() {
  XmlElementHandler *modelHandler = popHandler();
  if (nullptr == qobject_cast<ModelDefinitionHandler*>(modelHandler)) {
    raiseError(QString("Unexpected type of parser on stack '%1', expected 'ModelDefinitionHandler'.")
               .arg(modelHandler->metaObject()->className()));
    delete modelHandler;
    return false;
  }

  _modelDefinition = qobject_cast<ModelDefinitionHandler*>(modelHandler)->takeDefinition();
  delete modelHandler;

  if (nullptr == _modelDefinition) {
    raiseError("No definition obtained from model specific parser.");
    return false;
  }

  _modelDefinition->setParent(this);

  return true;
}


/* ********************************************************************************************** *
 * Implementation of ModelDefinitionHandler
 * ********************************************************************************************** */
ModelDefinitionHandler::ModelDefinitionHandler(ModelDefinitionParser *parent)
  : XmlElementHandler{parent}
{
  // pass...
}

bool
ModelDefinitionHandler::beginNameElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <name> element, no model definition created yet.");
    return false;
  }

  pushHandler(new XmlTextHandler(this));

  return true;
}

bool
ModelDefinitionHandler::endNameElement() {
  auto handler = qobject_cast<XmlTextHandler*>(popHandler());
  qobject_cast<ModelDefinitionHandler*>(parent())
      ->definition()->setName(handler->content());
  delete handler;
  return true;
}


bool
ModelDefinitionHandler::beginManufacturerElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <manufacturer> element, no model definition created yet.");
    return false;
  }

  pushHandler(new XmlTextHandler(this));

  return true;
}

bool
ModelDefinitionHandler::endManufacturerElement() {
  auto handler = qobject_cast<XmlTextHandler*>(popHandler());
  qobject_cast<ModelDefinitionHandler*>(parent())
      ->definition()->setManufacturer(handler->content());
  delete handler;
  return true;
}


bool
ModelDefinitionHandler::beginDescriptionElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <description> element, no model definition created yet.");
    return false;
  }

  pushHandler(new XmlTextHandler(this));

  return true;
}

bool
ModelDefinitionHandler::endDescriptionElement() {
  auto handler = qobject_cast<XmlTextHandler*>(popHandler());
  qobject_cast<ModelDefinitionHandler*>(parent())
      ->definition()->setDescription(handler->content());
  delete handler;
  return true;
}


bool
ModelDefinitionHandler::beginUrlElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <url> element, no model definition created yet.");
    return false;
  }

  pushHandler(new XmlUrlHandler(this));

  return true;
}

bool
ModelDefinitionHandler::endUrlElement() {
  auto handler = qobject_cast<XmlUrlHandler*>(popHandler());
  qobject_cast<ModelDefinitionHandler*>(parent())
      ->definition()->setUrl(handler->url());
  delete handler;
  return true;
}


bool
ModelDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <memory> element, no model definition created yet.");
    return false;
  }

  pushHandler(new ModelMemoryDefinitionHandler(this));

  return true;
}


bool
ModelDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<ModelMemoryDefinitionHandler*>(popHandler());
  foreach (auto mapping, handler->mappings()) {
    qobject_cast<ModelDefinitionHandler*>(parent())
        ->definition()->storeRom(mapping.address, mapping.content);
  }
  delete handler;
  return true;
}



/* ********************************************************************************************** *
 * Implementation of ModelMemoryDefinitionHandler
 * ********************************************************************************************** */
ModelMemoryDefinitionHandler::ModelMemoryDefinitionHandler(XmlElementHandler *parent)
  : XmlElementHandler{parent}
{
  // pass...
}


const QList<Rom::Segment> &
ModelMemoryDefinitionHandler::mappings() const {
  return _mappings;
}


bool
ModelMemoryDefinitionHandler::beginMapElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("address")) {
    raiseError("No 'address' attribute specified.");
    return false;
  }

  bool ok; uint32_t address = attributes.value("address").toUInt(&ok, 16);
  if (! ok) {
    raiseError(QString("Invalid address '%1'.").arg(attributes.value("address")));
    return false;
  }

  _mappings.append({address, ""});
  pushHandler(new XmlHexDataHandler(this));

  return true;
}

bool
ModelMemoryDefinitionHandler::endMapElement() {
  auto mapParser = qobject_cast<XmlHexDataHandler*>(popHandler());
  _mappings.back().content.append(mapParser->data());
  delete mapParser;
  return true;
}




/* ********************************************************************************************** *
 * Implementation of ModelFirmwareDefinitionHandler
 * ********************************************************************************************** */
ModelFirmwareDefinitionHandler::ModelFirmwareDefinitionHandler(ModelDefinitionHandler *parent)
  : XmlElementHandler{parent}
{
  // pass...
}



bool
ModelFirmwareDefinitionHandler::beginDescriptionElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <description> element, no firmware definition created yet.");
    return false;
  }

  pushHandler(new XmlTextHandler(this));

  return true;
}

bool
ModelFirmwareDefinitionHandler::endDescriptionElement() {
  auto handler = qobject_cast<XmlTextHandler*>(popHandler());
  qobject_cast<ModelFirmwareDefinitionHandler*>(parent())
      ->definition()->setDescription(handler->content());
  delete handler;
  return true;
}


bool
ModelFirmwareDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <memory> element, no firmware definition created yet.");
    return false;
  }

  pushHandler(new ModelMemoryDefinitionHandler(this));

  return true;
}


bool
ModelFirmwareDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<ModelMemoryDefinitionHandler*>(popHandler());
  foreach (auto mapping, handler->mappings()) {
    qobject_cast<ModelFirmwareDefinitionHandler*>(parent())
        ->definition()->storeRom(mapping.address, mapping.content);
  }
  delete handler;
  return true;
}

