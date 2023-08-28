#ifndef LOGMESSAGELIST_HH
#define LOGMESSAGELIST_HH

#include <QAbstractTableModel>
#include <QFileInfo>
#include "logger.hh"

class LogItem
{
public:
  LogItem(const LogMessage &msg);

  LogMessage::Level level;
  QFileInfo file;
  int line;
  QString message;
};

class LogMessageList: public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit LogMessageList(QObject *parent=nullptr);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
  void addMessage(const LogMessage &message);

protected:
  QVector<LogItem> _messages;
};


class LogHandlerAdapter: public LogHandler
{
  Q_OBJECT

public:
  explicit LogHandlerAdapter(LogMessageList *list, QObject *parent=nullptr);

  void handle(const LogMessage &message);

private slots:
  void onListDeleted();

private:
  LogMessageList *_list;
};


#endif // LOGMESSAGELIST_HH
