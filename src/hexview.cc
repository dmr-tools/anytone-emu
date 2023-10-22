#include "hexview.hh"
#include <QFontMetrics>
#include <QFontDatabase>
#include <QPainter>
#include <QPaintEvent>


/* ********************************************************************************************* *
 * Implementation of HexView::Style
 * ********************************************************************************************* */
HexView::Style::Style()
{
  _headerFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  _headerFont.setFixedPitch(true);
  _headerFont.setPointSize(14);
  _blockHeaderFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  _blockHeaderFont.setFixedPitch(true);
  _blockHeaderFont.setPointSize(12);
  _codeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  _codeFont.setFixedPitch(true);
  _codeFont.setPointSize(10);

  _defaultColor = QColor(Qt::black);
  _keepColor = QColor(Qt::darkGray);
  _removeColor = QColor(Qt::darkRed);
  _addColor = QColor(Qt::darkGreen);

  _showOnlyDiffs = true;
}

const QFont &
HexView::Style::headerFont() const {
  return _headerFont;
}

const QFont &
HexView::Style::blockHeaderFont() const {
  return _blockHeaderFont;
}

const QFont &
HexView::Style::codeFont() const {
  return _codeFont;
}

const QColor &
HexView::Style::defaultColor() const {
  return _defaultColor;
}

const QColor &
HexView::Style::keepColor() const {
  return _keepColor;
}

const QColor &
HexView::Style::removeColor() const {
  return _removeColor;
}

const QColor &
HexView::Style::addColor() const {
  return _addColor;
}

bool
HexView::Style::showOnlyDiffs() const {
  return _showOnlyDiffs;
}


/* ********************************************************************************************* *
 * Implementation of HexView::Metric
 * ********************************************************************************************* */
HexView::Metric::Metric(const Style &style, const QPaintDevice *device)
  : _headerMetrics(style.headerFont(), device),
    _blockHeaderMetrics(style.blockHeaderFont(), device), _codeMetrics(style.codeFont(), device)
{
  _blockSkip = 10;
  _charWidth = _codeMetrics.horizontalAdvance(QChar(' '));
  _addressSkip = 10;
  _byteSkip = _charWidth/2;
  _codeSkip = 3*_byteSkip;
}

const QFontMetrics &
HexView::Metric::headerMetrics() const {
  return _headerMetrics;
}

const QFontMetrics &
HexView::Metric::blockHeaderMetrics() const {
  return _blockHeaderMetrics;
}

const QFontMetrics &
HexView::Metric::codeMetrics() const {
  return _codeMetrics;
}

int
HexView::Metric::blockHeaderHeight() const {
  return _blockHeaderMetrics.height();
}

int
HexView::Metric::blockSkip() const {
  return _blockSkip;
}

int
HexView::Metric::lineSpacing() const {
  return _codeMetrics.lineSpacing();
}

int
HexView::Metric::charWidth() const {
  return _charWidth;
}

int
HexView::Metric::addressSkip() const {
  return _addressSkip;
}

int
HexView::Metric::codeSkip() const {
  return _codeSkip;
}

int
HexView::Metric::byteSkip() const {
  return _byteSkip;
}

int
HexView::Metric::addressWidth() const {
  return 8 * _charWidth;
}

int
HexView::Metric::hexBlockWidth() const {
  return 32*_charWidth + 15*_byteSkip;
}

int
HexView::Metric::asciiBlockWidth() const {
  return 16*_charWidth;
}


/* ********************************************************************************************* *
 * Implementation of HexView
 * ********************************************************************************************* */
HexView::HexView(HexImage *document, QWidget *parent)
  : QWidget{parent}, _document(document), _style(), _metric(_style, this)
{
  // pass...
}

HexView::~HexView() {
  if (_document)
    delete _document;
}

void
HexView::setHexViewStyle(const Style &style) {
  _style = style;
  _metric = Metric(_style, this);
  repaint();
}

