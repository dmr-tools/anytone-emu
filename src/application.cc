#include "application.hh"
#include "config.hh"
#include "image.hh"
#include "device.hh"
#include "codeplugpattern.hh"

Application::Application(int &argc, char *argv[])
  : QApplication(argc, argv), _collection(new Collection(this)), _device(nullptr)
{
  setApplicationDisplayName("AnyTone emulator");
  setApplicationName("anytone-emu-gui");
  setApplicationVersion(PROJECT_VERSION);
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


