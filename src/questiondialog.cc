#include "questiondialog.hh"
#include <QCheckBox>
#include <QSettings>

QuestionDialog::QuestionDialog(QWidget *parent)
  : QMessageBox(parent)
{
  setIcon(QMessageBox::Question);
  setCheckBox(new QCheckBox(tr("Don't show again.")));
}

QMessageBox::StandardButton
QuestionDialog::ask(const QString &id, const QString &title, const QString &text,
                    const QString &detailedText)
{
  QuestionDialog box;
  QSettings settings;
  bool skip = settings.value("questionDialog/skip/" + id, false).toBool();
  if (skip)
    return QMessageBox::Yes;
  box.setWindowTitle(title);
  box.setText(text);
  if (! detailedText.isEmpty())
    box.setDetailedText(detailedText);
  box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
  box.setDefaultButton(QMessageBox::No);

  if (QMessageBox::Yes != box.exec())
    return QMessageBox::No;

  settings.setValue("questionDialog/skip/" + id, box.checkBox()->isChecked());
  return QMessageBox::Yes;
}
