#include "elementdialog.hh"
#include "ui_elementdialog.h"
#include "codeplugpattern.hh"
#include <QMessageBox>


ElementDialog::ElementDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ElementDialog)
{
  ui->setupUi(this);
}

ElementDialog::~ElementDialog()
{
  delete ui;
}

void
ElementDialog::setPattern(ElementPattern *pattern) {
  _pattern = pattern;
  if (pattern->hasImplicitAddress() && !pattern->hasAddress()) {
    ui->address->setText("implicit");
    ui->address->setEnabled(false);
  } else if (pattern->hasImplicitAddress()) {
    ui->address->setText(pattern->address().toString());
    ui->address->setEnabled(false);
  } else {
    ui->address->setText(pattern->address().toString());
  }
  ui->metaEdit->setPatternMeta(&pattern->meta());
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
