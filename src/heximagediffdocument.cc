#include "heximagediffdocument.hh"

#include "hexdump.hh"

#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocumentFragment>


HexImageDiffDocument::HexImageDiffDocument(bool darkMode, const HexImage &img, QObject *parent)
  : HexDocument{darkMode, parent}
{
  setUndoRedoEnabled(false);

  QTextCursor cursor = this->rootFrame()->firstCursorPosition();
  for (unsigned int ei=0; ei<img.size(); ei++) {
    const HexElement &element = img.element(ei);
    if (! element.hasDiff())
      continue;
    putElement(element, cursor);
  }
}

void
HexImageDiffDocument::putElement(const HexElement &element, QTextCursor &cursor) {
  cursor.insertFrame(_elementFormat);
  putElementTitle(element, cursor);
  putOffsets(cursor);
  for (unsigned int li=0; li < element.size(); li++) {
    const HexLine &line = element.line(li);
    if (! line.hasDiff())
      continue;
    putLine(line, cursor);
  }
}

void
HexImageDiffDocument::putLine(const HexLine &line, QTextCursor &cursor) {
  cursor.insertBlock(_lineFormat);

  putAddress(line.address(), cursor);

  putValues(line.left(), cursor);

  cursor.insertText(QString(" "), _separatorFormat);

  putValues(line.right(), cursor);

  cursor.insertText(QString(" "), _separatorFormat);

  putChars(line.left(), cursor);

  cursor.insertText(QString(" "), _separatorFormat);

  putChars(line.right(), cursor);
}

void
HexImageDiffDocument::putOffsets(QTextCursor &cursor) {
  cursor.insertBlock(_lineFormat);
  cursor.insertText(QString("         "), _addressFormat);
  putValueOffsets(cursor);
  cursor.insertText(QString(" "), _separatorFormat);
  putValueOffsets(cursor);
}
