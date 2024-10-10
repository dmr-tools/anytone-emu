#include "newpatterndialog.hh"
#include "ui_newpatterndialog.h"
#include "pattern.hh"


NewPatternDialog::NewPatternDialog(AbstractPattern *parentPattern, const Address &addr, QWidget *parent)
  : QDialog(parent), ui(new Ui::NewPatternDialog)
{
  ui->setupUi(this);

  if (nullptr == parentPattern)
    return;

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

NewPatternDialog::~NewPatternDialog()
{
  delete ui;
}


AbstractPattern *
NewPatternDialog::create() const {
  AbstractPattern *pattern = ui->patternSelection->createPattern();

  if (ui->address->isEnabled()) {
    pattern->setAddress(Address::fromString(ui->address->text().simplified()));
  }

  return pattern;
}

