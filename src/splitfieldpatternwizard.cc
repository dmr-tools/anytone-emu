#include "splitfieldpatternwizard.hh"
#include "pattern.hh"
#include "ui_splitfieldpatternwizard.h"

SplitFieldPatternWizard::SplitFieldPatternWizard(FieldPattern *field, QWidget *parent) :
  QWizard(parent), ui(new Ui::SplitFieldPatternWizard), _field(field)
{
  ui->setupUi(this);
  ui->addressRange->setText(tr("Address range %1 - %2")
                            .arg(_field->address().toString())
                            .arg((_field->address()+_field->size()).toString()));
}

SplitFieldPatternWizard::~SplitFieldPatternWizard() {
  delete ui;
}


