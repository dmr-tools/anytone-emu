#include "modelparsertest.hh"
#include "modelparser.hh"
#include "anytonemodelparser.hh"
#include <QXmlStreamReader>
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
  rom.write(0x0107, "ghi");

  QCOMPARE(rom.segmentCount(), 3);
}


void
ModelParserTest::testRomOverride() {
  ModelRom rom;

  rom.write(0x0100, "abc");
  rom.write(0x0103, "def");
  QCOMPARE(rom.segmentCount(), 1);

  rom.write(0x0101, "012");
  QCOMPARE(rom.segmentCount(), 1);

  QByteArray buffer;
  QVERIFY(rom.read(0x0100, 6, buffer));
  QCOMPARE(buffer, QByteArray("a012ef"));
}


void
ModelParserTest::testRomExtend() {
  ModelRom rom;

  rom.write(0x0100, "abc");
  rom.write(0x0106, "ghi");
  QCOMPARE(rom.segmentCount(), 2);

  rom.write(0x0103, "def");
  QCOMPARE(rom.segmentCount(), 1);

  QByteArray buffer;
  QVERIFY(rom.read(0x0100, 9, buffer));
  QCOMPARE(buffer, QByteArray("abcdefghi"));
}


void
ModelParserTest::testRomMerge() {

}


void
ModelParserTest::testAnyToneModel() {
  QString def = R"(<?xml version="1.0"?>)"
R"(<model class="AnyTone">)"
R"( <name>DMR6X2UV</name>)"
R"( <manufacturer>DMR6X2UV</manufacturer>)"
R"( <memory>)"
R"(   <model>36 58 32 55 56 0</model>)"
R"(   <map address="0100">61 62 63</map>)"
R"( </memory>)"
R"(</model>)";

  QXmlStreamReader reader(def);
  ModelDefinitionParser parser;
  QVERIFY2(parser.parse(reader), parser.errorMessage().toLocal8Bit().constData());
}


void
ModelParserTest::testAnyToneModelFirmware() {

}



QTEST_MAIN(ModelParserTest)
#include "modelparsertest.moc"
