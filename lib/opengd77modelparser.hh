#ifndef OPENGD77MODELPARSER_HH
#define OPENGD77MODELPARSER_HH


#include "modelparser.hh"
#include "opengd77modeldefinition.hh"


class OpenGD77ModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit OpenGD77ModelDefinitionHandler(const QString &context, const QString& id, ModelDefinitionParser *parent);

public:
  ModelDefinition *definition() const;
  ModelDefinition *takeDefinition();

protected slots:
  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

protected:
  OpenGD77ModelDefinition *_definition;
};



class OpenGD77ModelFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  explicit OpenGD77ModelFirmwareDefinitionHandler(
      const QString& context, const QString &name, const QDate &released, const QString &codeplug,
      ModelDefinitionHandler *parent);

  ~OpenGD77ModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

protected:
  OpenGD77ModelFirmwareDefinition *_definition;
};


#endif // OPENGD77MODELPARSER_HH
