#include "device.hh"
#include "pattern.hh"
#include "model.hh"

#include <QIODevice>
#include "logger.hh"



/* ********************************************************************************************* *
 * Implementation of Device
 * ********************************************************************************************* */
Device::Device(CodeplugPattern *pattern, ImageCollector *handler, QObject *parent)
  : QObject{parent}, _pattern(pattern), _handler(handler), _rom()
{
  if (_pattern)
    _pattern->setParent(this);
  if (_handler) {
    _handler->setParent(this);
    connect(this, &Device::startProgram, _handler, &ImageCollector::startProgram);
    connect(this, &Device::endProgram, _handler, &ImageCollector::endProgram);
  }
}


bool
Device::read(uint32_t address, uint8_t len, QByteArray &buffer) {
  if (! rom().read(address, len, buffer)) {
    logError() << "Cannot read " << len
               << "b from ROM at address " << QString::number(address, 16) << "h.";
    return false;
  }

  return true;
}

bool
Device::write(uint32_t addr, const QByteArray &data) {
  if (nullptr != _handler)
    return _handler->write(addr, data);
  return false;
}


ImageCollector *
Device::handler() const {
  return _handler;
}

void
Device::setHandler(ImageCollector *handler) {
  if (nullptr != _handler)
    delete _handler;

  _handler = handler;

  if (_handler) {
    _handler->setParent(this);
    connect(this, &Device::startProgram, _handler, &ImageCollector::startProgram);
    connect(this, &Device::endProgram, _handler, &ImageCollector::endProgram);
  }
}

CodeplugPattern *
Device::pattern() const {
  return _pattern;
}


const ModelRom &
Device::rom() const {
  return _rom;
}

ModelRom &
Device::rom() {
  return _rom;
}



