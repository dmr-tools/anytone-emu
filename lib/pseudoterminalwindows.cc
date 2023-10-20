#include "pseudoterminalwindows.hh"
#include "logger.hh"

PseudoTerminal::PseudoTerminal(const QString &symLink, QObject *parent)
  : QIODevice{parent}
{
  // pass...
}

qint64
PseudoTerminal::writeData(const char *data, qint64 maxSize) {
  setErrorString("Pseudoterminals are not supported under windows.");
  return -1;
}

qint64
PseudoTerminal::readData(char *data, qint64 maxSize) {
  setErrorString("Pseudoterminals are not supported under windows.");
  return -1;
}
