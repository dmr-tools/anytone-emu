#include "model.hh"
#include "image.hh"
#include "logger.hh"
#include "pattern.hh"


/* ********************************************************************************************* *
 * Implementation of ImageCollector
 * ********************************************************************************************* */
ImageCollector::ImageCollector(QObject *parent)
  : QObject(parent), _images()
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
ImageCollector::read(uint32_t address, uint8_t length, QByteArray &payload) {
  Q_UNUSED(address); Q_UNUSED(length); Q_UNUSED(payload)
  return false;
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

