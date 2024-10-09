#ifndef NEWPATTERNDIALOG_HH
#define NEWPATTERNDIALOG_HH

#include "pattern.hh"
#include <QDialog>

class AbstractPattern;


namespace Ui {
class NewPatternDialog;
}


class NewPatternDialog : public QDialog
{
  Q_OBJECT

public:
  NewPatternDialog(AbstractPattern *parentPattern, QWidget *parent = nullptr);
  ~NewPatternDialog();

private:
  Ui::NewPatternDialog *ui;
};

#endif // NEWPATTERNDIALOG_HH
