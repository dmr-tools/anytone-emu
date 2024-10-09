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

  connect(ui->category, &QComboBox::activated, ui->stack, &QStackedWidget::setCurrentIndex);

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
    ui->fieldPattern->addItem(tr("ASCII string"), "ascii");
    ui->fieldPattern->addItem(tr("unicode string"), "uc16");
    ui->fieldPattern->addItem(tr("enumeration"), "enum");
    ui->fieldPattern->addItem(tr("unknown"), "unknown");
    ui->fieldPattern->addItem(tr("unused"), "unused");

    ui->integerPattern->addItem(tr("boolean"), "bool");
    ui->integerPattern->addItem(tr("unsigend integer"), "uint");
    ui->integerPattern->addItem(tr("signed integer"), "int");
    ui->integerPattern->addItem(tr("8bit unsigned integer"), "uint8");
    ui->integerPattern->addItem(tr("8bit signed integer"), "int8");
    ui->integerPattern->addItem(tr("16bit unsigned integer little-endian"), "uint16le");
    ui->integerPattern->addItem(tr("16bit signed integer little-endian"), "int16le");
    ui->integerPattern->addItem(tr("16bit unsigned integer big-endian"), "uint16be");
    ui->integerPattern->addItem(tr("16bit signed integer big-endian"), "int16be");
    ui->integerPattern->addItem(tr("32bit unsigned integer little-endian"), "uint32le");
    ui->integerPattern->addItem(tr("32bit signed integer little-endian"), "int32le");
    ui->integerPattern->addItem(tr("32bit unsigned integer big-endian"), "uint32be");
    ui->integerPattern->addItem(tr("32bit signed integer big-endian"), "int32be");

    ui->structurePattern->addItem(tr("element"), "element");
    ui->structurePattern->addItem(tr("fixed repeat"), "fixed repeat");
  }

  if (pattern | PatternType::DensePattern) {
    ui->structurePattern->addItem(tr("block repeat"), "block repeat");
  }

  if (pattern | PatternType::SparsePattern) {
    ui->structurePattern->addItem(tr("repeat"), "repeat");
  }
}

NewPatternDialog::~NewPatternDialog()
{
  delete ui;
}
