#ifndef CODEPLUGANNOTATION_HH
#define CODEPLUGANNOTATION_HH

#include <QObject>
#include <QVariant>
#include <QVector>
#include "offset.hh"

class Image;
class Element;
class AbstractPattern;
class CodeplugPattern;
class RepeatPattern;
class BlockPattern;
class BlockRepeatPattern;
class FixedRepeatPattern;
class ElementPattern;
class FieldPattern;
class FieldAnnotation;


class AbstractAnnotation: public QObject
{
  Q_OBJECT

protected:
  explicit AbstractAnnotation(const BlockPattern *pattern, const Address &addr, const Size &size, QObject *parent = nullptr);

public:
  virtual const Address &address() const;
  virtual const Size &size() const;
  virtual bool contains(const Address &addr) const;
  virtual const FieldAnnotation *resolve(const Address& addr) const = 0;

  const BlockPattern *pattern() const;

  QStringList path() const;

public:
  template <class T>
  bool is() const {
    return nullptr != qobject_cast<const T*>(this);
  }

  template<class T>
  const T* as() const {
    return qobject_cast<const T *>(this);
  }

  template<class T>
  T* as() {
    return qobject_cast<T *>(this);
  }

private slots:
  void onPatternDeleted();

protected:
  Address _address;
  Size _size;
  const BlockPattern *_pattern;
};


class AnnotationCollection
{
protected:
  AnnotationCollection();

public:
  bool isEmpty() const;
  unsigned int numChildren() const;
  const AbstractAnnotation *child(unsigned int) const;
  const AbstractAnnotation *at(const Address& addr) const;

  virtual void addChild(AbstractAnnotation *annotation);

protected:
  QVector<AbstractAnnotation *> _annotations;
};


class StructuredAnnotation: public AbstractAnnotation, public AnnotationCollection
{
  Q_OBJECT

public:
  explicit StructuredAnnotation(const BlockPattern *pattern, const Address &addr, QObject *parent = nullptr);

  void addChild(AbstractAnnotation *child);
  const FieldAnnotation *resolve(const Address &addr) const;
};


class FieldAnnotation: public AbstractAnnotation
{
  Q_OBJECT

public:
  explicit FieldAnnotation(const FieldPattern *pattern, const Address &addr, const QVariant &value, QObject *parent = nullptr);

  const FieldAnnotation *resolve(const Address &addr) const;

  const QVariant &value() const;

protected:
  QVariant _value;
};


class ImageAnnotation: public QObject, public AnnotationCollection
{
  Q_OBJECT

public:
  explicit ImageAnnotation(const Image *image, const CodeplugPattern *pattern, QObject *parent=nullptr);

  void addChild(AbstractAnnotation *child);
  const FieldAnnotation *resolve(const Address &addr) const;

protected:
  static bool annotate(AnnotationCollection &parent, const Image *image, const CodeplugPattern *pattern);
  static bool annotate(AnnotationCollection &parent, const Image *image, const RepeatPattern *pattern, const Address &address);
  static bool annotate(AnnotationCollection &parent, const Element *element, const BlockPattern *pattern, const Address& addr);
  static bool annotate(AnnotationCollection &parent, const Element *element, const BlockRepeatPattern *pattern, const Address &address);
  static bool annotate(AnnotationCollection &parent, const Element *element, const FixedRepeatPattern *pattern, const Address &address);
  static bool annotate(AnnotationCollection &parent, const Element *element, const ElementPattern *pattern, const Address &addr);
  static bool annotate(AnnotationCollection &parent, const Element *element, const FieldPattern *pattern, const Address &address);

protected:
  const Image *_image;
  const CodeplugPattern *_pattern;
};


#endif // CODEPLUGANNOTATION_HH
