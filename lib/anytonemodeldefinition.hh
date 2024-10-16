#ifndef ANYTONEMODELDEFINITION_HH
#define ANYTONEMODELDEFINITION_HH

#include "modeldefinition.hh"



class AnyToneModelDefinition : public ModelDefinition
{
  Q_OBJECT

  Q_PROPERTY(QByteArray modelId READ modelId WRITE setModelId)
  Q_PROPERTY(QByteArray revision READ revision WRITE setRevision)

public:
  explicit AnyToneModelDefinition(QObject *parent = nullptr);

  const QByteArray &modelId() const;
  void setModelId(const QByteArray &id);

  const QByteArray &revision() const;
  void setRevision(const QByteArray &rev);

protected:
  QByteArray _modelId;
  QByteArray _revision;
};



class AnyToneModelFirmwareDefinition : public ModelFirmwareDefinition
{
  Q_OBJECT

  Q_PROPERTY(QByteArray modelId READ modelId WRITE setModelId)
  Q_PROPERTY(QByteArray revision READ revision WRITE setRevision)

public:
  explicit AnyToneModelFirmwareDefinition(AnyToneModelDefinition *parent);

  const QByteArray &modelId() const;
  void setModelId(const QByteArray &id);

  const QByteArray &revision() const;
  void setRevision(const QByteArray &rev);

protected:
  QByteArray _modelId;
  QByteArray _revision;
};



class AnyToneModelDefinitionHandler: public ModelDefinitionHandler
{
  Q_OBJECT

protected:
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



#endif // ANYTONEMODELDEFINITION_HH
