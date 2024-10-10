#ifndef HEXIMAGEDUMPDOCUMENT_HH
#define HEXIMAGEDUMPDOCUMENT_HH

#include "heximagedocument.hh"

class HexImageDumpDocument: public HexDocument
{
  Q_OBJECT

public:
  HexImageDumpDocument(bool darkMode, const HexImage &img, QObject *parent = nullptr);

protected:
  void putElement(const HexElement &element, QTextCursor &cursor);
  void putLine(const HexLine &line, QTextCursor &cursor);
};

#endif // HEXIMAGEDUMPDOCUMENT_HH
