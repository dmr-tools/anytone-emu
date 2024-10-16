#include "setupdialog.hh"
#include "application.hh"
#include "pseudoterminal.hh"
#include "logger.hh"
#include "device.hh"
#include "d868uv.hh"
#include "d868uve.hh"
#include "d878uv.hh"
#include "d878uv2.hh"
#include "d578uv.hh"
#include "d578uv2.hh"
#include "dmr6x2uv.hh"
#include "dmr6x2uv2.hh"
#include "djmd5.hh"
#include "djmd5x.hh"

#include "image.hh"
#include "imagecollectionadapter.hh"
#include "pattern.hh"
#include "patternparser.hh"

#include <QSerialPort>
#include <QXmlStreamReader>

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

    QIODevice *interface = nullptr;
    if (SetupDialog::Interface::PTY == setup.interface()) {
      interface = new PseudoTerminal(setup.ptySymlinkPath());
    } else if (SetupDialog::Interface::Serial == setup.interface()) {
      interface = new QSerialPort(setup.serialPort());
    }

    ImageCollectionAdapter *adapter = nullptr;
    switch (setup.device()) {
    case SetupDialog::Device::D868UV:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d868uv/codeplug.xml"));
      break;
    case SetupDialog::Device::D868UVE:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d868uve/codeplug.xml"));
      break;
    case SetupDialog::Device::D878UV:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d878uv/codeplug.xml"));
      break;
    case SetupDialog::Device::D878UV2:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d878uv/codeplug.xml"));
      break;
    case SetupDialog::Device::D578UV:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d578uv/codeplug.xml"));
      break;
    case SetupDialog::Device::D578UV2:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/d578uv2/codeplug.xml"));
      break;
    case SetupDialog::Device::DMR6X2UV:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/dmr6x2uv/codeplug.xml"));
      break;
    case SetupDialog::Device::DMR6X2UV2:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/dmr6x2uv2/codeplug.xml"));
      break;
    case SetupDialog::Device::DJMD5:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/djmd5/codeplug.xml"));
      break;
    case SetupDialog::Device::DJMD5X:
      adapter = new ImageCollectionAdapter(
            app.collection(), CodeplugPattern::load(setup.patternDir()+"/djmd5x/codeplug.xml"));
      break;
    }

    switch (setup.device()) {
    case SetupDialog::Device::D868UV:    app.setDevice(new D868UV(interface, adapter)); break;
    case SetupDialog::Device::D868UVE:   app.setDevice(new D868UVE(interface, adapter)); break;
    case SetupDialog::Device::D878UV:    app.setDevice(new D878UV(interface, adapter)); break;
    case SetupDialog::Device::D878UV2:   app.setDevice(new D878UV2(interface, adapter)); break;
    case SetupDialog::Device::D578UV:    app.setDevice(new D578UV(interface, adapter)); break;
    case SetupDialog::Device::D578UV2:   app.setDevice(new D578UV2(interface, adapter)); break;
    case SetupDialog::Device::DMR6X2UV:  app.setDevice(new DMR6X2UV(interface, adapter)); break;
    case SetupDialog::Device::DMR6X2UV2: app.setDevice(new DMR6X2UV2(interface, adapter)); break;
    case SetupDialog::Device::DJMD5:     app.setDevice(new DJMD5(interface, adapter)); break;
    case SetupDialog::Device::DJMD5X:    app.setDevice(new DJMD5X(interface, adapter)); break;
    }

    MainWindow  mainwindow;
    mainwindow.show();

    app.exec();
  }

  return 0;
}
