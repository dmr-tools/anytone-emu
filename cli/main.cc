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
                    "'error' or 'fatal'. Default: 'info'.", "loglevel", "info"});
  parser.addOption({"output", "Specifies the sprintf-like file pattern to save received codeplugs to. "
                    "For every codeplug received, a counter gets increased. E.g., 'codeplug_%02d.hex' "
                    "will produce the file-sequence 'codeplug_00.hex', 'codeplug_01.hex', ...",
                    "output", "codeplug_%04d.hex"});
  parser.addOption({"dump", "Just dumps received codeplugs. Either to stdout or to a file, "
                    "if --output ist set."});
  parser.addOption({"diff", "Shows diffs between received codeplugs. The mode specifies, which "
                    "differences are taken and shown. If 'first' is set, the difference "
                    "is always between the first and last received codeplugs. If 'previous' (default) is set, "
                    "the diffs are taken between the last two received codeplugs.", "mode", "previous"});
  parser.addOption({"firmware", "Specifies the firmware version to emulate. "
                    "This usually has no effect on the emulation. If no firmware is specified, "
                    "the latest found is used. If set to '?' all firmwares defined for the model are "
                    "shown.", "firmware", "latest"});

  parser.addPositionalArgument("catalog", "Specifies the catalog file to use. "
                               "See http://github.com/dmr-tools/codeplugs/.");
  parser.addPositionalArgument("device", "Specifies the device to emulate, e.g. 'd868uv', "
                               "'d868uve', 'd878uv', 'd878uv2', 'd578uv', 'd578uv2', 'dmr6x2uv', 'dmr6x2uv2', "
                               "'djmd5' or 'djmd5x'. For a full list of the devices of the catalog, "
                               "omit this argument.", "[device");
  parser.addPositionalArgument("interface", "Specifies the interface to the CPS. This can either be 'pty' or "
    "the name of a serial interface. If 'pty' is set, the emulator will generate a "
    "new pseudo terminal. This can then be used to emulate a COM port under wine. "
    "Under windows, a serial port of a virtual null-modem must be choosen. The "
    "second port of that null-modem is then selected in the CPS.", "[interface]]");

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

  // If no catalog is specified -> print help and exit
  if (0 == parser.positionalArguments().size())
    parser.showHelp(-1);

  QTextStream stream(stdout);
  ModelCatalog catalog;
  ModelDefinitionParser modelParser(&catalog);
  QFile catalogFile(parser.positionalArguments().at(0));
  if (! catalogFile.open(QIODevice::ReadOnly)) {
    logError() << "Cannot parse catalog file '" << catalogFile.fileName()
               << "': " << catalogFile.errorString() << ".";
    return -1;
  }
  QXmlStreamReader reader(&catalogFile);
  if (! modelParser.parse(reader, QFileInfo(catalogFile))) {
    logError() << "Cannot parse catalog file '" << catalogFile.fileName()
               << "': " << modelParser.errorMessage() << ".";
    return -1;
  }

  if ((2 > parser.positionalArguments().size()) ||
      (!catalog.hasModel(parser.positionalArguments().at(1)))) {
    if (2 <= parser.positionalArguments().size())
      logError() << "Cannot find model '" << parser.positionalArguments().at(1)
                 << "' unknown device.";
    QStringList devices;
    for (auto dev: catalog)
      devices.append(QString("%1 (%2)").arg(dev->name()).arg(dev->id()));
    logInfo() << "The following devices are specified in '" << catalogFile.fileName()
              << "': \n  * " << devices.join("\n  * ") << ".";
    return -1;
  }

  ModelDefinition *modelDef = catalog.model(parser.positionalArguments().at(1));
  if (nullptr == modelDef) {
    logError() << "Mode '" << parser.positionalArguments().at(1)
               << "' not found in catalog " << catalogFile.fileName() << ".";
    return -1;
  }

  ModelFirmwareDefinition *modelFirmwareDef = nullptr;
  if ((! parser.isSet("firmware")) || ("latest" == parser.value("firmware")))
    modelFirmwareDef = modelDef->latestFirmware();
  else
    modelFirmwareDef = modelDef->firmware(parser.value("firmware"));
  if (nullptr == modelFirmwareDef) {
    if ((!parser.isSet("firmware")) || ("?" != parser.value("firmware")))
      logError() << "Cannot find firmware '" << parser.value("firmware")
                 << "' for device " << modelDef->name()
                 << " in " << catalogFile.fileName() << ".";
    QStringList fwVersions;
    for (auto fw: *modelDef)
      fwVersions.append(fw->name());
    logInfo() << "Possible firmware version for " << modelDef->name()
              << ": " << fwVersions.join(", ") << ".";
    return -1;
  }

  QString portFile = QDir::home().absoluteFilePath(".local/share/anytone-emu/anytoneport");
  QIODevice *interface = nullptr;
  if (3 > parser.positionalArguments().size()) {
    logError() << "No interface specified.";
    parser.showHelp(-1);
  }
  auto interfaceName = parser.positionalArguments().at(2);
  if ("pty" == interfaceName) {
    interface = new PseudoTerminal(portFile);
  } else {
    QSerialPortInfo portInfo(interfaceName);
    if (portInfo.isNull()) {
      QStringList names;
      foreach (QSerialPortInfo port, QSerialPortInfo::availablePorts())
        names.append(port.portName());
      QString nameList = (! names.empty()) ? names.join(", ") : "[no ports found]";
      logError() << "Cannot open serial port '" << interfaceName
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
  } else if ((!parser.isSet("diff")) || ("previous" == parser.value("diff"))) {
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
