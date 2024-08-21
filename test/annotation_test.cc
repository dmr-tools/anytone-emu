#include "annotation_test.hh"

#include <QXmlStreamReader>
#include "patternparser.hh"
#include "pattern.hh"
#include "annotation.hh"
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

  const FieldAnnotation *unk = image.element(0)->annotationAt(Address::zero())->as<FieldAnnotation>();
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

  const FieldAnnotation *unk = image.find(Address::zero())->annotationAt(Address::zero())->as<FieldAnnotation>();
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
  const Element *el = image.element(0);
  const FieldAnnotation *bit0  = el->annotationAt(Address::fromByte(0,7))->as<FieldAnnotation>();
  const FieldAnnotation *bit1  = el->annotationAt(Address::fromByte(0,6))->as<FieldAnnotation>();
  const FieldAnnotation *uint0 = el->annotationAt(Address::fromByte(0,5))->as<FieldAnnotation>();
  const FieldAnnotation *int1  = el->annotationAt(Address::fromByte(1))->as<FieldAnnotation>();
  const FieldAnnotation *uint2 = el->annotationAt(Address::fromByte(2))->as<FieldAnnotation>();
  const FieldAnnotation *uint3 = el->annotationAt(Address::fromByte(4))->as<FieldAnnotation>();
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

  const Element *el = image.element(0);
  QCOMPARE(el->numAnnotations(), 1);
  QVERIFY(el->annotationAt(Address::fromByte(0)) && el->annotationAt(Address::fromByte(0))->is<StructuredAnnotation>());
  const StructuredAnnotation *root = el->annotationAt(Address::fromByte(0))->as<StructuredAnnotation>();

  QCOMPARE(root->numAnnotations(), 3);
  Address addr=Address::fromByte(0);
  for (int i=0,v=1; i<root->numAnnotations(); i++, addr+=Offset::fromByte(3)) {
    QVERIFY(root->annotationAt(addr) && root->annotationAt(addr)->is<StructuredAnnotation>());
    const StructuredAnnotation *elm = root->annotationAt(addr)->as<StructuredAnnotation>();
    QVERIFY(elm->annotationAt(addr) && elm->annotationAt(addr)->is<FieldAnnotation>());
    QCOMPARE(elm->annotationAt(addr)->as<FieldAnnotation>()->value().value<uint8_t>(), v++);
    QVERIFY(elm->annotationAt(addr+Offset::fromByte(1)) && elm->annotationAt(addr+Offset::fromByte(1))->is<FieldAnnotation>());
    QCOMPARE(elm->annotationAt(addr+Offset::fromByte(1))->as<FieldAnnotation>()->value().value<uint16_t>(), v++);
  }
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
  const Element *el = image.element(0);
  QCOMPARE(el->numAnnotations(), 1);
  QVERIFY(el->annotationAt(Address::fromByte(0)) && el->annotationAt(Address::fromByte(0))->is<StructuredAnnotation>());
  const StructuredAnnotation *root = el->annotationAt(Address::fromByte(0))->as<StructuredAnnotation>();

  QCOMPARE(root->numAnnotations(), 2);
  Address addr = Address::fromByte(0);
  for (int i=0,v=1; i<root->numAnnotations(); i++) {
    QVERIFY(root->annotationAt(addr) && root->annotationAt(addr)->is<StructuredAnnotation>());
    const StructuredAnnotation *elm = root->annotationAt(addr)->as<StructuredAnnotation>();

    QVERIFY(elm->annotationAt(addr) && elm->annotationAt(addr)->is<FieldAnnotation>());
    QCOMPARE(elm->annotationAt(addr)->as<FieldAnnotation>()->value().value<uint8_t>(), v++);
    addr += Offset::fromByte(1);

    QVERIFY(elm->annotationAt(addr) && elm->annotationAt(addr)->is<FieldAnnotation>());
    QCOMPARE(elm->annotationAt(addr)->as<FieldAnnotation>()->value().value<uint8_t>(), v++);
    addr += Offset::fromByte(2);
  }
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
  image.append(0, QByteArray::fromHex("0000010002"));

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QVERIFY(image.annotate(codeplug));
  const Element *el = image.element(0);
  QCOMPARE(el->numAnnotations(), 3);
  Address addr = Address::fromByte(0);
  for (int i=0,v=0; i<el->numAnnotations(); i++) {
    QVERIFY(el->annotationAt(addr) && el->annotationAt(addr)->is<StructuredAnnotation>());
    const StructuredAnnotation *elm = el->annotationAt(addr)->as<StructuredAnnotation>();
    QCOMPARE(elm->numAnnotations(), 1);
    QVERIFY(elm->annotationAt(addr) && elm->annotationAt(addr)->is<FieldAnnotation>());
    QCOMPARE(elm->annotationAt(addr)->as<FieldAnnotation>()->value().value<uint8_t>(), v++);
    addr += Offset::fromByte(2);
  }
}

QTEST_MAIN(AnnotationTest)
#include "annotation_test.moc"
