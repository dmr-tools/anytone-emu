#include "newpatterndialog.hh"
#include "ui_newpatterndialog.h"
#include "pattern.hh"

enum class PatternType {
  None = 0,
  FixedPattern = 1,
  DensePattern = 3,
  SparsePattern = 7
};

inline bool operator|(PatternType left, PatternType right) {
  return (int) left | (int) right;
}


NewPatternDialog::NewPatternDialog(AbstractPattern *parentPattern, QWidget *parent)
  : QDialog(parent), ui(new Ui::NewPatternDialog)
{
  ui->setupUi(this);

  if (nullptr == parentPattern)
    return;

  PatternType pattern = PatternType::None;
  if (parentPattern->is<CodeplugPattern>()  ) {
    pattern = PatternType::SparsePattern;
  } else if (parentPattern->is<ElementPattern>()) {
    pattern = PatternType::FixedPattern;
  } else if (parentPattern->is<StructuredPattern>()) {
    pattern = (0 == parentPattern->as<StructuredPattern>()->numChildPattern())
        ? PatternType::DensePattern
        : PatternType::None;
  }

  if (pattern | PatternType::FixedPattern) {

  }
}

NewPatternDialog::~NewPatternDialog()
{
  delete ui;
}
