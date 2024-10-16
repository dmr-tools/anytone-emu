#include "xmlparser.hh"
#include <QXmlStreamReader>
#include <QRegularExpression>

XmlParser::XmlParser(QObject *parent)
  : QObject(parent)
{
  pushHandler(this);
}

bool
XmlParser::beginDocument() {
  return true;
}

bool
XmlParser::endDocument() {
  return true;
}

bool
XmlParser::beginElement(const QStringView &name, const QXmlStreamAttributes &attributes) {
  // Search for matching slot by name
  QStringList tagName = name.toString().split(QRegularExpression(R"([\-_.])"), Qt::SkipEmptyParts);
  for (int i=0; i<tagName.size(); i++)
    tagName[i][0] = tagName[i][0].toUpper();
  QString slotName = QString("begin%1Element").arg(tagName.join(""));

  bool ok;
  if (this->metaObject()->invokeMethod(
        this, slotName.toLatin1().constData(), Qt::DirectConnection,
        Q_RETURN_ARG(bool, ok),
        Q_ARG(QXmlStreamAttributes, attributes)))
    return ok;

  return true;
}

bool
XmlParser::endElement(const QStringView &name) {
  // Search for matching slot by name
  QString tagName = name.toString(); tagName[0] = tagName[0].toUpper();
  QString slotName = QString("end%1Element").arg(tagName);

  bool ok;
  if (this->metaObject()->invokeMethod(
        this, slotName.toLatin1().constData(), Qt::DirectConnection,
        Q_RETURN_ARG(bool, ok)))
    return ok;

  return true;
}

bool
XmlParser::processCDATA(const QStringView &name) {
  return true;
}

bool
XmlParser::processText(const QStringView &name) {
  return true;
}

bool
XmlParser::parse(QXmlStreamReader &reader) {
  if (_handler.isEmpty()) {
    raiseError(QString("Near %1:%2: No handler given to process elements.")
               .arg(reader.lineNumber())
               .arg(reader.errorString()));
    return false;
  }

  while (! reader.atEnd()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    switch(token) {
    case QXmlStreamReader::NoToken:
    case QXmlStreamReader::Invalid:
      continue;
    case QXmlStreamReader::StartDocument:
      if (! this->_handler.back()->beginDocument())
        reader.raiseError(errorMessage());
      continue;
    case QXmlStreamReader::EndDocument:
      if (! this->_handler.back()->endDocument())
        reader.raiseError(errorMessage());
      continue;
    case QXmlStreamReader::StartElement:
      if (! this->_handler.back()->beginElement(reader.name(), reader.attributes()))
        reader.raiseError(errorMessage());
      continue;
    case QXmlStreamReader::EndElement:
      if (! this->_handler.back()->endElement(reader.name()))
        reader.raiseError(errorMessage());
    case QXmlStreamReader::Characters:
      if (reader.isCDATA() && ! this->_handler.back()->processCDATA(reader.text()))
        reader.raiseError(errorMessage());
      else if (! reader.isCDATA() && ! this->_handler.back()->processText(reader.text()))
        reader.raiseError(errorMessage());
      continue;
    }
  }
  if (reader.hasError()) {
    raiseError(QString("Near %1:%2: %3")
               .arg(reader.lineNumber())
               .arg(reader.columnNumber())
               .arg(reader.errorString()));
  }

  return !reader.hasError();
}

const QString &
XmlParser::errorMessage() const {
  return _errorMessage;
}

void
XmlParser::raiseError(const QString &message) {
  _errorMessage = message;
}


void
XmlParser::pushHandler(XmlParser *parser) {
  _handler.append(parser);
}

XmlParser *
XmlParser::popHandler() {
  if (_handler.isEmpty())
    return nullptr;
  return _handler.takeLast();
}
