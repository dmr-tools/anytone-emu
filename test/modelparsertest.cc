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
R"(<model class="AnyTone">)"
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


void
ModelParserTest::testAnyToneModelFirmware() {
  QString def = R"(<?xml version="1.0"?>)"
R"(<model class="AnyTone">)"
R"( <name>DMR6X2UV</name>)"
R"( <manufacturer>DMR6X2UV</manufacturer>)"
R"( <memory>)"
R"(   <id>36 58 32 55 56 00</id>)"
R"(   <revision>56 31 30 30 00 00</revision>)"
R"(   <map address="0100">61 62 63</map>)"
R"( </memory>)"
R"( <firmware name="2.10" release="2024-04-10">)"
R"(   <memory>)"
R"(     <revision>56 31 31 30 00 00</revision>)"
R"(     <map address="0100">64 65 66</map>)"
R"( </memory>)"
R"( </firmware>)"
R"( <firmware name="2.11" release="2024-08-30">)"
R"(   <memory>)"
R"(     <revision>56 31 32 30 00 00</revision>)"
R"(     <map address="0100">67 68 69</map>)"
R"(   </memory>)"
R"( </firmware>)"
R"(</model>)";

  QXmlStreamReader reader(def);
  ModelCatalog catalog;
  ModelDefinitionParser parser(&catalog, "");
  QVERIFY2(parser.parse(reader), parser.errorMessage().toLocal8Bit().constData());

  QCOMPARE(catalog.count(), 1);
  ModelDefinition *model = catalog.model(0);
  QByteArray buffer;

  QCOMPARE(model->firmwares().count(), 2);

  ModelFirmwareDefinition *fw1 = model->firmwares().at(0);
  ModelFirmwareDefinition *fw2 = model->firmwares().at(1);

  QVERIFY(fw1->rom().read(0x0100, 3, buffer));
  QCOMPARE(buffer, QByteArray("def"));

  QVERIFY(fw2->rom().read(0x0100, 3, buffer));
  QCOMPARE(buffer, QByteArray("ghi"));

  QVERIFY(qobject_cast<AnyToneModelFirmwareDefinition *>(fw1));
  QCOMPARE(qobject_cast<AnyToneModelFirmwareDefinition *>(fw1)->modelId(), QByteArray("6X2UV\x00",6));
  QCOMPARE(qobject_cast<AnyToneModelFirmwareDefinition *>(fw1)->revision(), QByteArray("V110\x00\x00",6));

  QVERIFY(qobject_cast<AnyToneModelFirmwareDefinition *>(fw2));
  QCOMPARE(qobject_cast<AnyToneModelFirmwareDefinition *>(fw2)->modelId(), QByteArray("6X2UV\x00",6));
  QCOMPARE(qobject_cast<AnyToneModelFirmwareDefinition *>(fw2)->revision(), QByteArray("V120\x00\x00",6));

  delete model;
}



QTEST_MAIN(ModelParserTest)
#include "modelparsertest.moc"
