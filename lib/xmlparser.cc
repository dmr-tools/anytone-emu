#include "xmlparser.hh"
#include <QXmlStreamReader>
#include <QRegularExpression>
#include <QMetaMethod>

#define SPLIT_TAG_NAME_PATTERN R"([\-_.])"


/* ********************************************************************************************* *
 * Implementation of XmlElementHandler
 * ********************************************************************************************* */
XmlElementHandler::XmlElementHandler(QObject *parent)
  : QObject(parent)
{
  // pass...
}

bool
XmlElementHandler::canBeginElement(const QStringView &name) const {
  // Search for matching slot by name
  QStringList tagName = name.toString().split(QRegularExpression(SPLIT_TAG_NAME_PATTERN), Qt::SkipEmptyParts);
  for (int i=0; i<tagName.size(); i++)
    tagName[i][0] = tagName[i][0].toUpper();
  QByteArray slotName = QString("begin%1Element(QXmlStreamAttributes)").arg(tagName.join("")).toLocal8Bit();
  return (-1 != this->metaObject()->indexOfMethod(slotName.constData())) ||
      (-1 != this->metaObject()->indexOfSlot(slotName.constData()));
}


bool
XmlElementHandler::beginElement(const QStringView &name, const QXmlStreamAttributes &attributes) {
  // Search for matching slot by name
  QStringList tagName = name.toString().split(QRegularExpression(SPLIT_TAG_NAME_PATTERN), Qt::SkipEmptyParts);
  for (int i=0; i<tagName.size(); i++)
    tagName[i][0] = tagName[i][0].toUpper();
  QByteArray slotName = QString("begin%1Element").arg(tagName.join("")).toLocal8Bit();

  bool ok;
  if (this->metaObject()->invokeMethod(
        this, slotName.constData(), Qt::DirectConnection,
        Q_RETURN_ARG(bool, ok),
        Q_ARG(QXmlStreamAttributes, attributes)))
    return ok;

  raiseError(QString("Could not invoke method '%1'.").arg(slotName));
  return false;
}

bool
XmlElementHandler::canEndElement(const QStringView &name) const {
  // Search for matching slot by name
  QStringList tagName = name.toString().split(QRegularExpression(SPLIT_TAG_NAME_PATTERN), Qt::SkipEmptyParts);
  for (int i=0; i<tagName.size(); i++)
    tagName[i][0] = tagName[i][0].toUpper();
  QByteArray slotName = QString("end%1Element()").arg(tagName.join("")).toLocal8Bit();
  return (-1 != this->metaObject()->indexOfMethod(slotName)) ||
      (-1 != this->metaObject()->indexOfSlot(slotName));
}

bool
XmlElementHandler::endElement(const QStringView &name) {
  // Search for matching slot by name
  QStringList tagName = name.toString().split(QRegularExpression(SPLIT_TAG_NAME_PATTERN), Qt::SkipEmptyParts);
  for (int i=0; i<tagName.size(); i++)
    tagName[i][0] = tagName[i][0].toUpper();
  QByteArray slotName = QString("end%1Element").arg(tagName.join("")).toLocal8Bit();

  bool ok;
  if (this->metaObject()->invokeMethod(
        this, slotName.constData(), Qt::DirectConnection,
        Q_RETURN_ARG(bool, ok)))
    return ok;

  return true;
}

bool
XmlElementHandler::processCDATA(const QStringView &content) {
  _textBuffer.append(content);
  return true;
}

bool
XmlElementHandler::processText(const QStringView &content) {
  _textBuffer.append(content);
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


const XmlParser *
XmlElementHandler::parser() const {
  if (nullptr == parent())
    return nullptr;
  if (auto parser = qobject_cast<const XmlParser *>(parent()))
    return parser;
  return nullptr;
}

XmlParser *
XmlElementHandler::parser() {
  if (nullptr == parent())
    return nullptr;
  if (auto parser = qobject_cast<XmlParser *>(parent()))
    return parser;
  return nullptr;
}


void
XmlElementHandler::pushHandler(XmlElementHandler *parser) {
  this->parser()->pushHandler(parser);
}


XmlElementHandler *
XmlElementHandler::topHandler() const {
  return this->parser()->topHandler();
}


XmlElementHandler *
XmlElementHandler::popHandler() {
  return this->parser()->popHandler();
}


const QString &
XmlElementHandler::textBuffer() const {
  return _textBuffer;
}

void
XmlElementHandler::clearTextBuffer() {
  _textBuffer.clear();
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
XmlParser::dispatchBeginElement(const QStringView &name, const QXmlStreamAttributes &attributes) {
  if (this->_handler.isEmpty() || (! topHandler()->canBeginElement(name))) {
    raiseError(
          QString("Cannot handle start of element <%1>, no handler present or unable to handle element.")
          .arg(name));
    return false;
  }

  if (! this->_handler.back()->beginElement(name, attributes)) {
    raiseError(this->_handler.back()->errorMessage());
    return false;
  }

  return true;
}

bool
XmlParser::dispatchEndElement(const QStringView &name) {
  QList<XmlElementHandler*>::const_reverse_iterator handler = _handler.rbegin();
  if (_handler.rend() == handler) {
    raiseError(QString("Cannot handle end of element <%1>, no handler present.").arg(name));
    return false;
  }

  if ((*handler)->canEndElement(name)) {
    if ((*handler)->endElement(name))
      return true;
    raiseError((*handler)->errorMessage());
    return false;
  }

  if ((_handler.rend() == (++handler)) || (! (*handler)->canEndElement(name))) {
    raiseError(QString("Cannot handle end of element <%1>, handler is unable to handle element.").arg(name));
    return false;
  }

  if (! (*handler)->endElement(name)) {
    raiseError((*handler)->errorMessage());
    return false;
  }

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
      if (! this->dispatchBeginElement(reader.name(), reader.attributes()))
        reader.raiseError(errorMessage());
      continue;
    case QXmlStreamReader::EndElement:
      if (! this->dispatchEndElement(reader.name()))
        reader.raiseError(errorMessage());
      continue;
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

void
XmlParser::pushHandler(XmlElementHandler *parser) {
  parser->setParent(this);
  _handler.append(parser);
}


XmlElementHandler *
XmlParser::topHandler() const {
  return _handler.back();
}


XmlElementHandler *
XmlParser::popHandler() {
  if (_handler.isEmpty())
    return nullptr;
  XmlElementHandler *handler = _handler.takeLast();
  handler->setParent(nullptr);
  return handler;
}
