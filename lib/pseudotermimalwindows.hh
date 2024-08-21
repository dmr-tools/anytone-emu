#ifndef PSEUDOTERMIMALWINDOWS_HH
#define PSEUDOTERMIMALWINDOWS_HH
/// @cond DO_NOT_DOCUMENT

#include <QIODevice>

/** This class is just a dummy implementaion.
 *
 * Under windows, PTY cannot be used to emulate serial ports.
 *
 * @ingroup interface */
class PseudoTermimal : public QIODevice
{
public:
  explicit PseudoTermimal(const QString &symLink = QString(), QObject *parent = nullptr);
};


/// @endcond
#endif // PSEUDOTERMIMALWINDOWS_HH
