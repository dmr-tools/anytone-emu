#ifndef UNUSEDFIELDDIALOG_HH
#define UNUSEDFIELDDIALOG_HH

#include <QDialog>
class UnusedFieldPattern;
class CodeplugPattern;


namespace Ui {
  class UnusedFieldDialog;
}


class UnusedFieldDialog : public QDialog
{
  Q_OBJECT

public:
  explicit UnusedFieldDialog(QWidget *parent = nullptr);
  ~UnusedFieldDialog();

public slots:
  void setPattern(UnusedFieldPattern *pattern, const CodeplugPattern *codeplug);
  void accept();
  void done(int res);

private:
  Ui::UnusedFieldDialog *ui;
  UnusedFieldPattern *_pattern;
};

#endif // UNUSEDFIELDDIALOG_HH
