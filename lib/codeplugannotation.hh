#ifndef CODEPLUGANNOTATION_HH
#define CODEPLUGANNOTATION_HH

#include <QObject>
#include <QVariant>
#include <QVector>
#include "offset.hh"

class Image;
class Element;
class CodeplugPattern;
class RepeatPattern;
class BlockPattern;
class BlockRepeatPattern;
class FixedRepeatPattern;
class ElementPattern;
class FieldPattern;


class FieldAnnotation: public QObject
{
  Q_OBJECT

public:
  explicit FieldAnnotation(const FieldPattern *pattern, const Address &addr, const QVariant &value, QObject *parent = nullptr);

  const Address &offset() const;
  const Size &size() const;
  bool contains(const Address &addr) const;
  const QVariant &value() const;

  const FieldPattern *pattern() const;

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
  const FieldPattern *_pattern;
  QVariant _value;
};


class ImageAnnotation: public QObject
{
  Q_OBJECT

public:
  explicit ImageAnnotation(const Image *image, const CodeplugPattern *pattern, QObject *parent=nullptr);

  bool isEmpty() const;

  FieldAnnotation *at(const Address& addr) const;

protected:
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Image *image, const CodeplugPattern *pattern);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Image *image, const RepeatPattern *pattern, const Address &address);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const BlockPattern *pattern, const Address& addr);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const BlockRepeatPattern *pattern, const Address &address);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const FixedRepeatPattern *pattern, const Address &address);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const ElementPattern *pattern, const Address &addr);
  static bool annotate(QVector<FieldAnnotation *> &annotations, const Element *element, const FieldPattern *pattern, const Address &address);

protected:
  const Image *_image;
  const CodeplugPattern *_pattern;
  QVector<FieldAnnotation *> _annotations;
};


#endif // CODEPLUGANNOTATION_HH
