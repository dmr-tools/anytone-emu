#include "modelparser.hh"
#include <QXmlStreamAttributes>



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


const QList<ModelRom::Segment> &
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

