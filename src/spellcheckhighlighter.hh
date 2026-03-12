#ifndef SPELLCHECKHIGHLIGHTER_H
#define SPELLCHECKHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class Hunspell;

class SpellCheckHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  explicit SpellCheckHighlighter(QObject *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;

protected:
  Hunspell *_spellChecker;
  QTextCharFormat _highlighted;
};

#endif // SPELLCHECKHIGHLIGHTER_H
