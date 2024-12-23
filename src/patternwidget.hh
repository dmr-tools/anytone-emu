#ifndef PATTERNWIDGET_HH
#define PATTERNWIDGET_HH

#include <QWidget>

class ElementPattern;

namespace Ui {
class patternWidget;
}


class PatternWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PatternWidget(QWidget *parent = nullptr);
  ~PatternWidget();

signals:
  void viewPattern(ElementPattern *element);

protected slots:
  void onViewPattern();
  void onClipboardDataChanged();

private:
  Ui::patternWidget *ui;
};

#endif // PATTERNWIDGET_HH
