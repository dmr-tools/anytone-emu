#include "patternmetaeditor.hh"
#include "ui_patternmetaeditor.h"
#include "pattern.hh"


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
PatternMetaEditor::setPatternMeta(PatternMeta *meta, const CodeplugPattern *codeplug) {
  _meta = meta;
  ui->name->setText(meta->name());
  if (meta->hasShortName())
    ui->shortName->setText(meta->shortName());
  if (meta->hasDescription())
    ui->description->setText(meta->description());
  if (meta->firmwareVersion().isEmpty() && (nullptr != codeplug) && (! codeplug->meta().firmwareVersion().isEmpty()))
    ui->version->setText(codeplug->meta().firmwareVersion());
  else if (! meta->firmwareVersion().isEmpty())
    ui->version->setText(meta->firmwareVersion());
}

void
PatternMetaEditor::apply() {
  _meta->setName(ui->name->text().simplified());
  _meta->setShortName(ui->shortName->text().simplified());
  _meta->setDescription(ui->description->toPlainText().simplified());
  _meta->setFirmwareVersion(ui->version->text().simplified());
}
