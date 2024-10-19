#include "setupdialog.hh"
#include "application.hh"
#include "pseudoterminal.hh"
#include "logger.hh"
#include "device.hh"

#include "imagecollectionadapter.hh"

#include <QSerialPort>
#include <QXmlStreamReader>
#include <QMessageBox>

#include "mainwindow.hh"

int
main(int argc, char *argv[])
{
  QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));

  Application app(argc, argv);
  if (app.palette().window().color().lightness() < app.palette().windowText().color().lightness())
    QIcon::setThemeName("dark");
  else
    QIcon::setThemeName("light");

  while (true) {
    SetupDialog setup;
    if (QDialog::Accepted != setup.exec()) {
      return 0;
    }

    ErrorStack err;
    auto device = setup.createDevice(err);
    if (nullptr == device) {
      QMessageBox::critical(nullptr, QString("Cannot create device"),
                            QString("Cannot create device: %1").arg(err.format()));
      continue;
    }

    device->setHandler(new ImageCollectionAdapter(app.collection()));
    app.setDevice(device);

    MainWindow  mainwindow;
    mainwindow.show();

    app.exec();
  }

  return 0;
}
