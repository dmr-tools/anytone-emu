#ifndef UNKNOWNPATTERNDIALOG_HH
#define UNKNOWNPATTERNDIALOG_HH

#include <QDialog>

class UnknownFieldPattern;
class CodeplugPattern;


namespace Ui {
class UnknownPatternDialog;
}



class UnknownPatternDialog : public QDialog
{
  Q_OBJECT

public:
  explicit UnknownPatternDialog(QWidget *parent = nullptr);
  ~UnknownPatternDialog();

public slots:
  void setPattern(UnknownFieldPattern *pattern, const CodeplugPattern *codeplug);
  void accept();

private:
  Ui::UnknownPatternDialog *ui;
  UnknownFieldPattern *_pattern;
};

#endif // UNKNOWNPATTERNDIALOG_HH
