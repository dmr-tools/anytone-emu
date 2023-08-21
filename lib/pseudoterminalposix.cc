#include "pseudoterminalposix.hh"
#include <pty.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <QString>
#include <QFileInfo>
#include <QDir>
#include "logger.hh"


PseudoTerminal::PseudoTerminal(const QString &symLink, QObject *parent)
  : QIODevice{parent}, _flags(O_NOCTTY|O_NONBLOCK), _dom(-1), _subPath(), _symLink(symLink),
    _readNotifier(nullptr)
{
  // pass...
}

bool
PseudoTerminal::open(OpenMode mode)
{
  _flags = O_NOCTTY|O_NONBLOCK;
  if ((mode & QIODevice::ReadOnly) && !(mode & QIODevice::WriteOnly))
    _flags |= O_RDONLY;
  else if ((mode & QIODevice::WriteOnly) && !(mode & QIODevice::ReadOnly))
    _flags |= O_WRONLY;
  else if ((mode & QIODevice::WriteOnly) && (mode & QIODevice::ReadOnly))
    _flags |= O_RDWR;

  if (reopen()) {
    QIODevice::open(mode);
    return true;
  }

  return false;
}

bool
PseudoTerminal::reopen()
{
  logDebug() << "(Re-)open pty.";
  if (_dom > 0) {
    logDebug() << "Close pty.";
    _readNotifier->setEnabled(false);
    delete _readNotifier;
    _readNotifier = nullptr;
    ::close(_dom);
    _dom = -1;
  }

  if (0 > (_dom = ::posix_openpt(_flags))) {
    setErrorString(QString("Cannot open dom PTY: %1.").arg(strerror(errno)));
    return false;
  }

  if (0 > ::grantpt(_dom)) {
    setErrorString(QString("Cannot pepare sub PTY: %1.").arg(strerror(errno)));
    ::close(_dom); _dom = -1;
    return false;
  }

  if (0 > ::unlockpt(_dom)) {
    setErrorString(QString("Cannot unlock sub PTY: %1.").arg(strerror(errno)));
    ::close(_dom); _dom = -1;
    return false;
  }

  char name[256];
  if (0 > ::ptsname_r(_dom, name, sizeof(name))) {
    setErrorString(QString("Cannot obtain sub PTY path: %1.").arg(strerror(errno)));
    ::close(_dom); _dom = -1;
    return false;
  }

  logInfo() << "Got pty at '" << name << "', binding to '" << ::ttyname(_dom) << "'.";

  _readNotifier = new QSocketNotifier(_dom, QSocketNotifier::Read, this);
  _readNotifier->setEnabled(false);

  connect(_readNotifier, &QSocketNotifier::activated, this, &PseudoTerminal::readyRead);
  _readNotifier->setEnabled(true);

  // Check if symlink is given:
  if (_symLink.isEmpty())
    return true;

  QFileInfo symLinkInfo(_symLink);
  _subPath = QFileInfo(name).absoluteFilePath();
  if (symLinkInfo.isSymLink() && (symLinkInfo.symLinkTarget() == _subPath)) {
    logInfo() << "Reuse existing symlink " << _symLink << " -> " << _subPath << ".";
    return true;
  }

  // Remove existing symlink
  if (symLinkInfo.exists() || symLinkInfo.isSymLink()) {
    logDebug() << "Remove exisiting symling at '" << _symLink << "'.";
    QFile::remove(_symLink);
  }

  QDir directory = symLinkInfo.absoluteDir();
  if (! directory.exists()) {
    logDebug() << "Create directory '" << directory.path() << "'.";
    directory.mkpath(".");
  }

  // (re-) create symlink to PTY
  logInfo() << "Create symlink '" << _symLink << "' -> '" << _subPath << "'.";
  if (! QFile::link(_subPath, _symLink)) {
    logWarn() << "Cannot create symlink '" << _symLink << "' to '" << _subPath
              << "'. You may need to use the sub pty " << _subPath << ".";
  }

  return true;
}

PseudoTerminal::~PseudoTerminal() {
  if (isOpen()) {
    ::close(_dom);
    _dom = -1;
  }
}

bool
PseudoTerminal::isSequential() const {
  return true;
}

void
PseudoTerminal::close() {
  logDebug() << "Close pty.";
  _readNotifier->setEnabled(false);
  delete _readNotifier;
  _readNotifier = nullptr;

  ::close(_dom);
  _dom = -1;

  QIODevice::close();
}

qint64
PseudoTerminal::readData(char *data, qint64 maxLen) {
  int n = ::read(_dom, data, maxLen);

  if (n > 0)
    logDebug() << "Read " << n << " of " << maxLen << "b from pty: "
               << QByteArray(data, n).toHex();

  if ((n < 0) && (EIO == errno)) {
    logDebug() << "Client side may closed the pty. Reopen.";
    setErrorString(QString("Cannot read from pty (%1): %2").arg(errno).arg(::strerror(errno)));
    _readNotifier->setEnabled(false);
    this->reopen();
  } else if ((n<0) && (EAGAIN == errno)) {
    // pass...
  } else if (n < 0) {
    setErrorString(QString("Cannot read from pty (%1): %2").arg(errno).arg(::strerror(errno)));
    logError() << errorString() << " Reopen.";
    _readNotifier->setEnabled(false);
    this->reopen();
  }

  return n;
}

qint64
PseudoTerminal::writeData(const char *data, qint64 maxLen) {
  int n = ::write(_dom, data, maxLen);

  if (n < 0)
    setErrorString(QString("Cannot write to pyt: %1.").arg(strerror(errno)));

  return n;
}
