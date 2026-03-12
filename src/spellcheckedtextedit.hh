#ifndef SPELLCHECKEDTEXTEDIT_HH
#define SPELLCHECKEDTEXTEDIT_HH

#include <QTextEdit>

class SpellCheckHighlighter;


class SpellCheckedTextEdit : public QTextEdit
{
  Q_OBJECT

public:
  explicit SpellCheckedTextEdit(QWidget *parent=nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

protected slots:
  void fixSpelling(int from, int to, const QString &replace);

private:
  SpellCheckHighlighter *_highlighter;
};

#endif // SPELLCHECKEDTEXTEDIT_HH
