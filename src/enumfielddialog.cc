#include "enumfielddialog.hh"
#include "ui_enumfielddialog.h"
#include "codeplugpattern.hh"
#include <QMessageBox>


EnumFieldDialog::EnumFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::EnumFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
}

EnumFieldDialog::~EnumFieldDialog()
{
  delete ui;
}


void
EnumFieldDialog::setPattern(EnumFieldPattern *pattern) {
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

  ui->width->setValue(_pattern->size().bits());


  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
EnumFieldDialog::accept() {
  Address addr = Address::fromString(ui->address->text());
  if (! addr.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid address format."),
                          tr("Invalid address format '%1'.").arg(ui->address->text()));
    return;
  }

  _pattern->setAddress(addr);
  _pattern->setWidth(Offset::fromBits(ui->width->value()));

  QDialog::accept();
}

