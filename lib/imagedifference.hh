#ifndef IMAGEDIFFERENCE_HH
#define IMAGEDIFFERENCE_HH

#include "image.hh"


class ElementDifference: public QObject
{
  Q_OBJECT

protected:
  explicit ElementDifference(const Element *left=nullptr, const Element *right=nullptr, QObject *parent=nullptr);

public:
  template <class T>
  bool is() const {
    return nullptr != dynamic_cast<const T*>(this);
  }

  template <class T>
  const T* as() const {
    return dynamic_cast<const T *>(this);
  }

  template <class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

protected slots:
  void onElementDeleted(QObject *obj);

protected:
  const Element *_left;
  const Element *_right;
};


class AddElement: public ElementDifference
{
  Q_OBJECT

public:
  explicit AddElement(const Element *b, QObject *parent=nullptr);
};


class DelElement: public ElementDifference
{
  Q_OBJECT

public:
  explicit DelElement(const Element *a, QObject *parent=nullptr);
};


class ModifyElement: public ElementDifference
{
  Q_OBJECT

public:
  struct Modification {
    enum Type { Add, Del, Mod };
    Type type;
    uint32_t offset;
    uint8_t added;
    uint8_t removed;

    inline bool isAdd() const { return Add == type; }
    inline bool isRem() const { return Del == type; }
    inline bool isMod() const { return Mod == type; }

    static Modification add(uint32_t at, uint8_t what);
    static Modification rem(uint32_t at, uint8_t what);
    static Modification mod(uint32_t at, uint8_t from, uint8_t to);
  };

public:
  explicit ModifyElement(const Element *a, const Element *b, QObject *parent=nullptr);


protected:
  QList<Modification> _changes;
};


class ImageDifference:  public QObject
{
  Q_OBJECT

public:
  explicit ImageDifference(const Image *a, const Image *b, QObject *parent = nullptr);

  unsigned int count() const;
  const ElementDifference *diff(unsigned int n) const;

protected slots:
  void onImageDeleted(QObject *img);

protected:
  const Image *_a, *_b;
  QVector<ElementDifference*> _differences;
};

#endif // IMAGEDIFFERENCE_HH
