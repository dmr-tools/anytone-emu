#include "imagecollectionwrapper.hh"
#include "image.hh"
#include "application.hh"
#include "device.hh"
#include "annotation.hh"
#include "pattern.hh"

#include <QIcon>


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

  auto object = reinterpret_cast<const QObject *>(parent.constInternalPointer());
  if (auto parentIm = qobject_cast<const Image *>(object))
    return createIndex(row, column, parentIm->element(row));
  else if (auto parentEl = qobject_cast<const Element *>(object))
    return createIndex(row, column, parentEl->annotation(row));
  else if (auto parentAn = qobject_cast<const StructuredAnnotation *>(object))
    return createIndex(row, column, parentAn->annotation(row));

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
  return 4;
}

bool
CollectionWrapper::hasChildren(const QModelIndex &parent) const {
  if (! parent.isValid() || (nullptr == parent.constInternalPointer()))
    return true;

  const QObject *obj = reinterpret_cast<const QObject *>(parent.constInternalPointer());

  if (auto img = qobject_cast<const Image *>(obj))
    return img->count();

  if (auto element = qobject_cast<const Element*>(obj))
    return element->numAnnotations();

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
  if (auto el = qobject_cast<const UnannotatedSegment*>(obj))
    flags |= Qt::ItemNeverHasChildren;
  return flags;
}

QVariant
CollectionWrapper::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (nullptr == index.constInternalPointer()))
    return QVariant();

  const QObject *obj = reinterpret_cast<const QObject *>(index.constInternalPointer());
  if (nullptr == obj)
    return QVariant();

  if ((Qt::DisplayRole == role) && (0 == index.column())) {
    return formatTypeName(obj);
  } else if ((Qt::DisplayRole == role) && (1 == index.column())) {
    return formatAddress(obj);
  } else if ((Qt::DisplayRole == role) && (2 == index.column())) {
    return formatSize(obj);
  } else if ((Qt::DisplayRole == role) && (3 == index.column())) {
    return formatFieldValue(obj);
  } else if ((Qt::DecorationRole  == role) && (0 == index.column())) {
    return getIcon(obj);
  } else if ((Qt::ToolTipRole == role) && (0==index.column())) {
    return formatTooltip(obj);
  }

  return QVariant();
}


QVariant
CollectionWrapper::formatTypeName(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  if (auto img = qobject_cast<const Image *>(obj)) {
    return tr("Image (%1)").arg(img->count());
  } else if (auto el = qobject_cast<const Element *>(obj)) {
    if (0 == el->numAnnotations())
      return tr("Element (unannotated)");
    return tr("Element (%1)").arg(el->numAnnotations());
  } else if (auto el = qobject_cast<const StructuredAnnotation *>(obj)) {
    return tr("Structure '%1'").arg(el->hasPattern() ? el->pattern()->meta().name() : "<unknown>");
  } else if (auto el = qobject_cast<const FieldAnnotation *>(obj)) {
    return tr("Field '%1'").arg(el->hasPattern() ? el->pattern()->meta().name() : "<unknown>");
  } else if (auto el = qobject_cast<const UnannotatedSegment*>(obj)) {
    return tr("Unannotated Segment");
  }

  return QVariant();
}


QVariant
CollectionWrapper::formatAddress(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  if (auto el = qobject_cast<const Element *>(obj)) {
    return el->address().toString();
  } else if (auto el = qobject_cast<const StructuredAnnotation *>(obj)) {
    return el->address().toString();
  } else if (auto el = qobject_cast<const FieldAnnotation *>(obj)) {
    if ((7 == el->address().bit()) && (0 == (el->size().bits() % 8)))
      return QString("%1h  ").arg(el->address().byte(), 0, 16);
    return QString("%1h:%2").arg(el->address().byte(), 0, 16).arg(el->address().bit(),0,8);
  } else if (auto el = qobject_cast<const UnannotatedSegment *>(obj)) {
    if ((7 == el->address().bit()) && (0 == (el->size().bits() % 8)))
      return QString("%1h  ").arg(el->address().byte(), 0, 16);
    return QString("%1h:%2").arg(el->address().byte(), 0, 16).arg(el->address().bit(),0,8);
  }

  return QVariant();
}


QVariant
CollectionWrapper::formatSize(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  if (auto el = qobject_cast<const Element *>(obj)) {
    return el->size().toString();
  } else if (auto el = qobject_cast<const StructuredAnnotation *>(obj)) {
    return el->size().toString();
  } else if (auto el = qobject_cast<const FieldAnnotation *>(obj)) {
    return el->size().toString();
  } else if (auto el = qobject_cast<const UnannotatedSegment *>(obj)) {
    return el->size().toString();
  }

  return QVariant();
}


