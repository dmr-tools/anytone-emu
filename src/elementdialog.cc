#include "elementdialog.hh"
#include "ui_elementdialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>


ElementDialog::ElementDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ElementDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-element").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-element"));

  QSettings settings;
  if (settings.contains("layout/elementDialogSize"))
    restoreGeometry(settings.value("layout/elementDialogSize").toByteArray());
}


ElementDialog::~ElementDialog()
{
  delete ui;
}


void
ElementDialog::done(int res) {
  QSettings().setValue("layout/elementDialogSize", saveGeometry());
  QDialog::done(res);
}


void
ElementDialog::setPattern(ElementPattern *pattern, const CodeplugPattern *codeplug) {
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
ElementDialog::accept() {
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
