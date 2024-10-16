#include "model.hh"
#include "image.hh"
#include "logger.hh"
#include "pattern.hh"

/* ********************************************************************************************* *
 * Implementation of Model
 * ********************************************************************************************* */
Model::Model(CodeplugPattern *pattern, QObject *parent)
  : QObject{parent}, _pattern(pattern)
{
  if (_pattern)
    _pattern->setParent(this);
}


bool
Model::read(uint32_t address, uint8_t length, QByteArray &payload) {
  return readRom(address, length, payload);
}

bool
Model::write(uint32_t address, const QByteArray &payload) {
  Q_UNUSED(address); Q_UNUSED(payload)
  return false;
}


bool
Model::readRom(uint32_t address, uint8_t length, QByteArray &payload) const {
  // Search element containing segment entirely
  foreach (auto element, _rom) {
    if ((element.first<=address) && (address+length)<=(element.first+element.second.size())) {
      payload = element.second.mid(address-element.first, length);
      return true;
    }
  }
  return false;
}


bool
Model::writeRom(uint32_t address, const QByteArray &payload) {
  // Check if we can append data to an element
  foreach (auto element, _rom) {
    if ((element.first + element.second.size()) == address) {
      element.second.append(payload);
      return true;
    }
  }

  // Create new element
  _rom.append({address, payload});
  std::sort(_rom.begin(), _rom.end(),
            [](const QPair<uint32_t,QByteArray> &a,const QPair<uint32_t,QByteArray> &b) -> bool {
    return a.first < b.first;
  });

  return true;
}


CodeplugPattern *
Model::pattern() const {
  return _pattern;
}

void
Model::setPattern(CodeplugPattern *pattern) {
  if (_pattern) {
    delete _pattern;
    _pattern = nullptr;
  }
  _pattern = pattern;
  if (_pattern)
    _pattern->setParent(this);
}

void
Model::startProgram() {
  // pass...
}

void
Model::endProgram() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ImageCollector
 * ********************************************************************************************* */
ImageCollector::ImageCollector(CodeplugPattern *pattern, QObject *parent)
  : Model(pattern, parent), _images()
{
  // pass...
}

unsigned int
ImageCollector::count() const {
  return _images.count();
}

const Image *
ImageCollector::image(unsigned int idx) const {
  return _images.at(idx);
}

const Image *
ImageCollector::first() const {
  if (0 == count())
    return nullptr;
  return _images.first();
}

const Image *
ImageCollector::last() const {
  if (0 == count())
    return nullptr;
  return _images.last();
}

const Image *
ImageCollector::previous() const {
  if (1 >= count())
    return nullptr;
  return _images.at(count()-2);
}

bool
ImageCollector::write(uint32_t address, const QByteArray &payload) {
  if (0 == count()) {
    logError() << "No image created yet.";
    return false;
  }
  _images.last()->append(address, payload);
  return true;
}

void
ImageCollector::startProgram() {
  if ((0 == _images.count()) || (0 != _images.last()->count())) {
    logInfo() << "Create new image.";
    _images.append(new Image(QString("Codeplug %1").arg(_images.count()), this));
  } else if (0 == _images.last()->count()) {
    logInfo() << "Reuse last image.";
  }
}

void
ImageCollector::endProgram() {
  if ((0 != _images.count()) && (0 != _images.last()->count())) {
    logInfo() << "Image received.";
    emit imageReceived();
  }
}

