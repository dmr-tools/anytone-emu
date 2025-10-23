#ifndef ANYTONEMODELPARSER_H
#define ANYTONEMODELPARSER_H

#include "modelparser.hh"
#include "definition.hh"


class AnyToneModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit AnyToneModelDefinitionHandler(const QString &context, const QString& id, ModelDefinitionParser *parent);

public:
  ModelDefinition *definition() const;
  ModelDefinition *takeDefinition();

protected slots:
  bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  bool endMemoryElement();

  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

  virtual bool beginBandElement(const QXmlStreamAttributes &attributes);
  virtual bool endBandElement();

protected:
  AnyToneModelDefinition *_definition;
};



class AnyToneModelMemoryDefinitionHandler: public ModelMemoryDefinitionHandler
{
  Q_OBJECT

public:
  explicit AnyToneModelMemoryDefinitionHandler(XmlElementHandler *parent);

  const QByteArray &modelId() const;
  const QByteArray &revision() const;

protected slots:
  virtual bool beginIdElement(const QXmlStreamAttributes &attributes);
  virtual bool endIdElement();

  virtual bool beginRevisionElement(const QXmlStreamAttributes &attributes);
  virtual bool endRevisionElement();

protected:
  QByteArray _modelId;
  QByteArray _revision;
};



class AnyToneModelFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  explicit AnyToneModelFirmwareDefinitionHandler(
      const QString& context, const QString &name, const QDate &released, const QString &codeplug,
      ModelDefinitionHandler *parent);

  ~AnyToneModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

  virtual bool beginBandElement(const QXmlStreamAttributes &attributes);
  virtual bool endBandElement();

protected:
  bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  bool endMemoryElement();

protected:
  AnyToneModelFirmwareDefinition *_definition;
};


#endif // ANYTONEMODELPARSER_H
