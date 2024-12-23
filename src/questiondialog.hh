#ifndef QUESTIONDIALOG_HH
#define QUESTIONDIALOG_HH

#include <QMessageBox>

class QuestionDialog : public QMessageBox
{
  Q_OBJECT

protected:
  explicit QuestionDialog(QWidget *parent=nullptr);

public:
  static QMessageBox::StandardButton ask(
      const QString &id, const QString &title, const QString &text,
      const QString &detailedText = "");
};

#endif // QUESTIONDIALOG_HH
