#ifndef COLLECTIONWRAPPER_HH
#define COLLECTIONWRAPPER_HH

#include <QAbstractItemModel>

class Collection;

class CollectionWrapper : public QAbstractItemModel
{
public:
  explicit CollectionWrapper(Collection *collection, QObject *parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private slots:
  void onImageAdded(unsigned int idx);

protected:
  Collection *_collection;
};

#endif // COLLECTIONWRAPPER_HH
