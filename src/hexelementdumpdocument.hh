#ifndef HEXELEMENTDUMPDOCUMENT_HH
#define HEXELEMENTDUMPDOCUMENT_HH

#include "heximagedocument.hh"

class HexElementDumpDocument: public HexDocument
{
  Q_OBJECT

public:
  HexElementDumpDocument(const HexElement &element, bool darkMode=false, QObject *parent = nullptr);

protected:
  void putElement(const HexElement &element, QTextCursor &cursor);
  void putLine(const HexLine &line, QTextCursor &cursor);
};

#endif // HEXIMAGEDUMPDOCUMENT_HH
