/** @defgroup annotation Applying patterns and annotate memory.
 * This module collects all classes and functions that are used to annotate recevied codeplugs
 * using a set of pattern definitions. */
#ifndef CODEPLUGANNOTATION_HH
#define CODEPLUGANNOTATION_HH

#include <QObject>
#include <QVariant>
#include <QVector>
#include "offset.hh"
#include "pattern.hh"

class Image;
class Element;
class FieldAnnotation;



/** Annotation issue class.
 * Also implements @c QTextStream, hence messages can be assmebled and formatted easily.
 * @ingroup annotation */
class AnnotationIssue: public QTextStream
{
public:
  /** Possible severity issues. */
  enum Severity {
    /** Minor codeplug incompatebility. Incompleteness or minor issue concering the compatibility.
     * That is, the codeplug remains functional, although not all features are encoded/decoded
     * correctly. */
    Note,
    /** Codeplug incompatebility. Encoding/decoding codeplug might be functional. */
    Warning,
    /** Severe codeplug incompatebility. Decoding fails, encoding will likely produce non-functional
     * codeplug. */
    Error
  };

public:
  /** Constructs a new issue at the given address with the given severity and message.
   * The message can be altered later on. */
  AnnotationIssue(const Address &address, Severity severity, const QString &message="");
  /** Copy constructor. */
  AnnotationIssue(const AnnotationIssue &other);
  /** Default copying assignment operator. */
  AnnotationIssue &operator=(const AnnotationIssue &other);

  /** Returns the address. */
  const Address &address() const;
  /** Returns the severity. */
  Severity severity() const;
  /** Returns the message of the issue. */
  const QString &message() const;

private:
  /** The address of the issue. */
  Address _address;
  /** The severity of the issue. */
  Severity _severity;
  /** The current message. */
  QString _message;
};


/** A list of annotation issues.
 * @ingroup annotation */
class AnnotationIssues
{
public:
  /** An iterator over the issues. It is a const iterator, as issues are read-only. */
  typedef QList<AnnotationIssue>::const_iterator iterator;

public:
  /** Default constructor. */
  AnnotationIssues();
  /** Disable copying. */
  AnnotationIssues(const AnnotationIssues &other) = delete;

  /** Add an issue. */
  void add(const AnnotationIssue &issue);
  /** Returns the number of issues. */
  unsigned int numIssues() const;
  /** Returns the n-th issue. */
  const AnnotationIssue &issue(unsigned int n) const;
  /** Returns @c true, if there is an issue with the given severity. */
  bool has(AnnotationIssue::Severity severity) const;

  /** Returns an iterator, pointing to the first issue. */
  iterator begin() const;
  /** Returns an iterator, pointing past the last issue. */
  iterator end() const;

protected:
  /** The list of issues. */
  QList<AnnotationIssue> _issues;
};



/** Base class for all annotations.
 * An annotation describes the content of a particular piece of memory. To this end, only
 * @c BlockPattern generate annotations. Also, describing some specific memory section, it must
 * have an address and a size.
 * @ingroup annotation */
class AbstractAnnotation: public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor.
   * @param pattern Specifies the pattern, that generates this annotation.
   * @param addr Specifies the start address of the matched memory section.
   * @param size Specifies the size of the matched memory section.
   * @param parent Optional QObject parent.  */
  explicit AbstractAnnotation(const BlockPattern *pattern, const Address &addr, const Size &size, QObject *parent = nullptr);

public:
  /** Returns the address of the matched memory section. */
  virtual const Address &address() const;
  /** Returns the size of the matched memory section. */
  virtual const Size &size() const;
  /** Returns @c true, if the given address lays within the matched memory section, that is between
   * @c address() and @c address()+size(). */
  virtual bool contains(const Address &addr) const;
  /** Recursively resolves the given address to the field, that contains this address.
   * If this annotation is a structured annotation, this method may not return an immediate child
   * annotation. Use @c StructutedAnnotation::at() for that. */
  virtual const FieldAnnotation *resolve(const Address& addr) const = 0;

  /** Returns the pattern, that generated this annotation. */
  const BlockPattern *pattern() const;

  /** Returns the list of names of the pattern of this annotation and all its parents. */
  QStringList path() const;

  /** Returns @c true if the annotation produced any issues. */
  bool hasIssues() const;
  /** Returns the list of annotation issues. */
  const AnnotationIssues &issues() const;
  /** Returns the list of annotation issues. */
  AnnotationIssues &issues();

