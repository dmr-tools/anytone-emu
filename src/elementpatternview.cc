#include "elementpatternview.hh"
#include "pattern.hh"
#include <QPainter>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <algorithm>


ElementPatternView::ElementPatternView(QWidget *parent)
  : QWidget{parent}, _pattern(nullptr), _selectedPattern(nullptr),
    _layout{QMargins(60, 40, 10, 10), 40, 40, 1, 3, 10},
    _items()
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
  return _pattern;
}

void
ElementPatternView::setPattern(ElementPattern *pattern) {
  _items.clear();
  _pattern = pattern;

  if (nullptr == _pattern) {
    update();
    return;
  }

  connect(_pattern, &QObject::destroyed, this, [this](QObject *obj){this->update();});
  connect(_pattern, &AbstractPattern::modified, this, [this](const AbstractPattern *pattern){this->update();});

  for (unsigned int n=0; n<_pattern->numChildPattern(); n++) {
    auto child = _pattern->childPattern(n)->as<FixedPattern>();
    _items.append({(unsigned int)((Offset)child->address()).bits(),
                   (unsigned int)child->size().bits(), QPointer<FixedPattern>(child)});
  }
}


QSize
ElementPatternView::sizeHint() const {
  return minimumSizeHint();
}

QSize
ElementPatternView::minimumSizeHint() const {
  unsigned int width = _layout.margins.left() + _layout.margins.right(),
      height = _layout.margins.top() + _layout.margins.bottom();

  if (! _items.empty()) {
    unsigned int numBits = _items.back().startBit + _items.back().width;
    unsigned int rows = (numBits / 32) + (numBits%32 ? 1 : 0),
        cols = (rows > 1) ? 32 : numBits;
    width += cols*_layout.colWidth;
    height += rows*_layout.rowHight;
  }

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

  foreach(Item item, _items) {
    unsigned int row=item.startBit/32, col=item.startBit%32, width=item.width;
    bool continuation = false;

    while (width) {
      unsigned int consume = std::min((32u-col), width); width -= consume;
      bool isEnd = (0 == width);
      int left = _layout.margins.left() + _layout.colWidth*col + _layout.lineWidth/2,
          right = _layout.margins.left() + _layout.colWidth*(col + consume) - _layout.lineWidth/2,
          top = _layout.margins.top() + _layout.rowHight*row + _layout.lineWidth/2,
          bottom = _layout.margins.top() + _layout.rowHight*(row+1) - _layout.lineWidth/2;
      QRect rect = QRect(left, top, right-left, bottom-top);

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

      painter.setPen(Qt::transparent);
      if (item.pattern == _selectedPattern)
        painter.setBrush(palette().highlight());
      else
        painter.setBrush(palette().alternateBase());
      painter.drawRect(rect);


      if (item.pattern == _selectedPattern)
        painter.setPen(QPen(palette().highlightedText(), _layout.lineWidth));
      else
        painter.setPen(QPen(palette().text(), _layout.lineWidth));
      painter.setBrush(QBrush(Qt::transparent));
      if (! continuation) {
        painter.drawArc(left, top, _layout.radius, _layout.radius,
                        16*90, 90*16);
        painter.drawArc(left, bottom-_layout.radius, _layout.radius, _layout.radius,
                        16*180, 90*16);
        painter.drawLine(left, bottom-_layout.radius+_layout.lineWidth, left, top+_layout.radius-_layout.lineWidth);
      }
      if (isEnd) {
        painter.drawLine(right, top+_layout.radius-_layout.lineWidth, right, bottom-_layout.radius+_layout.lineWidth);
        painter.drawArc(right-_layout.radius, top, _layout.radius, _layout.radius,
                        0, 90*16);
        painter.drawArc(right-_layout.radius, bottom-_layout.radius, _layout.radius, _layout.radius,
                        16*270, 90*16);
      }
      painter.drawLine(left+_layout.radius-_layout.lineWidth, top,
                       right-_layout.radius+_layout.lineWidth, top);
      painter.drawLine(right-_layout.radius+_layout.lineWidth, bottom,
                       left+_layout.radius-_layout.lineWidth, bottom);

      if (item.pattern == _selectedPattern)
        painter.setPen(QPen(palette().highlightedText(), _layout.lineWidth));
      else
        painter.setPen(QPen(palette().text(), _layout.lineWidth));
      painter.setPen(QPen(palette().text().color()));
      painter.setBrush(palette().text());
      painter.setFont(defaultFont);
      if (! continuation) {
        painter.drawText(rect.adjusted(_layout.padding, _layout.padding,
                                       -_layout.padding, -_layout.padding),
                         item.pattern->meta().name(),
                         QTextOption(Qt::AlignLeft|Qt::AlignVCenter));
      } else {
        painter.drawText(rect.adjusted(_layout.padding, _layout.padding,
                                       -_layout.padding, -_layout.padding),
                         "...",
                         QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));
      }

      col += consume; row += col/32; col = col % 32;
      continuation = true;
    }
  }
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
ElementPatternView::mouseReleaseEvent(QMouseEvent *event) {
  if (auto pattern = findPatternAt(event->position().toPoint())) {
    event->accept();
    if (pattern != _selectedPattern) {
      _selectedPattern = pattern;
      update();
    }
  } else {
    event->ignore();
    if (nullptr != _selectedPattern) {
      _selectedPattern = nullptr;
      update();
    }
  }

  QWidget::mouseReleaseEvent(event);
}


int
ElementPatternView::findItemAt(const QPoint &pos) const {
  if ((pos.x() < _layout.margins.left()) || (pos.y() < _layout.margins.top()))
      return -1;

  int x = std::max(0, pos.x()-_layout.margins.left()),
      y = std::max(0, pos.y()-_layout.margins.right());
  int col = x/_layout.colWidth, row = y/_layout.rowHight;
  int bit = 32*row + col;

  for (int i=0; i<_items.size(); i++) {
    Address a = _items.at(i).pattern->address(), b = a + _items.at(i).pattern->size(),
        c = Address(Offset::fromBits(bit));
    if ((a <= c) && ((a+b) > c))
      return i;
  }

  return -1;
}

FixedPattern *
ElementPatternView::findPatternAt(const QPoint &pos) const {
  int idx = findItemAt(pos);
  if (0 > idx)
    return nullptr;
  return _items.at(idx).pattern;
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
