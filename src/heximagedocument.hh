#ifndef HEXIMAGEDOCUMENT_HH
#define HEXIMAGEDOCUMENT_HH

#include <QTextDocument>
#include <QTextFrameFormat>
#include <QTextBlockFormat>
#include <QTextCharFormat>

#include "hexdump.hh"

class HexDocument: public QTextDocument
{
  Q_OBJECT

public:
  HexDocument(QObject *parent = nullptr);

protected:
  virtual void putElement(const HexElement &element, QTextCursor &cursor) = 0;
  virtual void putLine(const HexLine &line, QTextCursor &cursor) = 0;

  virtual void putElementTitle(const HexElement &element, QTextCursor &cursor);
  virtual void putAddress(uint32_t address, QTextCursor &cursor);
  virtual void putValues(const QVector<HexLine::Byte> &values, QTextCursor &cursor);
  virtual void putChars(const QVector<HexLine::Byte> &values, QTextCursor &cursor);

protected:
  QTextFrameFormat _elementFormat;
  QTextBlockFormat _elementTitleFormat;
  QTextBlockFormat _lineFormat;
  QTextCharFormat  _baseFormat;
  QTextCharFormat  _addressFormat;
  QTextCharFormat  _valueFormat;
  QTextCharFormat  _keepValueFormat;
  QTextCharFormat  _addValueFormat;
  QTextCharFormat  _remValueFormat;
  QTextCharFormat  _unusedValueFormat;
  QTextCharFormat  _charsFormat;
  QTextCharFormat  _separatorFormat;
};

#endif // HEXIMAGEDOCUMENT_HH
