#include "uniondialog.hh"
#include "ui_uniondialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>


UnionDialog::UnionDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::UnionDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-union").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-union"));

  QSettings settings;
  if (settings.contains("layout/unionDialogSize"))
    restoreGeometry(settings.value("layout/unionDialogSize").toByteArray());
}


UnionDialog::~UnionDialog()
{
  delete ui;
}


void
UnionDialog::done(int res) {
  QSettings().setValue("layout/unionDialogSize", saveGeometry());
  QDialog::done(res);
}


void
UnionDialog::setPattern(UnionPattern *pattern, const CodeplugPattern *codeplug) {
  _pattern = pattern;
  if (pattern->hasImplicitAddress() && !pattern->hasAddress()) {
    ui->address->setText("implicit");
    ui->address->setEnabled(false);
  } else if (pattern->hasImplicitAddress()) {
    ui->address->setText(pattern->address().toString());
    ui->address->setEnabled(false);
  } else if (pattern->hasAddress()) {
    ui->address->setText(pattern->address().toString());
  }
  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
UnionDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  ui->metaEdit->apply();

  QDialog::accept();
}
