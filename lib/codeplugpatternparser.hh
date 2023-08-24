#ifndef CODEPLUGPATTERNPARSER_HH
#define CODEPLUGPATTERNPARSER_HH

#include "xmlparser.hh"
#include <QObject>

class AbstractPattern;

class CodeplugPatternParser: public XmlParser
{
  Q_OBJECT

protected:
  enum class State {
    None, MetaName, MetaDescription, MetaFWVersion
  };

public:
  explicit CodeplugPatternParser(QObject *parent = nullptr);

  template<class T>
  bool topIs() const {
    if (_stack.isEmpty())
      return false;
    return nullptr != dynamic_cast<T *>(_stack.back());
  }

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

  template<class T>
  T *topAs() {
    if (_stack.isEmpty())
      return nullptr;

    return dynamic_cast<T *>(_stack.back());
  }

protected:
  bool endDocument();

  bool processText(const QStringView &content);

protected slots:
  bool beginMetaElement(const QXmlStreamAttributes &attributes);
  bool endMetaElement();
  bool beginNameElement(const QXmlStreamAttributes &attributes);
  bool endNameElement();
  bool beginDescriptionElement(const QXmlStreamAttributes &attributes);
  bool endDescriptionElement();
  bool beginVersionElement(const QXmlStreamAttributes &attributes);
  bool endVersionElement();

  bool beginCodeplugElement(const QXmlStreamAttributes &attributes);
  bool endCodeplugElement();

  bool beginRepeatElement(const QXmlStreamAttributes &attributes);
  bool endRepeatElement();

  bool beginElementElement(const QXmlStreamAttributes &attributes);
  bool endElementElement();

  bool beginUnusedElement(const QXmlStreamAttributes &attributes);
  bool endUnusedElement();
  bool beginUnknownElement(const QXmlStreamAttributes &attributes);
  bool endUnknownElement();

  bool beginIntElement(const QXmlStreamAttributes &attributes);
  bool endIntElement();

  bool beginBitElement(const QXmlStreamAttributes &attributes);
  bool endBitElement();

  bool beginBcdElement(const QXmlStreamAttributes &attributes);
  bool endBcdElement();

  bool beginBcd8Element(const QXmlStreamAttributes &attributes);
  bool endBcd8Element();

  bool beginUintElement(const QXmlStreamAttributes &attributes);
  bool endUintElement();

  bool beginInt8Element(const QXmlStreamAttributes &attributes);
  bool endInt8Element();

  bool beginUint8Element(const QXmlStreamAttributes &attributes);
  bool endUint8Element();

  bool beginUint16leElement(const QXmlStreamAttributes &attributes);
  bool endUint16leElement();

  bool beginUint16beElement(const QXmlStreamAttributes &attributes);
  bool endUint16beElement();

  bool beginUint32leElement(const QXmlStreamAttributes &attributes);
  bool endUint32leElement();

  bool beginEnumElement(const QXmlStreamAttributes &attributes);
  bool endEnumElement();
  bool beginItemElement(const QXmlStreamAttributes &attributes);
  bool endItemElement();

  bool processDefaultArgs(const QXmlStreamAttributes &attributes);

protected:
  void push(QObject *element);
  QObject *pop();

protected:
  State _state;
  QList<QObject *> _stack;
};

#endif // CODEPLUGPATTERNPARSER_HH
