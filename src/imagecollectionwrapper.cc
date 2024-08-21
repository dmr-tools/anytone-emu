#include "imagecollectionwrapper.hh"
#include "image.hh"
#include "application.hh"
#include "device.hh"
#include "annotation.hh"
#include "pattern.hh"

CollectionWrapper::CollectionWrapper(Collection *collection, QObject *parent)
  : QAbstractItemModel{parent}, _collection(collection)
{
  connect(_collection, &Collection::imageAdded, this, &CollectionWrapper::onImageAdded);
  connect(_collection, &Collection::imageAnnotated, this, &CollectionWrapper::onImageAnnotated);
}

QModelIndex
CollectionWrapper::index(int row, int column, const QModelIndex &parent) const {
  if (! parent.isValid()) { // Is Image
    if (row >= _collection->count())
      return QModelIndex();
    return createIndex(row, column, _collection->image(row));
  }

  auto parentObj = reinterpret_cast<const QObject *>(parent.constInternalPointer());
  return createIndex(row, column, parentObj->children().at(row));

  return QModelIndex();
}

QModelIndex
CollectionWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid() || (nullptr == child.constInternalPointer()))
    return QModelIndex();

  const QObject *obj = reinterpret_cast<const QObject *>(child.constInternalPointer());
  if (auto img = qobject_cast<const Image *>(obj))
    return QModelIndex();

  if (nullptr == obj->parent())
    return QModelIndex();

  int row = obj->parent()->children().indexOf(obj);
  if (0 > row)
    return QModelIndex();

  return createIndex(row, 0, obj->parent());
}

int
CollectionWrapper::rowCount(const QModelIndex &parent) const {
  if (! parent.isValid())
    return _collection->count();

  const QObject *obj = reinterpret_cast<const QObject *>(parent.constInternalPointer());
  return obj->children().count();
}

int
CollectionWrapper::columnCount(const QModelIndex &parent) const {
  return 3;
}

bool
CollectionWrapper::hasChildren(const QModelIndex &parent) const {
  if (! parent.isValid() || (nullptr == parent.constInternalPointer()))
    return true;

  const QObject *obj = reinterpret_cast<const QObject *>(parent.constInternalPointer());
  return obj->children().count();
}

Qt::ItemFlags
CollectionWrapper::flags(const QModelIndex &index) const {
  if (! index.isValid() || (nullptr == index.constInternalPointer()))
    return QAbstractItemModel::flags(index);

  const QObject *obj = reinterpret_cast<const QObject *>(index.constInternalPointer());
  Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (auto el = qobject_cast<const FieldAnnotation*>(obj))
    flags |= Qt::ItemNeverHasChildren;
  return flags;
}

QVariant
CollectionWrapper::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (nullptr == index.constInternalPointer()) || (Qt::DisplayRole != role))
    return QVariant();

  const QObject *obj = reinterpret_cast<const QObject *>(index.constInternalPointer());
  if (auto img = qobject_cast<const Image *>(obj)) {
    if (0 == index.column()) {
      return tr("Image (%1)").arg(img->count());
    }
    return QVariant();
  } else if (auto el = qobject_cast<const Element *>(obj)) {
    if (0 == index.column()) {
      if (0 == el->numAnnotations())
        return tr("Element (unannotated)");
      return tr("Element (%1)").arg(el->numAnnotations());
    } else if (1 == index.column()) {
      return el->address().toString();
    }
    return QVariant();
  } else if (auto el = qobject_cast<const StructuredAnnotation *>(obj)) {
    if (0 == index.column()) {
      return tr("Structure '%1'").arg(el->pattern()->meta().name());
    } else if (1 == index.column()) {
      return el->address().toString();
    }
    return QVariant();
  } else if (auto el = qobject_cast<const FieldAnnotation *>(obj)) {
    if (0 == index.column()) {
      return tr("Field '%1'").arg(el->pattern()->meta().name());
    } else if (1 == index.column()) {
      return el->address().toString();
    } else if (2 == index.column()) {
      return formatFieldValue(el);
    }
    return QVariant();
  }

  return QVariant();
}

QVariant
CollectionWrapper::formatFieldValue(const FieldAnnotation *annotation) const {
  auto pattern = annotation->pattern()->as<FieldPattern>();
  if (nullptr == pattern)
    return QVariant();

  if (auto enumPattern = pattern->as<EnumFieldPattern>()) {
    unsigned int val = annotation->value().toUInt();
    EnumFieldPatternItem *item = enumPattern->itemByValue(val);
    if (nullptr != item) {
      return QString("%1 (%2)").arg(item->name()).arg(val);
    }
    return tr("Enum value %1").arg(val);
  }

  return annotation->value();
}


QVariant
CollectionWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation) || (0 == section))
    return QVariant();

  if (1 == section)
    return tr("Address");

  if (2 == section)
    return tr("Value");

  return QVariant();
}

void
CollectionWrapper::onImageAdded(unsigned int idx) {
  beginInsertRows(QModelIndex(), idx, idx+1);
  endInsertRows();
}

void
CollectionWrapper::onImageAnnotated(unsigned int idx) {
  const Image *img = _collection->image(idx);
  for (int i=0; i<img->count(); i++) {
    onAnnotationAdded(img, img->element(i));
  }
}

void
CollectionWrapper::onAnnotationAdded(const Image *img, const Element *el) {
  QModelIndex imageIdx = index(_collection->indexOf(img), 0, QModelIndex());
  QModelIndex elementIdx = index(img->children().indexOf(el), 0, imageIdx);
  beginInsertRows(elementIdx, 0, el->numAnnotations()-1);
  endInsertRows();
  emit dataChanged(elementIdx, elementIdx);
}

