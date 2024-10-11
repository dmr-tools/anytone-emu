#include "patternselectionwidget.hh"
#include "ui_patternselectionwidget.h"
#include "pattern.hh"


PatternSelectionWidget::PatternSelectionWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PatternSelectionWidget)
{
  ui->setupUi(this);

  ui->patternType->setCurrentIndex(0); ui->stack->setCurrentIndex(0);
  connect(ui->patternType, &QComboBox::activated, ui->stack, &QStackedWidget::setCurrentIndex);
  setPatternTypes((int)PatternType::None);
}

PatternSelectionWidget::~PatternSelectionWidget() {
  delete ui;
}


QLabel *
PatternSelectionWidget::headLabel() const {
  return ui->headLabel;
}

int
PatternSelectionWidget::patternTypes() const {
  return _patternTypes;
}

void
PatternSelectionWidget::setPatternTypes(int mask) {
  ui->fieldPattern->clear();
  ui->integerPattern->clear();
  ui->structuredPattern->clear();

  _patternTypes = mask;
  if (_patternTypes | PatternType::FixedPattern) {
    ui->fieldPattern->addItem(tr("ASCII string"), "ascii");
    ui->fieldPattern->addItem(tr("unicode string"), "uc16");
    ui->fieldPattern->addItem(tr("enumeration"), "enum");
    ui->fieldPattern->addItem(tr("unknown"), "unknown");
    ui->fieldPattern->addItem(tr("unused"), "unused");

    ui->integerPattern->addItem(tr("boolean"), "uint1");
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

    ui->structuredPattern->addItem(tr("element"), "element");
    ui->structuredPattern->addItem(tr("fixed repeat"), "fixed repeat");
  }

  if (_patternTypes & PatternType::DensePattern) {
    ui->structuredPattern->addItem(tr("block repeat"), "block repeat");
  }

  if (_patternTypes & PatternType::SparsePattern) {
    ui->structuredPattern->addItem(tr("repeat"), "repeat");
  }
}

AbstractPattern *
PatternSelectionWidget::createPattern() const {
  // Dispatch by type
  QString type;
  if (0 == ui->patternType->currentIndex()) {
    type = ui->fieldPattern->currentData().toString();
  } else if (1 == ui->patternType->currentIndex()) {
    type = ui->integerPattern->currentData().toString();
  } else if (2 == ui->patternType->currentIndex()) {
    type = ui->structuredPattern->currentData().toString();
  }

  if ("ascii" == type) {
    auto pattern = new StringFieldPattern();
    pattern->setFormat(StringFieldPattern::Format::ASCII);
    return pattern;
  }
  if ("uc16" == type) {
    auto pattern = new StringFieldPattern();
    pattern->setFormat(StringFieldPattern::Format::Unicode);
    return pattern;
  }
  if ("enum" == type) {
    return new EnumFieldPattern();
  }
  if ("unknown" == type) {
    return new UnknownFieldPattern();
  }
  if ("unused" == type) {
    return new UnusedFieldPattern();
  }

  if ("element" == type) {
    return new ElementPattern();
  }

  if ("fixed repeat" == type) {
    return new FixedRepeatPattern();
  }

  if ("block repeat" == type) {
    return new BlockRepeatPattern();
  }

  if ("repeat" == type) {
    return new RepeatPattern();
  }

  QRegularExpression regexp("(u|)int([0-9]{1,3})(le|be|)");
  QRegularExpressionMatch match = regexp.match(type);
  if (match.isValid()) {
    auto pattern = new IntegerFieldPattern();
    pattern->setFormat(("u" == match.captured(1))
                       ? IntegerFieldPattern::Format::Unsigned
                       : IntegerFieldPattern::Format::Signed);
    pattern->setWidth(Offset::fromBits(match.captured(2).toUInt()));
    pattern->setEndian( ("be" == match.captured(1))
                        ? IntegerFieldPattern::Endian::Big
                        : IntegerFieldPattern::Endian::Little );
    return pattern;
  }

  return nullptr;

}
