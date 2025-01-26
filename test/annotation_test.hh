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
  void annotateFixedRepeatTest();
  void annotateBlockRepeatTest();
  /** Regression test for #76. */
  void annotateEmptyBlockRepeatTest();
  void annotateSparseRepeatTest();
};

#endif // ANNOTATIONTEST_HH
