#include "address_test.hh"

#include "offset.hh"


AddressTest::AddressTest(QObject *parent)
  : QObject(parent)
{
  // pass...
}


void
AddressTest::testOffset() {
  Address addr1 = Address::fromByte(0x00100), addr2 = Address::fromByte(0x00104);
  Offset offset1 = Offset::fromByte(1, 3), offset2 = Offset::fromByte(0, -3);

  QCOMPARE(addr2 - addr1, Offset::fromByte(4));
  QCOMPARE(addr1 - addr1, Offset::fromByte(0));
  QVERIFY((addr1 - addr2).isValid());
  QCOMPARE(addr1 - addr2, Offset::fromByte(-4));


  QCOMPARE(addr1 + offset1, Address::fromByte(0x00101, 4));
  QCOMPARE(addr1 - offset1, Address::fromByte(0x000fe, 2));

  QCOMPARE(addr1 + offset2, Address::fromByte(0x000ff, 2));
  QCOMPARE(addr1 - offset2, Address::fromByte(0x00100, 4));
}


QTEST_MAIN(AddressTest)
#include "address_test.moc"
