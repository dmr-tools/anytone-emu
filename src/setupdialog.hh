#ifndef SETUPDIALOG_HH
#define SETUPDIALOG_HH

#include <QDialog>
#include "modeldefinition.hh"

class Device;

namespace Ui {
  class SetupDialog;
}

class SetupDialog : public QDialog
{
  Q_OBJECT

public:
  enum class Interface {
    PTY, Serial
  };
  Q_ENUM(Interface)

public:
  explicit SetupDialog(QWidget *parent = nullptr);
  ~SetupDialog() override;

  QString catalog() const;
  Device *createDevice(const ErrorStack &err=ErrorStack());

public slots:
  void done(int res);

private slots:
  void reloadModels();
  void onSelectCatalogFile();
  void onDeviceSelected(int idx);
  void onFirmwareSelected(int idx);
  void onInterfaceSelected(int idx);

private:
  Ui::SetupDialog *ui;
  QString _currentFile;
  ModelCatalog _catalog;
};

#endif // SETUPDIALOG_HH
