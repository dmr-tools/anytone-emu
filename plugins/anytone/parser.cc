#include "parser.hh"
#include <QXmlStreamAttributes>
#include <QFileInfo>



/* ********************************************************************************************* *
 * Implementation of AnyToneModelDefinitionHandler
 * ********************************************************************************************* */
AnyToneModelDefinitionHandler::AnyToneModelDefinitionHandler(const QString &id, ModelDefinitionParser *parent)
  : ModelDefinitionHandler{id, parent}, _definition(new AnyToneModelDefinition(id, this))
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
  QFileInfo codeplugFileInfo(parser()->context().directory().absoluteFilePath(codeplug));
  if (!codeplugFileInfo.isFile() || !codeplugFileInfo.isReadable()) {
    raiseError(QString("Cannot read codeplug file '%1'.").arg(codeplugFileInfo.filePath()));
    return false;
  }

  QDate released;
  if (attributes.hasAttribute("released")) {
    released = QDate::fromString(attributes.value("released"));
  }

  pushHandler(new AnyToneModelFirmwareDefinitionHandler(name, codeplug, released, this));

  return true;
}

bool
AnyToneModelDefinitionHandler::endFirmwareElement() {
  auto handler = qobject_cast<AnyToneModelFirmwareDefinitionHandler*>(popHandler());
  definition()->addFirmware(handler->takeDefinition());
  delete handler;
  return true;
}


bool
AnyToneModelDefinitionHandler::beginBandElement(const QXmlStreamAttributes &attributes) {
  _textBuffer.clear();
  return true;
}

bool
AnyToneModelDefinitionHandler::endBandElement() {
  bool ok; uint8_t band = _textBuffer.toUInt(&ok);
  if (! ok) {
    raiseError(QString("Cannot convert '%1' to band enum value.").arg(_textBuffer));
    return false;
  }
  _textBuffer.clear();

  qobject_cast<AnyToneModelDefinition*>(definition())->setBand(band);

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
AnyToneModelMemoryDefinitionHandler::beginIdElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
AnyToneModelMemoryDefinitionHandler::endIdElement() {
  _modelId = QByteArray::fromHex(textBuffer().toLocal8Bit());
  if (6 != _modelId.size()) {
    raiseError(QString("Model ID must be exactly 6 bytes long. Got %1.").arg(_modelId.size()));
    return false;
  }
  return true;
}


bool
AnyToneModelMemoryDefinitionHandler::beginRevisionElement(const QXmlStreamAttributes &attributes) {
  clearTextBuffer();
  return true;
}

bool
AnyToneModelMemoryDefinitionHandler::endRevisionElement() {
  _revision = QByteArray::fromHex(textBuffer().toLocal8Bit());
  if (6 != _revision.size()) {
    raiseError(QString("Revision ID must be exactly 6 bytes long. Got %1.").arg(_revision.size()));
    return false;
  }
  return true;
}



/* ********************************************************************************************* *
 * Implementation of AnyToneModelFirmwareDefinitionHandler
 * ********************************************************************************************* */
AnyToneModelFirmwareDefinitionHandler::AnyToneModelFirmwareDefinitionHandler(
    const QString &name, const QString &codeplug, const QDate &released, ModelDefinitionHandler *parent)
  : ModelFirmwareDefinitionHandler{parent},
    _definition(new AnyToneModelFirmwareDefinition{nullptr})
{
  _definition->setName(name);
  _definition->setReleased(released);
  _definition->setCodeplug(parser()->context().directory().absoluteFilePath(codeplug));
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


bool
AnyToneModelFirmwareDefinitionHandler::beginBandElement(const QXmlStreamAttributes &attributes) {
  _textBuffer.clear();
  return true;
}

bool
AnyToneModelFirmwareDefinitionHandler::endBandElement() {
  bool ok; uint8_t band = _textBuffer.toUInt(&ok);
  if (! ok) {
    raiseError(QString("Cannot convert '%1' to band enum value.").arg(_textBuffer));
    return false;
  }
  _textBuffer.clear();

  qobject_cast<AnyToneModelFirmwareDefinition*>(definition())->setBand(band);

  return true;
}

