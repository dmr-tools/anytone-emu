#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QXmlStreamReader>

#include "pseudoterminal.hh"
#include "logger.hh"
#include "model.hh"
#include "device.hh"
#include "modeldefinition.hh"
#include "modelparser.hh"
#include "hexdump.hh"
#include "config.hh"


int
main(int argc, char *argv[])
{
  QTextStream err(stderr);

  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("anytone-emu");
  QCoreApplication::setApplicationVersion(PROJECT_VERSION);
  QCoreApplication::setOrganizationName("Hannes Matuschek");
  QCoreApplication::setOrganizationDomain("dm3mat.darc.de");

  QCommandLineParser parser;
  parser.setApplicationDescription(
        "Emulates AnyTone devices to assist in reverse-engineering those pesky codeplugs. ");
  parser.addHelpOption();
  parser.addVersionOption();

  parser.addOption({"loglevel", "Sets the log-level. Must be one of 'debug', 'info', 'warning', "
                    "'error' or 'fatal'. Default: 'warning'.", "loglevel", "warning"});
  parser.addOption({"output", "Specifies the sprintf-like file pattern to save received codeplugs to. "
                    "For every received codeplug, a counter gets increased. E.g., 'codeplug_%02d.hex' "
                    "will produce the file-sequence 'codeplug_00.hex', 'codeplug_01.hex', ...",
                    "output", "codeplug_%04d.hex"});
  parser.addOption({"dump", "Just dumps received codeplugs. Either to stdout or to a file, "
                    "if --output ist set."});
  parser.addOption({"diff", "Shows diffs between received codeplugs. The mode specifies, which "
                    "differences are taken and shown. If 'first' (default) is set, the difference "
                    "is alway between the first and last received codeplugs. If 'previous' is set, "
                    "the diffs are taken between the last two received codeplugs.", "mode", "first"});
  parser.addOption({"device", "Specifies the device to emulate. This must be one of 'd868uv', "
                    "'d868uve', 'd878uv', 'd878uv2', 'd578uv', 'd578uv2', 'dmr6x2uv', 'dmr6x2uv2', "
                    "'djmd5' or 'djmd5x'.", "device"});
  parser.addOption({"interface", "Specifies the interface to the CPS. This can either be 'pty' or "
                    "the name of a serial interface. If 'pty' is set, the emulator will generate a "
                    "new pseudo terminal. This can then be used to emulate a COM port under wine. "
                    "Under windows, a serial port of a virtual null-modem must be choosen. The "
                    "second port of that null-modem is then selected in the CPS.", "interface", "pty"});

  parser.process(app);

  if ("debug" == parser.value("loglevel"))
    Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));
  else if ("info" == parser.value("loglevel"))
    Logger::get().addHandler(new StreamLogHandler(err, LogMessage::INFO, true));
  else if ("warning" == parser.value("loglevel"))
    Logger::get().addHandler(new StreamLogHandler(err, LogMessage::WARNING, true));
  else if ("error" == parser.value("loglevel"))
    Logger::get().addHandler(new StreamLogHandler(err, LogMessage::ERROR, true));
  else if ("fatal" == parser.value("loglevel"))
    Logger::get().addHandler(new StreamLogHandler(err, LogMessage::FATAL, true));

  if ( (parser.isSet("dump") && parser.isSet("diff")) ||
       (!parser.isSet("dump") && !parser.isSet("diff")) )
    parser.showHelp(-1);

  QTextStream stream(stdout);
  ModelCatalog catalog;
  ModelDefinitionParser modelParser(&catalog, ":/codeplug/");
  QFile catalogFile(":/codeplug/catalog.xml");
  if (! catalogFile.open(QIODevice::ReadOnly)) {
    logError() << "Cannot parse catalog file '" << catalogFile.fileName()
               << "': " << catalogFile.errorString() << ".";
    return -1;
  }
  QXmlStreamReader reader(&catalogFile);
  if (! modelParser.parse(reader)) {
    logError() << "Cannot parse catalog file '" << catalogFile.fileName()
               << "': " << modelParser.errorMessage() << ".";
    return -1;
  }

  if (! catalog.hasModel(parser.value("device"))) {
    logError() << "Cannot find model '" << parser.value("device")
               << "' unknown device.";
    return -1;
  }
  ModelDefinition *modelDef = catalog.model(parser.value("device"));
  ModelFirmwareDefinition *modelFirmwareDef = modelDef->latestFirmware();
  /// @todo Use optional argument to determine firmware version.

  QString portFile = QDir::home().absoluteFilePath(".local/share/anytone-emu/anytoneport");
  QIODevice *interface = nullptr;
  if ("pty" == parser.value("interface")) {
    interface = new PseudoTerminal(portFile);
  } else {
    QSerialPortInfo portInfo(parser.value("interface"));
    if (portInfo.isNull()) {
      QStringList names;
      foreach (QSerialPortInfo port, QSerialPortInfo::availablePorts())
        names.append(port.portName());
      QString nameList = names.size() ? names.join(", ") : "[no ports found]";
      logError() << "Cannot open serial port '" << parser.value("interface")
                 << "', use one of the known ports: " << nameList << ".";
      return -1;
    }
    logDebug() << "Use port '" << portInfo.portName() << "': " << portInfo.description() << ".";
    interface = new QSerialPort(portInfo);
  }

  Device *dev = modelFirmwareDef->createDevice(interface);
  auto imageHandler = new ImageCollector();
  dev->setHandler(imageHandler);

  if (parser.isSet("dump")) {
    if (parser.isSet("output"))
      logDebug() << "Use pattern '" << parser.value("output") << "' for dump files, e.g., "
                 << QString::asprintf(parser.value("output").toStdString().c_str(), 42) << ".";
    QObject::connect(imageHandler, &ImageCollector::imageReceived, [&parser, &stream, imageHandler] {
      static unsigned int count = 0;
      HexImage hex(imageHandler->last());
      if (! parser.isSet("output")) {
        hexdump(hex, stream);
      } else {
        QString filename = QString::asprintf(parser.value("output").toStdString().c_str(), count++);
        QFile output(filename);
        if (! output.open(QIODevice::WriteOnly)) {
          logError() << "Cannot write received codeplug dump to file '" << filename
                     << "': " << output.errorString() << ".";
          return;
        }
        QTextStream outStream(&output);
        logInfo() << "Write codeplug to '" << filename << "'.";
        hexdump(hex, outStream);
        output.close();
      }
    });
  } else if ("first" == parser.value("diff")) {
    QObject::connect(imageHandler, &ImageCollector::imageReceived, [&stream, imageHandler] {
      if ((nullptr == imageHandler->first()) || (nullptr == imageHandler->last()))
        return;
      HexImage hex(imageHandler->first(), imageHandler->last());
      if (hex.hasDiff())
        hexdump(hex, stream);
      else
        logInfo() << "No differences found.";
    });
  } else if ("previous" == parser.value("diff")) {
    QObject::connect(imageHandler, &ImageCollector::imageReceived, [&stream, imageHandler] {
      if ((nullptr == imageHandler->previous()) || (nullptr == imageHandler->last()))
        return;
      HexImage hex(imageHandler->previous(), imageHandler->last());
      if (hex.hasDiff())
        hexdump(hex, stream);
      else
        logInfo() << "No differences found.";
    });
  }

  app.exec();

  delete dev;

  return 0;
}
