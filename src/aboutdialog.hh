#ifndef ABOUTDIALOG_HH
#define ABOUTDIALOG_HH

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget *parent = nullptr);
  ~AboutDialog();

public slots:
  void done(int res);

private:
  Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_HH
