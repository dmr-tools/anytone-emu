#ifndef INTEGERFIELDDIALOG_HH
#define INTEGERFIELDDIALOG_HH

#include <QDialog>
class IntegerFieldPattern;

namespace Ui {
  class IntegerFieldDialog;
}


class IntegerFieldDialog : public QDialog
{
  Q_OBJECT

public:
  explicit IntegerFieldDialog(QWidget *parent = nullptr);
  ~IntegerFieldDialog();

public slots:
  void setPattern(IntegerFieldPattern *pattern);
  void accept();

private:
  Ui::IntegerFieldDialog *ui;
  IntegerFieldPattern *_pattern;
};

#endif // INTEGERFIELDDIALOG_HH
