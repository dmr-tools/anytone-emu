#include "patternwrapper.hh"
#include "patterndefinition.hh"
#include <QIcon>
#include <QApplication>
#include <QPalette>


PatternWrapper::PatternWrapper(CodeplugPatternDefinition *pattern, QObject *parent)
  : QAbstractItemModel{parent}, _pattern(pattern)
{
  connect(_pattern, &QObject::destroyed, this, &PatternWrapper::deleteLater);
  connect(_pattern, &AbstractPatternDefinition::modified, this, &PatternWrapper::onPatternModified);
  connect(_pattern, &AbstractPatternDefinition::added, this, &PatternWrapper::onPatternAdded);
  connect(_pattern, &AbstractPatternDefinition::removing, this, &PatternWrapper::onRemovingPattern);
  connect(_pattern, &AbstractPatternDefinition::removed, this, &PatternWrapper::onPatternRemoved);
}

QModelIndex
PatternWrapper::index(int row, int column, const QModelIndex &parent) const {
  if (! parent.isValid()) {
    return createIndex(row, column, _pattern);
  }
  auto parentPattern = reinterpret_cast<AbstractPatternDefinition *>(parent.internalPointer());
  if (! parentPattern->is<StructuredPatternDefinition>())
    return QModelIndex();

  if (row >= parentPattern->as<StructuredPatternDefinition>()->numChildPattern())
    return QModelIndex();

  return createIndex(row, column, parentPattern->as<StructuredPatternDefinition>()->childPattern(row));
}

QModelIndex
PatternWrapper::parent(const QModelIndex &child) const {
  if (! child.isValid())
    return QModelIndex();
  auto pattern = reinterpret_cast<AbstractPatternDefinition *>(child.internalPointer());
  if ((nullptr == pattern) ||pattern->is<CodeplugPatternDefinition>())
    return QModelIndex();
  auto parent = dynamic_cast<AbstractPatternDefinition *>(pattern->parent());
  if (parent->is<CodeplugPatternDefinition>())
    return createIndex(0,0, parent);
  auto grandParent = dynamic_cast<StructuredPatternDefinition *>(parent->parent());
  return createIndex(grandParent->indexOf(parent), 0, parent);
}

int
PatternWrapper::rowCount(const QModelIndex &parent) const {
  if (! parent.isValid())
    return 1;
  auto pattern = reinterpret_cast<AbstractPatternDefinition *>(parent.internalPointer());
  if (! pattern->is<StructuredPatternDefinition>())
    return 0;
  return pattern->as<StructuredPatternDefinition>()->numChildPattern();
}

int
PatternWrapper::columnCount(const QModelIndex &parent) const {
  return 3;
}

Qt::ItemFlags
PatternWrapper::flags(const QModelIndex &index) const {
  return QAbstractItemModel::flags(index);
}


QVariant
PatternWrapper::data(const QModelIndex &index, int role) const {
  auto pattern = reinterpret_cast<AbstractPatternDefinition *>(index.internalPointer());
  if (nullptr == pattern)
    return QVariant();

  if ((0 == index.column()) && (Qt::DisplayRole == role))
    return getName(pattern);
  if (Qt::ToolTipRole == role)
    return getTooltip(pattern, index.column());
  if ((0 == index.column()) && (Qt::DecorationRole == role))
    return getIcon(pattern);
  if ((0 == index.column()) && (Qt::TextAlignmentRole == role))
    return Qt::AlignLeft;

  if ((1 == index.column()) && (Qt::DisplayRole == role))
    return getAddress(pattern);
  if ((1 == index.column()) && (Qt::ForegroundRole == role))
    return getAddressColor(pattern);
  if ((1 == index.column()) && (Qt::TextAlignmentRole == role))
    return Qt::AlignRight;

  if ((2 == index.column()) && (Qt::DisplayRole == role))
    return getSize(pattern);
  if ((2 == index.column()) && (Qt::ForegroundRole == role))
    return getSizeColor(pattern);
  if ((2 == index.column()) && (Qt::TextAlignmentRole == role))
    return Qt::AlignRight;

  return QVariant();
}

QVariant
PatternWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::Horizontal == orientation) && (Qt::DisplayRole == role)) {
    switch (section) {
    case 0: return tr("Pattern");
    case 1: return tr("Address");
    case 2: return tr("Size");
    }
  }
  return QVariant();
}

QVariant
PatternWrapper::getName(const AbstractPatternDefinition *pattern) const {
  QString name = QString("Unnamed %1").arg(pattern->metaObject()->className());
  if (! pattern->meta().name().isEmpty())
    name = pattern->meta().name();
  if (pattern->is<CodeplugPatternDefinition>()) {
    auto *codeplug = pattern->as<CodeplugPatternDefinition>();
    if (! codeplug->source().isFile())
      name.append(" [build-in]");
    else if (codeplug->isModified())
      name.append("*");
  }
  return name;
}

