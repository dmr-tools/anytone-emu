#include <QApplication>
#include <QDir>

#include "libraryview.hh"
#include "librarywrapper.hh"
#include "patterndefinition.hh"
#include "logger.hh"

int
main(int argc, char *argv[])
{
  QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));

  QApplication app(argc, argv);

  PatternDefinitionLibrary lib;
  lib.load(QDir("./codeplugs/"));

  auto view = new LibraryView(&lib);
  view->show();

  app.exec();

  return 0;
}
