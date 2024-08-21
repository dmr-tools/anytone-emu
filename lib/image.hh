/** @defgroup codeplug Codeplug memory representation.
 * This module collects all classes that hold the received codeplug memory. */
#ifndef IMAGE_HH
#define IMAGE_HH

#include <QObject>
#include <QVector>
#include "offset.hh"
#include "annotation.hh"

class CodeplugPattern;


/** A continuous element of codeplug memory.
 *
 * @ingroup codeplug */
class Element: public QObject, public AnnotationCollection
{
  Q_OBJECT

public:
  /** Constructs an element of the specified size starting at the given address. */
  explicit Element(const Address &address, uint32_t size = 0, QObject *parent=nullptr);
  /** Constructs an element from the given data at the specified address. */
  explicit Element(const Address &address, const QByteArray &data, QObject *parent=nullptr);

  /** Comparison operator wrt the address. */
  bool operator <= (const Element &other) const;
  /** Comparison operator wrt the address. */
  bool operator <  (const Element &other) const;
  /** Comparison operator wrt the address and data. */
  bool operator == (const Element &other) const;
  /** Comparison operator wrt the address. */
  bool operator != (const Element &other) const;

  /** Returns the address of the element. */
  const Address &address() const;
  /** Returns the size of the element. */
  Size size() const;
  /** Extends the element to the specified address. */
  bool extends(uint32_t address) const;
  /** Extends the element to the specified address. */
  bool extends(const Address &address) const;
  /** Returns @c true if the element contains the given address and the specified number of
   *  bytes. */
  bool contains(uint32_t address, uint32_t size=0) const;
  /** Returns @c true if the element contains the given address and the specified size. */
  bool contains(const Address &address, const Size &size=Size::zero()) const;

  /** Returns the data of the element. */
  const QByteArray &data() const;
  /** Returns the data of the element at the specified address. */
  const uint8_t *data(uint32_t address) const;
  /** Returns the data of the element at the specified address. */
  const uint8_t *data(const Address &address) const;
  /** Appends some data to the element. */
  void append(const QByteArray &data);

  void addAnnotation(AbstractAnnotation *annotation);

signals:
  /** Get emitted, if the element is modified at the specified address. */
  void modified(uint32_t address);

protected:
  /** The start address of the element. */
  Address _address;
  /** The content of the element. */
  QByteArray _data;
};


/** An entire codeplug image.
 *
 * Each image contains at least one @c Element of codeplug memory.
 *
 * @ingroup codeplug */
class Image : public QObject
{
  Q_OBJECT

public:
  /** Constructs a new image.
   * @param label Specifies an optional label for the image.
   * @param parent The QObject parent. */
  explicit Image(const QString &label="", QObject *parent = nullptr);

  /** Returns the number of elements in that image. */
  unsigned int count() const;
  /** Returns the element at the given index. */
  const Element *element(unsigned int idx) const;
  /** Find the element containing the given address.
   * Returns @c nullptr if there is none. */
  Element *find(const Address &address) const;
  /** Find the element that precedes the given address.
   * Returns @c nullptr if there is none. */
  Element *findPred(const Address &address) const;

  /** Returns a pointer to the data at the specified address.
   * Returns @c nullptr if there is no element containing this address. */
  const uint8_t *data(uint32_t address) const;
  /** Returns a pointer to the data at the specified address.
   * Returns @c nullptr if there is no element containing this address. */
  const uint8_t *data(const Address &address) const;

  /** Appends some data to the image.
   * This may extend an existing element if the address points to the end of an element. If not, a
   * new element is added to the image. */
  void append(uint32_t address, const QByteArray &data);
  /** Appends some data to the image.
   * This may extend an existing element if the address points to the end of an element. If not, a
   * new element is added to the image. */
  void append(const Address &address, const QByteArray &data);

  /** Returns the label of the image. */
  const QString &label() const;
  /** Sets the label of the image. */
  void setLabel(const QString &label);

  /** Annotates the image using the given pattern. */
  bool annotate(const CodeplugPattern *pattern);

signals:
  /** Gets emitted when the image is modified at the specified address. */
  void modified(unsigned int image, uint32_t address);
  /** Gets emitted when the image is annotated. */
  void annotated(Image *img);

protected:
  /** Helper function to insert a new element into the image. */
  void add(Element *el);
  /** Binary search for insertion index of the address. */
  unsigned int findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const;
  /** Binary search for insertion index of the address. */
  unsigned int findInsertionIndex(const Address &address, unsigned int a, unsigned int b) const;

protected:
  /** The label of the image. */
  QString _label;
  /** The elements of the image, sorted by ascending address. */
  QVector<Element *> _elements;
};



/** A collection of several received codeplug images.
 * @ingroup codeplug */
class Collection: public QObject
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit Collection(QObject *parent = nullptr);

  /** Retunrs the number of images received. */
  unsigned int count() const;
  /** Returns the image at the specified index. */
  const Image *image(unsigned int idx) const;
  /** Searches for the index of the given image. */
  int indexOf(const Image *img) const;

  /** Appends an image. */
  void append(Image *image);

signals:
  /** Gets emitted when an image is added. */
  void imageAdded(unsigned int idx);
  /** Gets emitted when an image is removed. */
  void imageRemoved(unsigned int idx);
  /** Gets emitted when an image is annotated. */
  void imageAnnotated(unsigned int idx);

protected slots:
  /** Internal callback on deleted images. */
  void onImageDeleted(QObject *obj);
  /** Internal callback on image annotations. */
  void onImageAnnotated(Image *img);

protected:
  /** The set of images. */
  QVector<Image *> _images;
};


#endif // IMAGE_HH

