#include "unknownpatterndialog.hh"
#include "ui_unknownpatterndialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>



UnknownPatternDialog::UnknownPatternDialog(QWidget *parent)
  : QDialog(parent), ui(new Ui::UnknownPatternDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-unknown").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-unknown"));

  QSettings settings;
  if (settings.contains("layout/unknownSettingsDialogSize"))
    restoreGeometry(settings.value("layout/unknownSettingsDialogSize").toByteArray());
}

UnknownPatternDialog::~UnknownPatternDialog() {
  delete ui;
}


void
UnknownPatternDialog::done(int res) {
  QSettings().setValue("layout/unknownSettingsDialogSize", saveGeometry());
  QDialog::done(res);
}


void
UnknownPatternDialog::setPattern(UnknownFieldPattern *pattern, const CodeplugPattern *codeplug) {
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

  if (_pattern->size().isValid())
    ui->size->setText(_pattern->size().toString());
  else
    ui->size->clear();
  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
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