QSize
HexView::minimumSizeHint() const {
  if (nullptr == _document)
    return QSize(0,0);

  if (_document->isDiff() && _style.showOnlyDiffs()) {
    unsigned int height = _metric.blockHeaderHeight()*_document->elementsWithDiff()
        + _metric.lineSpacing()*_document->totalDiffLines();
    if (_document->elementsWithDiff())
      height += _metric.blockSkip() * (_document->elementsWithDiff()-1);

    unsigned int width = _metric.addressWidth() + _metric.addressSkip() + _metric.hexBlockWidth()
        + _metric.codeSkip() + _metric.asciiBlockWidth()
        + 2*_metric.codeSkip() + _metric.hexBlockWidth() + _metric.asciiBlockWidth();

    return QSize(width, height);
  }

  unsigned int height = _metric.blockHeaderHeight()*_document->size()
      + _metric.lineSpacing()*_document->totalLineCount();
  if (_document->size())
    height += _metric.blockSkip() * (_document->size()-1);

  unsigned int width = _metric.addressWidth() + _metric.addressSkip() + _metric.hexBlockWidth()
      + _metric.codeSkip() + _metric.asciiBlockWidth();
  if (_document->isDiff())
    width += 2*_metric.codeSkip() + _metric.hexBlockWidth() + _metric.asciiBlockWidth();

  return QSize(width, height);
}

QSize
HexView::sizeHint() const {
  return minimumSizeHint();
}

void
HexView::setDocument(HexImage *document) {
  if (_document)
    delete _document;
  _document = document;
  repaint();
}

void
HexView::paintEvent(QPaintEvent *event) {
  if (nullptr == _document)
    return;

  QPainter painter(this);
  painter.setClipRegion(event->rect());
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setBackground(this->palette().brush(QPalette::Base));
  painter.fillRect(event->rect(),this->palette().brush(QPalette::Base));

  QPoint pos(0,0);
  for (int i=0; i<_document->size(); i++) {
    if (_document->isDiff() && _style.showOnlyDiffs() && (! _document->element(i).hasDiff()))
      continue;
    paintBlock(_document->element(i), pos, event->rect(), painter);
    if ((i+1) < _document->size())
      pos += QPoint{0, (int)_metric.blockSkip()};
  }
}


void
HexView::paintBlock(const HexElement &block, QPoint &pos, const QRect &region, QPainter &painter) {
  QRect bb;
  if (block.isDiff() && _style.showOnlyDiffs())
    bb = QRect(pos, QSize {
                 std::max(_metric.addressWidth() + _metric.addressSkip() + _metric.hexBlockWidth()
                 + _metric.codeSkip() + _metric.asciiBlockWidth(),
                 _metric.blockHeaderMetrics().horizontalAdvance(block.title())),
                 _metric.blockHeaderHeight() + _metric.lineSpacing()*(int)block.diffLines()
               });
  else
    bb = QRect(pos, QSize {
                 std::max(_metric.addressWidth() + _metric.addressSkip() + _metric.hexBlockWidth()
                 + _metric.codeSkip() + _metric.asciiBlockWidth(),
                 _metric.blockHeaderMetrics().horizontalAdvance(block.title())),
                 _metric.blockHeaderHeight() + _metric.lineSpacing()*(int)block.size()
               });


  if (! bb.intersects(region)) {
    pos += QPoint{0, bb.height()};
    return;
  }

  pos += QPoint{0, _metric.blockHeaderMetrics().ascent()};
  painter.setFont(_style.blockHeaderFont());
  painter.setPen(_style.defaultColor());
  painter.drawText(pos, block.title());
  pos += QPoint{0, _metric.blockHeaderMetrics().descent() + _metric.codeMetrics().leading()};
  for (unsigned int i=0; i<block.size(); i++) {
    if (block.isDiff() && _style.showOnlyDiffs() && (! block.line(i).hasDiff()))
      continue;
    paintLine(block.line(i), pos, region, painter);
    pos += QPoint{0, _metric.codeMetrics().leading()};
  }
}


