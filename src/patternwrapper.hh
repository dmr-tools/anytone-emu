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

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private slots:
  void onPatternModified(const AbstractPattern *pattern);
  void onPatternAdded(const AbstractPattern *pattern);
  void onRemovingPattern(const AbstractPattern *pattern);

protected:
  CodeplugPattern *_pattern;
};

#endif // PATTERNWRAPPER_HH
