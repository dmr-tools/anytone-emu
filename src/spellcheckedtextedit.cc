#include "spellcheckedtextedit.hh"
#include "spellcheckhighlighter.hh"


SpellCheckedTextEdit::SpellCheckedTextEdit(QWidget *parent)
  : QTextEdit(parent)
{
  auto highlighter = new SpellCheckHighlighter(this);
  highlighter->setDocument(this->document());
}
