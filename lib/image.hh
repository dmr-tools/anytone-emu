#ifndef IMAGE_HH
#define IMAGE_HH

#include <QObject>
#include <QVector>


class Element: public QObject
{
  Q_OBJECT

public:
  explicit Element(uint32_t address, uint32_t size = 0, QObject *parent=nullptr);
  explicit Element(uint32_t address, const QByteArray &data, QObject *parent=nullptr);

  bool operator <= (const Element &other) const;
  bool operator <  (const Element &other) const;
  bool operator == (const Element &other) const;
  bool operator != (const Element &other) const;

  uint32_t address() const;
  uint32_t size() const;
  bool extends(uint32_t address) const;

  const QByteArray &data() const;
  const uint8_t *data(uint32_t address) const;
  void append(const QByteArray &data);

signals:
  void modified(uint32_t address);

protected:
  uint32_t _address;
  QByteArray _data;
};


class Image : public QObject
{
  Q_OBJECT

public:
  explicit Image(QObject *parent = nullptr);

  unsigned int count() const;
  const Element *element(unsigned int idx) const;

  const uint8_t *data(uint32_t address) const;
  void append(uint32_t address, const QByteArray &data);

signals:
  void modified(unsigned int image, uint32_t address);

protected:
  void add(Element *el);
  Element *findPred(uint32_t address) const;
  unsigned int findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const;

protected:
  QVector<Element *> _elements;
};


#endif // IMAGE_HH

