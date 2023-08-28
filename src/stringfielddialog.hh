#ifndef STRINGFIELDDIALOG_HH
#define STRINGFIELDDIALOG_HH

#include <QDialog>
class StringFieldPattern;

namespace Ui {
  class StringFieldDialog;
}


class StringFieldDialog : public QDialog
{
  Q_OBJECT

public:
  explicit StringFieldDialog(QWidget *parent = nullptr);
  ~StringFieldDialog();

public slots:
  void setPattern(StringFieldPattern *pattern);
  void accept();

private:
  Ui::StringFieldDialog *ui;
  StringFieldPattern *_pattern;
};

#endif // STRINGFIELDDIALOG_HH
