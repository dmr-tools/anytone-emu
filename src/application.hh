#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <QApplication>
#include <QWidget>

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

  template <class T>
  T *findObject(const QString &name) const {
    foreach (QWidget *window, topLevelWidgets()) {
      if (T *obj = window->findChild<T *>(name))
        return obj;
    }
    return nullptr;
  }

signals:
  void patternModified();

protected:
  Device *_device;
  Collection *_collection;
};

#endif // APPLICATION_HH
