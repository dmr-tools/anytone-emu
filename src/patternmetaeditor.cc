#include "patternmetaeditor.hh"
#include "ui_patternmetaeditor.h"
#include "pattern.hh"


PatternMetaEditor::PatternMetaEditor(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PatternMetaEditor)
{
  ui->setupUi(this);
  ui->tag->setItemData(0, QVariant::fromValue(PatternMeta::Flags::None));
  ui->tag->setItemData(1, QVariant::fromValue(PatternMeta::Flags::Done));
  ui->tag->setItemData(2, QVariant::fromValue(PatternMeta::Flags::NeedsReview));
  ui->tag->setItemData(3, QVariant::fromValue(PatternMeta::Flags::Incomplete));
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
  ui->tag->setCurrentIndex(ui->tag->findData(QVariant::fromValue(_meta->flags())));
}

void
PatternMetaEditor::apply() {
  _meta->setName(ui->name->text().simplified());
  _meta->setShortName(ui->shortName->text().simplified());
  _meta->setDescription(ui->description->toPlainText().simplified());
  _meta->setFirmwareVersion(ui->version->text().simplified());
  _meta->setFlags(ui->tag->currentData().value<PatternMeta::Flags>());
}
