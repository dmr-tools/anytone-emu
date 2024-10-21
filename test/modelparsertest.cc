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
R"(<catalog>)"
R"(<model class="AnyTone" id="dmr6x2uv">)"
R"( <name>DMR6X2UV</name>)"
R"( <manufacturer>DMR6X2UV</manufacturer>)"
R"( <memory>)"
R"(   <id>36 58 32 55 56 00</id>)"
R"(   <revision>56 31 30 30 00 00</revision>)"
R"(   <map address="0100">61 62 63</map>)"
R"( </memory>)"
R"(</model>)"
R"(</catalog>)";

  QXmlStreamReader reader(def);
  ModelCatalog catalog;
  ModelDefinitionParser parser(&catalog, "");
  QVERIFY2(parser.parse(reader), parser.errorMessage().toLocal8Bit().constData());

  QCOMPARE(catalog.count(), 1);
  ModelDefinition *model = catalog.model(0);
  QByteArray buffer;

  QVERIFY(model->rom().read(0x0100, 3, buffer));
  QCOMPARE(buffer, QByteArray("abc"));

  QVERIFY(qobject_cast<AnyToneModelDefinition*>(model));
  QCOMPARE(qobject_cast<AnyToneModelDefinition*>(model)->modelId(), QByteArray("6X2UV\x00",6));
  QCOMPARE(qobject_cast<AnyToneModelDefinition*>(model)->revision(), QByteArray("V100\x00\x00",6));
  delete model;
}




QTEST_MAIN(ModelParserTest)
#include "modelparsertest.moc"
