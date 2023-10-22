#include <QApplication>
#include <QScrollArea>

#include "hexview.hh"
#include "image.hh"
#include "logger.hh"

int
main(int argc, char *argv[])
{
  QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));

  QApplication app(argc, argv);

  Image image1, image2;
  QByteArray buffer1("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
  QByteArray buffer2("\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);
  image1.append(0x08, buffer1);
  image2.append(0x08, buffer2);
  image1.append(0x28, buffer1);
  image2.append(0x28, buffer1);

  QScrollArea window; window.setWidget(new HexView(new HexImage(&image1, &image2))); window.show();

  app.exec();

  return 0;
}
