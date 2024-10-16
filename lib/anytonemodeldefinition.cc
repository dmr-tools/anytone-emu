#include "anytonemodeldefinition.hh"
#include <QXmlStreamAttributes>


/* ********************************************************************************************* *
 * Implementation of AnyToneModelDefinition
 * ********************************************************************************************* */
AnyToneModelDefinition::AnyToneModelDefinition(QObject *parent)
  : ModelDefinition{parent}, _modelId(), _revision()
{
  // pass...
}


const QByteArray &
AnyToneModelDefinition::modelId() const {
  return _modelId;
}

void
AnyToneModelDefinition::setModelId(const QByteArray &id) {
  _modelId = id;
}


const QByteArray &
AnyToneModelDefinition::revision() const {
  return _revision;
}

void
AnyToneModelDefinition::setRevision(const QByteArray &rev) {
  _revision = rev;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneModelFirmwareDefinition
 * ********************************************************************************************* */
AnyToneModelFirmwareDefinition::AnyToneModelFirmwareDefinition(AnyToneModelDefinition *parent)
  : ModelFirmwareDefinition(parent), _modelId(), _revision()
{
  // pass...
}

const QByteArray &
AnyToneModelFirmwareDefinition::modelId() const {
  if (! _modelId.isEmpty())
    return _modelId;
  return qobject_cast<AnyToneModelDefinition *>(parent())->modelId();
}

void
AnyToneModelFirmwareDefinition::setModelId(const QByteArray &id) {
  _modelId = id;
}


const QByteArray &
AnyToneModelFirmwareDefinition::revision() const {
  if (! _revision.isEmpty())
    return _revision;
  return qobject_cast<AnyToneModelDefinition *>(parent())->revision();
}

void
AnyToneModelFirmwareDefinition::setRevision(const QByteArray &rev) {
  _revision = rev;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneModelDefinitionHandler
 * ********************************************************************************************* */
AnyToneModelDefinitionHandler::AnyToneModelDefinitionHandler(ModelDefinitionParser *parent)
  : ModelDefinitionHandler{parent}, _definition(new AnyToneModelDefinition(this))
{
  // pass...
}

ModelDefinition *
AnyToneModelDefinitionHandler::definition() const {
  return _definition;
}

ModelDefinition *
AnyToneModelDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
AnyToneModelDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <memory> element, no model definition created yet.");
    return false;
  }

  pushHandler(new AnyToneModelMemoryDefinitionHandler(this));

  return true;
}

bool
AnyToneModelDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<AnyToneModelMemoryDefinitionHandler*>(topHandler());
  if (! handler->modelId().isEmpty())
    qobject_cast<AnyToneModelDefinition*>(definition())->setModelId(handler->modelId());
  if (! handler->revision().isEmpty())
    qobject_cast<AnyToneModelDefinition*>(definition())->setRevision(handler->revision());

  return ModelDefinitionHandler::endMemoryElement();
}


bool
AnyToneModelDefinitionHandler::beginFirmwareElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <firmware> element, no model definition created yet.");
    return false;
  }

  if (! attributes.hasAttribute("name")) {
    raiseError("No 'name' attribute given.");
    return false;
  }
  QString name(attributes.value("name").toString());
  QDate released;
  if (attributes.hasAttribute("released")) {
    released = QDate::fromString(attributes.value("released"));
  }

  pushHandler(new AnyToneModelFirmwareDefinitionHandler(name, released, this));

  return true;
}

bool
AnyToneModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<AnyToneModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
  delete handler;
  return true;
}


/* ********************************************************************************************* *
 * Implementation of AnyToneModelMemoryDefinitionHandler
 * ********************************************************************************************* */
AnyToneModelMemoryDefinitionHandler::AnyToneModelMemoryDefinitionHandler(XmlElementHandler *parent)
  : ModelMemoryDefinitionHandler{parent}, _modelId(), _revision()
{
  // pass...
}


const QByteArray &
AnyToneModelMemoryDefinitionHandler::modelId() const {
  return _modelId;
}

const QByteArray &
AnyToneModelMemoryDefinitionHandler::revision() const {
  return _revision;
}


bool
AnyToneModelMemoryDefinitionHandler::beginModelElement(const QXmlStreamAttributes &attributes) {
  pushHandler(new XmlHexDataHandler(this));
  return true;
}

bool
AnyToneModelMemoryDefinitionHandler::endModelElement() {
  auto handler = qobject_cast<XmlHexDataHandler *>(popHandler());
  if (6 != handler->data().size()) {
    raiseError(QString("Model ID must be exactly 6 bytes long. Got %1.").arg(handler->data().size()));
    delete handler;
    return false;
  }

  _modelId = handler->data();
  delete handler;
  return true;
}


bool
AnyToneModelMemoryDefinitionHandler::beginRevisionElement(const QXmlStreamAttributes &attributes) {
  pushHandler(new XmlHexDataHandler(this));
  return true;
}

bool
AnyToneModelMemoryDefinitionHandler::endRevisionElement() {
  auto handler = qobject_cast<XmlHexDataHandler *>(popHandler());
  if (6 != handler->data().size()) {
    raiseError(QString("Revision ID must be exactly 6 bytes long. Got %1.").arg(handler->data().size()));
    delete handler;
    return false;
  }

  _revision = handler->data();
  delete handler;
  return true;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
AnyToneModelFirmwareDefinitionHandler::AnyToneModelFirmwareDefinitionHandler(
    const QString &name, const QDate &released, ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent}, _definition(new AnyToneModelFirmwareDefinition(nullptr))
{
  _definition->setName(name);
  _definition->setReleased(released);
}

AnyToneModelFirmwareDefinitionHandler::~AnyToneModelFirmwareDefinitionHandler() {
  if (nullptr != _definition)
    delete _definition;
}


ModelFirmwareDefinition *
AnyToneModelFirmwareDefinitionHandler::definition() const {
  return _definition;
}

ModelFirmwareDefinition *
AnyToneModelFirmwareDefinitionHandler::takeDefinition() {
  if (nullptr == _definition)
    return nullptr;
  auto def = _definition;
  _definition = nullptr;
  def->setParent(nullptr);
  return def;
}


bool
AnyToneModelFirmwareDefinitionHandler::beginMemoryElement(const QXmlStreamAttributes &attributes) {
  if (nullptr == definition()) {
    raiseError("Unexpected <memory> element, no firmware definition created yet.");
    return false;
  }

  pushHandler(new AnyToneModelMemoryDefinitionHandler(this));

  return true;
}

bool
AnyToneModelFirmwareDefinitionHandler::endMemoryElement() {
  auto handler = qobject_cast<AnyToneModelMemoryDefinitionHandler*>(topHandler());
  if (! handler->modelId().isEmpty())
    qobject_cast<AnyToneModelFirmwareDefinition*>(definition())->setModelId(handler->modelId());
  if (! handler->revision().isEmpty())
    qobject_cast<AnyToneModelFirmwareDefinition*>(definition())->setRevision(handler->revision());

  return ModelFirmwareDefinitionHandler::endMemoryElement();
}

