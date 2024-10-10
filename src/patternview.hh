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

signals:
  void canAppendPattern(bool enable);
  void canInsertPatternAbove(bool enable);
  void canSplitFieldPattern(bool enable);
  void canInsertPatternBelow(bool enable);
  void canRemove(bool enable);
  void canEdit(bool enable);

public slots:
  void editPattern();
  void appendPattern();
  void insertPatternAbove();
  void splitFieldPattern();
  void insertPatternBelow();
  void removeSelected();
  void save();

protected:
  bool _editPattern(AbstractPattern *pattern);

protected:
  AbstractPattern *selectedPattern() const;
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private slots:
  void onShowContextMenu(const QPoint &point);

private:
  CodeplugPattern *_pattern;
};

#endif // PATTERNVIEW_HH
