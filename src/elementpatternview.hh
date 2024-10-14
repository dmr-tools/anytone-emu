#ifndef ELEMENTPATTERNVIEW_HH
#define ELEMENTPATTERNVIEW_HH

class FixedPattern;
class ElementPattern;
class FixedRepeatPattern;
class UnknownFieldPattern;
class UnusedFieldPattern;
class EnumFieldPattern;
class StringFieldPattern;
class IntegerFieldPattern;


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
    unsigned int rowHight, colWidth, lineWidth, padding, radius;
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
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  int findItemAt(const QPoint &pos) const;
  FixedPattern *findPatternAt(const QPoint &pos) const;

  QString formatTooltip(const FixedPattern *pattern) const;
  QString formatTooltipElement(const ElementPattern *pattern) const;
  QString formatTooltipFixedRepeat(const FixedRepeatPattern *pattern) const;
  QString formatTooltipUnknownField(const UnknownFieldPattern *pattern) const;
  QString formatTooltipUnusedField(const UnusedFieldPattern *pattern) const;
  QString formatTooltipEnumField(const EnumFieldPattern *pattern) const;
  QString formatTooltipStringField(const StringFieldPattern *pattern) const;
  QString formatTooltipIntegerField(const IntegerFieldPattern *pattern) const;

protected:
  ElementPattern *_pattern;
  FixedPattern *_selectedPattern;
  Layout _layout;
  QVector<Item> _items;
};

#endif // ELEMENTPATTERNVIEW_HH
