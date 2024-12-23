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

  void setCatalog(const QString &filename);
  const QString &catalog() const;

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

  static Application *instance();

signals:
  void patternModified();

protected:
  QString _catalog;
  Device *_device;
  Collection *_collection;
};

#endif // APPLICATION_HH
