#include "md32uvmodelparser.hh"

#include <QXmlStreamAttributes>
#include <QFileInfo>


/* ********************************************************************************************* *
 * Implementation of MD32UVModelDefinitionHandler
 * ********************************************************************************************* */
MD32UVModelDefinitionHandler::MD32UVModelDefinitionHandler(const QString &context, const QString &id, ModelDefinitionParser *parent)
  : ModelDefinitionHandler{context, id, parent}, _definition(new MD32UVModelDefinition(id, this))
{
  // pass...
}

ModelDefinition *
MD32UVModelDefinitionHandler::definition() const {
  return _definition;
}

ModelDefinition *
MD32UVModelDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
MD32UVModelDefinitionHandler::beginFirmwareElement(const QXmlStreamAttributes &attributes) {
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

  pushHandler(new MD32UVModelFirmwareDefinitionHandler(_context, name, released, codeplug, this));

  return true;
}

bool
MD32UVModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<MD32UVModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
  delete handler;
  return true;
}



/* ********************************************************************************************* *
 * Implementation of MD32UVModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
MD32UVModelFirmwareDefinitionHandler::MD32UVModelFirmwareDefinitionHandler(
    const QString &context, const QString &name, const QDate &released, const QString &codeplug,
    ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
    _definition(new MD32UVModelFirmwareDefinition{context, nullptr})
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(codeplug);
}

MD32UVModelFirmwareDefinitionHandler::~MD32UVModelFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
MD32UVModelFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
MD32UVModelFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


