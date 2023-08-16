#include "pseudoterminalposix.hh"
#include <pty.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <QString>
#include <QFileInfo>
#include "logger.hh"


PseudoTerminal::PseudoTerminal(const QString &symLink, QObject *parent)
  : QIODevice{parent}, _dom(-1), _path(), _symLink(symLink), _readNotifier(nullptr)
{
  if (0 > (_dom = ::posix_openpt(O_RDWR|O_NOCTTY))) {
    logError() << "Cannot open dom PTY: " << strerror(errno);
    return;
  }

  if (0 > ::fcntl(_dom, F_SETFL, O_NONBLOCK)) {
    logError() << "Cannot set file properties: " << strerror(errno);
    ::close(_dom); _dom = -1;
    return;
  }

  if (0 > ::grantpt(_dom)) {
    logError() << "Cannot pepare sub PTY: " << strerror(errno);
    ::close(_dom); _dom = -1;
    return;
  }

  if (0 > ::unlockpt(_dom)) {
    logError() << "Cannot unlock sub PTY: " << strerror(errno);
    ::close(_dom); _dom = -1;
    return;
  }

  char name[256];
  if (0 > ::ptsname_r(_dom, name, sizeof(name))) {
    logError() << "Cannot obtain sub PTY path: " << strerror(errno);
    ::close(_dom); _dom = -1;
    return;
  }

  logInfo() << "Got pty at '" << name << "', binding to '" << ::ttyname(_dom) << "'.";

  _readNotifier = new QSocketNotifier(_dom, QSocketNotifier::Read, this);
  _readNotifier->setEnabled(false);

  connect(_readNotifier, &QSocketNotifier::activated, this, &PseudoTerminal::onReady);

  _path = QFileInfo(name).absoluteFilePath();

  // Check if symlink is given:
  if (_symLink.isEmpty())
    return;

  // Remove existing symlink
  QFileInfo symLinkInfo(_symLink);
  if (symLinkInfo.exists() || symLinkInfo.isSymLink()) {
    logWarn() << "Remove exisiting symling at '" << _symLink << "'.";
    QFile::remove(_symLink);
  }
  // (re-) create symlink to PTY
  logInfo() << "Create symlink at '" << _symLink << "' to '" << _path << "'.";
  if (! QFile::link(_path, _symLink)) {
    logError() << "Cannot create symlink from '" << _path << "' to '" << _symLink << "'.";
  }
}

PseudoTerminal::~PseudoTerminal() {
  if (isOpen())
    ::close(_dom);
}

bool
PseudoTerminal::isSequential() const {
  return true;
}

bool
PseudoTerminal::open(OpenMode mode) {
  if (0 > _dom)
    return false;

  if (! QIODevice::open(mode))
    return false;

  _readNotifier->setEnabled(true);

  return true;
}

void
PseudoTerminal::close() {
  _readNotifier->setEnabled(false);
  ::close(_dom); _dom = -1;
}

qint64
PseudoTerminal::readData(char *data, qint64 maxLen) {
  return ::read(_dom, data, maxLen);
}

qint64
PseudoTerminal::writeData(const char *data, qint64 maxLen) {
  qint64 nWritten = ::write(_dom, data, maxLen);
  return nWritten;
}

void
PseudoTerminal::onReady(QSocketDescriptor socket, QSocketNotifier::Type type) {
  if (QSocketNotifier::Read == type) {
    emit readyRead();
  } else if (QSocketNotifier::Write == type) {

  }
}
