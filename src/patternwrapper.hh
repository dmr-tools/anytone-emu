#ifndef PATTERNWRAPPER_HH
#define PATTERNWRAPPER_HH

#include <QAbstractItemModel>

class AbstractPattern;
class CodeplugPattern;


class PatternWrapper : public QAbstractItemModel
{
public:
  explicit PatternWrapper(CodeplugPattern *pattern, QObject *parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
  QVariant getIcon(const AbstractPattern *pattern) const;
  QVariant getName(const AbstractPattern *pattern) const;
  QVariant getTooltip(const AbstractPattern *pattern, int column) const;
  QVariant getAddress(const AbstractPattern *pattern) const;
  QVariant getAddressColor(const AbstractPattern *pattern) const;
  QVariant getSize(const AbstractPattern *pattern) const;
  QVariant getSizeColor(const AbstractPattern *pattern) const;

private slots:
  void onPatternModified(const AbstractPattern *pattern);
  void onPatternAdded(const AbstractPattern *parent, unsigned int idx);
  void onRemovingPattern(const AbstractPattern *parent, unsigned int idx);
  void onPatternRemoved(const AbstractPattern *parent, unsigned int idx);

protected:
  CodeplugPattern *_pattern;
};

#endif // PATTERNWRAPPER_HH
