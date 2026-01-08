#include "modeldefinition.hh"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QXmlStreamReader>

#include "modelparser.hh"
#include "logger.hh"
#include "pattern.hh"
#include "device.hh"



/* ********************************************************************************************** *
 * Implementation of ModelCatalog
 * ********************************************************************************************** */
ModelCatalog::ModelCatalog(QObject *parent)
  : QObject{parent}, _models()
{
  // pass...
}

unsigned int
ModelCatalog::count() const {
  return _models.count();
}

bool
ModelCatalog::hasModel(const QString &id) const {
  return _ids.contains(id);
}

ModelDefinition *
ModelCatalog::model(const QString &id) const {
  return _ids.value(id);
}

ModelDefinition *
ModelCatalog::model(unsigned int idx) const {
  return _models.at(idx);
}

void
ModelCatalog::addModel(ModelDefinition *definition) {
  if (nullptr == definition)
    return;
  definition->setParent(this);
  connect(definition, &QObject::destroyed, this, &ModelCatalog::onModelDefinitionDeleted);
  _models.append(definition);
  _ids.insert(definition->id(), definition);
}

ModelCatalog::const_iterator
ModelCatalog::begin() const {
  return _models.begin();
}

ModelCatalog::const_iterator
ModelCatalog::end() const {
  return _models.end();
}


void
ModelCatalog::clear() {
  _ids.clear();
  _models.clear();
}

bool
ModelCatalog::load(const QString &catalogFile) {
  QFile file(catalogFile);
  QFileInfo fileInfo(catalogFile);
  if (! file.open(QIODevice::ReadOnly)) {
    logError() << "Cannot open catalog at '" << catalogFile << "': " << file.errorString() << ".";
    return false;
  }

  QXmlStreamReader reader(&file);
  ModelDefinitionParser parser(this);
  if (! parser.parse(reader, XmlParser::Context(fileInfo))) {
    logError() << "Parse catalog '" << catalogFile << "': " << parser.errorMessage() << ".";
    return false;
  }

  return true;
}


void
ModelCatalog::onModelDefinitionDeleted(QObject *object) {
  if (nullptr == object)
    return;
  if (ModelDefinition *model = qobject_cast<ModelDefinition *>(object)) {
    _models.removeAll(model);
    _ids.remove(model->id());
    connect(model, &QObject::destroyed, this, &ModelCatalog::onModelDefinitionDeleted);
  }
}



/* ********************************************************************************************** *
 * Implementation of ModelDefinition
 * ********************************************************************************************** */
ModelDefinition::ModelDefinition(const QString& id, QObject *parent)
  : QObject{parent}, _id(id), _name(), _manufacturer(), _description(), _url(), _rom()
{
  // pass...
}


const QString &
ModelDefinition::id() const {
  return _id;
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

const ModelRom &
ModelDefinition::rom() const {
  return _rom;
}

ModelFirmwareDefinition *
ModelDefinition::latestFirmware() const {
  if (_firmwares.isEmpty())
    return nullptr;
  return _firmwares.back();
}

const QList<ModelFirmwareDefinition *> &
ModelDefinition::firmwares() const {
  return _firmwares;
}

void
ModelDefinition::addFirmware(ModelFirmwareDefinition *firmware) {
  firmware->setParent(this);
  _firmwares.append(firmware);
}

bool
ModelDefinition::hasFirmware(const QString &version) const {
  for (auto firmware: _firmwares) {
    if (firmware->name() == version)
      return true;
  }
  return false;
}

ModelFirmwareDefinition *
ModelDefinition::firmware(const QString &version) const {
  for (auto firmware: _firmwares) {
    if (firmware->name() == version)
      return firmware;
  }
  return nullptr;
}


ModelDefinition::const_iterator
ModelDefinition::begin() const {
  return _firmwares.begin();
}

ModelDefinition::const_iterator
ModelDefinition::end() const {
  return _firmwares.end();
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


const QString &
ModelFirmwareDefinition::codeplug() const {
  return _codeplug;
}

void
ModelFirmwareDefinition::setCodeplug(const QString &codeplug) {
  _codeplug = codeplug;
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

ModelRom
ModelFirmwareDefinition::rom() const {
  if (nullptr == parent())
    return _rom;
  return qobject_cast<ModelDefinition*>(parent())->rom() + _rom;
}



/* ********************************************************************************************** *
 * Implementation of GenericModelFirmwareDefinition
 * ********************************************************************************************** */
GenericModelFirmwareDefinition::GenericModelFirmwareDefinition(
    DeviceClassPluginInterface *plugin, ModelDefinition *parent)
  : ModelFirmwareDefinition{parent}, _plugin(plugin)
{
  // pass...
}


Device *
GenericModelFirmwareDefinition::createDevice(QIODevice *interface, const ErrorStack &err) const {
  CodeplugPattern *codeplug = CodeplugPattern::load(this->codeplug(), err);
  if (nullptr == codeplug) {
    errMsg(err) << "Cannot parse codeplug file '" << this->codeplug() << "'.";
    return nullptr;
  }

  Device *dev = _plugin->device(interface, this, nullptr);
  dev->rom() += qobject_cast<ModelDefinition *>(parent())->rom();
  dev->rom() += this->rom();

  return dev;
}

