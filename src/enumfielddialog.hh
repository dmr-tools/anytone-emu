#ifndef ENUMFIELDDIALOG_HH
#define ENUMFIELDDIALOG_HH

#include <QDialog>
class EnumFieldPattern;

namespace Ui {
  class EnumFieldDialog;
}


class EnumFieldDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EnumFieldDialog(QWidget *parent = nullptr);
  ~EnumFieldDialog();

public slots:
  void setPattern(EnumFieldPattern *pattern);
  void accept();
  void done(int res);

private slots:
  void onAddItem();
  void onDelItem();

private:
  Ui::EnumFieldDialog *ui;
  EnumFieldPattern *_pattern;
};

#endif // ENUMFIELDDIALOG_HH
