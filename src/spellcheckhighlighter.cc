#include "spellcheckhighlighter.hh"
#include <QRegularExpression>
#include <hunspell.hxx>
#include <QStandardPaths>


SpellCheckHighlighter::SpellCheckHighlighter(QObject *parent)
  : QSyntaxHighlighter{parent}, _spellChecker(nullptr)
{
  _highlighted.setFontUnderline(true);
  _highlighted.setUnderlineColor(QColor(Qt::magenta));
  _highlighted.setUnderlineStyle(QTextCharFormat::WaveUnderline);
  auto affPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "hunspell/en_US.aff");
  auto dicPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "hunspell/en_US.dic");
  _spellChecker = new Hunspell(affPath.toLocal8Bit().constData(), dicPath.toLocal8Bit().constData());
}


void
SpellCheckHighlighter::highlightBlock(const QString &text) {
  static QRegularExpression expression(R"(\b[A-Za-z\-]+\b)");
  if (nullptr == _spellChecker)
    return;

  QRegularExpressionMatchIterator i = expression.globalMatch(text);
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    if (! _spellChecker->spell(match.captured().toStdString()))
      setFormat(match.capturedStart(), match.capturedLength(), _highlighted);
  }
}
