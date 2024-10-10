#include "patternview.hh"
#include "pattern.hh"
#include "device.hh"
#include "application.hh"
#include "patternwrapper.hh"
#include "logger.hh"

#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include "sparserepeatdialog.hh"
#include "blockrepeatdialog.hh"
#include "fixedrepeatdialog.hh"
#include "elementdialog.hh"
#include "integerfielddialog.hh"
#include "enumfielddialog.hh"
#include "stringfielddialog.hh"
#include "unusedfielddialog.hh"
#include "newpatterndialog.hh"



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

  if (pattern->is<RepeatPattern>()) {
    SparseRepeatDialog dialog;
    dialog.setPattern(pattern->as<RepeatPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<BlockRepeatPattern>()) {
    BlockRepeatDialog dialog;
    dialog.setPattern(pattern->as<BlockRepeatPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<FixedRepeatPattern>()) {
    FixedRepeatDialog dialog;
    dialog.setPattern(pattern->as<FixedRepeatPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<ElementPattern>()) {
    ElementDialog dialog;
    dialog.setPattern(pattern->as<ElementPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<IntegerFieldPattern>()) {
    IntegerFieldDialog dialog;
    dialog.setPattern(pattern->as<IntegerFieldPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<EnumFieldPattern>()) {
    EnumFieldDialog dialog;
    dialog.setPattern(pattern->as<EnumFieldPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<StringFieldPattern>()) {
    StringFieldDialog dialog;
    dialog.setPattern(pattern->as<StringFieldPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  } else if (pattern->is<UnusedFieldPattern>()) {
    UnusedFieldDialog dialog;
    dialog.setPattern(pattern->as<UnusedFieldPattern>());
    if (QDialog::Accepted == dialog.exec()) {

    }
  }
}

void
PatternView::appendPattern() {
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
  if (! structure->addChildPattern(newPattern)) {
    QMessageBox::information(nullptr, tr("Cannot append pattern."),
                             tr("Cannot append pattern to {}.").arg(parent->meta().name()));
    newPattern->deleteLater();
    return;
  }
}

void
PatternView::insertPatternAbove() {
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

  if (! parent->as<ElementPattern>()->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}


void
PatternView::insertPatternBelow() {
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

  if (! parent->as<ElementPattern>()->insertChildPattern(newPattern->as<FixedPattern>(), insertionIndex)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern to element."),
                             tr("Element pattern rejected child."));
    newPattern->deleteLater();
    return;
  }
}



void
PatternView::removeSelected() {
  if (nullptr == selectedPattern())
    return;

  StructuredPattern *parent = dynamic_cast<StructuredPattern *>(selectedPattern()->parent());
  if (nullptr == parent)
    return;

  parent->deleteChild(parent->indexOf(selectedPattern()));
}


void
PatternView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  QTreeView::selectionChanged(selected, deselected);

  if (0 == selected.indexes().size()) {
    emit canEdit(false);
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    return;
  }

  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(
        selected.indexes().back().internalPointer());
  if (nullptr == pattern) {
    emit canEdit(false);
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    return;
  }

  emit canEdit(true);

  if (pattern->is<CodeplugPattern>()) {
    emit canAppendPattern(true);
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    return;
  }

  AbstractPattern *parent = qobject_cast<AbstractPattern*>(pattern->parent());
  if (nullptr == parent) {
    emit canAppendPattern(false);
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
    emit canRemove(false);
    return;
  }

  emit canRemove(true);

  if (pattern->is<RepeatPattern>() || pattern->is<BlockRepeatPattern>() || pattern->is<FixedRepeatPattern>()) {
    emit canAppendPattern(0 == pattern->as<StructuredPattern>()->numChildPattern());
  } else if (pattern->is<ElementPattern>()) {
    emit canAppendPattern(true);
  } else {
    emit canAppendPattern(false);
  }

  if (parent->is<ElementPattern>()) {
    emit canInsertPatternAbove(true);
    emit canInsertPatternBelow(true);
  } else {
    emit canInsertPatternAbove(false);
    emit canInsertPatternBelow(false);
  }
}

void
PatternView::onShowContextMenu(const QPoint &point) {
  Application *app = qobject_cast<Application *>(Application::instance());

  QMenu contextMenu(this);
  contextMenu.addAction(app->findObject<QAction>("actionEdit_pattern"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionAppend_pattern"));
  contextMenu.addAction(app->findObject<QAction>("actionInsert_above"));
  contextMenu.addAction(app->findObject<QAction>("actionInsert_below"));
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
