#include "radtelmodelparser.hh"

#include <QXmlStreamAttributes>
#include <QFileInfo>


/* ********************************************************************************************* *
 * Implementation of RadtelModelDefinitionHandler
 * ********************************************************************************************* */
RadtelModelDefinitionHandler::RadtelModelDefinitionHandler(const QString &context, const QString &id, ModelDefinitionParser *parent)
  : ModelDefinitionHandler{context, id, parent}, _definition(new RadtelModelDefinition(id, this))
{
  // pass...
}

ModelDefinition *
RadtelModelDefinitionHandler::definition() const {
  return _definition;
}

ModelDefinition *
RadtelModelDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
RadtelModelDefinitionHandler::beginFirmwareElement(const QXmlStreamAttributes &attributes) {
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

  pushHandler(new RadtelModelFirmwareDefinitionHandler(_context, name, released, codeplug, this));

  return true;
}

bool
RadtelModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<RadtelModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
  delete handler;
  return true;
}



/* ********************************************************************************************* *
 * Implementation of RadtelModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
RadtelModelFirmwareDefinitionHandler::RadtelModelFirmwareDefinitionHandler(
    const QString &context, const QString &name, const QDate &released, const QString &codeplug,
    ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
    _definition(new RadtelModelFirmwareDefinition{context, nullptr})
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(codeplug);
}

RadtelModelFirmwareDefinitionHandler::~RadtelModelFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
RadtelModelFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
RadtelModelFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


