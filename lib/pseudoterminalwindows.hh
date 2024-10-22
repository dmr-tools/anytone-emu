#ifndef PSEUDOTERMINALWINDOWS_HH
#define PSEUDOTERMINALWINDOWS_HH
/// @cond DO_NOT_DOCUMENT

#include <QIODevice>

/** This class is just a dummy implementaion.
 *
 * Under windows, PTY cannot be used to emulate serial ports.
 *
 * @ingroup interface */
class PseudoTerminal : public QIODevice
{
public:
  explicit PseudoTerminal(const QString &symLink = QString(), QObject *parent = nullptr);

protected:
  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);
};


/// @endcond
#endif // PSEUDOTERMINALWINDOWS_HH
