#include "setupdialog.hh"
#include "application.hh"
#include "pseudoterminal.hh"
#include "logger.hh"
#include "device.hh"

#include "imagecollectionadapter.hh"

#include <QSerialPort>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QStyleHints>

#include "mainwindow.hh"

int
main(int argc, char *argv[])
{
  QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));

  QApplication::setStyle("fusion");
  Application app(argc, argv);

#if QT_VERSION >= QT_VERSION_CHECK(6,5,0)
  bool darkMode = Qt::ColorScheme::Dark == app.styleHints()->colorScheme();
#else
  bool darkMode = (app.palette().text().color().lightness() >
                   app.palette().window().color().lightness());
#endif

  if (darkMode) {
    QIcon::setThemeName("dark");
  } else {
    QIcon::setThemeName("light");
  }

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
    app.setCatalog(setup.catalog());
    app.setDevice(device);

    MainWindow  mainwindow;
    mainwindow.show();

    app.exec();
  }

  return 0;
}
