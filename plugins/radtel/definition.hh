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



#endif // RADTELMODELDEFINITION_HH
