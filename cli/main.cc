#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>

#include "pseudoterminal.hh"
#include "dmr6x2uv2.hh"
#include "logger.hh"
#include "model.hh"
#include "hexdump.hh"

int
main(int argc, char *argv[])
{
  QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));

  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName("anytone-emu");
  QCoreApplication::setApplicationVersion("0.1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Emulate AnyTone devices");
  parser.addHelpOption();
  parser.addVersionOption();

  parser.addOption({"dump", "Just dumps the received codeplugs."});
  parser.addOption({"diff", "Shows diffs between received codeplugs.", "mode", "first"});

  parser.process(app);

  if ( (parser.isSet("dump") && parser.isSet("diff")) ||
       (!parser.isSet("dump") && !parser.isSet("diff")) )
    parser.showHelp(-1);

  QTextStream stream(stdout);

  ImageCollector *model = new ImageCollector();

  if (parser.isSet("dump")) {
    QObject::connect(model, &ImageCollector::imageReceived, [&stream, model] {
      hexdump(model->last(), stream);
    });
  } else {

  }

  QString portFile = QDir::home().absoluteFilePath(".local/share/anytone-emu/anytoneport");
  DMR6X2UV2 device(new PseudoTerminal(portFile), model);

  app.exec();

  return 0;
}
