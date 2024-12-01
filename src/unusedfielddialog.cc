#include "unusedfielddialog.hh"
#include "ui_unusedfielddialog.h"
#include "pattern.hh"
#include <QMessageBox>


UnusedFieldDialog::UnusedFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::UnusedFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-unused").pixmap(QSize(64,64)));
}

UnusedFieldDialog::~UnusedFieldDialog()
{
  delete ui;
}


void
UnusedFieldDialog::setPattern(UnusedFieldPattern *pattern, const CodeplugPattern *codeplug) {
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

  if (_pattern->size().isValid())
    ui->size->setText(_pattern->size().toString());
  else
    ui->size->clear();

  ui->value->setText(_pattern->content().toHex());

  ui->metaEdit->setPatternMeta(&pattern->meta(), codeplug);
}


void
UnusedFieldDialog::accept() {
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

  _pattern->setContent(QByteArray::fromHex(ui->value->text().toLatin1()));

  ui->metaEdit->apply();

  QDialog::accept();
}