void
HexView::paintLine(const HexLine &line, QPoint &pos, const QRect &region, QPainter &painter) {
  QFontMetrics lineMetric(_style.codeFont(), this);

  QRect bb(pos, QSize{_metric.addressWidth() + _metric.addressSkip() + _metric.hexBlockWidth()
                      + _metric.codeSkip() + _metric.asciiBlockWidth(), lineMetric.height()});
  if (! bb.intersects(region)) {
    pos += QPoint{0, bb.height()};
    return;
  }

  pos += QPoint{0, lineMetric.ascent()};
  QPoint cursor = pos; pos += QPoint{0, lineMetric.descent()};

  painter.setFont(_style.codeFont());
  painter.setPen(_style.defaultColor());
  painter.drawText(cursor, QString("%1").arg(line.address(), 8, 16, QChar('0')));
  cursor += QPoint{_metric.addressWidth() + _metric.addressSkip(), 0};

  for (int i=0; i<16; i++) {
    switch (line.left(i).type) {
    case HexLine::Byte::Unused: break;
    case HexLine::Byte::Keep: painter.setPen(_style.keepColor()); break;
    case HexLine::Byte::Remove: painter.setPen(_style.removeColor()); break;
    case HexLine::Byte::Add: painter.setPen(_style.removeColor()); break;
    }
    switch(line.left(i).type) {
    case HexLine::Byte::Unused: break;
    case HexLine::Byte::Keep:
    case HexLine::Byte::Remove:
    case HexLine::Byte::Add:
      painter.drawText(cursor, QString("%1").arg(line.left(i).value, 2, 16, QChar('0')));
      break;
    }
    cursor += QPoint{2*_metric.charWidth(), 0};
    if (16 != (i+1))
      cursor += QPoint{_metric.byteSkip(), 0};
  }
  cursor += QPoint{_metric.codeSkip(), 0};
  if (line.isDiff()) {
    for (int i=0; i<16; i++) {
      switch (line.right(i).type) {
      case HexLine::Byte::Unused: break;
      case HexLine::Byte::Keep: painter.setPen(_style.keepColor()); break;
      case HexLine::Byte::Remove: painter.setPen(_style.removeColor()); break;
      case HexLine::Byte::Add: painter.setPen(_style.removeColor()); break;
      }
      switch(line.right(i).type) {
      case HexLine::Byte::Unused: break;
      case HexLine::Byte::Keep:
      case HexLine::Byte::Remove:
      case HexLine::Byte::Add:
        painter.drawText(cursor, QString("%1").arg(line.right(i).value, 2, 16, QChar('0')));
        break;
      }
      cursor += QPoint{2*_metric.charWidth(), 0};
      if (16 != (i+1))
        cursor += QPoint{_metric.byteSkip(), 0};
    }
    cursor += QPoint{_metric.codeSkip(), 0};
  }
  for (int i=0; i<16; i++) {
    switch (line.left(i).type) {
    case HexLine::Byte::Unused: break;
    case HexLine::Byte::Keep: painter.setPen(_style.keepColor()); break;
    case HexLine::Byte::Remove: painter.setPen(_style.removeColor()); break;
    case HexLine::Byte::Add: painter.setPen(_style.removeColor()); break;
    }
    switch(line.left(i).type) {
    case HexLine::Byte::Unused: break;
    case HexLine::Byte::Keep:
    case HexLine::Byte::Remove:
    case HexLine::Byte::Add:
      painter.drawText(cursor, ((32<=line.left(i).value) && (126>=line.left(i).value)) ? QChar(line.left(i).value) : QChar('.'));
      break;
    }
    cursor += QPoint{_metric.charWidth(), 0};
  }
  if (line.isDiff()) {
    cursor += QPoint{_metric.codeSkip(), 0};
    for (int i=0; i<16; i++) {
      switch (line.right(i).type) {
      case HexLine::Byte::Unused: break;
      case HexLine::Byte::Keep: painter.setPen(_style.keepColor()); break;
      case HexLine::Byte::Remove: painter.setPen(_style.removeColor()); break;
      case HexLine::Byte::Add: painter.setPen(_style.removeColor()); break;
      }
      switch(line.right(i).type) {
      case HexLine::Byte::Unused: break;
      case HexLine::Byte::Keep:
      case HexLine::Byte::Remove:
      case HexLine::Byte::Add:
        painter.drawText(cursor, ((32<=line.right(i).value) && (126>=line.right(i).value)) ? QChar(line.right(i).value) : QChar('.'));
        break;
      }
      cursor += QPoint{_metric.charWidth(), 0};
    }
  }
}
