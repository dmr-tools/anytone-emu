#ifndef ELEMENTVIEW_HH
#define ELEMENTVIEW_HH

class FixedPattern;
class ElementPattern;

#include <QWidget>
#include <QPointer>
#include <QVector>


class ElementPatternView : public QWidget
{
  Q_OBJECT

protected:
  struct Item {
    unsigned int startBit, width;
    QPointer<FixedPattern> pattern;
  };

  struct Layout {
    QMargins margins;
    unsigned int rowHight, colWidth;
  };

public:
  explicit ElementPatternView(QWidget *parent = nullptr);

  const Layout &layout() const;
  void setLayout(const Layout &layout);

  const ElementPattern *pattern() const;
  void setPattern(ElementPattern *pattern);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

protected:
  ElementPattern *_pattern;
  Layout _layout;
  QVector<Item> _items;
};

#endif // ELEMENTVIEW_HH
