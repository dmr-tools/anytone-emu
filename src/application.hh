#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>

class Device;
class Collection;

class Application: public QApplication
{
  Q_OBJECT

public:
  Application(int &argc, char *argv[]);

  Collection *collection();

  void setDevice(Device *device);
  const Device *device() const;

protected:
  Device *_device;
  Collection *_collection;
};

#endif // APPLICATION_HH
