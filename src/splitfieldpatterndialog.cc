#include "splitfieldpatterndialog.hh"
#include "ui_splitfieldpatterndialog.h"
#include "pattern.hh"
#include <QMessageBox>


SplitFieldPatternDialog::SplitFieldPatternDialog(FieldPattern *fieldPattern, QWidget *parent) :
  QDialog(parent), ui(new Ui::SplitFieldPatternDialog), _field(fieldPattern)
{
  ui->setupUi(this);
  ui->address->setText(_field->address().toString());
  ui->patternSelection->setPatternTypes((int)PatternSelectionWidget::PatternType::FieldPattern);
  ui->addressRange->setText(tr("Valid range %1 - %2.")
                            .arg(_field->address().toString())
                            .arg((_field->address() + _field->size()).toString()));
  ui->patternSelection->headLabel()->setText(tr("Specify the field to insert."));
}


SplitFieldPatternDialog::~SplitFieldPatternDialog() {
  delete ui;
}

Address
SplitFieldPatternDialog::address() const {
  return Address::fromString(ui->address->text());
}

FixedPattern *
SplitFieldPatternDialog::createPattern() const {
  return ui->patternSelection->createPattern()->as<FixedPattern>();
}


void
SplitFieldPatternDialog::accept() {
  Address addr = Address::fromString(ui->address->text());
  if (! addr.isValid()) {
    QMessageBox::information(nullptr, tr("Invalid address"), tr("Address format is invalid."));
    return;
  }

  if ((addr < _field->address()) || (addr >= (_field->address()+_field->size()))) {
    QMessageBox::information(nullptr, tr("Invalid address"), tr("Address is outside valid range."));
    return;
  }

  QDialog::accept();
}
