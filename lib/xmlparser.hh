#ifndef XMLPARSER_HH
#define XMLPARSER_HH

#include <QString>
#include <QObject>
#include <QList>
#include <QUrl>
#include <QDir>

class XmlParser;
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
  /** Returns @c true, if the handler can process the start of this element. */
  virtual bool canBeginElement(const QStringView &name) const;
  /** Handles the start of an element.
   * Dispatches to the corresponding slot based on the tag name. */
  virtual bool beginElement(const QStringView &name, const QXmlStreamAttributes &attributes);
  /** Returns @c true, if the handler can process the end of this element. */
  virtual bool canEndElement(const QStringView &name) const;
  /** Handles the end of an element.
   * Dispatches to the corresponding slot based on the tag name. */
  virtual bool endElement(const QStringView &name);

  /** Handles some character data. */
  virtual bool processCDATA(const QStringView &content);
  /** Handles some text. */
  virtual bool processText(const QStringView &content);

  /** Raises an error (not an exception). */
  void raiseError(const QString &message);

protected:
  /** Resolves the root parser of the handler stack. */
  XmlParser *parser();
  const XmlParser *parser() const;

  /** Puts the given handler on the top of the stack. */
  virtual void pushHandler(XmlElementHandler *parser);
  /** Returns the first handler from the stack. */
  virtual XmlElementHandler *topHandler() const;
  /** Pops the first handler from the stack. */
  virtual XmlElementHandler *popHandler();

  /** Retruns the content of the curren text buffer. */
  const QString &textBuffer() const;
  /** Clears the current text buffer. */
  void clearTextBuffer();

protected:
  /** The current error Message. */
  QString _errorMessage;

  /** Holds the current text buffer, gets filled by processCDATA, processText. */
  QString _textBuffer;

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
    /** Default constructor. */
    Context();
    /** From file info. */
    Context(const QFileInfo &info, unsigned int line=0, unsigned int column=0);
    /** Constructor from filename, line and column. */
    explicit Context(const QString &filename, unsigned int line=0, unsigned int column=0);
    /** Copy constructor. */
    Context(const Context &other) = default;
    /** Assignement operator. */
    Context &operator= (const Context &other) = default;

    /** Updates the contexts position. */
    void setPosition(unsigned int line, unsigned int column);
    QString filename() const;
    /** Returns the directory portion of the path. */
    QDir directory() const;

  protected:
    /** The current filename. */
    QFileInfo _fileInfo;
    /** The current line number. */
    unsigned int _line;
    /** The current column number. */
    unsigned int _column;
  };

public:
  /** Default constructor. */
  explicit XmlParser(QObject *parent=nullptr);

  /** Parses a XML document using the given XML stream reader. */
  virtual bool parse(QXmlStreamReader &reader, const Context &context, bool ignoreDocumentToken=false);

  Context context() const;

protected:
  /** Handles the start of a document. */
  virtual bool beginDocument();
  /** Handles the end of a document. */
  virtual bool endDocument();

  /** Handles the start of an element.
   * Dispatches to the corresponding slot based on the tag name of the current handler. */
  virtual bool dispatchBeginElement(const QStringView &name, const QXmlStreamAttributes &attributes);
  /** Handles the end of an element.
   * Dispatches to the corresponding slot based on the tag name of the current handler. */
  virtual bool dispatchEndElement(const QStringView &name);

  /** Handles an XInclude element. */
  virtual bool handleInclude(const QXmlStreamAttributes &attributes);

  /** Puts the given handler on the top of the stack. */
  void pushHandler(XmlElementHandler *parser);
  /** Returns the first handler from the stack. */
  XmlElementHandler *topHandler() const;
  /** Pops the first handler from the stack. */
  XmlElementHandler *popHandler();

  /** The stack of handler.
   * This stack at least contains an instance of itself. */
  QList<XmlElementHandler *> _handler;
  /** The context stack. */
  QList<Context> _context;

  friend class XmlElementHandler;
};




#endif // XMLPARSER_HH
