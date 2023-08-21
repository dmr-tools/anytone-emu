#include "hexelementdumpdocument.hh"
#include "hexdump.hh"

#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocumentFragment>

HexElementDumpDocument::HexElementDumpDocument(const HexElement &element, QObject *parent)
  : HexDocument{parent}
{
  _keepValueFormat.setForeground(Qt::black);

  QTextCursor cursor = this->rootFrame()->firstCursorPosition();
  putElement(element, cursor);
}


void
HexElementDumpDocument::putElement(const HexElement &element, QTextCursor &cursor) {
  cursor.insertFrame(_elementFormat);
  putElementTitle(element, cursor);
  for (unsigned int li=0; li < element.size(); li++) {
    const HexLine &line = element.line(li);
    putLine(line, cursor);
  }
}

void
HexElementDumpDocument::putLine(const HexLine &line, QTextCursor &cursor) {
  cursor.insertBlock(_lineFormat);

  putAddress(line.address(), cursor);

  putValues(line.left(), cursor);

  cursor.insertText(QString(" "), _separatorFormat);

  putChars(line.left(), cursor);
}
