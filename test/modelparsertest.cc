#include "modelparsertest.hh"
#include "modelrom.hh"
#include <QTest>


ModelParserTest::ModelParserTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}


void
ModelParserTest::testRomAppend() {
  ModelRom rom;

  rom.write(0x0100, "abc");
  rom.write(0x0000, "012");
  rom.write(0x0103, "def");

  QCOMPARE(rom.segmentCount(), 2);
}


void
ModelParserTest::testRomMerge() {

}

QTEST_MAIN(ModelParserTest)
#include "modelparsertest.moc"
