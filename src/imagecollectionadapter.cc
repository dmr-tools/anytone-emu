#include "imagecollectionadapter.hh"
#include "image.hh"
#include "logger.hh"


ImageCollectionAdapter::ImageCollectionAdapter(Collection *collection, QObject *parent)
  : Model{parent}, _image(nullptr), _collection(collection)
{
  // pass...
}

bool
ImageCollectionAdapter::write(uint32_t address, const QByteArray &payload) {
  if (nullptr==_image) {
    logError() << "No image created yet.";
    return false;
  }
  _image->append(address, payload);
  return true;
}

void
ImageCollectionAdapter::startProgram() {
  if ((nullptr == _image) || (0 != _image->count())) {
    logInfo() << "Create new image.";
    _image = new Image(QString("Codeplug %1").arg(_collection->count()), this);
  } else {
    logInfo() << "Reuse last image.";
  }
}

void
ImageCollectionAdapter::endProgram() {
  if (nullptr != _image) {
    logInfo() << "Image received.";
    _collection->append(_image);
    _image = nullptr;
  }
}


