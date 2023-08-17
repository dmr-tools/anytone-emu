#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>

#include "pseudoterminal.hh"
#include "dmr6x2uv2.hh"
#include "logger.hh"
#include "model.hh"

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


  parser.process(app);

  ImageCollector *model = new ImageCollector();
  QString portFile = QDir::home().absoluteFilePath(".local/share/anytone-emu/anytoneport");
  DMR6X2UV2 device(new PseudoTerminal(portFile), model);

  app.exec();

  return 0;
}
