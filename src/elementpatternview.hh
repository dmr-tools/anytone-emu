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
  FixedPattern *selectedPattern() const;

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

signals:
  void selectionChanged(FixedPattern *pattern);
  void doubleClicked(FixedPattern *pattern);

protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;


private:
  int findItemAt(const QPoint &pos) const;
  FixedPattern *findPatternAt(const QPoint &pos) const;

  void renderBlock(QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd);
  void renderBlockBorder(QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd);
  void renderBlockText(QPainter &painter, FixedPattern *pattern, const QRect &rect, bool isContinuation, bool isEnd);

  QString formatTooltip(const FixedPattern *pattern) const;
  QString formatTooltipElement(const ElementPattern *pattern) const;
  QString formatTooltipFixedRepeat(const FixedRepeatPattern *pattern) const;
  QString formatTooltipUnknownField(const UnknownFieldPattern *pattern) const;
  QString formatTooltipUnusedField(const UnusedFieldPattern *pattern) const;
  QString formatTooltipEnumField(const EnumFieldPattern *pattern) const;
  QString formatTooltipStringField(const StringFieldPattern *pattern) const;
  QString formatTooltipIntegerField(const IntegerFieldPattern *pattern) const;

protected:
  QPointer<ElementPattern> _pattern;
  QPointer<FixedPattern> _selectedPattern;
  Layout _layout;
};

#endif // ELEMENTPATTERNVIEW_HH
