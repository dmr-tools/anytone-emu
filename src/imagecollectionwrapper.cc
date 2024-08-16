#include "imagecollectionwrapper.hh"
#include "image.hh"
#include "application.hh"
#include "device.hh"
#include "codeplugannotation.hh"
#include "codeplugpattern.hh"

CollectionWrapper::CollectionWrapper(Collection *collection, QObject *parent)
  : QAbstractItemModel{parent}, _collection(collection)
{
  connect(_collection, &Collection::imageAdded, this, &CollectionWrapper::onImageAdded);
}

QModelIndex
CollectionWrapper::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid()) { // Is element
    const Image *image = reinterpret_cast<const Image *>(parent.constInternalPointer());
    if (row >= image->count())
      return QModelIndex();
    return createIndex(row, column, image->element(row));
  }

  // Is Image
  if (row >= _collection->count())
    return QModelIndex();
  return createIndex(row, column, _collection->image(row));
}

QModelIndex
CollectionWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid() || (nullptr == child.constInternalPointer()))
    return QModelIndex();

  const QObject *obj = reinterpret_cast<const QObject *>(child.constInternalPointer());
  if (nullptr != dynamic_cast<const Image *>(obj)) {
    return QModelIndex();
  } else if (const Element *el = dynamic_cast<const Element *>(obj)) {
    const Image *img = dynamic_cast<const Image *>(el->parent());
    int row = _collection->indexOf(img);
    if (0 > row)
      return QModelIndex();
    return createIndex(row, 0, img);
  }

  return QModelIndex();
}

int
CollectionWrapper::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    const QObject *obj = reinterpret_cast<const QObject *>(parent.constInternalPointer());
    if (const Image *img = dynamic_cast<const Image *>(obj)) {
      return img->count();
    }
    return 0;
  }
  return _collection->count();
}

int
CollectionWrapper::columnCount(const QModelIndex &parent) const {
  return 1;
}

QVariant
CollectionWrapper::data(const QModelIndex &index, int role) const {
  Application *app = qobject_cast<Application *>(Application::instance());

  if (index.parent().isValid()) { // Element
    const Element *el = reinterpret_cast<const Element *>(index.constInternalPointer());
    const Image *img = reinterpret_cast<const Image *>(el->parent());
    if (Qt::DisplayRole == role) {
      if (img->annotations()) {
        if (const AbstractAnnotation *annotation = img->annotations()->at(el->address())) {
          return QString("%1 @ %2h")
              .arg(annotation->pattern()->meta().name())
              .arg(el->address().byte(), 8, 16, QChar('0'));
        }
      }
      return QString("Unkown Element @ %1h")
          .arg(el->address().byte(), 8, 16, QChar('0'));
    }
  } else { // Image
    if (Qt::DisplayRole == role) {
      const Image *img = reinterpret_cast<const Image *>(index.constInternalPointer());
      return QString("%1 (%2)").arg(img->label()).arg(img->count());
    } else if (Qt::EditRole == role) {
      const Image *img = reinterpret_cast<const Image *>(index.constInternalPointer());
      return img->label();
    }
  }

  return QVariant();
}

QVariant
CollectionWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole == role) && (Qt::Horizontal == orientation) && (0 == section)) {
    return tr("Codeplugs");
  }
  return QVariant();
}

void
CollectionWrapper::onImageAdded(unsigned int idx) {
  beginInsertRows(QModelIndex(), idx, idx+1);
  endInsertRows();
}
