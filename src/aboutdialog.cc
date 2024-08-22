#include "aboutdialog.hh"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->iconLabel->setPixmap(QIcon::fromTheme("application-anyton-emu").pixmap(QSize(64,64)));
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
