#ifndef IMAGE_HH
#define IMAGE_HH

#include <QObject>
#include <QVector>

class ImageAnnotation;
class CodeplugPattern;

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
  explicit Image(const QString &label="", QObject *parent = nullptr);

  unsigned int count() const;
  const Element *element(unsigned int idx) const;
  Element *findPred(uint32_t address) const;

  const uint8_t *data(uint32_t address) const;
  void append(uint32_t address, const QByteArray &data);

  const QString &label() const;
  void setLabel(const QString &label);

  bool annotate(const CodeplugPattern *pattern);
  const ImageAnnotation *annotations() const;

signals:
  void modified(unsigned int image, uint32_t address);

protected:
  void add(Element *el);
  unsigned int findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const;

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

