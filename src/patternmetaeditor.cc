#include "patternmetaeditor.hh"
#include "ui_patternmetaeditor.h"

PatternMetaEditor::PatternMetaEditor(QWidget *parent)
  : QTabWidget(parent), ui(new Ui::metaEditor), _meta(nullptr)
{
  ui->setupUi(this);
  ui->flags->addItem(tr("None"), QVariant::fromValue(PatternMeta::Flags::None));
  ui->flags->addItem(tr("Done"), QVariant::fromValue(PatternMeta::Flags::Done));
  ui->flags->addItem(tr("Needs review"), QVariant::fromValue(PatternMeta::Flags::NeedsReview));
  ui->flags->addItem(tr("Incomplete"), QVariant::fromValue(PatternMeta::Flags::Incomplete));
}

PatternMetaEditor::~PatternMetaEditor() {
  delete ui;
}


void
PatternMetaEditor::setPatternMeta(PatternMeta *meta, const CodeplugPattern *codeplug){
  _meta = meta;

  ui->name->setText(meta->name());

  if (meta->hasFirmwareVersion())
    ui->version->setText(meta->firmwareVersion());
  else
    ui->version->clear();

  if (meta->hasBriefDescription())
    ui->briefDescription->setText(meta->briefDescription());
  else
    ui->briefDescription->clear();

  switch (meta->flags()) {
  case PatternMeta::Flags::None: ui->flags->setCurrentIndex(0); break;
  case PatternMeta::Flags::Done: ui->flags->setCurrentIndex(1); break;
  case PatternMeta::Flags::NeedsReview: ui->flags->setCurrentIndex(2); break;
  case PatternMeta::Flags::Incomplete: ui->flags->setCurrentIndex(3); break;
  }

  if (meta->hasDescription())
    ui->description->setText(meta->description());
  else
    ui->description->clear();
}


void
PatternMetaEditor::apply() {
  _meta->setName(ui->name->text().simplified());
  _meta->setFirmwareVersion(ui->version->text().simplified());
  _meta->setBriefDescription(ui->briefDescription->toPlainText().simplified());
  _meta->setFlags(ui->flags->currentData().value<PatternMeta::Flags>());
  _meta->setDescription(ui->description->toPlainText().simplified());
}
