#include "librarywrapper.hh"
#include "patterndefinition.hh"

#include <QIcon>


LibraryWrapper::LibraryWrapper(PatternDefinitionLibrary *lib, QObject *parent)
  : QAbstractItemModel{parent}, _lib(lib)
{
  if (nullptr != _lib)
    connect(_lib, &QObject::destroyed, this, &LibraryWrapper::onLibraryDeleted);
}


QModelIndex
LibraryWrapper::index(int row, int column, const QModelIndex &parent) const {
  QObject *parentObject = _lib;
  if (parent.isValid())
    parentObject = reinterpret_cast<QObject *>(parent.internalPointer());

  if (auto plib = qobject_cast<PatternDefinitionLibrary *>(parentObject)) {
    if (plib->itemCount() <= row)
      return QModelIndex();
    return createIndex(row, column, plib->item(plib->key(row)));
  }

  return QModelIndex();
}

QModelIndex
LibraryWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid())
    return QModelIndex();

  QObject *object = reinterpret_cast<QObject *>(child.internalPointer());

  QObject *parent= object->parent();
  if (nullptr == parent)
    return QModelIndex();

  QObject *grandParent = parent->parent();
  if (nullptr == grandParent)
    return QModelIndex();

  auto lib = qobject_cast<PatternDefinitionLibrary *>(grandParent);
  if (nullptr == lib)
    return QModelIndex();

  int row = lib->indexOf(parent);
  if (-1 == row)
    return QModelIndex();

  return createIndex(row, 0, parent);
}


int
LibraryWrapper::rowCount(const QModelIndex &parent) const {
  QObject *parentObject = _lib;
  if (parent.isValid())
    parentObject = reinterpret_cast<QObject *>(parent.internalPointer());

  if (auto plib = qobject_cast<PatternDefinitionLibrary *>(parentObject))
    return plib->itemCount();

  return 0;
}


int
LibraryWrapper::columnCount(const QModelIndex &parent) const {
  return 1;
}


Qt::ItemFlags
LibraryWrapper::flags(const QModelIndex &index) const {
  QObject *object = _lib;
  if (index.isValid())
    object = reinterpret_cast<QObject *>(index.internalPointer());

  if (auto plib = qobject_cast<PatternDefinitionLibrary *>(object))
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  else if (auto ppattern = qobject_cast<AbstractPatternDefinition*>(object))
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;

  return Qt::NoItemFlags;
}


QVariant
LibraryWrapper::data(const QModelIndex &index, int role) const {
  if (! index.isValid())
    return QVariant();

  auto obj = reinterpret_cast<QObject *>(index.internalPointer());
  if (nullptr == obj)
    return QVariant();

  auto lib = qobject_cast<PatternDefinitionLibrary *>(obj->parent());
  if (nullptr == lib)
    return QVariant();

  if ((Qt::DisplayRole == role) && (0 == index.column())) {
    return getName(lib, index.row(), obj);
  } else if (Qt::DecorationRole == role) {
    return getIcon(lib, index.row(), obj);
  } else if (Qt::ToolTipRole == role) {
    return getTooltip(lib, index.row(), obj);
  }

  return QVariant();
}

QVariant
LibraryWrapper::getName(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const {
  if (row >= lib->itemCount())
    return QVariant();
  return lib->key(row);
}

QVariant
LibraryWrapper::getIcon(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const {
  if (qobject_cast<const PatternDefinitionLibrary *>(obj)) {
    return QIcon(QPixmap(":/icons/library"));
  } else if (qobject_cast<const AbstractPatternDefinition *>(obj)) {
      return QIcon(QPixmap(":/icons/pattern"));
  }

  return QVariant();
}

QVariant
LibraryWrapper::getTooltip(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const {
  if (auto def = qobject_cast<const AbstractPatternDefinition *>(obj)) {
    return def->meta().name();
  }

  return QVariant();
}


void
LibraryWrapper::onLibraryDeleted() {
  beginResetModel();
  _lib = nullptr;
  endResetModel();
}
