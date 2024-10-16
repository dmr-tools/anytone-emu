#ifndef PATTERNVIEW_HH
#define PATTERNVIEW_HH

#include <QTreeView>

class CodeplugPattern;
class AbstractPattern;

class PatternView : public QTreeView
{
  Q_OBJECT

public:
  explicit PatternView(QWidget *parent = nullptr);

  AbstractPattern *selectedPattern() const;

signals:
  void canAppendPattern(bool enable);
  void canInsertPatternAbove(bool enable);
  void canSplitFieldPattern(bool enable);
  void canInsertPatternBelow(bool enable);
  void canRemove(bool enable);
  void canEdit(bool enable);
  void canView(bool enable);

public slots:
  void editPattern();
  void save();

  void appendNewPattern();
  void insertNewPatternAbove();
  void splitFieldPattern();
  void insertNewPatternBelow();

  void copySelected();
  void pastePatternAsChild();
  void pastePatternAbove();
  void pastePatternBelow();
  void removeSelected();

public:
  static bool showPatternEditor(AbstractPattern *pattern, const CodeplugPattern *codeplug=nullptr);

protected:
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private slots:
  void onShowContextMenu(const QPoint &point);

private:
  CodeplugPattern *_pattern;
};

#endif // PATTERNVIEW_HH
