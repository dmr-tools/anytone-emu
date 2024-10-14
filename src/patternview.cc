#include "patternview.hh"
#include "pattern.hh"
#include "device.hh"
#include "application.hh"
#include "patternwrapper.hh"

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QClipboard>

#include "codeplugdialog.hh"
#include "sparserepeatdialog.hh"
#include "blockrepeatdialog.hh"
#include "fixedrepeatdialog.hh"
#include "elementdialog.hh"
#include "integerfielddialog.hh"
#include "enumfielddialog.hh"
#include "stringfielddialog.hh"
#include "unusedfielddialog.hh"
#include "unknownpatterndialog.hh"
#include "newpatterndialog.hh"
#include "splitfieldpatterndialog.hh"
#include "patternmimedata.hh"


PatternView::PatternView(QWidget *parent)
  : QTreeView{parent}, _pattern(nullptr)
{
  Application *app = qobject_cast<Application *>(Application::instance());

  _pattern = app->device()->pattern();
  if (_pattern)
    setModel(new PatternWrapper(_pattern));

  connect(this, &QWidget::customContextMenuRequested, this, &PatternView::onShowContextMenu);
}

AbstractPattern *
PatternView::selectedPattern() const {
  QModelIndexList selection = selectedIndexes();
  if (selection.isEmpty())
    return nullptr;
  QModelIndex selected = selection.back();
  return reinterpret_cast<AbstractPattern *>(selected.internalPointer());
}

void
PatternView::editPattern() {
  AbstractPattern *pattern = selectedPattern();
  if (nullptr == pattern) {
    QMessageBox::information(nullptr, tr("Cannot edit pattern"),
                             tr("Select a pattern first."));
    return;
  }

  _editPattern(pattern);
}


