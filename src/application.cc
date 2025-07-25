#include "application.hh"
#include "config.hh"
#include "image.hh"
#include "device.hh"
#include "pattern.hh"
#include <QIcon>


Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _collection(new Collection(this)), _catalog(), _device(nullptr)
{
  setApplicationDisplayName("AnyTone emulator");
  setOrganizationName("DMRTools");
  setOrganizationDomain("dmr-tools.github.io");
  setApplicationName("anytone-emu-gui");
  setApplicationVersion(PROJECT_VERSION);
}


const QString &
Application::catalog() const {
  return _catalog;
}

void
Application::setCatalog(const QString &filename) {
  _catalog = filename;
}


Collection *
Application::collection() {
  return _collection;
}

void
Application::setDevice(Device *device) {
  if (_device)
    _device->deleteLater();
  _device = device;
  if (_device->pattern()) {
    connect(_device->pattern(), &CodeplugPattern::modified, this, &Application::patternModified);
    connect(_device->pattern(), &CodeplugPattern::added, this, &Application::patternModified);
    connect(_device->pattern(), &CodeplugPattern::removed, this, &Application::patternModified);
  }
}

const Device *
Application::device() const {
  return _device;
}


Application *
Application::instance() {
  return qobject_cast<Application *>(QApplication::instance());
}

