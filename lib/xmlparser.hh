#ifndef XMLPARSER_HH
#define XMLPARSER_HH

#include <QString>
#include <QObject>

class QStringView;
class QXmlStreamReader;
class QXmlStreamAttributes;


class XmlParser: public QObject
{
  Q_OBJECT

public:
  class Context
  {
  public:
    explicit Context(const QString &filename, unsigned int line, unsigned int column);
    Context(const Context &other) = default;
    Context &operator= (const Context &other) = default;

  protected:
    QString _filename;
    unsigned int _line;
    unsigned int _column;
  };

public:
  explicit XmlParser(QObject *parent=nullptr);

  virtual bool parse(QXmlStreamReader &reader);
  const QString &errorMessage() const;

protected:
  virtual bool beginDocument();
  virtual bool endDocument();

  virtual bool beginElement(const QStringView &name, const QXmlStreamAttributes &attributes);
  virtual bool endElement(const QStringView &name);

  virtual bool processCDATA(const QStringView &name);
  virtual bool processText(const QStringView &name);

  void raiseError(const QString &message);

protected:
  QString _errorMessage;
};

#endif // XMLPARSER_HH
