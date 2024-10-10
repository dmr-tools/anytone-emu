#include "heximagedocument.hh"
#include "hexdump.hh"
#include "application.hh"
#include "device.hh"

#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocumentFragment>

HexDocument::HexDocument(bool darkMode, QObject *parent)
  : QTextDocument{parent}, _elementFormat(), _elementTitleFormat(), _lineFormat(), _baseFormat(),
    _addressFormat(), _valueFormat(), _keepValueFormat(), _addValueFormat(), _remValueFormat(),
    _unusedValueFormat(), _charsFormat(), _separatorFormat()
{
  clear();
  setDocumentMargin(4);

  _elementFormat.setPadding(0);
  _elementFormat.setTopMargin(0);
  _elementFormat.setBottomMargin(0);

  _elementTitleFormat.setTopMargin(10);
  _elementTitleFormat.setBottomMargin(3);
  _elementTitleFormat.setHeadingLevel(3);

  _lineFormat.setTopMargin(0);
  _lineFormat.setBottomMargin(0);
  _lineFormat.setNonBreakableLines(true);

  _baseFormat.setFontFamilies({"monospace"});
  _baseFormat.setFontFixedPitch(true);
  _baseFormat.setFontPointSize(10);

  _addressFormat = _baseFormat;
  _addressFormat.setFontWordSpacing(10);

  _valueFormat = _baseFormat;
  _valueFormat.setFontFixedPitch(true);
  _valueFormat.setFontWordSpacing(5);

  _keepValueFormat = _valueFormat;

  _addValueFormat = _valueFormat;

  _remValueFormat = _valueFormat;

  _unusedValueFormat = _valueFormat;

  _charsFormat = _baseFormat;

  _separatorFormat = _baseFormat;
  _separatorFormat.setFontWordSpacing(10);

  enableDarkMode(darkMode);
}

void
HexDocument::enableDarkMode(bool enable) {
  if (enable) {
    _addressFormat.setForeground(QColor(Qt::cyan));
    _keepValueFormat.setForeground(QColor(Qt::lightGray));
    _addValueFormat.setForeground(QColor(Qt::green));
    _remValueFormat.setForeground(QColor(Qt::red));
    _unusedValueFormat.setForeground(QColor(Qt::gray));
  } else {
    _addressFormat.setForeground(QColor(Qt::darkBlue));
    _keepValueFormat.setForeground(QColor(Qt::gray));
    _addValueFormat.setForeground(QColor(Qt::darkGreen));
    _remValueFormat.setForeground(QColor(Qt::darkRed));
    _unusedValueFormat.setForeground(QColor(Qt::lightGray));
  }
}

void
HexDocument::putElementTitle(const HexElement &element, QTextCursor &cursor) {
  cursor.insertBlock(_elementTitleFormat);

  QTextCharFormat titleFormat = _elementTitleFormat.toCharFormat();
  titleFormat.setFontPointSize(titleFormat.fontPointSize()*1.6);

  cursor.insertText(QString("Unknown Element at %1h").arg(element.address(), 0, 16), titleFormat);
}

void
HexDocument::putAddress(uint32_t address, QTextCursor &cursor) {
  cursor.insertText(QString("%1 ").arg(address, 8, 16, QChar('0')), _addressFormat);
}

void
HexDocument::putValues(const QVector<HexLine::Byte> &values, QTextCursor &cursor) {
  for (unsigned int i=0; i<values.size(); i++) {
    switch(values[i].type) {
    case HexLine::Byte::Keep:
      cursor.insertText(QString("%1 ").arg(values[i].value, 2, 16, QChar('0')), _keepValueFormat);
      break;
    case HexLine::Byte::Add:
      cursor.insertText(QString("%1 ").arg(values[i].value, 2, 16, QChar('0')), _addValueFormat);
      break;
    case HexLine::Byte::Remove:
      cursor.insertText(QString("%1 ").arg(values[i].value, 2, 16, QChar('0')), _remValueFormat);
      break;
    case HexLine::Byte::Unused:
      cursor.insertText(QString(".. "), _unusedValueFormat);
      break;
    }
  }
}

void
HexDocument::putChars(const QVector<HexLine::Byte> &values, QTextCursor &cursor)
{
  for (unsigned int i=0; i<values.size(); i++) {
    if (values[i].value>=0x20 && values[i].value<0x7f)
      cursor.insertText(QString("%1").arg((char)values[i].value), _charsFormat);
    else
      cursor.insertText(".", _charsFormat);
  }
}


