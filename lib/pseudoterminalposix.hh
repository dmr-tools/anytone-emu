#ifndef PSEUDOTERMINALPOSIX_HH
#define PSEUDOTERMINALPOSIX_HH

#include <QIODevice>
#include <QSocketNotifier>

class PseudoTerminal : public QIODevice
{
  Q_OBJECT

public:
  explicit PseudoTerminal(const QString &symLink = QString(), QObject *parent = nullptr);

  ~PseudoTerminal();

  bool isSequential() const;
  bool open(OpenMode mode);
  void close();

protected:
  qint64 readData(char *data, qint64 maxSize);
  qint64 writeData(const char *data, qint64 maxSize);

protected slots:
  void onReady(QSocketDescriptor socket, QSocketNotifier::Type type);

protected:
  int _dom;
  QString _path;
  QString _symLink;
  QSocketNotifier *_readNotifier;
};

#endif // PSEUDOTERMINALPOSIX_HH
