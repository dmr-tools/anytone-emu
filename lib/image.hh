#ifndef IMAGE_HH
#define IMAGE_HH

#include <QObject>
#include <QVector>
#include "offset.hh"

class ImageAnnotation;
class CodeplugPattern;

class Element: public QObject
{
  Q_OBJECT

public:
  explicit Element(const Address &address, uint32_t size = 0, QObject *parent=nullptr);
  explicit Element(const Address &address, const QByteArray &data, QObject *parent=nullptr);

  bool operator <= (const Element &other) const;
  bool operator <  (const Element &other) const;
  bool operator == (const Element &other) const;
  bool operator != (const Element &other) const;

  const Address &address() const;
  Size size() const;
  bool extends(uint32_t address) const;
  bool extends(const Address &address) const;
  bool contains(uint32_t address, uint32_t size=0) const;
  bool contains(const Address &address, const Size &size=Size::zero()) const;

  const QByteArray &data() const;
  const uint8_t *data(uint32_t address) const;
  const uint8_t *data(const Address &address) const;
  void append(const QByteArray &data);

signals:
  void modified(uint32_t address);

protected:
  Address _address;
  QByteArray _data;
};


class Image : public QObject
{
  Q_OBJECT

public:
  explicit Image(const QString &label="", QObject *parent = nullptr);

  unsigned int count() const;
  const Element *element(unsigned int idx) const;
  Element *find(const Address &address) const;
  Element *findPred(const Address &address) const;

  const uint8_t *data(uint32_t address) const;
  const uint8_t *data(const Address &address) const;
  void append(uint32_t address, const QByteArray &data);
  void append(const Address &address, const QByteArray &data);

  const QString &label() const;
  void setLabel(const QString &label);

  bool annotate(const CodeplugPattern *pattern);
  const ImageAnnotation *annotations() const;

signals:
  void modified(unsigned int image, uint32_t address);

protected:
  void add(Element *el);
  unsigned int findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const;
  unsigned int findInsertionIndex(const Address &address, unsigned int a, unsigned int b) const;
protected:
  QString _label;
  QVector<Element *> _elements;
  ImageAnnotation *_annotations;
};


class Collection: public QObject
{
  Q_OBJECT

public:
  explicit Collection(QObject *parent = nullptr);

  unsigned int count() const;
  const Image *image(unsigned int idx) const;
  int indexOf(const Image *img) const;

  void append(Image *image);

signals:
  void imageAdded(unsigned int idx);
  void imageRemoved(unsigned int idx);

protected slots:
  void onImageDeleted(QObject *obj);

protected:
  QVector<Image *> _images;
};


#endif // IMAGE_HH

