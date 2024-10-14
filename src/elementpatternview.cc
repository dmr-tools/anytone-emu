#include "elementview.hh"
#include "pattern.hh"
#include <QPainter>

ElementPatternView::ElementPatternView(QWidget *parent)
  : QWidget{parent}, _pattern(nullptr), _layout{QMargins(50, 30, 10, 10), 30, 30},
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

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(QPen(QPalette::HighlightedText));

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
      //painter.setBrush(QBrush(QPalette::AlternateBase));
      QRect rect = QRect(left, top, right-left, bottom-top);
      painter.drawRect(rect);
      //painter.setBrush(QBrush());

      painter.setPen(QPen(QPalette::HighlightedText));
      if (! continuation) painter.drawLine(left, bottom, left, top);
      painter.drawLine(left, bottom, left, top);
      if (isEnd) painter.drawLine(right, top, right, bottom);
      painter.drawLine(right, bottom, left, bottom);

      painter.setPen(QPen(QPalette::Text));
      if (! continuation)
        painter.drawText(rect, item.pattern->meta().name());
      else
        painter.drawText(rect, "...");
    }
  }
}





