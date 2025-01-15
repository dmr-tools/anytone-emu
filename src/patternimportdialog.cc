#include "patternimportdialog.hh"
#include "ui_patternimportdialog.h"
#include "patternwrapper.hh"
#include "pattern.hh"



PatternImportDialog::PatternImportDialog(const QString &catalog, QWidget *parent) :
  QDialog(parent), ui(new Ui::PatternImportDialog), _catalog()
{
  ui->setupUi(this);
  if (! _catalog.load(catalog)) {
    ui->modelSelectionBox->setEnabled(false);
    return;
  }

  for (auto model: _catalog) {
    ui->modelSelectionBox->addItem(
          QString("%1 (%2)").arg(model->name()).arg(model->manufacturer()),
          QVariant(model->id()));
  }

  connect(ui->modelSelectionBox, &QComboBox::currentIndexChanged,
          this, &PatternImportDialog::onModelSelected);

  connect(ui->firmwareSelectionBox, &QComboBox::currentIndexChanged,
          this, &PatternImportDialog::onFirmwareSelected);

  if (ui->modelSelectionBox->count())
    ui->modelSelectionBox->setCurrentIndex(0);
}

PatternImportDialog::~PatternImportDialog() {
  delete ui;
}


AbstractPattern *
PatternImportDialog::copy() {
  auto indices = ui->elementSelection->selectionModel()->selectedIndexes();
  if (0 == indices.count())
    return nullptr;

  auto pattern = (AbstractPattern *)indices.first().internalPointer();
  if (nullptr == pattern)
    return nullptr;

  return markNeedsReview(pattern->clone());
}


void
PatternImportDialog::onModelSelected(int index) {
  ui->firmwareSelectionBox->clear();
  ui->firmwareSelectionBox->setEnabled(false);
  ui->elementSelection->setModel(nullptr);
  ui->elementSelection->setEnabled(false);

  ModelDefinition *model = _catalog.model(ui->modelSelectionBox->currentData().toString());
  if (nullptr == model)
    return;

  if (model->latestFirmware())
    ui->firmwareSelectionBox->addItem("latest", QVariant::fromValue(model->latestFirmware()));

  for (auto firmware: *model) {
    ui->firmwareSelectionBox->addItem(firmware->name(), QVariant::fromValue(firmware));
  }

  if (ui->firmwareSelectionBox->count()) {
    ui->firmwareSelectionBox->setEnabled(true);
    ui->firmwareSelectionBox->setCurrentIndex(0);
  }
}


void
PatternImportDialog::onFirmwareSelected(int index) {
  QAbstractItemModel *mod = ui->elementSelection->model();
  ui->elementSelection->setModel(nullptr);
  ui->elementSelection->setEnabled(false);
  delete mod;

  ModelFirmwareDefinition *firmware = ui->firmwareSelectionBox->currentData()
      .value<ModelFirmwareDefinition *>();
  if (nullptr == firmware)
    return;

  CodeplugPattern *cp = CodeplugPattern::load(firmware->codeplug());
  if (nullptr == cp)
    return;

  auto wrapper = new PatternWrapper(cp);
  cp->setParent(wrapper);
  ui->elementSelection->setModel(wrapper);
  ui->elementSelection->setEnabled(true);
}


AbstractPattern *
PatternImportDialog::markNeedsReview(AbstractPattern *pattern) {
  pattern->meta().setFlags(PatternMeta::Flags::NeedsReview);
  pattern->meta().setFirmwareVersion("");
  if (pattern->is<StructuredPattern>()) {
    auto strpat = pattern->as<StructuredPattern>();
    for (unsigned int i=0; i<strpat->numChildPattern(); i++)
      markNeedsReview(strpat->childPattern(i));
  }

  return pattern;
}

