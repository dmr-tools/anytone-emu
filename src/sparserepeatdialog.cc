#include "sparserepeatdialog.hh"
#include "ui_sparserepeatdialog.h"
#include "codeplugpattern.hh"
#include <QMessageBox>


SparseRepeatDialog::SparseRepeatDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::SparseRepeatDialog), _pattern(nullptr)
{
  ui->setupUi(this);
}


void
SparseRepeatDialog::setPattern(RepeatPattern *pattern) {
  _pattern = pattern;
  ui->address->setText(pattern->address().toString());
  ui->offset->setText(pattern->step().toString());
  ui->minRepetitionUnset->setChecked(! pattern->hasMinRepetition());
  if (pattern->hasMinRepetition())
    ui->minRepetition->setValue(pattern->minRepetition());
  ui->maxRepetitionUnset->setChecked(! pattern->hasMaxRepetition());
  if (pattern->hasMaxRepetition())
    ui->maxRepetition->setValue(pattern->maxRepetition());
  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
SparseRepeatDialog::accept() {
  Address addr = Address::fromString(ui->address->text());
  if (! addr.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid address format."),
                          tr("Invalid address format '%1'.").arg(ui->address->text()));
    return;
  }

  Offset step = Offset::fromString(ui->offset->text());
  if (! step.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid offset format."),
                          tr("Invalid offset format '%1'.").arg(ui->offset->text()));
    return;
  }

  _pattern->setAddress(addr);
  _pattern->setStep(step);
  if (! ui->minRepetitionUnset->isChecked())
    _pattern->setMinRepetition(ui->minRepetition->value());
  if (! ui->maxRepetitionUnset->isChecked())
    _pattern->setMaxRepetition(ui->maxRepetition->value());

  ui->metaEdit->apply();

  QDialog::accept();
}


SparseRepeatDialog::~SparseRepeatDialog()
{
  delete ui;
}
