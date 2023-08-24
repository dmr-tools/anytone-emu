#include "annotation_test.hh"

#include <QXmlStreamReader>
#include "codeplugpatternparser.hh"
#include "codeplugpattern.hh"
#include "codeplugannotation.hh"
#include "image.hh"


AnnotationTest::AnnotationTest(QObject *parent)
  : QObject{parent}
{
  // pass...
}

void
AnnotationTest::annotateUnusedTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <unused at="0">01</unused>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromRawData("\x01", 1));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();

  QVERIFY(image.annotate(codeplug));

  FieldAnnotation *unk = image.annotations()->at(Address::zero());
  QVERIFY(unk->pattern()->is<UnusedFieldPattern>());
  QVERIFY(! unk->value().toByteArray().isEmpty());
  QCOMPARE(unk->value().toByteArray().at(0), 0x01);
}


void
AnnotationTest::annotateUnknownTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <unknown at="0" size="1"/>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromRawData("\x01", 1));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();

  QVERIFY(image.annotate(codeplug));

  FieldAnnotation *unk = image.annotations()->at(Address::zero());
  QVERIFY(unk->pattern()->is<UnknownFieldPattern>());
  QVERIFY(! unk->value().toByteArray().isEmpty());
  QCOMPARE(unk->value().toByteArray().at(0), 0x01);
}

void
AnnotationTest::annotateIntTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <bit at="0:7"/>)"
      R"(  <bit at="0:6"/>)"
      R"(  <uint at="0:5" width="0:6"/>)"
      R"(  <int8 at="1"/>)"
      R"(  <uint16be at="2"/>)"
      R"(  <uint16le at="4"/>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromHex("42ff00020300"));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();

  QVERIFY(image.annotate(codeplug));

  FieldAnnotation *bit0  = image.annotations()->at(Address::fromByte(0,7));
  FieldAnnotation *bit1  = image.annotations()->at(Address::fromByte(0,6));
  FieldAnnotation *uint0 = image.annotations()->at(Address::fromByte(0,5));
  QCOMPARE(bit0->value().value<uint8_t>(), false);
  QCOMPARE(bit1->value().value<uint8_t>(), true);
  QCOMPARE(uint0->value().value<uint8_t>(), 2);
}


QTEST_MAIN(AnnotationTest)
#include "annotation_test.moc"
