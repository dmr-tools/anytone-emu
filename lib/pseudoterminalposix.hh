#ifndef PSEUDOTERMINALPOSIX_HH
#define PSEUDOTERMINALPOSIX_HH

#include <QIODevice>
#include <QSocketNotifier>

/** Implements an interface to the CPS via a pseudo terminal. This can be used to configure a
 * virtual COM port in wine or other virtualizations running windows and the CPS.
 * @ingroup interface */
class PseudoTerminal : public QIODevice
{
  Q_OBJECT

public:
  /** Constructor.
   *
   * Creates a new pseudo terminal and opens it. If @c symLink is given, a symbolic link to the
   * newly created pseudo terminal is created too. This allows to configure a fixed path to the
   * otherwise unpredictable path of the pseudo terminal. */
  explicit PseudoTerminal(const QString &symLink = QString(), QObject *parent = nullptr);

  /** Destructor, also closes the PTY. */
  ~PseudoTerminal();

  /** Retruns @c true. We cannot seek here. */
  bool isSequential() const;
  /** Opens the PTY with the specified mode. */
  bool open(OpenMode mode);
  /** Closes the PTY. */
  void close();

protected:
  /** Internal function, reading from the PTY. */
  qint64 readData(char *data, qint64 maxSize);
  /** Internal function, writing to the PTY. */
  qint64 writeData(const char *data, qint64 maxSize);
  /** Internal function to reopen the PTY. */
  bool reopen();

protected:
  /** Internal flags. */
  int _flags;
  /** The handle for the PTY. */
  int _dom;
  /** Path for the other end of the PTY. */
  QString _subPath;
  /** Optional path for the symlink to @c _subPath. */
  QString _symLink;
  /** Notifier for read operations. */
  QSocketNotifier *_readNotifier;
};

#endif // PSEUDOTERMINALPOSIX_HH
