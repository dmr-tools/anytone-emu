#include "integerfielddialog.hh"
#include "ui_integerfielddialog.h"
#include "codeplugpattern.hh"
#include <QMessageBox>


IntegerFieldDialog::IntegerFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::IntegerFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
}

void
IntegerFieldDialog::setPattern(IntegerFieldPattern *pattern) {
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

  ui->format->setItemData(0, QVariant::fromValue(IntegerFieldPattern::Format::Unsigned));
  ui->format->setItemData(1, QVariant::fromValue(IntegerFieldPattern::Format::Signed));
  ui->format->setItemData(2, QVariant::fromValue(IntegerFieldPattern::Format::BCD));
  switch (_pattern->format()) {
  case IntegerFieldPattern::Format::Unsigned: ui->format->setCurrentIndex(0); break;
  case IntegerFieldPattern::Format::Signed: ui->format->setCurrentIndex(1); break;
  case IntegerFieldPattern::Format::BCD: ui->format->setCurrentIndex(2); break;
  }

  ui->endian->setItemData(0, QVariant::fromValue(IntegerFieldPattern::Endian::Little));
  ui->endian->setItemData(1, QVariant::fromValue(IntegerFieldPattern::Endian::Big));
  switch (_pattern->endian()) {
  case IntegerFieldPattern::Endian::Little: ui->endian->setCurrentIndex(0); break;
  case IntegerFieldPattern::Endian::Big: ui->endian->setCurrentIndex(1); break;
  }

  if (_pattern->hasDefaultValue()) {
    ui->defaultValueSet->setChecked(true);
    ui->defaultValue->setText(QString("%1h").arg(_pattern->defaultValue(), 0, 16));
  } else {
    ui->defaultValueSet->setChecked(false);
    ui->defaultValue->setText(QString("%1h").arg(_pattern->defaultValue(), 0, 16));
    ui->defaultValue->setEnabled(false);
  }

  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
IntegerFieldDialog::accept() {
  if (! _pattern->hasAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
  _pattern->setAddress(addr);
  }

  _pattern->setWidth(Offset::fromBits(ui->width->value()));
  _pattern->setFormat(ui->format->currentData().value<IntegerFieldPattern::Format>());
  _pattern->setEndian(ui->endian->currentData().value<IntegerFieldPattern::Endian>());
  if (ui->defaultValueSet->isChecked()) {
    _pattern->setDefaultValue(ui->defaultValue->text().toUInt(nullptr, 16));
  } else {
    _pattern->clearDefaultValue();
  }

  ui->metaEdit->apply();

  QDialog::accept();
}


IntegerFieldDialog::~IntegerFieldDialog()
{
  delete ui;
}
