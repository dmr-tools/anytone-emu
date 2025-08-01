#include "integerfielddialog.hh"
#include "ui_integerfielddialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>



IntegerFieldDialog::IntegerFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::IntegerFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-integer").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-integer"));

  QSettings settings;
  if (settings.contains("layout/integerFieldDialogSize"))
    restoreGeometry(settings.value("layout/integerFieldDialogSize").toByteArray());
}


void
IntegerFieldDialog::done(int res) {
  QSettings().setValue("layout/integerFieldDialogSize", saveGeometry());
  QDialog::done(res);
}


void
IntegerFieldDialog::setPattern(IntegerFieldPattern *pattern, const CodeplugPattern *codeplug) {
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

  if (_pattern->hasSize())
    ui->width->setValue(_pattern->size().bits());
  else
    ui->width->clear();

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

  if (_pattern->hasDefaultValue())
    ui->defaultValue->setText(QString("%1h").arg(_pattern->defaultValue(), 0, 16));
  else
    ui->defaultValue->clear();

  if (_pattern->hasMinValue())
    ui->minValue->setText(QString("%1h").arg(_pattern->minValue(), 0, 16));
  else
    ui->minValue->clear();

  if (_pattern->hasMaxValue())
    ui->maxValue->setText(QString("%1h").arg(_pattern->maxValue(), 0, 16));
  else
    ui->maxValue->clear();

  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
IntegerFieldDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  if (! ui->width->text().simplified().isEmpty())
    _pattern->setWidth(Offset::fromBits(ui->width->value()));

  _pattern->setFormat(ui->format->currentData().value<IntegerFieldPattern::Format>());
  _pattern->setEndian(ui->endian->currentData().value<IntegerFieldPattern::Endian>());

  if (! ui->defaultValue->text().simplified().isEmpty()) {
    _pattern->setDefaultValue(ui->defaultValue->text().toUInt(nullptr, 16));
  } else {
    _pattern->clearDefaultValue();
  }

  if (! ui->minValue->text().simplified().isEmpty()) {
    _pattern->setMinValue(ui->minValue->text().toUInt(nullptr, 16));
  } else {
    _pattern->clearMinValue();
  }

  if (! ui->maxValue->text().simplified().isEmpty()) {
    _pattern->setMaxValue(ui->maxValue->text().toUInt(nullptr, 16));
  } else {
    _pattern->clearMaxValue();
  }

  ui->metaEdit->apply();

  QDialog::accept();
}


IntegerFieldDialog::~IntegerFieldDialog()
{
  delete ui;
}
