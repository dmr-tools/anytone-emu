#ifndef BLOCKREPEATDIALOG_HH
#define BLOCKREPEATDIALOG_HH

#include <QDialog>

class BlockRepeatPattern;
class CodeplugPattern;

namespace Ui {
  class BlockRepeatDialog;
}


class BlockRepeatDialog : public QDialog
{
  Q_OBJECT

public:
  explicit BlockRepeatDialog(QWidget *parent = nullptr);
  ~BlockRepeatDialog();

public slots:
  void setPattern(BlockRepeatPattern *pattern, const CodeplugPattern *codeplug);
  void accept();
  void done(int res);

private:
  Ui::BlockRepeatDialog *ui;
  BlockRepeatPattern *_pattern;
};

#endif // BLOCKREPEATDIALOG_HH
