#ifndef SETUPDIALOG_HH
#define SETUPDIALOG_HH

#include <QDialog>

namespace Ui {
  class SetupDialog;
}

class SetupDialog : public QDialog
{
  Q_OBJECT

public:
  enum class Device {
    D868UV, D868UVE, D878UV, D878UV2, D578UV, D578UV2, DMR6X2UV, DMR6X2UV2, DJMD5, DJMD5X
  };
  Q_ENUM(Device)

  enum class Interface {
    PTY, Serial
  };
  Q_ENUM(Interface)

public:
  explicit SetupDialog(QWidget *parent = nullptr);
  ~SetupDialog();

  Device device() const;
  Interface interface() const;
  QString ptySymlinkPath() const;
  QString serialPort() const;

private slots:
  void onDeviceSelected(int idx);
  void onInterfaceSelected(int idx);

private:
  Ui::SetupDialog *ui;
};

#endif // SETUPDIALOG_HH
