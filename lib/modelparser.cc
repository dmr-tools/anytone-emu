#include "modelparser.hh"
#include <QXmlStreamAttributes>

#include "modeldefinition.hh"

#include "anytonemodelparser.hh"
#include "opengd77modelparser.hh"
#include "radtelmodelparser.hh"

#include <QFileInfo>



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
  } else if ("OpenGD77" == modelClass) {
    pushHandler(qobject_cast<ModelDefinitionHandler*>(new OpenGD77ModelDefinitionHandler(_context, modelId, this)));
  } else if ("Radtel" == modelClass) {
    pushHandler(qobject_cast<ModelDefinitionHandler*>(new RadtelModelDefinitionHandler(_context, modelId, this)));
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
 * Implementation of GenericModelDefinitionHandler
 * ********************************************************************************************** */
GenericModelDefinitionHandler::GenericModelDefinitionHandler(
    DeviceClassPluginInterface *plugin, const QString &context, const QString& id,
    ModelDefinitionParser *parent)
  : ModelDefinitionHandler{context, id, parent}, _plugin(plugin),
    _definition(plugin->modelDefinition(id, nullptr))
{
  // pass...
}

ModelDefinition *
GenericModelDefinitionHandler::definition() const {
  return _definition;
}

ModelDefinition *
GenericModelDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
GenericModelDefinitionHandler::beginFirmwareElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("name")) {
    raiseError("No 'name' attribute given.");
    return false;
  }
  QString name(attributes.value("name").toString());

  if (! attributes.hasAttribute("codeplug")) {
    raiseError("No 'codeplug' attribute given.");
    return false;
  }

  QString codeplug(attributes.value("codeplug").toString());
  QFileInfo codeplugFileInfo(_context + "/" + codeplug);
  if (!codeplugFileInfo.isFile() || !codeplugFileInfo.isReadable()) {
    raiseError(QString("Cannot read codeplug file '%1'.").arg(codeplugFileInfo.filePath()));
    return false;
  }

  QDate released;
  if (attributes.hasAttribute("released")) {
    released = QDate::fromString(attributes.value("released"));
  }

  pushHandler(new GenericModelFirmwareDefinitionHandler(_plugin, _context, name, released, codeplug, this));

  return true;
}

bool
GenericModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<ModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
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


/* ********************************************************************************************* *
 * Implementation of GenericModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
GenericModelFirmwareDefinitionHandler::GenericModelFirmwareDefinitionHandler(
    DeviceClassPluginInterface *plugin, const QString &context, const QString &name,
    const QDate &released, const QString &codeplug, ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
    _definition(new GenericModelFirmwareDefinition(plugin, context, nullptr))
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(codeplug);
}

GenericModelFirmwareDefinitionHandler::~GenericModelFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
GenericModelFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
GenericModelFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


