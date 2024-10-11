#include "unknownpatterndialog.hh"
#include "ui_unknownpatterndialog.h"
#include <QMessageBox>
#include "pattern.hh"


UnknownPatternDialog::UnknownPatternDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::UnknownPatternDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-unknown").pixmap(QSize(64,64)));
}

UnknownPatternDialog::~UnknownPatternDialog() {
  delete ui;
}

void
UnknownPatternDialog::setPattern(UnknownFieldPattern *pattern) {
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

  ui->size->setText(_pattern->size().toString());
  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
UnknownPatternDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  Size size = Size::fromString(ui->size->text().simplified());
  if (! size.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid size format."),
                          tr("Invalid size format '%1'.").arg(ui->address->text()));
  }
  _pattern->setWidth(size);

  ui->metaEdit->apply();

  QDialog::accept();
}

