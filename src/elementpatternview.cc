#include "elementpatternview.hh"
#include "pattern.hh"
#include <QPainter>
#include <QEvent>
#include <QMenu>
#include <QHelpEvent>
#include <QToolTip>
#include <algorithm>
#include <QPainterPath>

ElementPatternView::ElementPatternView(QWidget *parent)
  : QWidget{parent}, _pattern(), _selectedPattern(),
    _layout{QMargins(60, 40, 10, 10), 40, 40, 1, 3, 10}
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
}


const ElementPatternView::Layout &
ElementPatternView::layout() const {
  return _layout;
}

void
ElementPatternView::setLayout(const Layout &layout) {
  _layout = layout;
  update();
}


const ElementPattern *
ElementPatternView::pattern() const {
  return _pattern.data();
}

void
ElementPatternView::setPattern(ElementPattern *pattern) {
  if (! _pattern.isNull())
    disconnect(_pattern, nullptr, this, nullptr);

  _pattern = pattern;
  update();

  if (_pattern.isNull())
    return;

  connect(_pattern, &AbstractPattern::modified,
          this, [this](const AbstractPattern *pattern){this->update();});
}


FixedPattern *
ElementPatternView::selectedPattern() const {
  return _selectedPattern;
}


QSize
ElementPatternView::sizeHint() const {
  return minimumSizeHint();
}

QSize
ElementPatternView::minimumSizeHint() const {
  unsigned int width = _layout.margins.left() + _layout.margins.right(),
      height = _layout.margins.top() + _layout.margins.bottom();

  if (_pattern.isNull() || (0 == _pattern->numChildPattern()))
    return QSize(width, height);

  unsigned int numBits = _pattern->size().bits();
  unsigned int rows = (numBits / 32) + (numBits%32 ? 1 : 0),
      cols = (rows > 1) ? 32 : numBits;
  width += cols*_layout.colWidth;
  height += rows*_layout.rowHight;

  return QSize(width, height);
}


void
ElementPatternView::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  QFont defaultFont = painter.font();
  QFont addressFont = defaultFont; addressFont.setFamily("monospace");

  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setPen(QPen(palette().text().color()));
  painter.setBrush(palette().text());
  for (int i=0; i<32; i++) {
    QRect r(_layout.margins.left() + i*_layout.colWidth, 0, _layout.colWidth, _layout.rowHight);
    r.adjust(_layout.padding, _layout.padding, -_layout.padding, -_layout.padding);
    painter.setFont(addressFont);
    painter.drawText(r, QString::number(7 - (i%8)),
                     QTextOption(Qt::AlignHCenter | Qt::AlignBottom));
  }

  if (_pattern.isNull() || (!_pattern->size().isValid()))
    return;

  for (unsigned int idx=0; idx < _pattern->numChildPattern(); idx++) {
    FixedPattern *pattern = _pattern->childPattern(idx)->as<FixedPattern>();
    // skip invalid sized pattern
    if (! pattern->size().isValid())
      continue;
    unsigned int row = Offset(pattern->address()).bits()/32,
        col=Offset(pattern->address()).bits()%32,
        width=pattern->size().bits();
    bool continuation = false;

    while (width) {
      unsigned int consume = std::min((32u-col), width); width -= consume;
      bool isEnd = (0 == width);
      int left = _layout.margins.left() + _layout.colWidth*col + _layout.lineWidth/2,
          right = _layout.margins.left() + _layout.colWidth*(col + consume) - _layout.lineWidth/2,
          top = _layout.margins.top() + _layout.rowHight*row + _layout.lineWidth/2,
          bottom = _layout.margins.top() + _layout.rowHight*(row+1) - _layout.lineWidth/2;
      QRect rect = QRect(left+_layout.lineWidth, top+_layout.lineWidth,
                         right-left-2*_layout.lineWidth, bottom-top-2*_layout.lineWidth);

      if (0 == col) {
        painter.setPen(QPen(palette().text().color()));
        painter.setBrush(palette().text());
        QRect r = QRect(0, _layout.margins.top() + row*_layout.rowHight,
                        _layout.margins.left(), _layout.rowHight);
        r.adjust(_layout.padding, _layout.padding, -_layout.padding, -_layout.padding);
        painter.setFont(addressFont);
        painter.drawText(
              r, QString("%1h").arg(4*row, 4, 16),
              QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
      }

      renderBlock(painter, pattern, rect, continuation, isEnd);

      col += consume; row += col/32; col = col % 32;
      continuation = true;
    }
  }
}


