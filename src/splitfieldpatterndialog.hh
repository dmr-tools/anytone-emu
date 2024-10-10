#ifndef SPLITFIELDPATTERNDIALOG_HH
#define SPLITFIELDPATTERNDIALOG_HH

#include <QDialog>

class FieldPattern;

namespace Ui {
class SplitFieldPatternDialog;
}

class SplitFieldPatternDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SplitFieldPatternDialog(FieldPattern *fieldPattern, QWidget *parent = nullptr);
  ~SplitFieldPatternDialog();

public slots:
  void accept() override;

private:
  Ui::SplitFieldPatternDialog *ui;
  FieldPattern *_field;
};

#endif // SPLITFIELDPATTERNDIALOG_HH
