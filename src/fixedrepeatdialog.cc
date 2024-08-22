#include "fixedrepeatdialog.hh"
#include "ui_fixedrepeatdialog.h"
#include "pattern.hh"
#include <QMessageBox>


FixedRepeatDialog::FixedRepeatDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::FixedRepeatDialog), _pattern(nullptr)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("pattern-fixedrepeat").pixmap(QSize(64,64)));
}

FixedRepeatDialog::~FixedRepeatDialog()
{
  delete ui;
}

void
FixedRepeatDialog::setPattern(FixedRepeatPattern *pattern) {
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

  ui->repetitions->setValue(pattern->repetition());
  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
FixedRepeatDialog::accept() {
  if (! _pattern->hasImplicitAddress()) {
    Address addr = Address::fromString(ui->address->text());
    if (! addr.isValid()) {
      QMessageBox::critical(nullptr, tr("Invalid address format."),
                            tr("Invalid address format '%1'.").arg(ui->address->text()));
      return;
    }
    _pattern->setAddress(addr);
  }

  _pattern->setRepetition(ui->repetitions->value());

  ui->metaEdit->apply();

  QDialog::accept();
}
