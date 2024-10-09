#ifndef HEXIMAGEDIFFDOCUMENT_HH
#define HEXIMAGEDIFFDOCUMENT_HH

#include <QTextDocument>
#include <QTextFrameFormat>
#include <QTextBlockFormat>

#include "heximagedocument.hh"
#include "hexdump.hh"

class HexImageDiffDocument : public HexDocument
{
public:
  explicit HexImageDiffDocument(const HexImage &img, bool darkMode=false, QObject *parent = nullptr);

protected:
  void putElement(const HexElement &element, QTextCursor &cursor);
  void putLine(const HexLine &line, QTextCursor &cursor);
};

#endif // HEXIMAGEDIFFDOCUMENT_HH
