#include "setupdialog.hh"
#include "ui_setupdialog.h"
#include "pseudoterminal.hh"
#include <QSerialPort>
#include <QSettings>
#include <QSerialPortInfo>
#include <QFileDialog>
#include "logger.hh"


SetupDialog::SetupDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SetupDialog)
{
  QSettings settings;

  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("application-anytone-emu"));
  ui->iconLabel->setPixmap(QIcon::fromTheme("settings-interface").pixmap(QSize(64,64)));

  if (settings.contains("layout/setupDialogSize"))
    restoreGeometry(settings.value("layout/setupDialogSize").toByteArray());

  if (settings.contains("catalogFile"))
    ui->catalogFile->setText(settings.value("catalogFile").toString());
  if (settings.contains("useBuildinPatterns"))
    ui->useBuildin->setChecked(settings.value("useBuildinPatterns").toBool());
  onUseBuildinPatternToggled(ui->useBuildin->isChecked());
  connect(ui->useBuildin, &QCheckBox::toggled, this, &SetupDialog::onUseBuildinPatternToggled);
  connect(ui->selectCatalogFile, &QPushButton::clicked, this, &SetupDialog::onSelectCatalogFile);

  if (ui->useBuildin->isChecked()) {
    _catalog.clear();
    _catalog.load(":/codeplugs/catalog.xml");
  } else {
    _catalog.clear();
    _catalog.load(ui->catalogFile->text().simplified());
  }

  if (ui->deviceSelection->count()) {
    if (settings.contains("device") &&
        (-1 != ui->deviceSelection->findData(settings.value("device"))) )
      ui->deviceSelection->setCurrentIndex(
            ui->deviceSelection->findData(settings.value("device")));
    else
      ui->deviceSelection->setCurrentIndex(0);
    onDeviceSelected(ui->deviceSelection->currentIndex());
  }
  connect(ui->deviceSelection, &QComboBox::currentIndexChanged, this, &SetupDialog::onDeviceSelected);
  connect(ui->firmwareSelection, &QComboBox::currentIndexChanged, this, &SetupDialog::onFirmwareSelected);

  ui->interfaceSelection->addItem("Pseudo Terminal", QVariant::fromValue(Interface::PTY));
  ui->interfaceSelection->addItem("Serial Port", QVariant::fromValue(Interface::Serial));
  connect(ui->interfaceSelection, &QComboBox::currentIndexChanged, this, &SetupDialog::onInterfaceSelected);
  ui->interfaceSelection->setCurrentIndex(0);

  foreach (QSerialPortInfo info, QSerialPortInfo::availablePorts()) {
    ui->portSelection->addItem(info.portName() + " " + info.description(), info.portName());
  }
  ui->symlinkPath->setText(settings.value("symlinkPath",
                                          "~/.local/share/anytone-emu/anytoneport").toString());
  ui->interfaceSettings->setCurrentIndex(0);

  if (settings.contains("interface")) {
    int idx = ui->interfaceSelection->findData(settings.value("interface"));
    if (idx >= 0)
      ui->interfaceSelection->setCurrentIndex(idx);
  }

  if (settings.contains("serialPort")) {
    int idx = ui->portSelection->findData(settings.value("serialPort").toString());
    if (idx >= 0)
      ui->portSelection->setCurrentIndex(idx);
  }

}


SetupDialog::~SetupDialog()
{
  delete ui;
}


void
SetupDialog::done(int res) {
  QSettings().setValue("layout/setupDialogSize", saveGeometry());
  QDialog::done(res);
}


QString
SetupDialog::catalog() const {
  if (ui->useBuildin->isChecked()) {
    return ":/codeplugs/catalog.xml";
  }
  return ui->catalogFile->text().simplified();
}


Device *
SetupDialog::createDevice(const ErrorStack &err) {

  QIODevice *interface = nullptr;
  switch (ui->interfaceSelection->currentData().value<Interface>()) {
  case Interface::PTY:
    interface = new PseudoTerminal(ui->symlinkPath->text().simplified());
  break;
  case Interface::Serial:
    interface = new QSerialPort(ui->portSelection->currentData().toString());
  break;
  }

  ModelDefinition *model = _catalog.model(ui->deviceSelection->currentData().toString());
  if (nullptr == model) {
    delete interface;
    return nullptr;
  }

  ModelFirmwareDefinition *firmware = model->firmwares().at(ui->firmwareSelection->currentIndex());
  if (nullptr == firmware) {
    delete interface;
    return nullptr;
  }

  Device *dev = firmware->createDevice(interface, err);
  if (nullptr == dev) {
    delete interface;
    return nullptr;
  }

  return dev;
}


void
SetupDialog::onUseBuildinPatternToggled(bool enabled) {
  ui->catalogFile->setEnabled(! enabled);
  ui->selectCatalogFile->setEnabled(! enabled);
  QSettings().setValue(
        "useBuildinPatterns", ui->useBuildin->isChecked());

  reloadModels();
}

void
SetupDialog::reloadModels() {
  ui->firmwareSelection->clear();
  ui->deviceSelection->clear();

  if (! ui->catalogFile->isEnabled()) {
    _catalog.clear();
    _catalog.load(":/codeplugs/catalog.xml");
  } else {
    _catalog.clear();
    _catalog.load(ui->catalogFile->text().simplified());
  }

  for (ModelCatalog::const_iterator model=_catalog.begin(); model!=_catalog.end(); model++) {
    ui->deviceSelection->addItem((*model)->name(), (*model)->id());
  }
}


void
SetupDialog::onSelectCatalogFile() {
  QString path = QFileDialog::getOpenFileName(
        nullptr, tr("Select catalog file."), ui->catalogFile->text());
  QFileInfo file(path);
  if (file.isReadable()) {
    ui->catalogFile->setText(file.absoluteFilePath());
    QSettings().setValue("catalogFile", file.absoluteFilePath());
  }

  reloadModels();
}


void
SetupDialog::onDeviceSelected(int idx) {
  ui->firmwareSelection->clear();
  QString modelId = ui->deviceSelection->currentData().toString();
  if (! _catalog.hasModel(modelId))
    return;

  QSettings settings;
  settings.setValue("device", modelId);
  ModelDefinition *model = _catalog.model(modelId);
  for (ModelDefinition::const_iterator firmware=model->begin(); firmware!=model->end(); firmware++) {
    int idx = ui->firmwareSelection->count();
    ui->firmwareSelection->addItem((*firmware)->name(), (*firmware)->name());
    if (settings.contains("firmware")
        && settings.value("firmware").toString() == (*firmware)->name())
      ui->firmwareSelection->setCurrentIndex(idx);
  }
}

void
SetupDialog::onFirmwareSelected(int idx) {
  QSettings().setValue("firmware", ui->firmwareSelection->currentData());
}


void
SetupDialog::onInterfaceSelected(int idx) {
  QSettings().setValue("interface", ui->interfaceSelection->currentData());
  if (Interface::PTY == ui->interfaceSelection->currentData().value<Interface>()) {
#ifdef Q_OS_WIN
    ui->interfaceSettings->setCurrentIndex(2);
#else
    ui->interfaceSettings->setCurrentIndex(0);
#endif
  } else if (Interface::Serial == ui->interfaceSelection->currentData().value<Interface>()) {
    ui->interfaceSettings->setCurrentIndex(1);
  }
}

