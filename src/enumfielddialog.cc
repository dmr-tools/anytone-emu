#include "enumfielddialog.hh"
#include "ui_enumfielddialog.h"
#include "pattern.hh"
#include "enumfieldpatternwrapper.hh"
#include <QStyledItemDelegate>
#include <QStandardItemEditorCreator>
#include <QSettings>
#include <QMessageBox>
#include "logger.hh"


EnumFieldDialog::EnumFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::EnumFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-enum").pixmap(QSize(64,64)));
  setWindowIcon(QIcon::fromTheme("pattern-enum"));

  QSettings settings;
  if (settings.contains("layout/enumFieldDialogSize"))
    restoreGeometry(settings.value("layout/enumFieldDialogSize").toByteArray());

}

EnumFieldDialog::~EnumFieldDialog()
{
  delete ui;
}


void
EnumFieldDialog::done(int res) {
  QSettings settings;

  settings.setValue("layout/enumFieldDialogSize", saveGeometry());

  ui->items->done(res);

  QDialog::done(res);
}


void
EnumFieldDialog::setPattern(EnumFieldPattern *pattern, const CodeplugPattern *codeplug) {
  QSettings settings;

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

  ui->width->setValue(_pattern->size().bits());
  ui->items->setPattern(pattern, codeplug);

  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
EnumFieldDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  _pattern->setWidth(Offset::fromBits(ui->width->value()));

  ui->metaEdit->apply();

  QDialog::accept();
}

