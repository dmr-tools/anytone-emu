#ifndef PATTERNPARSER_TEST_HH
#define PATTERNPARSER_TEST_HH

#include <QTest>

class PatternParserTest: public QObject
{
  Q_OBJECT

public:
  explicit PatternParserTest(QObject *parent=nullptr);

private slots:
  void parseCodeplugTest();
  void parseCodeplugMetaTest();
  void parseUnusedFieldTest();
  void parseIntFieldTest();
  void parseEnumFieldTest();
};


#endif // PATTERNPARSER_TEST_HH
