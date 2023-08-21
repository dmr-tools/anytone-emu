#include "setupdialog.hh"
#include "ui_setupdialog.h"
#include <QSettings>
#include <QSerialPortInfo>


SetupDialog::SetupDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::SetupDialog)
{
  QSettings settings;

  ui->setupUi(this);

  ui->deviceSelection->addItem("AnyTone AT-D868UV", QVariant::fromValue(Device::D868UV));
  ui->deviceSelection->addItem("AnyTone AT-D868UVE", QVariant::fromValue(Device::D868UVE));
  ui->deviceSelection->addItem("AnyTone AT-D878UV", QVariant::fromValue(Device::D878UV));
  ui->deviceSelection->addItem("AnyTone AT-D878UV II", QVariant::fromValue(Device::D878UV2));
  ui->deviceSelection->addItem("AnyTone AT-D578UV", QVariant::fromValue(Device::D578UV));
  ui->deviceSelection->addItem("AnyTone AT-D578UV II", QVariant::fromValue(Device::D578UV2));
  ui->deviceSelection->addItem("BTECH DMR-6X2", QVariant::fromValue(Device::DMR6X2UV));
  ui->deviceSelection->addItem("BTEXH DMR-6X2 Pro", QVariant::fromValue(Device::DMR6X2UV2));
  ui->deviceSelection->addItem("Alinco DJ-MD5", QVariant::fromValue(Device::DJMD5));
  ui->deviceSelection->addItem("Alinco DJ-MD5X", QVariant::fromValue(Device::DJMD5X));
  connect(ui->deviceSelection, &QComboBox::currentIndexChanged, this, &SetupDialog::onDeviceSelected);
  ui->deviceSelection->setCurrentIndex(0);

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

  if (settings.contains("device")) {
    int idx = ui->deviceSelection->findData(settings.value("device"));
    if (idx >= 0)
      ui->deviceSelection->setCurrentIndex(idx);
  }

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

SetupDialog::Device
SetupDialog::device() const {
  return ui->deviceSelection->currentData().value<Device>();
}

SetupDialog::Interface
SetupDialog::interface() const {
  return ui->interfaceSelection->currentData().value<Interface>();
}

QString
SetupDialog::ptySymlinkPath() const {
  return ui->symlinkPath->text().simplified();
}

QString
SetupDialog::serialPort() const {
  return ui->portSelection->currentData().toString();
}

void
SetupDialog::onDeviceSelected(int idx) {
  QSettings().setValue("device", ui->deviceSelection->currentData());
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
