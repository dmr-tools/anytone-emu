#ifndef MODELDEFINITION_HH
#define MODELDEFINITION_HH

#include <QUrl>
#include <QDate>
#include "modelrom.hh"

class ModelFirmwareDefinition;


/** Base class of all model definitions. */
class ModelDefinition : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)
  Q_PROPERTY(QString manufacturer READ manufacturer WRITE setManufacturer)
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QUrl url READ url WRITE setUrl)

protected:
  /** Hidden constructor. */
  explicit ModelDefinition(QObject *parent = nullptr);

public:
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

  virtual void addFirmware(ModelFirmwareDefinition *firmware);

protected:
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

protected:
  /** Hidden constrcutor. */
  ModelFirmwareDefinition(ModelDefinition *parent=nullptr);

public:
  const QString &name() const;
  void setName(const QString &name);

  const QDate& released() const;
  void setReleased(const QDate &released);

  const QString &description() const;
  void setDescription(const QString &description);

  void storeRom(uint32_t address, const QByteArray &data);
  ModelRom rom() const;

protected:
  QString _name;
  QDate _released;
  QString _description;
  ModelRom _rom;
};




#endif // MODELDEFINITION_HH
