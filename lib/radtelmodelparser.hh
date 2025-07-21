#ifndef RADTELMODELPARSER_HH
#define RADTELMODELPARSER_HH


#include "modelparser.hh"
#include "radtelmodeldefinition.hh"


class RadtelModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

public:
  Q_INVOKABLE explicit RadtelModelDefinitionHandler(const QString &context, const QString& id, ModelDefinitionParser *parent);

public:
  ModelDefinition *definition() const;
  ModelDefinition *takeDefinition();

protected slots:
  virtual bool beginFirmwareElement(const QXmlStreamAttributes &attributes);
  virtual bool endFirmwareElement();

protected:
  RadtelModelDefinition *_definition;
};



class RadtelModelFirmwareDefinitionHandler: public ModelFirmwareDefinitionHandler
{
  Q_OBJECT

public:
  explicit RadtelModelFirmwareDefinitionHandler(
      const QString& context, const QString &name, const QDate &released, const QString &codeplug,
      ModelDefinitionHandler *parent);

  ~RadtelModelFirmwareDefinitionHandler();

  ModelFirmwareDefinition *definition() const;
  ModelFirmwareDefinition *takeDefinition();

protected:
  RadtelModelFirmwareDefinition *_definition;
};


#endif // RADTELMODELPARSER_HH
