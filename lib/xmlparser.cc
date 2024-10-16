#include "xmlparser.hh"
#include <QXmlStreamReader>
#include <QRegularExpression>

/* ********************************************************************************************* *
 * Implementation of XmlElementHandler
 * ********************************************************************************************* */
XmlElementHandler::XmlElementHandler(QObject *parent)
  : QObject(parent)
{
  // pass...
}

bool
XmlElementHandler::beginElement(const QStringView &name, const QXmlStreamAttributes &attributes) {
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
XmlElementHandler::endElement(const QStringView &name) {
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
XmlElementHandler::processCDATA(const QStringView &name) {
  return true;
}

bool
XmlElementHandler::processText(const QStringView &name) {
  return true;
}

const QString &
XmlElementHandler::errorMessage() const {
  return _errorMessage;
}

void
XmlElementHandler::raiseError(const QString &message) {
  _errorMessage = message;
}


void
XmlElementHandler::pushHandler(XmlElementHandler *parser) {
  parser->setParent(this);
  _handler.append(parser);
}


XmlElementHandler *
XmlElementHandler::topHandler() const {
  return _handler.back();
}


XmlElementHandler *
XmlElementHandler::popHandler() {
  if (_handler.isEmpty())
    return nullptr;
  XmlElementHandler *handler = _handler.takeLast();
  handler->setParent(nullptr);
  return handler;
}



/* ********************************************************************************************* *
 * Implementation of XmlParser
 * ********************************************************************************************* */
XmlParser::XmlParser(QObject *parent)
  : XmlElementHandler{parent}
{
  _handler.append(this);
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
      if (! this->beginDocument())
        reader.raiseError(errorMessage());
      continue;
    case QXmlStreamReader::EndDocument:
      if (! this->endDocument())
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



/* ********************************************************************************************* *
 * Implementation of XmlTextHandler
 * ********************************************************************************************* */
XmlTextHandler::XmlTextHandler(XmlElementHandler *parent)
  : XmlElementHandler{parent}, _content()
{
  // pass...
}

const QString &
XmlTextHandler::content() const {
  return _content;
}

bool
XmlTextHandler::processText(const QStringView &content) {
  _content.append(content);
  return true;
}



/* ********************************************************************************************* *
 * Implementation of XmlUrlHandler
 * ********************************************************************************************* */
XmlUrlHandler::XmlUrlHandler(XmlElementHandler *parent)
  : XmlElementHandler{parent}, _url()
{
  // pass...
}

const QUrl &
XmlUrlHandler::url() const {
  return _url;
}

bool
XmlUrlHandler::processText(const QStringView &content) {
  _url = QUrl(content.toString());
  return _url.isValid();
}



/* ********************************************************************************************* *
 * Implementation of XmlHexDataHandler
 * ********************************************************************************************* */
XmlHexDataHandler::XmlHexDataHandler(XmlElementHandler *parent)
  : XmlElementHandler{parent}, _data()
{
  // pass...
}

const QByteArray &
XmlHexDataHandler::data() const {
  return _data;
}

bool
XmlHexDataHandler::processText(const QStringView &content) {
  _data.append(QByteArray::fromHex(content.toLatin1()));
  return true;
}
