#ifndef MODELPARSER_HH
#define MODELPARSER_HH

#include "xmlparser.hh"
#include "deviceclassplugininterface.hh"
#include <modeldefinition.hh>


/** Parser for model definitions. */
class ModelDefinitionParser: public XmlParser
{
  Q_OBJECT

public:
  explicit ModelDefinitionParser(ModelCatalog *catalog, const QString &context, QObject *parent=nullptr);

public slots:
  virtual bool beginCatalogElement(const QXmlStreamAttributes &attributes);
  virtual bool endCatalogElement();

  virtual bool beginModelElement(const QXmlStreamAttributes &attributes);
  virtual bool endModelElement();

protected:
  QString _context;
  ModelCatalog *_catalog;

protected:
  static QHash<QString, DeviceClassPluginInterface *> _modelHandler;
};



class ModelDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

protected:
  explicit ModelDefinitionHandler(const QString &context, const QString& id, ModelDefinitionParser *parent);

public:
  virtual ModelDefinition *definition() const = 0;
  virtual ModelDefinition *takeDefinition() = 0;

  const QString &id() const;

public slots:
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

protected:
  QString _context;
  QString _id;
};



class GenericModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  GenericModelDefinitionHandler(DeviceClassPluginInterface *plugin, const QString &context,
                                const QString& id, ModelDefinitionParser *parent);

  ModelDefinition *definition() const override;
  ModelDefinition *takeDefinition() override;

protected slots:
  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

protected:
  DeviceClassPluginInterface *_plugin;
  ModelDefinition *_definition;
};



class ModelMemoryDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

public:
  explicit ModelMemoryDefinitionHandler(XmlElementHandler *parent);

  const QList<ModelRom::Segment> &mappings() const;

public slots:
  bool beginMapElement(const QXmlStreamAttributes &attributes);
  bool endMapElement();

protected:
  QList<ModelRom::Segment> _mappings;
};


class ModelFirmwareDefinitionHandler: public XmlElementHandler
{
  Q_OBJECT

protected:
  explicit ModelFirmwareDefinitionHandler(ModelDefinitionHandler *parent);

public:
  virtual ModelFirmwareDefinition *definition() const = 0;
  virtual ModelFirmwareDefinition *takeDefinition() = 0;

public slots:
  virtual bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  virtual bool endDescriptionElement();

  virtual bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  virtual bool endMemoryElement();
};


class GenericModelFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  GenericModelFirmwareDefinitionHandler(
      DeviceClassPluginInterface *plugin, const QString& context, const QString &name,
      const QDate &released, const QString &codeplug, ModelDefinitionHandler *parent);

  ~GenericModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const override;
  ModelFirmwareDefinition *takeDefinition() override;

protected:
  ModelFirmwareDefinition *_definition;
};

#endif // MODELPARSER_HH
