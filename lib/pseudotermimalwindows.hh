#ifndef PSEUDOTERMIMALWINDOWS_HH
#define PSEUDOTERMIMALWINDOWS_HH

#include <QIODevice>

class PseudoTermimal : public QIODevice
{
public:
  explicit PseudoTermimal(const QString &symLink = QString(), QObject *parent = nullptr);
};

#endif // PSEUDOTERMIMALWINDOWS_HH
