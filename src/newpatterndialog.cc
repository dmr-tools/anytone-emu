#include "newpatterndialog.hh"
#include "ui_newpatterndialog.h"
#include "pattern.hh"

#include <QMessageBox>
#include <QSettings>



NewPatternDialog::NewPatternDialog(AbstractPattern *parentPattern, const Address &addr, QWidget *parent)
  : QDialog(parent), ui(new Ui::NewPatternDialog)
{
  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("document-new"));

  QSettings settings;
  if (settings.contains("layout/newPatternDialog"))
    restoreGeometry(settings.value("layout/newPatternDialog").toByteArray());

  if (nullptr == parentPattern)
    return;

  if (addr.isValid())
    ui->address->setText(addr.toString());
  ui->address->setEnabled(parentPattern->is<CodeplugPattern>());

  int pattern = (int)PatternSelectionWidget::PatternType::None;
  if (parentPattern->is<CodeplugPattern>()  ) {
    pattern = PatternSelectionWidget::PatternType::FixedPattern
        | PatternSelectionWidget::PatternType::DensePattern
        | PatternSelectionWidget::PatternType::SparsePattern;
  } else if (parentPattern->is<ElementPattern>()) {
    pattern = (int)PatternSelectionWidget::PatternType::FixedPattern;
  } else if (parentPattern->is<StructuredPattern>()) {
    pattern = (0 == parentPattern->as<StructuredPattern>()->numChildPattern())
        ? (PatternSelectionWidget::PatternType::FixedPattern
           | PatternSelectionWidget::PatternType::DensePattern)
        : (int) PatternSelectionWidget::PatternType::None;
  }
  ui->patternSelection->setPatternTypes(pattern);
  ui->patternSelection->headLabel()->setText(tr("Select new pattern type:"));
}

NewPatternDialog::~NewPatternDialog() {
  delete ui;
}


void
NewPatternDialog::done(int res) {
  QSettings().setValue("layout/newPatternDialog", saveGeometry());
  QDialog::done(res);
}


AbstractPattern *
NewPatternDialog::create() const {
  AbstractPattern *pattern = ui->patternSelection->createPattern();

  if (ui->address->isEnabled()) {
    pattern->setAddress(Address::fromString(ui->address->text().simplified()));
  }

  return pattern;
}

void
NewPatternDialog::accept() {
  auto addr = Address::fromString(ui->address->text().simplified());
  if (ui->address->isEnabled() && (! addr.isValid())) {
    QMessageBox::information(
          nullptr, "Invalid address", "You have to set a valid address for the pattern.");
    return;
  }

  QDialog::accept();
}

