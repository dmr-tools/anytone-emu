#include "patternview.hh"
#include "pattern.hh"
#include "device.hh"
#include "application.hh"
#include "patternwrapper.hh"

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QClipboard>
#include <QContextMenuEvent>

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
#include "questiondialog.hh"
#include "patternimportdialog.hh"
#include "logger.hh"



PatternView::PatternView(QWidget *parent)
  : QTreeView{parent}, _pattern(nullptr)
{
  Application *app = qobject_cast<Application *>(Application::instance());

  _pattern = app->device()->pattern();
  if (_pattern)
    setModel(new PatternWrapper(_pattern));

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

  showPatternEditor(pattern);
}


bool
PatternView::showPatternEditor(AbstractPattern *pattern, const CodeplugPattern *codeplug) {
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
  AbstractPattern *parent = selectedParent();
  if (nullptr == parent)
    return;

  NewPatternDialog dialog(parent, Address(), nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.create();

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! dynamic_cast<StructuredPattern*>(parent)->addChildPattern(newPattern)) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to %1.").arg(parent->meta().name()));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertNewPatternAbove() {
  auto nextSibling = selectedSibling();
  if (nullptr == nextSibling)
    return;
  auto parent = qobject_cast<ElementPattern *>(nextSibling->parent());

  Address insertionAddress = nextSibling->address();
  unsigned int insertionIndex = parent->indexOf(nextSibling);

  NewPatternDialog dialog(parent, insertionAddress, nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  auto newPattern = dialog.create()->as<FixedPattern>();

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(newPattern, insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertNewPatternBelow() {
  auto prevSibling = selectedSibling();
  auto parent = qobject_cast<ElementPattern *>(prevSibling->parent());

  Address insertionAddress = prevSibling->address();
  if (FixedPattern *fixed = prevSibling->as<FixedPattern>())
    insertionAddress += fixed->size();
  unsigned int insertionIndex = parent->indexOf(prevSibling) + 1;

  NewPatternDialog dialog(parent, insertionAddress, nullptr);
  if (QDialog::Accepted != dialog.exec())
    return;

  auto newPattern = dialog.create()->as<FixedPattern>();

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(newPattern, insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::appendImportedPattern() {
  auto parent = selectedParent();
  if (nullptr == parent)
    return;

  PatternImportDialog dialog(Application::instance()->catalog());
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.copy();
  newPattern->setAddress(Address());
  newPattern->setParent(parent);

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->as<StructuredPattern>()->addChildPattern(newPattern)) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to %1.").arg(parent->meta().name()));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertImportedPatternAbove() {
  auto nextSibling = selectedSibling();
  if (nullptr == nextSibling)
    return;
  auto parent = qobject_cast<ElementPattern *>(nextSibling->parent());

  Address insertionAddress = nextSibling->address();
  unsigned int insertionIndex = parent->indexOf(nextSibling);

  PatternImportDialog dialog(Application::instance()->catalog());
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.copy();
  newPattern->setAddress(Address());
  newPattern->setParent(parent);

  if (! newPattern->is<FixedPattern>()) {
    QMessageBox::critical(nullptr, tr("Canont import pattern to element."),
                          tr("Can only import fixed element."));
    newPattern->deleteLater();
    return;
  }

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertImportedPatternBelow(){
  auto prevSibling = selectedSibling();
  auto parent = qobject_cast<ElementPattern *>(prevSibling->parent());

  Address insertionAddress = prevSibling->address();
  if (FixedPattern *fixed = prevSibling->as<FixedPattern>())
    insertionAddress += fixed->size();
  unsigned int insertionIndex = parent->indexOf(prevSibling) + 1;

  PatternImportDialog dialog(Application::instance()->catalog());
  if (QDialog::Accepted != dialog.exec())
    return;

  AbstractPattern *newPattern = dialog.copy();
  newPattern->setAddress(Address());
  newPattern->setParent(parent);

  if (! newPattern->is<FixedPattern>()) {
    QMessageBox::critical(nullptr, tr("Canont import pattern to element."),
                          tr("Can only import fixed element."));
    newPattern->deleteLater();
    return;
  }

  if (! showPatternEditor(newPattern, parent->codeplug())) {
    newPattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::pastePatternAsChild() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  auto parent = selectedParent();
  if (nullptr == parent)
    return;

  auto pattern = mimeData->pattern();
  pattern->setParent(parent);
  QGuiApplication::clipboard()->clear();
  pattern->setAddress(Address());

  if (! showPatternEditor(pattern, parent->codeplug()))
    return;

  if (! parent->as<StructuredPattern>()->addChildPattern(pattern)) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to {}.").arg(parent->meta().name()));
    pattern->deleteLater();
    return;
  }

  QGuiApplication::clipboard()->clear();
}


void
PatternView::pastePatternAbove() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  auto nextSibling = selectedSibling();
  if (nullptr == nextSibling)
    return;

  auto parent = qobject_cast<ElementPattern *>(nextSibling->parent());
  auto pattern = mimeData->pattern();
  pattern->setParent(parent);
  QGuiApplication::clipboard()->clear();

  Address insertionAddress = nextSibling->address();
  unsigned int insertionIndex = parent->indexOf(nextSibling);
  pattern->setAddress(insertionAddress);

  if (! pattern->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can onyl add fixed-sized patterns to an element pattern."));
    pattern->deleteLater();
    return;
  }

  if (! showPatternEditor(pattern, parent->codeplug())) {
    pattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(pattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    pattern->deleteLater();
    return;
  }

  QGuiApplication::clipboard()->clear();
}


void
PatternView::pastePatternBelow() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if ((nullptr == mimeData) || (nullptr == mimeData->pattern()))
    return;

  auto prevSibling = selectedSibling();
  if (nullptr == prevSibling)
    return;

  auto parent = qobject_cast<ElementPattern *>(prevSibling->parent());
  auto pattern = mimeData->pattern();
  pattern->setParent(parent);
  QGuiApplication::clipboard()->clear();

  Address insertionAddress = prevSibling->address() + prevSibling->size();
  unsigned int insertionIndex = parent->indexOf(prevSibling) + 1;
  pattern->setAddress(insertionAddress);

  if (! pattern->is<FixedPattern>()) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Can only add fixed-sized patterns to an element pattern."));
    pattern->deleteLater();
    return;
  }

  if (! showPatternEditor(pattern, parent->codeplug())) {
    pattern->deleteLater();
    return;
  }

  if (! parent->insertChildPattern(pattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    pattern->deleteLater();
    return;
  }

  QGuiApplication::clipboard()->clear();
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
  if (! showPatternEditor(inserted, parent->codeplug())) {
    inserted->deleteLater();
    return;
  }

  // compute size of head and tail unknown fields:
  Offset headFieldSize = insertionAddr - startAddress;
  // check size of inserted field
  if ((! inserted->size().isValid()) || (0 == inserted->size().bits())) {
    QMessageBox::information(
          nullptr, tr("Inserted field has no size"),
          tr("The newly inserted field should have some size."));
    inserted->deleteLater();
    return;
  } else if (originalSize < (headFieldSize + inserted->size())) {
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

  auto res = QuestionDialog::ask(
        "patternViewDeleteSelected", tr("Delete pattern?"),
        tr("You are abot to delete the pattern '%1'. "
           "The cannot be undone. Do you want to proceed?").arg(selectedPattern()->meta().name()));
  if (QMessageBox::Yes != res)
    return;

  parent->deleteChild(parent->indexOf(selectedPattern()));
}


void
PatternView::markAsUpdated() {
  if (nullptr == selectedPattern()) {
    QMessageBox::information(nullptr, tr("Select a pattern first"),
                             tr("Select the pattern to mark as updated."));
    return;
  }

  selectedPattern()->meta().setFlags(PatternMeta::Flags::Done);
  const CodeplugPattern *codeplug = selectedPattern()->codeplug();
  if ((nullptr != codeplug) && codeplug->meta().hasFirmwareVersion())
    selectedPattern()->meta().setFirmwareVersion(codeplug->meta().firmwareVersion());
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
    emit canMarkUpdated(false);
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
    emit canMarkUpdated(false);
    return;
  }

  emit canEdit(true);
  emit canMarkUpdated(! pattern->is<UnknownFieldPattern>());

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
    emit canView(false);
    return;
  }

  emit canRemove(true);

  if (pattern->is<RepeatPattern>() || pattern->is<BlockRepeatPattern>() || pattern->is<FixedRepeatPattern>()) {
    emit canAppendPattern(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canSplitFieldPattern(false);
    emit canView(false);
  } else if (pattern->is<ElementPattern>()) {
    emit canAppendPattern(true);
    emit canSplitFieldPattern(false);
    emit canView(true);
  } else {
    emit canView(true);
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
PatternView::contextMenuEvent(QContextMenuEvent *event) {
  QMenu contextMenu;
  contextMenu.addActions({ parent()->findChild<QAction*>("actionViewElement"),
                           parent()->findChild<QAction*>("actionEditPattern")});
  contextMenu.addSeparator();
  contextMenu.addActions({ parent()->findChild<QAction*>("actionAppendNewPattern"),
                           parent()->findChild<QAction*>("actionInsertNewPatternAbove"),
                           parent()->findChild<QAction*>("actionSplitUnknownField"),
                           parent()->findChild<QAction*>("actionInsertNewPatternBelow") });
  contextMenu.addSeparator();
  contextMenu.addActions({ parent()->findChild<QAction*>("actionAppendImportedPattern"),
                           parent()->findChild<QAction*>("actionInsertImportedPatternAbove"),
                           parent()->findChild<QAction*>("actionInsertImportedPatternBelow") });
  contextMenu.addSeparator();
  contextMenu.addActions({ parent()->findChild<QAction*>("actionCopyPattern"),
                           parent()->findChild<QAction*>("actionPastePatternAsChild"),
                           parent()->findChild<QAction*>("actionPastePatternAbove"),
                           parent()->findChild<QAction*>("actionPastePatternBelow") });
  contextMenu.addSeparator();
  contextMenu.addAction(parent()->findChild<QAction*>("actionMarkPatternAsUpdated"));
  contextMenu.addSeparator();
  contextMenu.addActions({ parent()->findChild<QAction*>("actionMarkFieldAsUnknown"),
                           parent()->findChild<QAction*>("actionDeletePattern") });
  contextMenu.exec(event->globalPos());
}


AbstractPattern *
PatternView::selectedParent() const {
  AbstractPattern *parent = selectedPattern();
  if ((nullptr == parent) || (! parent->is<StructuredPattern>())) {
    QMessageBox::information(nullptr, tr("Select a structured pattern first."),
                             tr("To append a child pattern, select a structured pattern first."));
    return nullptr;
  }

  return parent;
}


FixedPattern *
PatternView::selectedSibling() const {
  AbstractPattern *nextSibling = selectedPattern();

  if ((nullptr == nextSibling) || (! nextSibling->is<FixedPattern>())) {
    QMessageBox::information(nullptr, tr("Select a sibling first."),
                             tr("To insert a pattern above another pattern, select a fixed pattern first."));
    return nullptr;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern *>(nextSibling->parent());
  if ((nullptr == parent) || (! parent->is<ElementPattern>())) {
    QMessageBox::information(nullptr, tr("Parent must be an element pattern."),
                             tr("The parent of the selected pattern must be an element pattern."));
    return nullptr;
  }

  return nextSibling->as<FixedPattern>();
}


void
PatternView::save() {
  if (nullptr == _pattern) {
    logWarn() << "Cannot save pattern, there is none.";
    return;
  }

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
