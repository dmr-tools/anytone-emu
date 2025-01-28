#ifndef PATTERNIMPORTDIALOG_HH
#define PATTERNIMPORTDIALOG_HH

#include <QDialog>
#include "modeldefinition.hh"


class AbstractPattern;
namespace Ui {
  class PatternImportDialog;
}


class PatternImportDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PatternImportDialog(const QString &catalog, QWidget *parent = nullptr);
  ~PatternImportDialog();

  AbstractPattern *copy();

public slots:
  void done(int res);
  void accept();

protected slots:
  void onModelSelected(int index);
  void onFirmwareSelected(int index);

protected:
  AbstractPattern *markNeedsReview(AbstractPattern *pattern);
  void closeEvent(QCloseEvent *evt);

private:
  Ui::PatternImportDialog *ui;
  ModelCatalog _catalog;
};


#endif // PATTERNIMPORTDIALOG_HH