bool
PatternView::_editPattern(AbstractPattern *pattern, const CodeplugPattern *codeplug) {
  if (pattern->is<CodeplugPattern>()) {
    CodeplugDialog dialog;
    dialog.setPattern(pattern->as<CodeplugPattern>());
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<RepeatPattern>()) {
    SparseRepeatDialog dialog;
    dialog.setPattern(pattern->as<RepeatPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<BlockRepeatPattern>()) {
    BlockRepeatDialog dialog;
    dialog.setPattern(pattern->as<BlockRepeatPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<FixedRepeatPattern>()) {
    FixedRepeatDialog dialog;
    dialog.setPattern(pattern->as<FixedRepeatPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<ElementPattern>()) {
    ElementDialog dialog;
    dialog.setPattern(pattern->as<ElementPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<IntegerFieldPattern>()) {
    IntegerFieldDialog dialog;
    dialog.setPattern(pattern->as<IntegerFieldPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<EnumFieldPattern>()) {
    EnumFieldDialog dialog;
    dialog.setPattern(pattern->as<EnumFieldPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<StringFieldPattern>()) {
    StringFieldDialog dialog;
    dialog.setPattern(pattern->as<StringFieldPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<UnusedFieldPattern>()) {
    UnusedFieldDialog dialog;
    dialog.setPattern(pattern->as<UnusedFieldPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  if (pattern->is<UnknownFieldPattern>()) {
    UnknownPatternDialog dialog;
    dialog.setPattern(pattern->as<UnknownFieldPattern>(), codeplug);
    return QDialog::Accepted == dialog.exec();
  }

  return false;
}


void
PatternView::appendNewPattern() {
  AbstractPattern *parent = selectedPattern();
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Select a structured pattern first."),
                             tr("To append a child pattern, select a structured pattern first."));
    return;
  }

  StructuredPattern *structure = parent->as<StructuredPattern>();

  Address insertionAddress;
  if (structure->numChildPattern()) {
    AbstractPattern *pred = structure->childPattern(structure->numChildPattern()-1);
    insertionAddress = pred->address();
    if (FixedPattern *fixed = pred->as<FixedPattern>())
      insertionAddress += fixed->size();
  }

  NewPatternDialog dialog(parent, insertionAddress, nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.create();

  if (! _editPattern(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! structure->addChildPattern(newPattern)) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to {}.").arg(parent->meta().name()));
    newPattern->deleteLater();
    return;
  }
}

void
PatternView::insertNewPatternAbove() {
  AbstractPattern *nextSibling = selectedPattern();

  if (nullptr == nextSibling) {
    QMessageBox::information(nullptr, tr("Select a sibling first."),
                             tr("To insert a pattern above another pattern, select a pattern first."));
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern *>(nextSibling->parent());
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be structured pattern."),
                             tr("The parent of the selected pattern must be a structured pattern."));
    return;
  }

  Address insertionAddress = nextSibling->address();

  StructuredPattern *structure = parent->as<StructuredPattern>();
  unsigned int insertionIndex = structure->indexOf(nextSibling);
  if (! parent->is<ElementPattern>()) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return;
  }

  NewPatternDialog dialog(parent, insertionAddress, nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.create();
  if (! newPattern->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can onyl add fixed-sized patterns to an element pattern."));
    newPattern->deleteLater();
    return;
  }

  if (! _editPattern(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->as<ElementPattern>()->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertNewPatternBelow() {
  AbstractPattern *nextSibling = selectedPattern();

  if (nullptr == nextSibling) {
    QMessageBox::information(nullptr, tr("Select a sibling first."),
                             tr("To insert a pattern below another pattern, select a pattern first."));
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern *>(nextSibling->parent());
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be structured pattern."),
                             tr("The parent of the selected pattern must be a structured pattern."));
    return;
  }

  Address insertionAddress = nextSibling->address();
  if (FixedPattern *fixed = nextSibling->as<FixedPattern>())
    insertionAddress += fixed->size();

  StructuredPattern *structure = parent->as<StructuredPattern>();
  unsigned int insertionIndex = structure->indexOf(nextSibling) + 1;
  if (! parent->is<ElementPattern>()) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return;
  }

  NewPatternDialog dialog(parent, insertionAddress, nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.create();
  if (! newPattern->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can onyl add fixed-sized patterns to an element pattern."));
    newPattern->deleteLater();
    return;
  }

  if (! _editPattern(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }


  if (! parent->as<ElementPattern>()->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::splitFieldPattern() {
  // Get pattern to be replaced ...
  AbstractPattern *replaced = selectedPattern();
  // ... and check its type.
  if ((nullptr == replaced) || (! replaced->is<UnknownFieldPattern>())) {
    QMessageBox::information(nullptr, tr("Select an unknown field first."),
                             tr("To split an unknown field, select one first."));
    return;
  }

  // Check type of parent pattern
  AbstractPattern *parent = qobject_cast<AbstractPattern *>(replaced->parent());
  if ((nullptr == parent) || (! parent->is<ElementPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return;
  }
  ElementPattern *parentElement = parent->as<ElementPattern>();

  // Get address, size and location of replaced field pattern
  Address startAddress = replaced->address();
  Offset originalSize = replaced->as<FieldPattern>()->size();
  unsigned int insertionIndex = parentElement->indexOf(replaced);

  // Get type and address of inserted pattern
  SplitFieldPatternDialog dialog(replaced->as<UnknownFieldPattern>());
  if (QDialog::Accepted != dialog.exec())
    return;
  Address insertionAddr = dialog.address();
  FixedPattern *inserted = dialog.createPattern();

  // Allow user to configure pattern
  if (! _editPattern(inserted, parent->codeplug())) {
    inserted->deleteLater();
    return;
  }

  // compute size of head and tail unknown fields:
  Offset headFieldSize = insertionAddr - startAddress;
  // check size of inserted field
  if (originalSize < (headFieldSize + inserted->size())) {
    QMessageBox::information(
          nullptr, tr("Inserted field too large."),
          tr("The size inserted field and its offset extends beyond the split field."));
    inserted->deleteLater();
    return;
  }  
  Offset tailFieldSize = Offset::fromBits(
        originalSize.bits() - headFieldSize.bits() - inserted->size().bits());

  // Remove "old" unknown field
  parentElement->deleteChild(insertionIndex);
  // Insert an unknown field, if needed
  if (headFieldSize.bits()) {
    auto head = new UnknownFieldPattern(); head->setWidth(headFieldSize);
    parentElement->insertChildPattern(head, insertionIndex++);
  }

  // Insert the replacement
  inserted->setAddress(Address());
  parentElement->insertChildPattern(inserted, insertionIndex++);

  // Insert a tail field, if needed
  if (tailFieldSize.bits()) {
    auto tail = new UnknownFieldPattern(); tail->setWidth(tailFieldSize);
    parentElement->insertChildPattern(tail, insertionIndex++);
  }

  // done
}


void
PatternView::copySelected() {
  if ((nullptr == selectedPattern()) || (selectedPattern()->is<CodeplugPattern>())) {
    QMessageBox::information(nullptr, tr("Select a pattern first"),
                             tr("Select the pattern to copy."));
    return;
  }

  QGuiApplication::clipboard()->setMimeData(new PatternMimeData(selectedPattern()->clone()));
}


void
PatternView::cutSelected() {
  if ((nullptr == selectedPattern()) || (selectedPattern()->is<CodeplugPattern>())) {
    QMessageBox::information(nullptr, tr("Select a pattern first"),
                             tr("Select the pattern to copy."));
    return;
  }

  StructuredPattern *parent = dynamic_cast<StructuredPattern *>(selectedPattern()->parent());
  if (nullptr == parent) {
    QMessageBox::information(nullptr, tr("Cannot remove pattern"),
                             tr("The parent of the selected pattern is not a structured pattern."));
    return;
  }

  auto pattern = parent->takeChild(parent->indexOf(selectedPattern()));
  QGuiApplication::clipboard()->setMimeData(new PatternMimeData(pattern));
}


void
PatternView::pastePatternAsChild() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  AbstractPattern *parent = selectedPattern();
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Select a structured pattern first."),
                             tr("To append a child pattern, select a structured pattern first."));
    return;
  }

  StructuredPattern *structure = parent->as<StructuredPattern>();

  Address insertionAddress;
  if (structure->numChildPattern()) {
    AbstractPattern *pred = structure->childPattern(structure->numChildPattern()-1);
    insertionAddress = pred->address();
    if (FixedPattern *fixed = pred->as<FixedPattern>())
      insertionAddress += fixed->size();
  }

  if (! _editPattern(mimeData->pattern(), parent->codeplug())) {
    return;
  }

  if (! structure->addChildPattern(mimeData->pattern())) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to {}.").arg(parent->meta().name()));
    return;
  }

  QGuiApplication::clipboard()->clear();
}


void
PatternView::pastePatternAbove() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  AbstractPattern *nextSibling = selectedPattern();
  if (nullptr == nextSibling) {
    QMessageBox::information(nullptr, tr("Select a sibling first."),
                             tr("To insert a pattern above another pattern, select a pattern first."));
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern *>(nextSibling->parent());
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be structured pattern."),
                             tr("The parent of the selected pattern must be a structured pattern."));
    return;
  }

  Address insertionAddress = nextSibling->address();

  StructuredPattern *structure = parent->as<StructuredPattern>();
  unsigned int insertionIndex = structure->indexOf(nextSibling);
  if (! parent->is<ElementPattern>()) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return;
  }

  if (! mimeData->pattern()->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can onyl add fixed-sized patterns to an element pattern."));
    return;
  }

  if (! _editPattern(mimeData->pattern(), parent->codeplug())) {
    return;
  }

  if (! parent->as<ElementPattern>()->insertChildPattern(mimeData->pattern()->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    return;
  }

  QGuiApplication::clipboard()->clear();
}


void
PatternView::pastePatternBelow() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  AbstractPattern *nextSibling = selectedPattern();
  if (nullptr == nextSibling) {
    QMessageBox::information(nullptr, tr("Select a sibling first."),
                             tr("To insert a pattern below another pattern, select a pattern first."));
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern *>(nextSibling->parent());
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be structured pattern."),
                             tr("The parent of the selected pattern must be a structured pattern."));
    return;
  }

  Address insertionAddress = nextSibling->address();
  if (FixedPattern *fixed = nextSibling->as<FixedPattern>())
    insertionAddress += fixed->size();

  StructuredPattern *structure = parent->as<StructuredPattern>();
  unsigned int insertionIndex = structure->indexOf(nextSibling) + 1;
  if (! parent->is<ElementPattern>()) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return;
  }

  if (! mimeData->pattern()->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can onyl add fixed-sized patterns to an element pattern."));
    return;
  }

  if (! _editPattern(mimeData->pattern(), parent->codeplug())) {
    return;
  }


  if (! parent->as<ElementPattern>()->insertChildPattern(mimeData->pattern()->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    return;
  }

  QGuiApplication::clipboard()->clear();
}


void
PatternView::removeSelected() {
  if (nullptr == selectedPattern()) {
    QMessageBox::information(nullptr, tr("Select a pattern first"),
                             tr("Select the pattern to remove."));
    return;
  }

  StructuredPattern *parent = dynamic_cast<StructuredPattern *>(selectedPattern()->parent());
  if (nullptr == parent) {
    QMessageBox::information(nullptr, tr("Cannot remove pattern"),
                             tr("The parent of the selected pattern is not a structured pattern."));
    return;
  }

  parent->deleteChild(parent->indexOf(selectedPattern()));
}


void
PatternView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  QTreeView::selectionChanged(selected, deselected);

  if (0 == selected.indexes().size()) {
    emit canEdit(false);
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canSplitFieldPattern(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    emit canView(false);
    return;
  }

  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(
        selected.indexes().back().internalPointer());
  if (nullptr == pattern) {
    emit canEdit(false);
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canSplitFieldPattern(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    emit canView(false);
    return;
  }

  emit canEdit(true);

  if (pattern->is<CodeplugPattern>()) {
    emit canAppendPattern(true);
    emit canInsertPatternAbove(false);
    emit canSplitFieldPattern(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    emit canView(false);
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern*>(pattern->parent());
  if (nullptr == parent) {
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canSplitFieldPattern(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    return;
  }

  emit canRemove(true);

  if (pattern->is<RepeatPattern>() || pattern->is<BlockRepeatPattern>() || pattern->is<FixedRepeatPattern>()) {
    emit canAppendPattern(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canSplitFieldPattern(false);
  } else if (pattern->is<ElementPattern>()) {
    emit canAppendPattern(true);
    emit canSplitFieldPattern(false);
    emit canView(true);
  } else {
    emit canAppendPattern(false);
  }

  if (parent->is<ElementPattern>()) {
    emit canInsertPatternAbove(true);
    emit canInsertPatternBelow(true);
    if (pattern->is<UnknownFieldPattern>())
      emit canSplitFieldPattern(true);
    else
      emit canSplitFieldPattern(false);
  } else {
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
    emit canSplitFieldPattern(false);
  }
}

void
PatternView::onShowContextMenu(const QPoint &point) {
  Application *app = qobject_cast<Application *>(Application::instance());

  QMenu contextMenu(this);
  contextMenu.addAction(app->findObject<QAction>("actionEdit_pattern"));
  contextMenu.addAction(app->findObject<QAction>("actionViewPattern"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionAppendNewPattern"));
  contextMenu.addAction(app->findObject<QAction>("actionInsert_above"));
  contextMenu.addAction(app->findObject<QAction>("actionSplitUnknownField"));
  contextMenu.addAction(app->findObject<QAction>("actionInsert_below"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionCopyPattern"));
  contextMenu.addAction(app->findObject<QAction>("actionCutPattern"));
  contextMenu.addAction(app->findObject<QAction>("actionPastePatternAsChild"));
  contextMenu.addAction(app->findObject<QAction>("actionPastePatternAbove"));
  contextMenu.addAction(app->findObject<QAction>("actionPastePatternBelow"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionDelete_pattern"));

  contextMenu.exec(mapToGlobal(point));
}


void
PatternView::save() {
  if (nullptr == _pattern)
    return;

  if (_pattern->source().isFile() && _pattern->source().isWritable()) {
    if (! _pattern->save()) {
      QMessageBox::critical(nullptr, tr("Cannot save pattern"),
                            tr("Cannot save codeplug pattern to '%1'.")
                            .arg(_pattern->source().filePath()));
    }
  } else {
    QMessageBox::critical(nullptr, tr("Cannot save pattern"),
                          tr("Cannot save codeplug pattern to '%1'. Pattern is likely build-in.")
                          .arg(_pattern->source().filePath()));
  }
}
