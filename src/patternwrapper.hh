#ifndef PATTERNWRAPPER_HH
#define PATTERNWRAPPER_HH

#include <QAbstractItemModel>

class AbstractPatternDefinition;
class CodeplugPatternDefinition;


class PatternWrapper : public QAbstractItemModel
{
public:
  explicit PatternWrapper(CodeplugPatternDefinition *pattern, QObject *parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
  QVariant getIcon(const AbstractPatternDefinition *pattern) const;
  QVariant getName(const AbstractPatternDefinition *pattern) const;
  QVariant getTooltip(const AbstractPatternDefinition *pattern, int column) const;
  QVariant getAddress(const AbstractPatternDefinition *pattern) const;
  QVariant getAddressColor(const AbstractPatternDefinition *pattern) const;
  QVariant getSize(const AbstractPatternDefinition *pattern) const;
  QVariant getSizeColor(const AbstractPatternDefinition *pattern) const;

private slots:
  void onPatternModified(const AbstractPatternDefinition *pattern);
  void onPatternAdded(const AbstractPatternDefinition *parent, unsigned int idx);
  void onRemovingPattern(const AbstractPatternDefinition *parent, unsigned int idx);
  void onPatternRemoved(const AbstractPatternDefinition *parent, unsigned int idx);

protected:
  CodeplugPatternDefinition *_pattern;
};

#endif // PATTERNWRAPPER_HH
