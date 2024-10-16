#ifndef XMLPARSER_HH
#define XMLPARSER_HH

#include <QString>
#include <QObject>
#include <QList>
#include <QUrl>


class QStringView;
class QXmlStreamReader;
class QXmlStreamAttributes;


class XmlElementHandler: public QObject
{
  Q_OBJECT

protected:
  explicit XmlElementHandler(QObject *parent=nullptr);

public:
  /** Returns the current error message. */
  const QString &errorMessage() const;

protected:
  /** Handles the start of an element.
   * Dispatches to the corresponding slot based on the tag name. */
  virtual bool beginElement(const QStringView &name, const QXmlStreamAttributes &attributes);
  /** Handles the end of an element.
   * Dispatches to the corresponding slot based on the tag name. */
  virtual bool endElement(const QStringView &name);

  /** Handles some character data. */
  virtual bool processCDATA(const QStringView &name);
  /** Handles some text. */
  virtual bool processText(const QStringView &name);

  /** Raises an error (not an exception). */
  void raiseError(const QString &message);

  /** Puts the given handler on the top of the stack. */
  void pushHandler(XmlElementHandler *parser);
  /** Returns the first handler from the stack. */
  XmlElementHandler *topHandler() const;
  /** Pops the first handler from the stack. */
  XmlElementHandler *popHandler();

protected:
  /** The current error Message. */
  QString _errorMessage;
  /** The stack of handler.
   * This stack at least contains an instance of itself. */
  QList<XmlElementHandler *> _handler;

  friend class XmlParser;
};



/** Nicer XML SAX parser.
 *
 * Calls @c beginTagElement and @c endTagElement slots (where @c Tag is the tag name) for each
 * tag encountered. If a slot is not found, an error is raised.
 *
 * @ingroup utils */
class XmlParser: public XmlElementHandler
{
  Q_OBJECT

public:
  /** Current parser context. */
  class Context
  {
  public:
    /** Constructor from filename, line and column. */
    explicit Context(const QString &filename, unsigned int line, unsigned int column);
    /** Copy constructor. */
    Context(const Context &other) = default;
    /** Assignement operator. */
    Context &operator= (const Context &other) = default;

  protected:
    /** The current filename. */
    QString _filename;
    /** The current line number. */
    unsigned int _line;
    /** The current column number. */
    unsigned int _column;
  };

public:
  /** Default constructor. */
  explicit XmlParser(QObject *parent=nullptr);

  /** Parses a XML document using the given XML stream reader. */
  virtual bool parse(QXmlStreamReader &reader);

protected:
  /** Handles the start of a document. */
  virtual bool beginDocument();
  /** Handles the end of a document. */
  virtual bool endDocument();
};



class XmlTextHandler: public XmlElementHandler
{
  Q_OBJECT

public:
  explicit XmlTextHandler(XmlElementHandler *parent);

  const QString &content() const;

protected:
  bool processText(const QStringView &content);

protected:
  QString _content;
};


class XmlUrlHandler: public XmlElementHandler
{
  Q_OBJECT

public:
  explicit XmlUrlHandler(XmlElementHandler *parent);

  const QUrl &url() const;

protected:
  bool processText(const QStringView &content);

protected:
  QUrl _url;
};


class XmlHexDataHandler: public XmlElementHandler
{
  Q_OBJECT

public:
  explicit XmlHexDataHandler(XmlElementHandler *parent);

  const QByteArray &data() const;

protected:
  bool processText(const QStringView &content);

protected:
  QByteArray _data;
};


#endif // XMLPARSER_HH
