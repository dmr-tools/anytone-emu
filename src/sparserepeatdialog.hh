#ifndef SPARSEREPEATDIALOG_HH
#define SPARSEREPEATDIALOG_HH

#include <QDialog>
class RepeatPattern;
class CodeplugPattern;

namespace Ui {
  class SparseRepeatDialog;
}

class SparseRepeatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SparseRepeatDialog(QWidget *parent = nullptr);
  ~SparseRepeatDialog();

public slots:
  void setPattern(RepeatPattern *pattern, const CodeplugPattern *codeplug);
  void accept();

private:
  Ui::SparseRepeatDialog *ui;
  RepeatPattern *_pattern;
};

#endif // SPARSEREPEATDIALOG_HH
