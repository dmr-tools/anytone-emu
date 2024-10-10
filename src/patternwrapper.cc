#include "patternwrapper.hh"
#include "pattern.hh"
#include <QIcon>
#include <QApplication>
#include <QPalette>
#include <QFont>
#include "logger.hh"


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
  if (! child.isValid())
    return QModelIndex();
  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(child.internalPointer());
  if ((nullptr == pattern) ||pattern->is<CodeplugPattern>())
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
  return 3;
}

Qt::ItemFlags
PatternWrapper::flags(const QModelIndex &index) const {
  return QAbstractItemModel::flags(index);
}


QVariant
PatternWrapper::data(const QModelIndex &index, int role) const {
  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(index.internalPointer());
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
  if ((1 == index.column()) && (Qt::FontRole == role))
    return QFont("monospace");

  if ((2 == index.column()) && (Qt::DisplayRole == role))
    return getSize(pattern);
  if ((2 == index.column()) && (Qt::ForegroundRole == role))
    return getSizeColor(pattern);
  if ((2 == index.column()) && (Qt::TextAlignmentRole == role))
    return Qt::AlignRight;
  if ((2 == index.column()) && (Qt::FontRole == role))
    return QFont("monospace");

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
PatternWrapper::getName(const AbstractPattern *pattern) const {
  QString name = QString("Unnamed %1").arg(pattern->metaObject()->className());
  if (! pattern->meta().name().isEmpty())
    name = pattern->meta().name();
  if (pattern->is<CodeplugPattern>()) {
    const CodeplugPattern *codeplug = pattern->as<CodeplugPattern>();
    if (! codeplug->source().isFile())
      name.append(" [build-in]");
    else if (codeplug->isModified())
      name.append("*");
  }
  return name;
}

QVariant
PatternWrapper::getTooltip(const AbstractPattern *pattern, int column) const {
  if (pattern->meta().hasDescription())
    return pattern->meta().description();
  return QVariant();
}

QVariant
PatternWrapper::getIcon(const AbstractPattern *pattern) const {
  if (pattern->is<CodeplugPattern>())
    return QIcon::fromTheme("pattern-codeplug");
  else if (pattern->is<RepeatPattern>())
    return QIcon::fromTheme("pattern-sparserepeat");
  else if (pattern->is<BlockRepeatPattern>())
    return QIcon::fromTheme("pattern-blockrepeat");
  else if (pattern->is<FixedRepeatPattern>())
    return QIcon::fromTheme("pattern-fixedrepeat");
  else if (pattern->is<ElementPattern>())
    return QIcon::fromTheme("pattern-element");
  else if (pattern->is<IntegerFieldPattern>())
    return QIcon::fromTheme("pattern-integer");
  else if (pattern->is<EnumFieldPattern>())
    return QIcon::fromTheme("pattern-enum");
  else if (pattern->is<StringFieldPattern>())
    return QIcon::fromTheme("pattern-stringfield");
  else if (pattern->is<UnusedFieldPattern>())
    return QIcon::fromTheme("pattern-unused");
  else if (pattern->is<UnknownFieldPattern>())
    return QIcon::fromTheme("pattern-unknown");
  return QVariant();
}

QVariant
PatternWrapper::getAddress(const AbstractPattern *pattern) const {
  if (pattern->hasImplicitAddress() && !pattern->hasAddress())
    return tr("variable");
  if ( pattern->hasAddress() && (7 == pattern->address().bit()) &&
      ((! pattern->is<FixedPattern>())
       || (! pattern->as<FixedPattern>()->hasSize())
       || (0 == (pattern->as<FixedPattern>()->size().bits() % 8))) )
    return QString("%1  ").arg(pattern->address().byte(), 0, 16);
  if (pattern->hasAddress())
    return QString("%1:%2").arg(pattern->address().byte(), 0, 16).arg(pattern->address().bit());
  return tr("not set");
}

QVariant
PatternWrapper::getAddressColor(const AbstractPattern *pattern) const {
  const QPalette &palette = QApplication::palette();
  if (pattern->hasImplicitAddress())
    return palette.brush(QPalette::Disabled, QPalette::Text);
  return palette.brush(QPalette::Normal, QPalette::Text);
}

QVariant
PatternWrapper::getSize(const AbstractPattern *pattern) const {
  if (! pattern->is<FixedPattern>())
    return QVariant();

  const FixedPattern *fixed = pattern->as<FixedPattern>();
  if (fixed->hasSize())
    return QString("%1:%2").arg(fixed->size().byte(), 0, 16).arg(fixed->size().bit());

  return tr("not set");
}

QVariant
PatternWrapper::getSizeColor(const AbstractPattern *pattern) const {
  const QPalette &palette = QApplication::palette();
  if (pattern->is<FieldPattern>())
    return palette.brush(QPalette::Normal, QPalette::Text);
  return palette.brush(QPalette::Disabled, QPalette::Text);
}

void
PatternWrapper::onPatternModified(const AbstractPattern *pattern) {
  AbstractPattern *parent = qobject_cast<AbstractPattern*>(pattern->parent());
  if (nullptr == parent) {
    // root (codeplug) data changed
    emit dataChanged(index(0,0, QModelIndex()), index(0,2, QModelIndex()));
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
  emit dataChanged(index(row, 0 , parentIndex), index(row, 2 , parentIndex));
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
