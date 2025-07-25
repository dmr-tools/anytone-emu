#include "aboutdialog.hh"
#include "ui_aboutdialog.h"
#include <QSettings>

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("application-anytone-emu").pixmap(QSize(64,64)));

  QSettings settings;
  if (settings.contains("layout/aboutDialogSize"))
    restoreGeometry(settings.value("layout/aboutDialogSize").toByteArray());
}


AboutDialog::~AboutDialog() {
  delete ui;
}


void
AboutDialog::done(int res) {
  QSettings().setValue("layout/aboutDialogSize", saveGeometry());
  QDialog::done(res);
}



