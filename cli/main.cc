#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "pseudoterminal.hh"
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
#include "logger.hh"
#include "model.hh"
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

  ImageCollector *model = new ImageCollector();

  if (parser.isSet("dump")) {
    if (parser.isSet("output"))
      logDebug() << "Use pattern '" << parser.value("output") << "' for dump files, e.g., "
                 << QString::asprintf(parser.value("output").toStdString().c_str(), 42) << ".";
    QObject::connect(model, &ImageCollector::imageReceived, [&parser, &stream, model] {
      static unsigned int count = 0;
      HexImage hex(model->last());
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
    QObject::connect(model, &ImageCollector::imageReceived, [&stream, model] {
      if ((nullptr == model->first()) || (nullptr == model->last()))
        return;
      HexImage hex(model->first(), model->last());
      if (hex.hasDiff())
        hexdump(hex, stream);
      else
        logInfo() << "No differences found.";
    });
  } else if ("previous" == parser.value("diff")) {
    QObject::connect(model, &ImageCollector::imageReceived, [&stream, model] {
      if ((nullptr == model->previous()) || (nullptr == model->last()))
        return;
      HexImage hex(model->previous(), model->last());
      if (hex.hasDiff())
        hexdump(hex, stream);
      else
        logInfo() << "No differences found.";
    });
  }

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

  Device *dev = nullptr;
  if ("d868uv" == parser.value("device")) {
    dev = new D868UV(interface, model);
  } else if ("d868uve" == parser.value("device")) {
    dev = new D868UVE(interface, model);
  } else if ("d878uv" == parser.value("device")) {
    dev = new D878UV(interface, model);
  } else if ("d878uv2" == parser.value("device")) {
    dev = new D878UV2(interface, model);
  } else if ("d578uv" == parser.value("device")) {
    dev = new D578UV(interface, model);
  } else if ("d578uv2" == parser.value("device")) {
    dev = new D578UV2(interface, model);
  } else if ("dmr6x2uv" == parser.value("device")) {
    dev = new DMR6X2UV(interface, model);
  } else if ("dmr6x2uv2" == parser.value("device")) {
    dev = new DMR6X2UV2(interface, model);
  } else if ("djmd5" == parser.value("device")) {
    dev = new DJMD5(interface, model);
  } else if ("djmd5x" == parser.value("device")) {
    dev = new DJMD5X(interface, model);
  } else {
    logError() << "Unknown device '" << parser.value("device") << "'. Must be one of "
               << "d868uv, d868uve, d878uv, d878uv2, d578uv, d578uv2, "
               << "dmr6x2uv, dmr6x2uv2, "
               << "djmd5, djmd5x.";
  }

  if (nullptr == dev) {
    delete interface;
    parser.showHelp(-1);
  }

  app.exec();

  delete dev;

  return 0;
}
