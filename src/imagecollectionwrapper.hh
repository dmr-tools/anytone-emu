#ifndef COLLECTIONWRAPPER_HH
#define COLLECTIONWRAPPER_HH

#include <QAbstractItemModel>

class Collection;
class Image;
class Element;
class FieldAnnotation;

class CollectionWrapper : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit CollectionWrapper(Collection *collection, QObject *parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  bool hasChildren(const QModelIndex &parent) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
  void clearAnnotation(unsigned int idx);

protected:
  QVariant getIcon(const QObject *obj) const;
  QVariant formatTypeName(const QObject *obj) const;
  QVariant formatAddress(const QObject *obj) const;
  QVariant formatSize(const QObject *obj) const;
  QVariant formatFieldValue(const QObject *obj) const;

private slots:
  void onImageAdded(unsigned int idx);
  void onImageAnnotated(unsigned int idx);
  void onAnnotationAdded(const Image *img, const Element *el);

protected:
  Collection *_collection;
};

#endif // COLLECTIONWRAPPER_HH
