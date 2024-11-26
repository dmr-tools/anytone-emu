#include "opengd77modelparser.hh"
#include <QXmlStreamAttributes>
#include <QFileInfo>


/* ********************************************************************************************* *
 * Implementation of OpenGD77ModelDefinitionHandler
 * ********************************************************************************************* */
OpenGD77ModelDefinitionHandler::OpenGD77ModelDefinitionHandler(const QString &context, const QString &id, ModelDefinitionParser *parent)
  : ModelDefinitionHandler{context, id, parent}, _definition(new OpenGD77ModelDefinition(id, this))
{
  // pass...
}

ModelDefinition *
OpenGD77ModelDefinitionHandler::definition() const {
  return _definition;
}

ModelDefinition *
OpenGD77ModelDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
OpenGD77ModelDefinitionHandler::beginFirmwareElement(const QXmlStreamAttributes &attributes) {
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

  pushHandler(new OpenGD77ModelFirmwareDefinitionHandler(_context, name, released, codeplug, this));

  return true;
}

bool
OpenGD77ModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<OpenGD77ModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
  delete handler;
  return true;
}



/* ********************************************************************************************* *
 * Implementation of OpenGD77ModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
OpenGD77ModelFirmwareDefinitionHandler::OpenGD77ModelFirmwareDefinitionHandler(
    const QString &context, const QString &name, const QDate &released, const QString &codeplug,
    ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
    _definition(new OpenGD77ModelFirmwareDefinition{context, nullptr})
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(codeplug);
}

OpenGD77ModelFirmwareDefinitionHandler::~OpenGD77ModelFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
OpenGD77ModelFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
OpenGD77ModelFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


