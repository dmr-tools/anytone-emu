#ifndef __TEST_ADDRESS_TEST_HH__
#define __TEST_ADDRESS_TEST_HH__

#include <QTest>

class AddressTest: public QObject
{
  Q_OBJECT

public:
  explicit AddressTest(QObject *parent=nullptr);

private slots:
  void testOffset();
};

#endif // __TEST_ADDRESS_TEST_HH__
