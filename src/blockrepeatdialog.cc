#include "blockrepeatdialog.hh"
#include "ui_blockrepeatdialog.h"
#include "pattern.hh"
#include <QMessageBox>
#include "logger.hh"

BlockRepeatDialog::BlockRepeatDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::BlockRepeatDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-blockrepeat").pixmap(QSize(64,64)));
}

BlockRepeatDialog::~BlockRepeatDialog()
{
  delete ui;
}

void
BlockRepeatDialog::setPattern(BlockRepeatPattern *pattern, const CodeplugPattern *codeplug) {
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

  ui->minRepetitionUnset->setChecked(! pattern->hasMinRepetition());
  if (pattern->hasMinRepetition())
    ui->minRepetition->setValue(pattern->minRepetition());
  ui->maxRepetitionUnset->setChecked(! pattern->hasMaxRepetition());
  if (pattern->hasMaxRepetition())
    ui->maxRepetition->setValue(pattern->maxRepetition());
  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
BlockRepeatDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  if (! ui->minRepetitionUnset->isChecked())
    _pattern->setMinRepetition(ui->minRepetition->value());
  if (! ui->maxRepetitionUnset->isChecked())
    _pattern->setMaxRepetition(ui->maxRepetition->value());

  ui->metaEdit->apply();

  QDialog::accept();
}
