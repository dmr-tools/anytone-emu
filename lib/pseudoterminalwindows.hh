#ifndef PSEUDOTERMINALWINDOWS_HH
#define PSEUDOTERMINALWINDOWS_HH

#include <QIODevice>

class PseudoTerminal : public QIODevice
{
public:
  explicit PseudoTerminal(const QString &symLink = QString(), QObject *parent = nullptr);

protected:
  qint64 readData(char *data, qint64 maxSize);
  qint64 writeData(const char *data, qint64 maxSize);

};

#endif // PSEUDOTERMINALWINDOWS_HH