void
ElementPatternView::renderBlock(
    QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd)
{
  renderBlockBorder(painter, pattern, rect, isContinuation, isEnd);
  renderBlockText(painter, pattern, rect, isContinuation, isEnd);
}

void
ElementPatternView::renderBlockBorder(QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd)
{
  QPainterPath borderPath;
  QPainterPath fillPath;

  if (isContinuation) {
    borderPath.moveTo(rect.topLeft()); borderPath.lineTo(rect.left()+_layout.radius, rect.top());
    fillPath.moveTo(rect.topLeft());   fillPath.lineTo(rect.left()+_layout.radius, rect.top());
  } else {
    borderPath.moveTo(rect.left(), rect.top()+_layout.radius);
    borderPath.arcTo(rect.left(), rect.top(), _layout.radius, _layout.radius, 180,-90);
    fillPath.moveTo(rect.left(), rect.top()+_layout.radius);
    fillPath.arcTo(rect.left(), rect.top(), _layout.radius, _layout.radius, 180,-90);
  }

  borderPath.lineTo(rect.right()-_layout.radius, rect.top());
  fillPath.lineTo(rect.right()-_layout.radius, rect.top());

  if (isEnd) {
    borderPath.arcTo(rect.right()-_layout.radius, rect.top(), _layout.radius, _layout.radius,
                     90, -90);
    borderPath.lineTo(rect.right(), rect.bottom()-_layout.radius);
    borderPath.arcTo(rect.right()-_layout.radius, rect.bottom()-_layout.radius,
                     _layout.radius, _layout.radius,
                     0, -90);
    fillPath.arcTo(rect.right()-_layout.radius, rect.top(), _layout.radius, _layout.radius,
                     90, -90);
    fillPath.lineTo(rect.right(), rect.bottom()-_layout.radius);
    fillPath.arcTo(rect.right()-_layout.radius, rect.bottom()-_layout.radius,
                     _layout.radius, _layout.radius,
                     0, -90);
  } else {
    borderPath.lineTo(rect.right(), rect.top());
    borderPath.moveTo(rect.right(), rect.bottom());
    borderPath.lineTo(rect.right()-_layout.radius, rect.bottom());
    fillPath.lineTo(rect.right(), rect.top());
    fillPath.lineTo(rect.right(), rect.bottom());
    fillPath.lineTo(rect.right()-_layout.radius, rect.bottom());
  }

  borderPath.lineTo(rect.left()+_layout.radius, rect.bottom());
  fillPath.lineTo(rect.left()+_layout.radius, rect.bottom());

  if (isContinuation) {
    borderPath.lineTo(rect.left(), rect.bottom());
    borderPath.moveTo(rect.topLeft());
    fillPath.lineTo(rect.left(), rect.bottom());
    fillPath.lineTo(rect.topLeft());
  } else {
    borderPath.arcTo(rect.left(), rect.bottom()-_layout.radius,
                     _layout.radius, _layout.radius, -90,-90);
    borderPath.lineTo(rect.left(), rect.top()+_layout.radius);
    fillPath.arcTo(rect.left(), rect.bottom()-_layout.radius,
                         _layout.radius, _layout.radius, -90,-90);
    fillPath.lineTo(rect.left(), rect.top()+_layout.radius);
  }

  if (pattern == _selectedPattern) {
    painter.strokePath(borderPath, QPen(palette().highlightedText(), _layout.lineWidth));
    painter.fillPath(fillPath, palette().highlight());
  } else {
    painter.strokePath(borderPath, QPen(palette().text(), _layout.lineWidth));
    painter.fillPath(fillPath, palette().alternateBase());
  }
}

