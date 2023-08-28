#ifndef ENUMFIELDPATTERNWRAPPER_HH
#define ENUMFIELDPATTERNWRAPPER_HH

#include <QAbstractTableModel>
#include <QComboBox>
#include "codeplugpattern.hh"
class EnumFieldPattern;


class EnumFieldPatternWrapper : public QAbstractTableModel
{
  Q_OBJECT

public:
  explicit EnumFieldPatternWrapper(EnumFieldPattern *pattern, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role);
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private slots:
  void onItemAdded(unsigned int idx);
  void onItemDeleted(unsigned int idx);

protected:
  EnumFieldPattern *_pattern;
};


class PatternMetaFlagsEditor: public QComboBox
{
  Q_OBJECT

  Q_PROPERTY(PatternMeta::Flags flags READ flags WRITE setFlags USER true)

public:
  PatternMetaFlagsEditor(QWidget *parent = nullptr);

  PatternMeta::Flags flags() const;
  void setFlags(PatternMeta::Flags flags);
};

#endif // ENUMFIELDPATTERNWRAPPER_HH
