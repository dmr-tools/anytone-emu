#ifndef SPELLCHECKEDTEXTEDIT_HH
#define SPELLCHECKEDTEXTEDIT_HH

#include <QTextEdit>

class SpellCheckedTextEdit : public QTextEdit
{
  Q_OBJECT

public:
  explicit SpellCheckedTextEdit(QWidget *parent=nullptr);
};

#endif // SPELLCHECKEDTEXTEDIT_HH