QVariant
CollectionWrapper::getIcon(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  QString suffix, prefix;
  AnnotationIssue::Severity severity = AnnotationIssue::None;
  if (auto image = qobject_cast<const Image *>(obj)) {
    prefix = "image";
    severity = image->annotationSeverity();
  } else if (auto element = qobject_cast<const Element *>(obj)) {
    prefix = "element";
    severity = element->severity();
  } else if (auto anno = qobject_cast<const AbstractAnnotation *>(obj)) {
    severity = anno->issues().severity();
    if (auto st = anno->as<StructuredAnnotation>()) {
      auto pattern = st->pattern();
      if (pattern && pattern->is<BlockRepeatPattern>())
        prefix = "pattern-blockrepeat";
      else if (pattern && pattern->is<FixedRepeatPattern>())
        prefix = "pattern-fixedrepeat";
      else if (pattern && pattern->is<ElementPattern>())
        prefix = "pattern-element";
    } else if (auto fld = anno->as<FieldAnnotation>()) {
      auto pattern = fld->pattern();
      if (pattern && pattern->is<IntegerFieldPattern>())
        prefix = "pattern-integer";
      else if (pattern && pattern->is<EnumFieldPattern>())
        prefix = "pattern-enum";
      else if (pattern && pattern->is<StringFieldPattern>())
        prefix = "pattern-stringfield";
      else if (pattern && pattern->is<UnusedFieldPattern>())
        prefix = "pattern-unused";
      else if (pattern && pattern->is<UnknownFieldPattern>())
        prefix = "pattern-unknown";
    } else if (anno->is<UnannotatedSegment>()) {
      prefix = "annotation-unannotated";
      severity = AnnotationIssue::Error;
    }
  }

  switch (severity) {
  case AnnotationIssue::None: suffix = "-okay"; break;
  case AnnotationIssue::Warning: suffix = "-warning"; break;
  case AnnotationIssue::Error: suffix = "-critical"; break;
  }

  return QIcon::fromTheme(prefix+suffix);
}


QVariant
CollectionWrapper::formatFieldValue(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  auto el = qobject_cast<const FieldAnnotation *>(obj);
  if (nullptr == el)
    return QVariant();

  if (! el->hasPattern())
    return QVariant();

  auto pattern = el->pattern()->as<FieldPattern>();
  if (nullptr == pattern)
    return QVariant();

  if (auto enumPattern = pattern->as<EnumFieldPattern>()) {
    unsigned int val = el->value().toUInt();
    EnumFieldPatternItem *item = enumPattern->itemByValue(val);
    if (nullptr != item)
      return QString("%1 (%2)").arg(item->name()).arg(val);
    return tr("Enum value %1").arg(val);
  }

  if (pattern->is<UnknownFieldPattern>() || pattern->is<UnusedFieldPattern>())
    return QVariant();

  return el->value();
}


QVariant
CollectionWrapper::formatTooltip(const QObject *obj) const {
  if (nullptr == obj)
    return QVariant();

  auto el = qobject_cast<const AbstractAnnotation *>(obj);
  if (nullptr == el)
    return QVariant();

  QString typeName = tr("Unknown pattern");
  if (el->is<UnannotatedSegment>())
    typeName = tr("Unannotated segment");
  else if (el->hasPattern())
    typeName = el->pattern()->metaObject()->className();

  QString tooltip = tr("<h3>%1 <i>%2</i> at <tt>%3</tt></h3>"
                       "<h5>Size <tt>%4</tt></h5>")
      .arg(typeName)
      .arg(el->hasPattern() ? el->pattern()->meta().name() : "")
      .arg(el->address().toString())
      .arg(el->size().toString());

  if (el->hasPattern() && el->pattern()->meta().hasFirmwareVersion())
    tooltip.append(QString("<h5>Firmware  version %2</h5>")
                   .arg(el->pattern()->meta().firmwareVersion()));

  if (el->hasPattern() && el->pattern()->meta().hasBriefDescription())
    tooltip.append(QString("<p>%1</p>")
                   .arg(el->pattern()->meta().briefDescription()));

  if (el->hasPattern() && el->pattern()->meta().hasDescription())
    tooltip.append(QString("<p>%1</p>")
                   .arg(el->pattern()->meta().description()));

  if (el->hasIssues()) {
    tooltip.append("<h3>Annotation issues:</h3><ul>");
    AnnotationIssues::const_iterator issue = el->issues().begin();
    for (; issue != el->issues().end(); issue++) {
      tooltip.append(QString("<li>%1</li>").arg(issue->message()));
    }
    tooltip.append("</ul>");
  }

  return QVariant(tooltip);
}


QVariant
CollectionWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::DisplayRole != role) || (Qt::Horizontal != orientation) || (0 == section))
    return QVariant();

  if (1 == section)
    return tr("Address");

  if (2 == section)
    return tr("Size");

  if (3 == section)
    return tr("Value");

  return QVariant();
}


void
CollectionWrapper::clearAnnotation(unsigned int idx) {
  if (idx >= _collection->count())
    return;
  auto image = _collection->image(idx);
  QModelIndex imageIdx = index(idx, 0, QModelIndex());

  for (unsigned int eidx = 0; eidx < image->count(); eidx++) {
    Element *element = image->element(eidx);
    QModelIndex elementIdx = index(eidx, 0, imageIdx);
    if (0 == element->numAnnotations())
      continue;
    beginRemoveRows(elementIdx, 0, element->numAnnotations()-1);
    element->clearAnnotations();
    endRemoveRows();
    emit dataChanged(elementIdx, elementIdx);
  }
}


void
CollectionWrapper::deleteImage(unsigned int idx) {
  if (idx >= _collection->count())
    return;

  beginRemoveRows(QModelIndex(), idx, idx);
  _collection->deleteImage(idx);
  endRemoveRows();
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

