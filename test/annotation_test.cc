#include "annotation_test.hh"

#include <QXmlStreamReader>
#include "codeplugpatternparser.hh"
#include "codeplugpattern.hh"
#include "codeplugannotation.hh"
#include "image.hh"
#include "logger.hh"


AnnotationTest::AnnotationTest(QObject *parent)
  : QObject{parent}
{
  static QTextStream err(stderr);
  Logger::get().addHandler(new StreamLogHandler(err, LogMessage::DEBUG, true));
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

  const FieldAnnotation *unk = image.annotations()->resolve(Address::zero());
  QVERIFY(unk->pattern()->is<UnusedFieldPattern>());
  QVERIFY(! unk->value().toByteArray().isEmpty());
  QCOMPARE(unk->value().toByteArray().at(0), 0x01);
}


void
AnnotationTest::annotateUnknownTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <unknown at="0" width="1"/>)"
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

  const FieldAnnotation *unk = image.annotations()->resolve(Address::zero());
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

  const FieldAnnotation *bit0  = image.annotations()->resolve(Address::fromByte(0,7));
  const FieldAnnotation *bit1  = image.annotations()->resolve(Address::fromByte(0,6));
  const FieldAnnotation *uint0 = image.annotations()->resolve(Address::fromByte(0,5));
  const FieldAnnotation *int1  = image.annotations()->resolve(Address::fromByte(1));
  const FieldAnnotation *uint2 = image.annotations()->resolve(Address::fromByte(2));
  const FieldAnnotation *uint3 = image.annotations()->resolve(Address::fromByte(4));
  QCOMPARE(bit0->value().value<uint8_t>(), false);
  QCOMPARE(bit1->value().value<uint8_t>(), true);
  QCOMPARE(uint0->value().value<uint8_t>(), 2);
  QCOMPARE(int1->value().value<int8_t>(), -1);
  QCOMPARE(uint2->value().value<uint16_t>(), 2);
  QCOMPARE(uint3->value().value<uint16_t>(), 3);
}


void
AnnotationTest::annotateFixedRepeatTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <repeat at="0" n="3">)"
      R"(    <element>)"
      R"(      <uint8/>)"
      R"(      <uint16be/>)"
      R"(    </element>)"
      R"(  </repeat>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromHex("010002030004050006"));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QVERIFY(image.annotate(codeplug));
  QVERIFY(image.annotations()->resolve(Address::fromByte(0)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(0))->value().value<uint8_t>(), 1);
  QVERIFY(image.annotations()->resolve(Address::fromByte(1)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(1))->value().value<uint16_t>(), 2);
  QVERIFY(image.annotations()->resolve(Address::fromByte(3)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(3))->value().value<uint8_t>(), 3);
  QVERIFY(image.annotations()->resolve(Address::fromByte(4)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(4))->value().value<uint16_t>(), 4);
  QVERIFY(image.annotations()->resolve(Address::fromByte(6)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(6))->value().value<uint8_t>(), 5);
  QVERIFY(image.annotations()->resolve(Address::fromByte(7)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(7))->value().value<uint16_t>(), 6);
}


void
AnnotationTest::annotateBlockRepeatTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <repeat at="0" min="1" max="3">)"
      R"(    <element>)"
      R"(      <uint8/>)"
      R"(      <uint16be/>)"
      R"(    </element>)"
      R"(  </repeat>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromHex("010002030004"));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QVERIFY(image.annotate(codeplug));
  QVERIFY(image.annotations()->resolve(Address::fromByte(0)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(0))->value().value<uint8_t>(), 1);
  QVERIFY(image.annotations()->resolve(Address::fromByte(1)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(1))->value().value<uint16_t>(), 2);
  QVERIFY(image.annotations()->resolve(Address::fromByte(3)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(3))->value().value<uint8_t>(), 3);
  QVERIFY(image.annotations()->resolve(Address::fromByte(4)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(4))->value().value<uint16_t>(), 4);
}


void
AnnotationTest::annotateSparseRepeatTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <repeat at="0" min="1" max="3" step="2">)"
      R"(    <element>)"
      R"(      <uint8/>)"
      R"(    </element>)"
      R"(  </repeat>)"
      R"(</codeplug>)";

  Image image;
  image.append(0, QByteArray::fromHex("0100020003"));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QVERIFY(image.annotate(codeplug));
  QVERIFY(image.annotations()->resolve(Address::fromByte(0)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(0))->value().value<uint8_t>(), 1);
  QVERIFY(image.annotations()->resolve(Address::fromByte(2)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(2))->value().value<uint8_t>(), 2);
  QVERIFY(image.annotations()->resolve(Address::fromByte(4)));
  QCOMPARE(image.annotations()->resolve(Address::fromByte(4))->value().value<uint8_t>(), 3);
}

QTEST_MAIN(AnnotationTest)
#include "annotation_test.moc"
