#include "elementpatternview.hh"
#include "pattern.hh"
#include <QPainter>


ElementPatternView::ElementPatternView(QWidget *parent)
  : QWidget{parent}, _pattern(nullptr), _layout{QMargins(50, 30, 10, 10), 30, 30, 1, 3},
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
  painter.setPen(QPen(QPalette::HighlightedText));

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
      int left = _layout.margins.left() + _layout.colWidth*col,
          right = _layout.margins.left() + _layout.colWidth*(col + consume),
          top = _layout.margins.top() + _layout.rowHight*row,
          bottom = _layout.margins.top() + _layout.rowHight*(row+1);
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
      painter.setBrush(palette().alternateBase());
      painter.drawRect(rect);

      painter.setPen(QPen(palette().text(), _layout.lineWidth));
      painter.setBrush(QBrush(Qt::transparent));
      if (! continuation) painter.drawLine(left, bottom, left, top);
      painter.drawLine(left, top, right, top);
      if (isEnd) painter.drawLine(right, top, right, bottom);
      painter.drawLine(right, bottom, left, bottom);

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





