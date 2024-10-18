#ifndef ANYTONEMODELPARSER_H
#define ANYTONEMODELPARSER_H

#include "modelparser.hh"
#include "anytonemodeldefinition.hh"


class AnyToneModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit AnyToneModelDefinitionHandler(ModelDefinitionParser *parent);

public:
  ModelDefinition *definition() const;
  ModelDefinition *takeDefinition();

protected:
  bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  bool endMemoryElement();

  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

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
  virtual bool beginModelElement(const QXmlStreamAttributes &attributes);
  virtual bool endModelElement();

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
      const QString &name, const QDate &released, ModelDefinitionHandler *parent);

  ~AnyToneModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

protected:
  bool beginMemoryElement(const QXmlStreamAttributes &attributes);
  bool endMemoryElement();

protected:
  AnyToneModelFirmwareDefinition *_definition;
};


#endif // ANYTONEMODELPARSER_H
