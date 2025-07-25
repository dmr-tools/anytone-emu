#ifndef MD32UVMODELDEFINITION_HH
#define MD32UVMODELDEFINITION_HH

#include "modeldefinition.hh"


#include "modeldefinition.hh"
#include "errorstack.hh"

class Device;


class MD32UVModelDefinition: public ModelDefinition
{
  Q_OBJECT

public:
  MD32UVModelDefinition(const QString &id, QObject *parent = nullptr);
};


class MD32UVModelFirmwareDefinition : public ModelFirmwareDefinition
{
  Q_OBJECT

public:
  explicit MD32UVModelFirmwareDefinition(const QString &context, MD32UVModelDefinition *parent);

  Device *createDevice(QIODevice *interface, const ErrorStack &err=ErrorStack()) const;
};


#endif // MD32UVMODELDEFINITION_HH
