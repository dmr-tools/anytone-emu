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
  NewPatternDialog(AbstractPattern *parentPattern, const Address &addr = Address(), QWidget *parent = nullptr);
  ~NewPatternDialog();

  AbstractPattern *create() const;

public slots:
  void accept();

private:
  Ui::NewPatternDialog *ui;
};

#endif // NEWPATTERNDIALOG_HH
