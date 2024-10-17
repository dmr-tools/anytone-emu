#ifndef MODELPARSER_HH
#define MODELPARSER_HH

#include "xmlparser.hh"
#include <modeldefinition.hh>


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

  const QList<ModelRom::Segment> &mappings() const;

protected slots:
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

protected:
  virtual bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  virtual bool endDescriptionElement();

  virtual bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  virtual bool endMemoryElement();
};


#endif // MODELPARSER_HH
