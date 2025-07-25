#include "definition.hh"

#include <QXmlStreamReader>
#include "pattern.hh"
#include "device.hh"


/* ********************************************************************************************* *
 * Implementation of OpenGD77ModelDefinition (nothing to do)
 * ********************************************************************************************* */
OpenGD77ModelDefinition::OpenGD77ModelDefinition(const QString &id, QObject *parent)
  : ModelDefinition(id, parent)
{
  // pass...
}
