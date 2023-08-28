#include "enumfieldpatternwrapper.hh"
#include "codeplugpattern.hh"
#include <QItemEditorFactory>

/* ********************************************************************************************* *
 * Implementation of EnumFieldPatternWrapper
 * ********************************************************************************************* */
EnumFieldPatternWrapper::EnumFieldPatternWrapper(EnumFieldPattern *pattern, QObject *parent)
  : QAbstractTableModel{parent}, _pattern(pattern)
{
  connect(_pattern, &QObject::destroyed, this, &QObject::deleteLater);
  connect(_pattern, &EnumFieldPattern::itemAdded, this, &EnumFieldPatternWrapper::onItemAdded);
  connect(_pattern, &EnumFieldPattern::itemDeleted, this, &EnumFieldPatternWrapper::onItemDeleted);
}

int
EnumFieldPatternWrapper::rowCount(const QModelIndex &parent) const {
  return _pattern->numItems();
}

int
EnumFieldPatternWrapper::columnCount(const QModelIndex &parent) const {
  return 5;
}

Qt::ItemFlags
EnumFieldPatternWrapper::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  return flags | Qt::ItemIsEditable;
}

QVariant
EnumFieldPatternWrapper::data(const QModelIndex &index, int role) const {
  if (index.row() >= _pattern->numItems())
    return QVariant();

  EnumFieldPatternItem *item = _pattern->item(index.row());

  if (Qt::DisplayRole == role) {
    if (0 == index.column())
      return item->value();
    else if (1 == index.column())
      return item->name();
    else if (2 == index.column())
      return item->description();
    else if (3 == index.column())
      return item->firmwareVersion();
    else if (4 == index.column())
      return QVariant::fromValue(item->flags());
  } else if (Qt::EditRole == role) {
    if (0 == index.column())
      return item->value();
    else if (1 == index.column())
      return item->name();
    else if (2 == index.column())
      return item->description();
    else if (3 == index.column())
      return item->firmwareVersion();
    else if (4 == index.column())
      return QVariant::fromValue(item->flags());
  } else if (Qt::ToolTipRole == role) {
    return item->description();
  }

  return QVariant();
}


bool
EnumFieldPatternWrapper::setData(const QModelIndex &index, const QVariant &value, int role) {
  if ((Qt::EditRole != role) || (index.row() >= _pattern->numItems()))
    return false;

  EnumFieldPatternItem *item = _pattern->item(index.row());
  bool ok = true;
  if (0 == index.column())
    item->setValue(value.toUInt(&ok));
  else if (1 == index.column())
    item->setName(value.toString().simplified());
  else if (2 == index.column())
    item->setDescription(value.toString().simplified());
  else if (3 == index.column())
    item->setFirmwareVersion(value.toString().simplified());
  else if (4 == index.column())
    item->setFlags(value.value<PatternMeta::Flags>());

  return ok;
}


QVariant
EnumFieldPatternWrapper::headerData(int section, Qt::Orientation orientation, int role) const {
  if ((Qt::Horizontal != orientation) || (Qt::DisplayRole != role))
    return QVariant();
  if (0 == section)
    return tr("Value");
  if (1 == section)
    return tr("Name");
  if (2 == section)
    return tr("Description");
  if (3 == section)
    return tr("FW Version");
  if (4 == section)
    return tr("Flags");
  return QVariant();
}

void
EnumFieldPatternWrapper::onItemAdded(unsigned int idx) {
  beginInsertRows(QModelIndex(), idx, idx);
  endInsertRows();
}

void
EnumFieldPatternWrapper::onItemDeleted(unsigned int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  endRemoveRows();
}


/* ********************************************************************************************* *
 * Implementation of PatternMetaFlagsEditor
 * ********************************************************************************************* */
PatternMetaFlagsEditor::PatternMetaFlagsEditor(QWidget *parent)
  : QComboBox{parent}
{
  addItem(tr("None"), QVariant::fromValue(PatternMeta::Flags::None));
  addItem(tr("Done"), QVariant::fromValue(PatternMeta::Flags::Done));
  addItem(tr("Needs review"), QVariant::fromValue(PatternMeta::Flags::NeedsReview));
  addItem(tr("Incomplete"), QVariant::fromValue(PatternMeta::Flags::Incomplete));
}

PatternMeta::Flags
PatternMetaFlagsEditor::flags() const {
  return currentData().value<PatternMeta::Flags>();
}

void
PatternMetaFlagsEditor::setFlags(PatternMeta::Flags flags) {
  switch (flags) {
  case PatternMeta::Flags::None: setCurrentIndex(0); break;
  case PatternMeta::Flags::Done: setCurrentIndex(1); break;
  case PatternMeta::Flags::NeedsReview: setCurrentIndex(2); break;
  case PatternMeta::Flags::Incomplete: setCurrentIndex(3); break;
  }
}
