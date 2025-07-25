#include "definition.hh"


#include <QXmlStreamReader>
#include "pattern.hh"
#include "device.hh"


/* ********************************************************************************************* *
 * Implementation of RadtelModelDefinition (nothing to do)
 * ********************************************************************************************* */
RadtelModelDefinition::RadtelModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition(id, parent)
{
  storeRom(0x00000000, QByteArray(1024, '\0'));
  storeRom(0x00002000, QByteArray(1024, '\0'));
}