public:
  /** Returns @c true, if the this instance can be cast to the given template argument. */
  template <class T>
  bool is() const {
    return nullptr != qobject_cast<const T*>(this);
  }

  /** Casts the this instance to the template argument. */
  template<class T>
  const T* as() const {
    return qobject_cast<const T *>(this);
  }

  /** Casts the this instance to the template argument. */
  template<class T>
  T* as() {
    return qobject_cast<T *>(this);
  }

private slots:
  /** Internal slot, handling the deletion of the source pattern. */
  void onPatternDeleted();

protected:
  /** THe address of the match. */
  Address _address;
  /** The size of the match. */
  Size _size;
  /** A weak reference to the source pattern. */
  const BlockPattern *_pattern;
  /** The list of annotation issues. */
  AnnotationIssues _issues;
};



/** Interface for a collection of annotations. This includes strctured annotations but also
 * @c Element instances, that are annotated. */
class AnnotationCollection
{
protected:
  /** Hidden constructor. */
  AnnotationCollection();

public:
  /** Returns @c true, if there are no child-annotations.
   * For @c Element instances, this means, that the element does not match any pattern or is
   * unannotated. */
  bool unAnnotated() const;
  /** Returns the number of annotations. */
  unsigned int numAnnotations() const;
  /** Returns the n-th annotation. */
  const AbstractAnnotation *annotation(unsigned int) const;
  /** Retunrs the annotation at the given address. It does not resolve the address recursively. */
  const AbstractAnnotation *annotationAt(const Address& addr) const;
  /** Adds an annotation. The ownership is taken. */
  virtual void addAnnotation(AbstractAnnotation *annotation);

protected:
  /** The vector of annotations. */
  QVector<AbstractAnnotation *> _annotations;
};



/** A structured annotation is one, that consists of other annotations. A structured annotation is
 * dense. That is, it does not contain unannotated memory.
 * @ingroup annotation */
class StructuredAnnotation: public AbstractAnnotation, public AnnotationCollection
{
  Q_OBJECT

public:
  /** Constructs a structured annotation. */
  explicit StructuredAnnotation(const BlockPattern *pattern, const Address &addr, QObject *parent = nullptr);

  /** Adds an annotation. Updates the size of this annotation. */
  void addAnnotation(AbstractAnnotation *child);
  /** Recursively resolves the given address to the field annotation containing this address. */
  const FieldAnnotation *resolve(const Address &addr) const;
};


/** Annotates an atomic section of memory. E.g., a value.
 * @ingroup annotation */
class FieldAnnotation: public AbstractAnnotation
{
  Q_OBJECT

public:
  /** Constructs a new field annotation for the given pattern, address and value. */
  explicit FieldAnnotation(const FieldPattern *pattern, const Address &addr,
                           const QVariant &value, QObject *parent = nullptr);

  /** Resolves to this instance, if the address is contained. */
  const FieldAnnotation *resolve(const Address &addr) const;

  /** Returns the decoded value. */
  const QVariant &value() const;

protected:
  /** The decoded value. */
  QVariant _value;
};


/** Just a namespace for all annotation functions.
 * @ingroup annotation */
class ImageAnnotator
{
public:
  /** Annotates all elements in the given image using the given codeplug pattern. */
  static bool annotate(const Image *image, const CodeplugPattern *pattern);

protected:
  /** Annotates possibly several elements within the given image by applying the specified repeat
   *  pattern at the given address.
   * @returns @c false if the annotation fails. */
  static bool annotate(const Image *image, const RepeatPattern *pattern, const Address &address);
  /** Annotates the given element by applying the specified block pattern at the given address. */
  static bool annotate(AnnotationCollection &parent, const Element *element, const BlockPattern *pattern, const Address& addr);
  /** Annotates the given element by applying the specified repeat pattern at the given address. */
  static bool annotate(AnnotationCollection &parent, const Element *element, const BlockRepeatPattern *pattern, const Address &address);
  /** Annotates the given element by applying the specified repeat pattern at the given address. */
  static bool annotate(AnnotationCollection &parent, const Element *element, const FixedRepeatPattern *pattern, const Address &address);
  /** Annotates the given element by applying the specified element pattern at the given address. */
  static bool annotate(AnnotationCollection &parent, const Element *element, const ElementPattern *pattern, const Address &addr);
  /** Annotates the given element by applying the specified field pattern at the given address. */
  static bool annotate(AnnotationCollection &parent, const Element *element, const FieldPattern *pattern, const Address &address);
};


#endif // CODEPLUGANNOTATION_HH
