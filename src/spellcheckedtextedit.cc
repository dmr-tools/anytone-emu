#include "spellcheckedtextedit.hh"
#include "spellcheckhighlighter.hh"
#include "spellchecker.hh"
#include "application.hh"
#include <QMenu>


SpellCheckedTextEdit::SpellCheckedTextEdit(QWidget *parent)
  : QTextEdit(parent), _highlighter(new SpellCheckHighlighter(this))
{
  _highlighter->setDocument(this->document());
}


void
SpellCheckedTextEdit::contextMenuEvent(QContextMenuEvent *event) {
  auto menu = createStandardContextMenu(event->pos());
  if (! event->pos().isNull()) {
    auto checker = qobject_cast<Application *>(QApplication::instance())->spellChecker();

    QTextCursor tc = cursorForPosition(event->pos());
    tc.select(QTextCursor::WordUnderCursor);
    auto word = tc.selectedText();
    if (! checker->check(word)) {
      menu->addSeparator();
      auto suggestions = checker->suggest(word);
      if (! suggestions.isEmpty()) {
        QMenu *subMenu = menu->addMenu(QIcon::fromTheme("spellcheck"), tr("Replace %1 with ...").arg(word));
        for (auto suggestion: suggestions) {
          auto action = subMenu->addAction(QIcon::fromTheme("spellcheck-replace"), suggestion);
          int start = tc.selectionStart(), end = tc.selectionEnd();
          connect(action, &QAction::triggered, [this, suggestion, start, end] () {
            this->fixSpelling(start, end, suggestion);
          });
        }
      }
      auto addAction = menu->addAction(QIcon::fromTheme("spellcheck-add"), tr("Add word to dictionary"));
      connect(addAction, &QAction::triggered, [this, word, &tc]() {
        qobject_cast<Application*>(QApplication::instance())->spellChecker()->addWord(word);
        this->_highlighter->rehighlight();
      });
    }
  }
  menu->exec(event->globalPos());
  delete menu;
}


void
SpellCheckedTextEdit::fixSpelling(int from, int to, const QString &replace) {
  auto cursor = textCursor();
  cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, from);
  cursor.select(QTextCursor::WordUnderCursor);
  cursor.removeSelectedText();
  cursor.insertText(replace);
  _highlighter->rehighlight();
}
