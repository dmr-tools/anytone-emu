#ifndef ELEMENTDIALOG_HH
#define ELEMENTDIALOG_HH

#include <QDialog>

class ElementPattern;

namespace Ui {
  class ElementDialog;
}


class ElementDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ElementDialog(QWidget *parent = nullptr);
  ~ElementDialog();

public slots:
  void setPattern(ElementPattern *pattern);
  void accept();

private:
  Ui::ElementDialog *ui;
  ElementPattern *_pattern;
};

#endif // ELEMENTDIALOG_HH
