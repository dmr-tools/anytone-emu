#ifndef PATTERNSELECTIONWIDGET_HH
#define PATTERNSELECTIONWIDGET_HH

#include <QWidget>

class AbstractPattern;
class QLabel;

namespace Ui {
  class PatternSelectionWidget;
}

class PatternSelectionWidget : public QWidget
{
  Q_OBJECT

public:
  enum class PatternType {
    None = 0,
    FixedPattern = 1,
    DensePattern = 2,
    SparsePattern = 4
  };

public:
  explicit PatternSelectionWidget(QWidget *parent = nullptr);
  ~PatternSelectionWidget();

  void setPatternTypes(int mask);
  int patternTypes() const;

  QLabel *headLabel() const;

  AbstractPattern *createPattern() const;

private:  
  Ui::PatternSelectionWidget *ui;
  int _patternTypes;
};


inline int operator|(PatternSelectionWidget::PatternType left, PatternSelectionWidget::PatternType right) {
  return (int) left | (int) right;
}
inline int operator|(int left, PatternSelectionWidget::PatternType right) {
  return left | (int) right;
}
inline int operator&(int left, PatternSelectionWidget::PatternType right) {
  return left & (int) right;
}


#endif // PATTERNSELECTIONWIDGET_HH
