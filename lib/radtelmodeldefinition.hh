#ifndef RADTELMODELDEFINITION_HH
#define RADTELMODELDEFINITION_HH

#include "modeldefinition.hh"


#include "modeldefinition.hh"
#include "errorstack.hh"

class Device;


class RadtelModelDefinition: public ModelDefinition
{
  Q_OBJECT

public:
  RadtelModelDefinition(const QString &id, QObject *parent = nullptr);
};


class RadtelModelFirmwareDefinition : public ModelFirmwareDefinition
{
  Q_OBJECT

public:
  explicit RadtelModelFirmwareDefinition(const QString &context, RadtelModelDefinition *parent);

  Device *createDevice(QIODevice *interface, const ErrorStack &err=ErrorStack()) const;
};


#endif // RADTELMODELDEFINITION_HH
