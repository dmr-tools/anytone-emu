#ifndef ELEMENTPATTERNEDITOR_HH
#define ELEMENTPATTERNEDITOR_HH

#include <QWidget>

class ElementPattern;
class FixedPattern;

namespace Ui {
  class ElementPatternEditor;
}


class ElementPatternEditor : public QWidget
{
  Q_OBJECT

public:
  explicit ElementPatternEditor(QWidget *parent = nullptr);
  ~ElementPatternEditor();

  const ElementPattern *pattern() const;
  void setPattern(ElementPattern *pattern);

protected slots:
  void onSelectionChanged(FixedPattern *selected);
  void onDoubleClick(FixedPattern *pattern);
  void onEditPattern();
  void onSplitField();
  void onMarkAsUnknown();

private:
  Ui::ElementPatternEditor *ui;
};

#endif // ELEMENTPATTERNEDITOR_HH
