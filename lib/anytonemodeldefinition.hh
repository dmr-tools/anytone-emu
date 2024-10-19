#ifndef ANYTONEMODELDEFINITION_HH
#define ANYTONEMODELDEFINITION_HH

#include "modeldefinition.hh"
#include "errorstack.hh"

class Device;


class AnyToneModelDefinition : public ModelDefinition
{
  Q_OBJECT

  Q_PROPERTY(QByteArray modelId READ modelId WRITE setModelId)
  Q_PROPERTY(QByteArray revision READ revision WRITE setRevision)

public:
  explicit AnyToneModelDefinition(const QString &id, QObject *parent = nullptr);

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
  explicit AnyToneModelFirmwareDefinition(const QString &context, AnyToneModelDefinition *parent);

  const QByteArray &modelId() const;
  void setModelId(const QByteArray &id);

  const QByteArray &revision() const;
  void setRevision(const QByteArray &rev);

  Device *createDevice(QIODevice *interface, const ErrorStack &err=ErrorStack()) const;

protected:
  QByteArray _modelId;
  QByteArray _revision;
};





#endif // ANYTONEMODELDEFINITION_HH
