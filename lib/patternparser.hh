#ifndef PATTERNPARSER_HH
#define PATTERNPARSER_HH

#include "xmlparser.hh"
#include <QObject>

class AbstractPattern;

/** Base class of all parser for XML codeplug pattern definitions. */
class PatternParser: public XmlParser
{
  Q_OBJECT

protected:
  /** Possible states of the parser. */
  enum class State {
    None,            ///< No special state, aka start.
    MetaName,        ///< Parse meta information name.
    MetaDescription, ///< Parse meta information description.
    MetaFWVersion    ///< Parse meta information firmware version.
  };

protected:
  /** Hidden constructor. */
  explicit PatternParser(QObject *parent = nullptr);

public:
  /** Tests if the top element on the stack can be casted to the template argument. */
  template<class T>
  bool topIs() const {
    if (_stack.isEmpty())
      return false;
    return nullptr != dynamic_cast<T *>(_stack.back());
  }

  /** Pops the top element on the stack and cast it the template argument. */
  template<class T>
  T *popAs() {
    if (_stack.isEmpty())
      return nullptr;

    QObject *obj = _stack.back(); _stack.pop_back();

    T *casted = dynamic_cast<T *>(obj);
    if (nullptr == casted) {
      delete obj;
      return nullptr;
    }

    obj->setParent(nullptr);
    return casted;
  }

  /** Returns but does not remove the top element on the stack and cast it the template argument. */
  template<class T>
  T *topAs() {
    if (_stack.isEmpty())
      return nullptr;

    return dynamic_cast<T *>(_stack.back());
  }

protected:
  bool processText(const QStringView &content);

protected slots:
  /** Handler for start of meta element. */
  bool beginMetaElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of meta element. */
  bool endMetaElement();
  /** Handler for start of name element. */
  bool beginNameElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of name element. */
  bool endNameElement();
  /** Handler for start of description element. */
  bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of description element. */
  bool endDescriptionElement();
  /** Handler for start of version element. */
  bool beginVersionElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of version element. */
  bool endVersionElement();
  /** Handler for start of done element. */
  bool beginDoneElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of done element. */
  bool endDoneElement();
  /** Handler for start of needs-review element. */
  bool beginNeedsReviewElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of needs-review element. */
  bool endNeedsReviewElement();
  /** Handler for start of incomplete element. */
  bool beginIncompleteElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of incomplete element. */
  bool endIncompleteElement();

  /** Handler for start of repeat element. */
  bool beginRepeatElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of repeat element. */
  bool endRepeatElement();

  /** Handler for start of element element. */
  bool beginElementElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of element element. */
  bool endElementElement();

  /** Handler for start of unused element. */
  bool beginUnusedElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of unused element. */
  bool endUnusedElement();
  /** Handler for start of unknown element. */
  bool beginUnknownElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of unknown element. */
  bool endUnknownElement();

  /** Handler for start of int element. */
  bool beginIntElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of int element. */
  bool endIntElement();

  /** Handler for start of bit element. */
  bool beginBitElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of bit element. */
  bool endBitElement();

  /** Handler for start of bcd element. */
  bool beginBcdElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of bcd element. */
  bool endBcdElement();

  /** Handler for start of bcd8 element. */
  bool beginBcd8Element(const QXmlStreamAttributes &attributes);
  /** Handler for end of bcd8 element. */
  bool endBcd8Element();

  /** Handler for start of uint element. */
  bool beginUintElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint element. */
  bool endUintElement();

  /** Handler for start of int8 element. */
  bool beginInt8Element(const QXmlStreamAttributes &attributes);
  /** Handler for end of int8 element. */
  bool endInt8Element();
  /** Handler for start of uint8 element. */
  bool beginUint8Element(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint8 element. */
  bool endUint8Element();

  /** Handler for start of uint16 element. */
  bool beginUint16Element(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint16 element. */
  bool endUint16Element();
  /** Handler for start of uint16le element. */
  bool beginUint16leElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint16le element. */
  bool endUint16leElement();
  /** Handler for start of uint16be element. */
  bool beginUint16beElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint16be element. */
  bool endUint16beElement();

  /** Handler for start of uint32 element. */
  bool beginUint32Element(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint32 element. */
  bool endUint32Element();
  /** Handler for start of uint32le element. */
  bool beginUint32leElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint32le element. */
  bool endUint32leElement();
  /** Handler for start of uint32be element. */
  bool beginUint32beElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of uint32be element. */
  bool endUint32beElement();

  /** Handler for start of enum element. */
  bool beginEnumElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of enum element. */
  bool endEnumElement();
  /** Handler for start of item element. */
  bool beginItemElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of item element. */
  bool endItemElement();

  /** Handler for start of string element. */
  bool beginStringElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of string element. */
  bool endStringElement();

  /** Handle default element attributes. */
  bool processDefaultArgs(const QXmlStreamAttributes &attributes);

protected:
  /** Push an element on the stack. */
  void push(QObject *element);
  /** Pop an element from the stack. */
  QObject *pop();

protected:
  /** The current state. */
  State _state;
  /** A stack of objects. */
  QList<QObject *> _stack;
};

#endif // PATTERNPARSER_HH
