#include "patternwrapper.hh"
#include "codeplugpattern.hh"
#include <QIcon>

PatternWrapper::PatternWrapper(CodeplugPattern *pattern, QObject *parent)
  : QAbstractItemModel{parent}, _pattern(pattern)
{
  connect(_pattern, &QObject::destroyed, this, &PatternWrapper::deleteLater);
  connect(_pattern, &AbstractPattern::modified, this, &PatternWrapper::onPatternModified);
  connect(_pattern, &AbstractPattern::added, this, &PatternWrapper::onPatternAdded);
  connect(_pattern, &AbstractPattern::removing, this, &PatternWrapper::onRemovingPattern);
  connect(_pattern, &AbstractPattern::removed, this, &PatternWrapper::onPatternRemoved);
}

QModelIndex
PatternWrapper::index(int row, int column, const QModelIndex &parent) const {
  if (! parent.isValid()) {
    return createIndex(row, column, _pattern);
  }
  AbstractPattern *parentPattern = reinterpret_cast<AbstractPattern *>(parent.internalPointer());
  if (! parentPattern->is<StructuredPattern>())
    return QModelIndex();

  if (row >= parentPattern->as<StructuredPattern>()->numChildPattern())
    return QModelIndex();

  return createIndex(row, column, parentPattern->as<StructuredPattern>()->childPattern(row));
}

QModelIndex
PatternWrapper::parent(const QModelIndex &child) const {
  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(child.internalPointer());
  if (pattern->is<CodeplugPattern>())
    return QModelIndex();
  AbstractPattern *parent = dynamic_cast<AbstractPattern *>(pattern->parent());
  if (parent->is<CodeplugPattern>())
    return createIndex(0,0, parent);
  StructuredPattern *grandParent = dynamic_cast<StructuredPattern *>(parent->parent());
  return createIndex(grandParent->indexOf(parent), 0, parent);
}

int
PatternWrapper::rowCount(const QModelIndex &parent) const {
  if (! parent.isValid())
    return 1;
  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(parent.internalPointer());
  if (! pattern->is<StructuredPattern>())
    return 0;
  return pattern->as<StructuredPattern>()->numChildPattern();
}

int
PatternWrapper::columnCount(const QModelIndex &parent) const {
  return 1;
}

QVariant
PatternWrapper::data(const QModelIndex &index, int role) const {
  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(index.internalPointer());

  if (Qt::DisplayRole == role) {
    if (!pattern->meta().name().isEmpty())
      return pattern->meta().name();
    return QString("Unnamed %1").arg(pattern->metaObject()->className());
  } else if (Qt::ToolTipRole == role) {
    if (pattern->meta().hasDescription())
      return pattern->meta().description();
    return QVariant();
  } else if (Qt::DecorationRole == role) {
    if (pattern->is<CodeplugPattern>())
      return QIcon(":/icons/16x16/codeplugpattern.png");
    else if (pattern->is<RepeatPattern>())
      return QIcon(":/icons/16x16/sparserepeat.png");
    else if (pattern->is<BlockRepeatPattern>())
      return QIcon(":/icons/16x16/blockrepeat.png");
    else if (pattern->is<FixedRepeatPattern>())
      return QIcon(":/icons/16x16/fixedrepeat.png");
    else if (pattern->is<ElementPattern>())
      return QIcon(":/icons/16x16/element.png");
    else if (pattern->is<IntegerFieldPattern>())
      return QIcon(":/icons/16x16/integer.png");
    else if (pattern->is<EnumFieldPattern>())
      return QIcon(":/icons/16x16/enum.png");
    else if (pattern->is<EnumFieldPattern>())
      return QIcon(":/icons/16x16/enum.png");
    else if (pattern->is<StringFieldPattern>())
      return QIcon(":/icons/16x16/stringfield.png");
    else if (pattern->is<UnusedFieldPattern>())
      return QIcon(":/icons/16x16/unused.png");
    else if (pattern->is<UnknownFieldPattern>())
      return QIcon(":/icons/16x16/unknown.png");
    return QVariant();
  }

  return QVariant();
}

QVariant
PatternWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((0 == section) && (Qt::Horizontal == orientation) && (Qt::DisplayRole == role))
    return "Pattern";
  return QVariant();
}

void
PatternWrapper::onPatternModified(const AbstractPattern *pattern) {
  AbstractPattern *parent = qobject_cast<AbstractPattern*>(pattern->parent());
  if (nullptr == parent) {
    // root (codeplug) data changed
    emit dataChanged(index(0,0, QModelIndex()), index(0,0, QModelIndex()));
    return;
  }

  QModelIndex parentIndex;
  if (parent->is<CodeplugPattern>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    StructuredPattern *grandParent = dynamic_cast<StructuredPattern *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  int row = parent->as<StructuredPattern>()->indexOf(pattern);
  emit dataChanged(index(row, 0 , parentIndex), index(row, 0 , parentIndex));
}

void
PatternWrapper::onPatternAdded(const AbstractPattern *parent, unsigned int idx) {
  QModelIndex parentIndex;
  if (parent->is<CodeplugPattern>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    StructuredPattern *grandParent = dynamic_cast<StructuredPattern *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  beginInsertRows(parentIndex, idx, idx);
  endInsertRows();
}

void
PatternWrapper::onRemovingPattern(const AbstractPattern *parent, unsigned int idx) {
  QModelIndex parentIndex;
  if (parent->is<CodeplugPattern>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    StructuredPattern *grandParent = dynamic_cast<StructuredPattern *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  beginRemoveRows(parentIndex, idx, idx);
}

void
PatternWrapper::onPatternRemoved(const AbstractPattern *parent, unsigned int idx) {
  Q_UNUSED(parent); Q_UNUSED(idx)
  endRemoveRows();
}
