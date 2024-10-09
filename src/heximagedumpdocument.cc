#include "heximagedumpdocument.hh"
#include "hexdump.hh"

#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocumentFragment>

HexImageDumpDocument::HexImageDumpDocument(const HexImage &img, bool darkMode, QObject *parent)
  : HexDocument{darkMode, parent}
{
  QTextCursor cursor = this->rootFrame()->firstCursorPosition();
  for (unsigned int ei=0; ei<img.size(); ei++) {
    const HexElement &element = img.element(ei);
    putElement(element, cursor);
  }
}


void
HexImageDumpDocument::putElement(const HexElement &element, QTextCursor &cursor) {
  cursor.insertFrame(_elementFormat);
  putElementTitle(element, cursor);
  for (unsigned int li=0; li < element.size(); li++) {
    const HexLine &line = element.line(li);
    putLine(line, cursor);
  }
}

void
HexImageDumpDocument::putLine(const HexLine &line, QTextCursor &cursor) {
  cursor.insertBlock(_lineFormat);

  putAddress(line.address(), cursor);

  putValues(line.left(), cursor);

  cursor.insertText(QString(" "), _separatorFormat);

  putChars(line.left(), cursor);
}
