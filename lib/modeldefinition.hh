#ifndef MODELDEFINITION_HH
#define MODELDEFINITION_HH

#include <QUrl>
#include <QDate>
#include "modelrom.hh"
#include "errorstack.hh"

class Device;
class QIODevice;
class ModelDefinition;
class ModelFirmwareDefinition;



class ModelCatalog: public QObject
{
  Q_OBJECT

public:
  typedef QList<ModelDefinition *>::const_iterator const_iterator;

public:
  explicit ModelCatalog(QObject *parent=nullptr);

  unsigned int count() const;
  bool hasModel(const QString &id) const;
  ModelDefinition *model(const QString &id) const;
  ModelDefinition *model(unsigned int i) const;
  void addModel(ModelDefinition *definition);

  const_iterator begin() const;
  const_iterator end() const;

  void clear();
  bool load(const QString &catalogFile);

protected slots:
  void onModelDefinitionDeleted(QObject *deleted);

protected:
  QHash<QString, ModelDefinition *> _ids;
  QList<ModelDefinition *> _models;
};



/** Base class of all model definitions. */
class ModelDefinition : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString id READ id)
  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString manufacturer READ manufacturer WRITE setManufacturer)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QUrl url READ url WRITE setUrl)

public:
  typedef QList<ModelFirmwareDefinition *>::const_iterator const_iterator;

protected:
  /** Hidden constructor. */
  explicit ModelDefinition(const QString &id, QObject *parent = nullptr);

public:
  const QString &id() const;

  const QString &name() const;
  void setName(const QString &name);

  const QString &manufacturer() const;
  void setManufacturer(const QString &manufacturer);

  const QString &description() const;
  void setDescription(const QString &description);

  const QUrl &url() const;
  void setUrl(const QUrl &url);

  void storeRom(uint32_t address, const QByteArray &data);
  const ModelRom &rom() const;

  virtual ModelFirmwareDefinition * latestFirmware() const;
  virtual const QList<ModelFirmwareDefinition *> &firmwares() const;
  virtual void addFirmware(ModelFirmwareDefinition *firmware);
  const_iterator begin() const;
  const_iterator end() const;

protected:
  QString _id;
  QString _name;
  QString _manufacturer;
  QString _description;
  QUrl _url;
  ModelRom _rom;
  QList<ModelFirmwareDefinition *> _firmwares;
};



/** Base class of firmware releases for a specific model. */
class ModelFirmwareDefinition: public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QDate released READ released WRITE setReleased)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QString codeplug READ codeplug WRITE setCodeplug)

protected:
  /** Hidden constrcutor. */
  ModelFirmwareDefinition(const QString &context, ModelDefinition *parent=nullptr);

public:
  const QString &name() const;
  void setName(const QString &name);

  const QDate& released() const;
  void setReleased(const QDate &released);

  const QString &description() const;
  void setDescription(const QString &description);

  const QString &codeplug() const;
  void setCodeplug(const QString &path);

  virtual Device *createDevice(QIODevice *interface, const ErrorStack &err=ErrorStack()) const = 0;

  void storeRom(uint32_t address, const QByteArray &data);
  ModelRom rom() const;

protected:
  QString _context;
  QString _name;
  QDate _released;
  QString _description;
  QString _codeplug;
  ModelRom _rom;
};




#endif // MODELDEFINITION_HH
