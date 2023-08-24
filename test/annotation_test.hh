#ifndef ANNOTATIONTEST_HH
#define ANNOTATIONTEST_HH

#include <QTest>

class AnnotationTest : public QObject
{
  Q_OBJECT

public:
  explicit AnnotationTest(QObject *parent = nullptr);

private slots:
  void annotateUnusedTest();
  void annotateUnknownTest();
  void annotateIntTest();
};

#endif // ANNOTATIONTEST_HH
