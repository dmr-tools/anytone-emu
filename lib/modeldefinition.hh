#ifndef MODELDEFINITION_HH
#define MODELDEFINITION_HH

#include <QUrl>
#include <QDate>
#include "xmlparser.hh"


class ModelFirmwareDefinition;


/** Implementes some fixed memory read from an emulated model. */
class Rom
{
public:
  struct Segment {
    uint32_t address;
    QByteArray content;

    bool operator<(const Segment &other) const;
    bool operator<(uint32_t address) const;
    bool contains(uint32_t address, uint8_t size=0) const;
  };

  typedef QVector<Segment>::iterator iterator;
  typedef QVector<Segment>::const_iterator const_iterator;

public:
  Rom();
  Rom(const Rom &other) = default;

  Rom operator+(const Rom &other) const;
  const Rom &operator+=(const Rom &other);

  void write(uint32_t address, const QByteArray &data);
  bool read(uint32_t address, uint8_t length, QByteArray &data) const;

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();

protected:
  QVector<Segment> _content;
};



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
  const Rom &rom() const;

  virtual void addFirmware(ModelFirmwareDefinition *firmware);

protected:
  QString _name;
  QString _manufacturer;
  QString _description;
  QUrl _url;
  Rom _rom;
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
  Rom rom() const;

protected:
  QString _name;
  QDate _released;
  QString _description;
  Rom _rom;
};



/** Parser for model definitions. */
class ModelDefinitionParser: public XmlParser
{
  Q_OBJECT

public:
  explicit ModelDefinitionParser(QObject *parent=nullptr);

  virtual ModelDefinition *definition() const;
  virtual ModelDefinition *takeDefinition();

protected slots:
  virtual bool beginModelElement(const QXmlStreamAttributes &attributes);
  virtual bool endModelElement();

protected:
  ModelDefinition *_modelDefinition;
};



class ModelDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

protected:
  explicit ModelDefinitionHandler(ModelDefinitionParser *parent);

public:
  virtual ModelDefinition *definition() const = 0;
  virtual ModelDefinition *takeDefinition() = 0;

protected:
  virtual bool beginNameElement(const QXmlStreamAttributes &attributes);
  virtual bool endNameElement();

  virtual bool beginManufacturerElement(const QXmlStreamAttributes &attributes);
  virtual bool endManufacturerElement();

  virtual bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  virtual bool endDescriptionElement();

  virtual bool beginUrlElement(const QXmlStreamAttributes &attributes);
  virtual bool endUrlElement();

  virtual bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  virtual bool endMemoryElement();
};


class ModelMemoryDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

public:
  explicit ModelMemoryDefinitionHandler(XmlElementHandler *parent);

  const QList<Rom::Segment> &mappings() const;

protected slots:
  bool beginMapElement(const QXmlStreamAttributes &attributes);
  bool endMapElement();

protected:
  QList<Rom::Segment> _mappings;
};


class ModelFirmwareDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

protected:
  explicit ModelFirmwareDefinitionHandler(ModelDefinitionHandler *parent);

public:
  virtual ModelFirmwareDefinition *definition() const = 0;
  virtual ModelFirmwareDefinition *takeDefinition() = 0;

protected:
  virtual bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  virtual bool endDescriptionElement();

  virtual bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  virtual bool endMemoryElement();
};


#endif // MODELDEFINITION_HH
