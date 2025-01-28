#ifndef FIXEDREPEATDIALOG_HH
#define FIXEDREPEATDIALOG_HH

#include <QDialog>

class FixedRepeatPattern;
class CodeplugPattern;

namespace Ui {
  class FixedRepeatDialog;
}


class FixedRepeatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit FixedRepeatDialog(QWidget *parent = nullptr);
  ~FixedRepeatDialog();

public slots:
  void setPattern(FixedRepeatPattern *pattern, const CodeplugPattern *codeplug);
  void accept();
  void done(int res);

private:
  Ui::FixedRepeatDialog *ui;
  FixedRepeatPattern *_pattern;
};

#endif // FIXEDREPEATDIALOG_HH
