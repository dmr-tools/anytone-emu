#ifndef OPENGD77MODELDEFINITION_HH
#define OPENGD77MODELDEFINITION_HH

#include "modeldefinition.hh"
#include "errorstack.hh"

class Device;


class OpenGD77ModelDefinition: public ModelDefinition
{
  Q_OBJECT

public:
  OpenGD77ModelDefinition(const QString &id, QObject *parent = nullptr);
};


class OpenGD77ModelFirmwareDefinition : public ModelFirmwareDefinition
{
  Q_OBJECT

public:
  explicit OpenGD77ModelFirmwareDefinition(const QString &context, OpenGD77ModelDefinition *parent);

  Device *createDevice(QIODevice *interface, const ErrorStack &err=ErrorStack()) const;
};

#endif // OPENGD77MODELDEFINITION_HH
