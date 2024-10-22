#include "pseudoterminalwindows.hh"
/// @cond DO_NOT_DOCUMENT

PseudoTerminal::PseudoTerminal(const QString &symLink, QObject *parent)
  : QIODevice{parent}
{
  // pass...
}


qint64
PseudoTerminal::readData(char *data, qint64 len) {
  Q_UNUSED(data); Q_UNUSED(len)
  setErrorString("Pseudoterminals are not supported under windows.");
  return -1;
}


qint64
PseudoTerminal::writeData(const char *data, qint64 len) {
  Q_UNUSED(data); Q_UNUSED(len)
  setErrorString("Pseudoterminals are not supported under windows.");
  return -1;
}

/// @endcond
