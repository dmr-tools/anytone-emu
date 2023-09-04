#include "logmessagelist.hh"


/* ********************************************************************************************* *
 * LogItem
 * ********************************************************************************************* */
LogItem::LogItem(const LogMessage &msg)
  : level(msg.level()), file(msg.file()), line(msg.line()), message(msg.message())
{
  // pass...
}


/* ********************************************************************************************* *
 * LogMessageList
 * ********************************************************************************************* */
LogMessageList::LogMessageList(QObject *parent)
  : QAbstractTableModel{parent}
{
  Logger::get().addHandler(new LogHandlerAdapter(this));
}

int
LogMessageList::rowCount(const QModelIndex &parent) const {
  return _messages.size();
}

int
LogMessageList::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant
LogMessageList::data(const QModelIndex &index, int role) const {
  if (index.row() >= _messages.size())
    return QVariant();

  const LogItem &message = _messages[index.row()];

  if (Qt::DisplayRole == role) {
    if (0 == index.column()) {
      switch (message.level) {
      case LogMessage::DEBUG: return "Debug";
      case LogMessage::INFO: return "Info";
      case LogMessage::WARNING: return "Warn";
      case LogMessage::ERROR: return "Error";
      case LogMessage::FATAL: return "Critical";
      }
    } else if (1 == index.column()) {
      return QString("%1:%2").arg(message.file.fileName()).arg(message.line);
    } else if (2 == index.column())
      return message.message;
  }

  return QVariant();
}

QVariant
LogMessageList::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation))
    return QVariant();
  switch (section) {
  case 0: return tr("Level");
  case 1: return tr("Source");
  case 2: return tr("Message");
  }
  return QVariant();
}

void
LogMessageList::addMessage(const LogMessage &message) {
  beginInsertRows(QModelIndex(), _messages.size(), _messages.size());
  _messages.append(LogItem(message));
  endInsertRows();
}


/* ********************************************************************************************* *
 * LogHandlerAdapter
 * ********************************************************************************************* */
LogHandlerAdapter::LogHandlerAdapter(LogMessageList *list, QObject *parent)
  : LogHandler{parent}, _list(list)
{
  connect(_list, &QObject::destroyed, this, &LogHandlerAdapter::onListDeleted);
}

void
LogHandlerAdapter::handle(const LogMessage &message) {
  if (_list)
    _list->addMessage(message);
}

void
LogHandlerAdapter::onListDeleted() {
  _list = nullptr;
}
