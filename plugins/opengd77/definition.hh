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


#endif // OPENGD77MODELDEFINITION_HH
