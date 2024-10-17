#ifndef MODELPARSERTEST_HH
#define MODELPARSERTEST_HH

#include <QObject>


class ModelParserTest : public QObject
{
  Q_OBJECT

public:
  explicit ModelParserTest(QObject *parent = nullptr);

private slots:
  void testRomAppend();
  void testRomOverride();
  void testRomExtend();
  void testRomMerge();

};

#endif // MODELPARSERTEST_HH
