#include "sparserepeatdialog.hh"
#include "ui_sparserepeatdialog.h"
#include "pattern.hh"
#include <QMessageBox>


SparseRepeatDialog::SparseRepeatDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::SparseRepeatDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-sparserepeat").pixmap(QSize(64,64)));
  connect(ui->minRepetitionUnset, &QCheckBox::toggled,
          [this](bool checked) {ui->minRepetition->setEnabled(! checked);});
  connect(ui->maxRepetitionUnset, &QCheckBox::toggled,
          [this](bool checked) {ui->maxRepetition->setEnabled(! checked);});
}


void
SparseRepeatDialog::setPattern(RepeatPattern *pattern, const CodeplugPattern *codeplug) {
  _pattern = pattern;
  if (pattern->address().isValid())
    ui->address->setText(pattern->address().toString());

  if (pattern->step().isValid())
    ui->offset->setText(pattern->step().toString());

  ui->minRepetitionUnset->setChecked(! pattern->hasMinRepetition());
  ui->minRepetition->setEnabled(! ui->minRepetitionUnset->isChecked());
  if (pattern->hasMinRepetition())
    ui->minRepetition->setValue(pattern->minRepetition());

  ui->maxRepetitionUnset->setChecked(! pattern->hasMaxRepetition());
  ui->maxRepetition->setEnabled(! ui->maxRepetitionUnset->isChecked());
  if (pattern->hasMaxRepetition())
    ui->maxRepetition->setValue(pattern->maxRepetition());

  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
SparseRepeatDialog::accept() {
  Address addr = Address::fromString(ui->address->text());
  if (! addr.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid address format."),
                          tr("You must set a valid address '%1'.").arg(ui->address->text()));
    return;
  }
  _pattern->setAddress(addr);

  Offset step = Offset::fromString(ui->offset->text());
  if (! step.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid offset format."),
                          tr("You must set a valid offset '%1'.").arg(ui->offset->text()));
    return;
  }
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
