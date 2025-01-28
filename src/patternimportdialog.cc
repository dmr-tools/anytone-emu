#include "patternimportdialog.hh"
#include "ui_patternimportdialog.h"
#include "patternwrapper.hh"
#include "pattern.hh"

#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>



PatternImportDialog::PatternImportDialog(const QString &catalog, QWidget *parent) :
  QDialog(parent), ui(new Ui::PatternImportDialog), _catalog()
{
  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("document-import"));

  if (! _catalog.load(catalog)) {
    ui->modelSelectionBox->setEnabled(false);
    return;
  }

  QSettings settings;
  if (settings.contains("layout/patternImportDialogSize"))
    restoreGeometry(settings.value("layout/patternImportDialogSize").toByteArray());

  for (auto model: _catalog) {
    ui->modelSelectionBox->addItem(
          QString("%1 (%2)").arg(model->name()).arg(model->manufacturer()),
          QVariant(model->id()));
  }

  connect(ui->modelSelectionBox, &QComboBox::currentIndexChanged,
          this, &PatternImportDialog::onModelSelected);

  connect(ui->firmwareSelectionBox, &QComboBox::currentIndexChanged,
          this, &PatternImportDialog::onFirmwareSelected);

  if (ui->modelSelectionBox->count()) {
    ui->modelSelectionBox->setCurrentIndex(0);
    onModelSelected(0);
  }
}


PatternImportDialog::~PatternImportDialog() {
  delete ui;
}


void
PatternImportDialog::closeEvent(QCloseEvent *event) {
  QDialog::closeEvent(event);
}


void
PatternImportDialog::done(int res) {
  QSettings settings;
  settings.setValue("layout/patternImportDialogSize", saveGeometry());
  if (ui->elementSelection->isEnabled())
    settings.setValue("layout/patternImportDialogHeaderState",
                      ui->elementSelection->header()->saveState());
  QDialog::done(res);
}


void
PatternImportDialog::accept() {
  if (nullptr == ui->elementSelection->model()) {
    QMessageBox::critical(nullptr, tr("Cannot import pattern."),
                          tr("Please select a model and firmware version first."));
    return;
  }

  if (ui->elementSelection->selectionModel()->selectedIndexes().isEmpty()) {
    QMessageBox::critical(nullptr, tr("Cannot import pattern."),
                          tr("Please select an element first"));
    return;
  }

  QDialog::accept();
}


AbstractPattern *
PatternImportDialog::copy() {
  if (nullptr == ui->elementSelection->model())
    return nullptr;

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
  QSettings settings;
  if (ui->elementSelection->isEnabled())
    settings.setValue("layout/patternImportDialogHeaderState",
                      ui->elementSelection->header()->saveState());

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
  if (settings.contains("layout/patternImportDialogHeaderState"))
    ui->elementSelection->header()->restoreState(
          settings.value("layout/patternImportDialogHeaderState").toByteArray());
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

