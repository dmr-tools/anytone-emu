#ifndef CODEPLUGDIALOG_HH
#define CODEPLUGDIALOG_HH

#include <QDialog>

class CodeplugPattern;

namespace Ui {
class CodeplugDialog;
}



class CodeplugDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CodeplugDialog(QWidget *parent = nullptr);
  ~CodeplugDialog();

public slots:
  void setPattern(CodeplugPattern *pattern);
  void accept();

private:
  Ui::CodeplugDialog *ui;
  CodeplugPattern *_codeplug;
};

#endif // CODEPLUGDIALOG_HH
