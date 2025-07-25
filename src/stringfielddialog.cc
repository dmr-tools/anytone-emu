#include "stringfielddialog.hh"
#include "ui_stringfielddialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>



StringFieldDialog::StringFieldDialog(QWidget *parent) :
  QDialog(parent),  ui(new Ui::StringFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-stringfield").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-stringfield"));

  QSettings settings;
  if (settings.contains("layout/stringFieldDialogSize"))
    restoreGeometry(settings.value("layout/stringFieldDialogSize").toByteArray());
}


StringFieldDialog::~StringFieldDialog() {
  delete ui;
}


void
StringFieldDialog::done(int res) {
  QSettings().setValue("layout/stringFieldDialogSize", saveGeometry());
  QDialog::done(res);
}


void
StringFieldDialog::setPattern(StringFieldPattern *pattern, const CodeplugPattern *codeplug) {
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

  ui->numChars->setValue(_pattern->numChars());
  ui->format->setItemData(0, QVariant::fromValue(StringFieldPattern::Format::ASCII));
  ui->format->setItemData(1, QVariant::fromValue(StringFieldPattern::Format::Unicode));
  switch (_pattern->format()) {
  case StringFieldPattern::Format::ASCII: ui->format->setCurrentIndex(0); break;
  case StringFieldPattern::Format::Unicode: ui->format->setCurrentIndex(1); break;
  }
  ui->padValue->setText(QString::number(_pattern->padValue(), 16));

  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
StringFieldDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  _pattern->setFormat(ui->format->currentData().value<StringFieldPattern::Format>());
  _pattern->setNumChars(ui->numChars->value());
  _pattern->setPadValue(ui->padValue->text().simplified().toUInt(nullptr, 16));

  ui->metaEdit->apply();

  QDialog::accept();
}

