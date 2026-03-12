#include "spellcheckhighlighter.hh"
#include <QRegularExpression>
#include "application.hh"
#include "spellchecker.hh"


SpellCheckHighlighter::SpellCheckHighlighter(QObject *parent)
  : QSyntaxHighlighter{parent},
  _spellChecker(qobject_cast<Application*>(QApplication::instance())->spellChecker())
{
  _highlighted.setFontUnderline(true);
  _highlighted.setUnderlineColor(QColor(Qt::magenta));
  _highlighted.setUnderlineStyle(QTextCharFormat::WaveUnderline);
}


void
SpellCheckHighlighter::highlightBlock(const QString &text) {
  static QRegularExpression expression(R"(\b[A-Za-z\-]+\b)");
  if (nullptr == _spellChecker)
    return;

  QRegularExpressionMatchIterator i = expression.globalMatch(text);
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    if (! _spellChecker->check(match.captured()))
      setFormat(match.capturedStart(), match.capturedLength(), _highlighted);
  }
}
