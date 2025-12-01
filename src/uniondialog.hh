#ifndef UNIONDIALOG_HH
#define UNIONDIALOG_HH

#include <QDialog>

class UnionPattern;
class CodeplugPattern;
namespace Ui {
class UnionDialog;
}


class UnionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit UnionDialog(QWidget *parent = nullptr);
  ~UnionDialog();

public slots:
  void setPattern(UnionPattern *pattern, const CodeplugPattern *codeplug);
  void accept();
  void done(int res);

private:
  Ui::UnionDialog *ui;
  UnionPattern *_pattern;
};


#endif // UNIONDIALOG_HH
