#ifndef MD32UVMODELPARSER_HH
#define MD32UVMODELPARSER_HH


#include "modelparser.hh"
#include "md32uvmodeldefinition.hh"


class MD32UVModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit MD32UVModelDefinitionHandler(const QString &context, const QString& id, ModelDefinitionParser *parent);

public:
  ModelDefinition *definition() const;
  ModelDefinition *takeDefinition();

protected slots:
  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

protected:
  MD32UVModelDefinition *_definition;
};



class MD32UVModelFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  explicit MD32UVModelFirmwareDefinitionHandler(
      const QString& context, const QString &name, const QDate &released, const QString &codeplug,
      ModelDefinitionHandler *parent);

  ~MD32UVModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

protected:
  MD32UVModelFirmwareDefinition *_definition;
};

#endif // MD32UVMODELPARSER_HH
