#include "codeplugdialog.hh"
#include "ui_codeplugdialog.h"
#include "pattern.hh"


CodeplugDialog::CodeplugDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::CodeplugDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-codeplug").pixmap(QSize(64,64)));
}


CodeplugDialog::~CodeplugDialog() {
  delete ui;
}


void
CodeplugDialog::setPattern(CodeplugPattern *pattern) {
  _codeplug = pattern;
  ui->metaEdit->setPatternMeta(&pattern->meta());
}

void
CodeplugDialog::accept() {
  ui->metaEdit->apply();
  QDialog::accept();
}
