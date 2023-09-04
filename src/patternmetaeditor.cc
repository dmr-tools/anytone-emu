#include "patternmetaeditor.hh"
#include "ui_patternmetaeditor.h"
#include "codeplugpattern.hh"


PatternMetaEditor::PatternMetaEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PatternMetaEditor)
{
  ui->setupUi(this);
}

PatternMetaEditor::~PatternMetaEditor()
{
  delete ui;
}

void
PatternMetaEditor::setPatternMeta(PatternMeta *meta) {
  _meta = meta;
  ui->name->setText(meta->name());
  ui->description->setText(meta->description());
  ui->version->setText(meta->firmwareVersion());
}

void
PatternMetaEditor::apply() {
  _meta->setName(ui->name->text().simplified());
  _meta->setDescription(ui->description->toPlainText().simplified());
  _meta->setFirmwareVersion(ui->version->text().simplified());
}
