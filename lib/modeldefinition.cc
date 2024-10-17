#include "modeldefinition.hh"


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

const ModelRom &
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

ModelRom
ModelFirmwareDefinition::rom() const {
  return qobject_cast<ModelDefinition*>(parent())->rom() + _rom;
}



