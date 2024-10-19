#include "modelparser.hh"
#include <QXmlStreamAttributes>

#include "anytonemodelparser.hh"

/* ********************************************************************************************** *
 * Implementation of ModelDefinitionParser
 * ********************************************************************************************** */
ModelDefinitionParser::ModelDefinitionParser(ModelCatalog* catalog, const QString& context, QObject *parent)
  : XmlParser(parent), _context(context), _catalog(catalog)
{
  // pass...
}

bool
ModelDefinitionParser::beginCatalogElement(const QXmlStreamAttributes &attributes) {
  return true;
}

bool
ModelDefinitionParser::endCatalogElement() {
  return true;
}


bool
ModelDefinitionParser::beginModelElement(const QXmlStreamAttributes &attributes) {
  if ((! attributes.hasAttribute("class")) || (attributes.value("class").isEmpty())) {
    raiseError("No 'class' attribute specified.");
    return false;
  }

  if ((! attributes.hasAttribute("id")) || (attributes.value("id").isEmpty())) {
    raiseError("No model 'id' attribute specified.");
    return false;
  }

  QString modelClass(attributes.value("class").toString());
  QString modelId(attributes.value("id").toString());

  if ("AnyTone" == modelClass) {
    pushHandler(qobject_cast<ModelDefinitionHandler*>(new AnyToneModelDefinitionHandler(_context, modelId, this)));
  } else {
    raiseError(QString("Unknown model class '%1'").arg(modelClass));
    return false;
  }

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

  auto modelDefinition = qobject_cast<ModelDefinitionHandler*>(modelHandler)->takeDefinition();
  delete modelHandler;

  if (nullptr == modelDefinition) {
    raiseError("No definition obtained from model specific parser.");
    return false;
  }

  modelDefinition->setParent(this);
  _catalog->addModel(modelDefinition);

  return true;
}


/* ********************************************************************************************** *
 * Implementation of ModelDefinitionHandler
 * ********************************************************************************************** */
ModelDefinitionHandler::ModelDefinitionHandler(const QString& context, const QString &id, ModelDefinitionParser *parent)
  : XmlElementHandler{parent}, _context(context), _id(id)
{
  // pass...
}

bool
ModelDefinitionHandler::beginNameElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
ModelDefinitionHandler::endNameElement() {
  definition()->setName(textBuffer().simplified());
  return true;
}


bool
ModelDefinitionHandler::beginManufacturerElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
ModelDefinitionHandler::endManufacturerElement() {
  definition()->setManufacturer(textBuffer().simplified());
  return true;
}


bool
ModelDefinitionHandler::beginDescriptionElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
ModelDefinitionHandler::endDescriptionElement() {
  definition()->setDescription(textBuffer().simplified());
  return true;
}


bool
ModelDefinitionHandler::beginUrlElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
ModelDefinitionHandler::endUrlElement() {
  definition()->setUrl(QUrl(textBuffer().simplified()));
  return true;
}


bool
ModelDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  pushHandler(new ModelMemoryDefinitionHandler(this));
  return true;
}


bool
ModelDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<ModelMemoryDefinitionHandler*>(popHandler());

  foreach (auto mapping, handler->mappings()) {
    definition()->storeRom(mapping.address, mapping.content);
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
  clearTextBuffer();
  return true;
}

bool
ModelMemoryDefinitionHandler::endMapElement() {
  _mappings.back().content.append(QByteArray::fromHex(textBuffer().toLocal8Bit()));
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
  clearTextBuffer();
  return true;
}

bool
ModelFirmwareDefinitionHandler::endDescriptionElement() {
  definition()->setDescription(textBuffer().simplified());
  return true;
}


bool
ModelFirmwareDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  pushHandler(new ModelMemoryDefinitionHandler(this));
  return true;
}


bool
ModelFirmwareDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<ModelMemoryDefinitionHandler*>(popHandler());

  foreach (auto mapping, handler->mappings()) {
    definition()->storeRom(mapping.address, mapping.content);
  }

  delete handler;
  return true;
}