QVariant
PatternWrapper::getTooltip(const AbstractPatternDefinition *pattern, int column) const {
  if (pattern->meta().hasDescription())
    return pattern->meta().description();
  return QVariant();
}

QVariant
PatternWrapper::getIcon(const AbstractPatternDefinition *pattern) const {
  if (pattern->is<CodeplugPatternDefinition>())
    return QIcon(":/icons/16x16/codeplugpattern.png");
  else if (pattern->is<RepeatPatternDefinition>())
    return QIcon(":/icons/16x16/sparserepeat.png");
  else if (pattern->is<BlockRepeatPatternDefinition>())
    return QIcon(":/icons/16x16/blockrepeat.png");
  else if (pattern->is<FixedRepeatPatternDefinition>())
    return QIcon(":/icons/16x16/fixedrepeat.png");
  else if (pattern->is<ElementPatternDefinition>())
  return QIcon(":/icons/16x16/element.png");
  else if (pattern->is<IntegerFieldPatternDefinition>())
    return QIcon(":/icons/16x16/integer.png");
  else if (pattern->is<EnumFieldPatternDefinition>())
    return QIcon(":/icons/16x16/enum.png");
  else if (pattern->is<EnumFieldPatternDefinition>())
    return QIcon(":/icons/16x16/enum.png");
  else if (pattern->is<StringFieldPatternDefinition>())
    return QIcon(":/icons/16x16/stringfield.png");
  else if (pattern->is<UnusedFieldPatternDefinition>())
    return QIcon(":/icons/16x16/unused.png");
  else if (pattern->is<UnknownFieldPatternDefinition>())
    return QIcon(":/icons/16x16/unknown.png");
  return QVariant();
}

QVariant
PatternWrapper::getAddress(const AbstractPatternDefinition *pattern) const {
  if (pattern->hasImplicitAddress() && !pattern->hasAddress())
    return tr("variable");
  if (pattern->hasAddress())
    return QString("%1:%2").arg(pattern->address().byte(), 0, 16).arg(pattern->address().bit());
  return tr("not set");
}

QVariant
PatternWrapper::getAddressColor(const AbstractPatternDefinition *pattern) const {
  const QPalette &palette = QApplication::palette();
  if (pattern->hasImplicitAddress())
    return palette.brush(QPalette::Disabled, QPalette::Text);
  return palette.brush(QPalette::Normal, QPalette::Text);
}

QVariant
PatternWrapper::getSize(const AbstractPatternDefinition *pattern) const {
  if (! pattern->is<FixedPatternDefinition>())
    return QVariant();

  auto fixed = pattern->as<FixedPatternDefinition>();
  if (fixed->hasSize())
    return QString("%1:%2").arg(fixed->size().byte(), 0, 16).arg(fixed->size().bit());

  return tr("not set");
}

QVariant
PatternWrapper::getSizeColor(const AbstractPatternDefinition *pattern) const {
  const QPalette &palette = QApplication::palette();
  if (pattern->is<FieldPatternDefinition>())
    return palette.brush(QPalette::Normal, QPalette::Text);
  return palette.brush(QPalette::Disabled, QPalette::Text);
}

void
PatternWrapper::onPatternModified(const AbstractPatternDefinition *pattern) {
  auto parent = qobject_cast<AbstractPatternDefinition*>(pattern->parent());
  if (nullptr == parent) {
    // root (codeplug) data changed
    emit dataChanged(index(0,0, QModelIndex()), index(0,2, QModelIndex()));
    return;
  }

  QModelIndex parentIndex;
  if (parent->is<CodeplugPatternDefinition>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    auto grandParent = dynamic_cast<StructuredPatternDefinition *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  int row = parent->as<StructuredPatternDefinition>()->indexOf(pattern);
  emit dataChanged(index(row, 0 , parentIndex), index(row, 2 , parentIndex));
}

void
PatternWrapper::onPatternAdded(const AbstractPatternDefinition *parent, unsigned int idx) {
  QModelIndex parentIndex;
  if (parent->is<CodeplugPatternDefinition>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    auto grandParent = dynamic_cast<StructuredPatternDefinition *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  beginInsertRows(parentIndex, idx, idx);
  endInsertRows();
}

void
PatternWrapper::onRemovingPattern(const AbstractPatternDefinition *parent, unsigned int idx) {
  QModelIndex parentIndex;
  if (parent->is<CodeplugPatternDefinition>()) {
    parentIndex = createIndex(0,0, parent);
  } else {
    auto grandParent = dynamic_cast<StructuredPatternDefinition *>(parent->parent());
    parentIndex = createIndex(grandParent->indexOf(parent), 0, parent);
  }
  beginRemoveRows(parentIndex, idx, idx);
}

void
PatternWrapper::onPatternRemoved(const AbstractPatternDefinition *parent, unsigned int idx) {
  Q_UNUSED(parent); Q_UNUSED(idx)
  endRemoveRows();
}