void
ElementPatternView::renderBlockText(QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd) {
  QFont defaultFont = painter.font();
  QFont addressFont = defaultFont; addressFont.setFamily("monospace");

  painter.setPen(QPen(palette().text().color()));
  painter.setBrush(palette().text());
  painter.setFont(defaultFont);

  QString text = pattern->meta().name();
  QTextOption textOption = QTextOption(Qt::AlignLeft|Qt::AlignVCenter);

  if (isContinuation) {
    text = "...";
    textOption = QTextOption(Qt::AlignHCenter|Qt::AlignVCenter);
  } else if ((pattern->size().bits() < 4) && pattern->meta().hasShortName()) {
    text = pattern->meta().shortName();
  }

  painter.drawText(rect.adjusted(_layout.padding, _layout.padding,
                                 -_layout.padding, -_layout.padding),
                   text, textOption);
}


bool
ElementPatternView::event(QEvent *event) {
  if (QEvent::ToolTip == event->type()) {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    FixedPattern *pattern = findPatternAt(helpEvent->pos());
    if (nullptr == pattern) {
      QToolTip::hideText();
      event->ignore();
    } else {
      QToolTip::showText(
            helpEvent->globalPos(),
            formatTooltip(pattern));
    }
  }

  return QWidget::event(event);
}


void
ElementPatternView::mousePressEvent(QMouseEvent *event) {
  QWidget::mousePressEvent(event);
}


void
ElementPatternView::mouseDoubleClickEvent(QMouseEvent *event) {
  auto pattern = findPatternAt(event->position().toPoint());
  if ((Qt::LeftButton & event->buttons()) && pattern) {
    event->accept();
    emit doubleClicked(pattern);
  }

  QWidget::mouseDoubleClickEvent(event);
}


void
ElementPatternView::mouseReleaseEvent(QMouseEvent *event) {
  if (auto pattern = findPatternAt(event->position().toPoint())) {
    if (pattern != _selectedPattern) {
      event->accept();
      _selectedPattern = pattern;
      emit selectionChanged(_selectedPattern);
      update();
    }
  } else {
    if (nullptr != _selectedPattern) {
      _selectedPattern = nullptr;
      emit selectionChanged(_selectedPattern);
      update();
    }
  }

  QWidget::mouseReleaseEvent(event);
}


void
ElementPatternView::contextMenuEvent(QContextMenuEvent *event) {
  if (_selectedPattern) {
    QMenu contextMenu;
    contextMenu.setTitle(_selectedPattern->meta().name());
    contextMenu.addActions(actions());
    contextMenu.exec(event->globalPos());
    event->accept();
  }

  QWidget::contextMenuEvent(event);
}

int
ElementPatternView::findItemAt(const QPoint &pos) const {
  if (_pattern.isNull())
    return -1;

  QRect bb(_layout.margins.left(), _layout.margins.top(),
           minimumSizeHint().width() - _layout.margins.left() - _layout.margins.right(),
           minimumSizeHint().height() - _layout.margins.top() - _layout.margins.bottom());
  if (! bb.contains(pos))
    return -1;

  int x = pos.x()-bb.left(), y = pos.y()-bb.top();
  int col = x/_layout.colWidth, row = y/_layout.rowHight;
  Address bit = Address(Offset::fromBits(32*row + col));

  for (unsigned int i=0; i<_pattern->numChildPattern(); i++) {
    Address a = _pattern->childPattern(i)->address(),
        b = a + _pattern->childPattern(i)->as<FixedPattern>()->size();
    if ((a <= bit) && (b > bit))
      return i;
  }

  return -1;
}

FixedPattern *
ElementPatternView::findPatternAt(const QPoint &pos) const {
  int idx = findItemAt(pos);
  if (0 > idx)
    return nullptr;
  return _pattern->childPattern(idx)->as<FixedPattern>();
}


QString
ElementPatternView::formatTooltip(const FixedPattern *pattern) const {
  QString res;
  // Dispatch by type:
  if (pattern->is<ElementPattern>())
    res = formatTooltipElement(pattern->as<ElementPattern>());
  else if (pattern->is<FixedRepeatPattern>())
    res = formatTooltipFixedRepeat(pattern->as<FixedRepeatPattern>());
  else if (pattern->is<UnknownFieldPattern>())
    res = formatTooltipUnknownField(pattern->as<UnknownFieldPattern>());
  else if (pattern->is<UnusedFieldPattern>())
    res = formatTooltipUnusedField(pattern->as<UnusedFieldPattern>());
  else if (pattern->is<EnumFieldPattern>())
    res = formatTooltipEnumField(pattern->as<EnumFieldPattern>());
  else if (pattern->is<StringFieldPattern>())
    res = formatTooltipStringField(pattern->as<StringFieldPattern>());
  else if (pattern->is<IntegerFieldPattern>())
    res = formatTooltipIntegerField(pattern->as<IntegerFieldPattern>());
  else
    res = tr("<h3>Unknown Pattern <i>%1</i> at <tt>%2</tt></h3>")
        .arg(pattern->meta().name())
        .arg(pattern->address().toString());

  if (pattern->meta().hasFirmwareVersion())
    res.append(tr("<h5>Firmware version %1</h5>").arg(pattern->meta().firmwareVersion()));

  if (pattern->meta().hasBriefDescription())
    res.append(tr("<p>%1</p>").arg(pattern->meta().briefDescription()));

  if (pattern->meta().hasDescription())
    res.append(tr("<p>%1</p>").arg(pattern->meta().description()));

  return res;
}


