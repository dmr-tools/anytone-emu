#include "enumfieldpatternwrapper.hh"
#include "pattern.hh"
#include <QItemEditorFactory>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMimeData>

#include "logger.hh"

#define dndMimeType "application/x-anytone-emu-enum-item-indices"

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
  return static_cast<int>(_pattern->numItems());
}

int
EnumFieldPatternWrapper::columnCount(const QModelIndex &parent) const {
  return 5;
}

Qt::ItemFlags
EnumFieldPatternWrapper::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QAbstractTableModel::flags(index);
  if (index.isValid())
    return flags | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  return flags | Qt::ItemIsDropEnabled;
}

Qt::DropActions
EnumFieldPatternWrapper::supportedDropActions() const {
  return Qt::MoveAction;
}

QStringList
EnumFieldPatternWrapper::mimeTypes() const {
  return { dndMimeType };
}

QMimeData *
EnumFieldPatternWrapper::mimeData(const QModelIndexList &indexes) const {
  if (indexes.isEmpty())
    return nullptr;

  QJsonArray rows;
  for (auto index: indexes)
    if (index.isValid() && (!rows.contains(index.row())))
      rows.append(index.row());

  if (rows.isEmpty()) return nullptr;

  auto data = new QMimeData();
  data->setData(dndMimeType, QJsonDocument(rows).toJson());
  return data;
}


bool
EnumFieldPatternWrapper::canDropMimeData(const QMimeData *data, Qt::DropAction action,
  int row, int column, const QModelIndex &parent) const
{
  if ( (!data->hasFormat(dndMimeType)) || (Qt::MoveAction != action))
    return false;

  auto doc = QJsonDocument::fromJson(data->data(dndMimeType));
  if (doc.isNull() || (! doc.isArray()))
    return false;

  if (parent.isValid())
    row = parent.row();

  // Check rows
  if (doc.array().isEmpty())
    return false;
  
  int source = doc.array().at(0).toInt();
  if ((row == source) || (row == source+1))
    return false;

  return true;
}

bool
EnumFieldPatternWrapper::dropMimeData(const QMimeData *data, Qt::DropAction action,
    int row, int column, const QModelIndex &parent)
{
  if (! canDropMimeData(data, action, row, column, parent))
    return false;

  if (parent.isValid())
    row = parent.row();

  auto doc = QJsonDocument::fromJson(data->data(dndMimeType));
  QList<int> rows;
  for (auto val: doc.array()) {
    if (! val.isDouble())
      continue;
    rows.append(val.toInt());
  }

  beginMoveRows(QModelIndex(), rows.first(), rows.last(), QModelIndex(), row);
  for (int source_row: rows)
    _pattern->moveItem(source_row, row++);
  endMoveRows();

  return true;
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

  if (ok) emit dataChanged(index, index);

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
