#ifndef PATTERNDEFINITION_HH
#define PATTERNDEFINITION_HH

#include <QObject>
#include "codeplugpattern.hh"

class AbstractPatternDefinition;


class PatternDefinitionLibrary: public QObject
{
  Q_OBJECT

public:
  explicit PatternDefinitionLibrary(QObject *parent=nullptr);

  bool add(const QString &qname, AbstractPatternDefinition *definition);
  bool has(const QString &qname) const;
  AbstractPatternDefinition *get(const QString &qname) const;

protected:
  QHash<QString, QObject *> _elements;
};


class AbstractPatternDefinition: public QObject
{
  Q_OBJECT

protected:
  explicit AbstractPatternDefinition(QObject *parent=nullptr);

public:
  virtual bool verify() const = 0;
  virtual AbstractPattern *instantiate() const = 0;
  virtual bool serialize(QXmlStreamWriter &writer) const = 0;

  bool hasAddress() const;
  bool hasImplicitAddress() const;
  const Address &address() const;
  void setAddress(const Address &offset);

  const PatternMeta &meta() const;
  PatternMeta &meta();

  template <class T>
  bool is() const {
    return nullptr != dynamic_cast<const T *>(this);
  }

  template <class T>
  const T* as() const {
    return dynamic_cast<const T *>(this);
  }

  template <class T>
  T* as() {
    return dynamic_cast<T *>(this);
  }

signals:
  void modified(const AbstractPatternDefinition *pattern);
  void added(AbstractPatternDefinition *parent, unsigned int idx);
  void removing(AbstractPatternDefinition *parent, unsigned int idx);
  void removed(AbstractPatternDefinition *parent, unsigned int idx);

private slots:
  void onMetaModified();

private:
  PatternMeta _meta;
  Address _address;
};


class StructuredPatternDefinition
{
protected:
  StructuredPatternDefinition();

public:
  virtual ~StructuredPatternDefinition();

  virtual int indexOf(const AbstractPattern *pattern) const = 0;
  virtual unsigned int numChildPattern() const = 0;
  virtual bool addChildPattern(AbstractPattern *pattern) = 0;
  virtual AbstractPattern *childPattern(unsigned int n) const = 0;
  virtual bool deleteChild(unsigned int n) = 0;
};


#endif // PATTERNDEFINITION_HH
