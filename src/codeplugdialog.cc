#include "codeplugdialog.hh"
#include "ui_codeplugdialog.h"
#include "pattern.hh"

#include <QSettings>


CodeplugDialog::CodeplugDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::CodeplugDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-codeplug").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-codeplug"));

  QSettings settings;
  if (settings.contains("layout/codeplugDialogSize"))
    restoreGeometry(settings.value("layout/codeplugDialogSize").toByteArray());
}


CodeplugDialog::~CodeplugDialog() {
  delete ui;
}


void
CodeplugDialog::done(int res) {
  QSettings().setValue("layout/codeplugDialogSize", saveGeometry());
  QDialog::done(res);
}

void
CodeplugDialog::setPattern(CodeplugPattern *pattern) {
  _codeplug = pattern;
  ui->metaEdit->setPatternMeta(&pattern->meta(), pattern);
}

void
CodeplugDialog::accept() {
  ui->metaEdit->apply();
  QDialog::accept();
}
