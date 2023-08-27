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
  void canAddSparse(bool enable);
  void canAddBlock(bool enable);
  void canAddFixed(bool enable);
  void canRemove(bool enable);
  void canEdit(bool enable);

public slots:
  void editPattern();
  void addSparseRepeat();
  void addBlockRepeat();
  void addFixedRepeat();
  void addElement();
  void addInteger();
  void addBit();
  void addUInt8();
  void addInt8();
  void addUInt16();
  void addInt16();
  void addUInt32();
  void addInt32();
  void addBCD8();
  void addEnum();
  void addUnused();
  void addUnknown();
  void removeSelected();

protected:
  AbstractPattern *selectedPattern() const;
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private slots:
  void onShowContextMenu(const QPoint &point);

private:
  CodeplugPattern *_pattern;
};

#endif // PATTERNVIEW_HH
