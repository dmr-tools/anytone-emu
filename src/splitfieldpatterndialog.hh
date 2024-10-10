#ifndef SPLITFIELDPATTERNDIALOG_HH
#define SPLITFIELDPATTERNDIALOG_HH

#include <QDialog>
#include "offset.hh"


class FieldPattern;
class FixedPattern;

namespace Ui {
class SplitFieldPatternDialog;
}

class SplitFieldPatternDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SplitFieldPatternDialog(FieldPattern *fieldPattern, QWidget *parent = nullptr);
  ~SplitFieldPatternDialog();

  Address address() const;
  FixedPattern *createPattern() const;

public slots:
  void accept() override;

private:
  Ui::SplitFieldPatternDialog *ui;
  FieldPattern *_field;
};

#endif // SPLITFIELDPATTERNDIALOG_HH