QString
ElementPatternView::formatTooltipElement(const ElementPattern *pattern) const {
  return tr("<h3>Element <i>%1</i> at <tt>%2</tt></h3>"
            "<h5>Size <tt>%3</tt></h5>")
      .arg(pattern->meta().name())
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
}

QString
ElementPatternView::formatTooltipFixedRepeat(const FixedRepeatPattern *pattern) const {
  return tr("<h3>Repetition <i>%1</i> at <tt>%2</tt></h3>"
            "<h5>Size <tt>%3</tt></h5>")
      .arg(pattern->meta().name())
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
}

QString
ElementPatternView::formatTooltipUnknownField(const UnknownFieldPattern *pattern) const {
  return tr("<h3>Unknown data at <tt>%1</tt></h3>"
            "<h5>Size <tt>%3</tt></h5>")
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
}

QString
ElementPatternView::formatTooltipUnusedField(const UnusedFieldPattern *pattern) const {
  return tr("<h3>Unused data at <tt>%1</tt></h3>"
            "<h5>Size <tt>%3</tt></h5>")
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
}

QString
ElementPatternView::formatTooltipEnumField(const EnumFieldPattern *pattern) const {
  QString res = tr("<h3>Enum <i>%1</i> at <tt>%2</tt></h3>"
                   "<h5>Size <tt>%3</tt></h5>")
      .arg(pattern->meta().name())
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
  if (pattern->numItems()) {
    res += tr("<h5>Items</h5>") + "<ul>";
    for (unsigned int i=0; i<pattern->numItems(); i++) {
      res += tr("<li><tt>%1</tt> &mdash; %2</li>")
          .arg(pattern->item(i)->value())
          .arg(pattern->item(i)->name());
    }
    res += "</ul>";
  }
  return res;
}

QString
ElementPatternView::formatTooltipStringField(const StringFieldPattern *pattern) const {
  return tr("<h3>%1 String <i>%2</i> at <tt>%3</tt></h3>"
            "<h5>Size <tt>%4</tt></h5>")
      .arg(StringFieldPattern::Format::ASCII == pattern->format() ? "ASCII" : "Unicode")
      .arg(pattern->meta().name())
      .arg(pattern->address().toString())
      .arg(pattern->size().toString());
}

QString
ElementPatternView::formatTooltipIntegerField(const IntegerFieldPattern *pattern) const {
  QString format;
  if (1 == pattern->size().bits())
    format = tr("Bit");
  else if (8 >= pattern->size().bits()) {
    format = tr("%1int%2 Integer")
        .arg(IntegerFieldPattern::Format::Unsigned == pattern->format() ? "u" : "")
        .arg(pattern->size().bits());
  } else if (IntegerFieldPattern::Format::BCD == pattern->format()) {
    format = tr("bcd%1%2 Integer")
        .arg(pattern->size().bits()/4)
        .arg(IntegerFieldPattern::Endian::Little == pattern->endian() ? "le" : "be");
  } else {
    format = tr("%1int%2%3 Integer")
        .arg(IntegerFieldPattern::Format::Unsigned == pattern->format() ? "u" : "")
        .arg(pattern->size().bits()/4)
        .arg(IntegerFieldPattern::Endian::Little == pattern->endian() ? "le" : "be");
  }

  return tr("<h3>%1 <i>%2</i> at <tt>%3</tt></h3>")
      .arg(format)
      .arg(pattern->meta().name())
      .arg(pattern->address().toString());
}
