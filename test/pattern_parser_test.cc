#include "pattern_parser_test.hh"
#include "codeplugpatternparser.hh"
#include <QXmlStreamReader>
#include "codeplugpattern.hh"


PatternParserTest::PatternParserTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
PatternParserTest::parseCodeplugTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(</codeplug>)";

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  QVERIFY(parser.topIs<CodeplugPattern>());
  // Verification should fail, as empty codeplugs are not valid.
  QVERIFY(!parser.topAs<CodeplugPattern>()->verify());
}


void
PatternParserTest::parseCodeplugMetaTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <meta>)"
      R"(    <name>Example Codeplug</name>)"
      R"(    <description>An Example Codeplug</description>)"
      R"(    <version>1.0.0-1</version>)"
      R"(  </meta>)"
      R"(</codeplug>)";

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QCOMPARE(codeplug->meta().name(), "Example Codeplug");
  QCOMPARE(codeplug->meta().description(), "An Example Codeplug");
  QCOMPARE(codeplug->meta().firmwareVersion(), "1.0.0-1");
}

void
PatternParserTest::parseUnusedFieldTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <unknown at="0" size="1:7"/>)"
      R"(</codeplug>)";

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());
  QCOMPARE(codeplug->numChildPattern(), 1);
  QVERIFY(codeplug->childPattern(0)->is<UnknownFieldPattern>());
  QCOMPARE(codeplug->childPattern(0)->address(), Address::zero());
  QCOMPARE(codeplug->childPattern(0)->size(), Size::fromByte(1,7));
}


void
PatternParserTest::parseIntFieldTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <int at="0" width=":40" format="unsigned" endian="big" min="0" max="1024" default="5">)"
      R"(    <meta>)"
      R"(      <name>Integer</name>)"
      R"(      <description>Some integer</description>)"
      R"(    </meta>)"
      R"(  </int>)"
      R"(  <bcd8 endian="big" at="4"/>)"
      R"(  <uint8 at="8"/>)"
      R"(  <uint16le  at="9"/>)"
      R"(  <uint32le  at="b"/>)"
      R"(</codeplug>)";

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QCOMPARE(codeplug->numChildPattern(), 5);
  QVERIFY(codeplug->childPattern(0)->is<IntegerFieldPattern>());
  QCOMPARE(codeplug->childPattern(0)->address(), Address::zero());
  QCOMPARE(codeplug->childPattern(0)->size(), Size::fromByte(4));

  QVERIFY(codeplug->childPattern(1)->is<IntegerFieldPattern>());
  QCOMPARE(codeplug->childPattern(1)->address(), Address::fromByte(4));
  QCOMPARE(codeplug->childPattern(1)->size(), Size::fromByte(4));

  QVERIFY(codeplug->childPattern(2)->is<IntegerFieldPattern>());
  QCOMPARE(codeplug->childPattern(2)->address(), Address::fromByte(8));
  QCOMPARE(codeplug->childPattern(2)->size(), Size::fromByte(1));

  QVERIFY(codeplug->childPattern(3)->is<IntegerFieldPattern>());
  QCOMPARE(codeplug->childPattern(3)->address(), Address::fromByte(9));
  QCOMPARE(codeplug->childPattern(3)->size(), Size::fromByte(2));

  QVERIFY(codeplug->childPattern(4)->is<IntegerFieldPattern>());
  QCOMPARE(codeplug->childPattern(4)->address(), Address::fromByte(11));
  QCOMPARE(codeplug->childPattern(4)->size(), Size::fromByte(4));
}


void
PatternParserTest::parseEnumFieldTest() {
  const char *content =
      R"(<?xml version="1.0"?>)"
      R"(<codeplug>)"
      R"(  <enum at="0" width=":8">)"
      R"(    <item value="0">)"
      R"(      <name>default</name>)"
      R"(      <description>Default selection</description>)"
      R"(    </item>)"
      R"(  </enum>)"
      R"(</codeplug>)";

  QXmlStreamReader reader(QByteArray::fromRawData(content, strlen(content)));
  CodeplugPatternParser parser;

  if (! parser.parse(reader)) {
    QFAIL(parser.errorMessage().toLatin1().constData());
  }

  CodeplugPattern *codeplug = parser.popAs<CodeplugPattern>();
  QVERIFY(codeplug->verify());

  QCOMPARE(codeplug->numChildPattern(), 1);
  QVERIFY(codeplug->childPattern(0)->is<EnumFieldPattern>());
  EnumFieldPattern *enumPattern = codeplug->childPattern(0)->as<EnumFieldPattern>();
  QCOMPARE(enumPattern->numItems(), 1);
  EnumFieldPatternItem *item = enumPattern->item(0);
  QCOMPARE(item->name(), "default");
  QCOMPARE(item->description(), "Default selection");
}


QTEST_MAIN(PatternParserTest)
#include "pattern_parser_test.moc"
