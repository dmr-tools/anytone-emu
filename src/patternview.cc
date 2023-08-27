#include "patternview.hh"
#include "codeplugpattern.hh"
#include "device.hh"
#include "application.hh"
#include "patternwrapper.hh"
#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include "sparserepeatdialog.hh"
#include "blockrepeatdialog.hh"
#include "elementdialog.hh"
#include "integerfielddialog.hh"
#include "enumfielddialog.hh"


PatternView::PatternView(QWidget *parent)
  : QTreeView{parent}, _pattern(nullptr)
{
  Application *app = qobject_cast<Application *>(Application::instance());

  _pattern = app->device()->pattern();
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
  }
}

void
PatternView::addSparseRepeat() {
  AbstractPattern *parent = selectedPattern();
  if ((nullptr == parent) || (! parent->is<GroupPattern>())) {
    QMessageBox::information(nullptr, tr("Select a group pattern first."),
                             tr("To add a sparse repeat pattern, select a group pattern first."));
    return;
  }

  StructuredPattern *structure = parent->as<StructuredPattern>();
  auto sr = new RepeatPattern();
  sr->meta().setName("Unnamed sparse repeat");

  if (! structure->addChildPattern(sr)) {
    QMessageBox::information(nullptr, tr("Cannot add pattern."),
                             tr("Parent rejected pattern."));
    delete sr;
  }
}

void
PatternView::addBlockRepeat() {

}

void
PatternView::addFixedRepeat() {

}

void
PatternView::addElement() {

}

void
PatternView::addInteger() {

}

void
PatternView::addBit() {

}

void
PatternView::addUInt8() {

}

void
PatternView::addInt8() {

}

void
PatternView::addUInt16() {

}

void
PatternView::addInt16() {

}

void
PatternView::addUInt32() {

}

void
PatternView::addInt32() {

}

void
PatternView::addBCD8() {

}

void
PatternView::addEnum() {

}

void
PatternView::addUnused() {

}

void
PatternView::addUnknown() {

}

void
PatternView::removeSelected() {

}


void
PatternView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
  QTreeView::selectionChanged(selected, deselected);

  if (0 == selected.indexes().size()) {
    emit canEdit(false);
    emit canAddSparse(false);
    emit canAddBlock(false);
    emit canAddFixed(false);
    emit canRemove(false);
    return;
  }

  AbstractPattern *pattern = reinterpret_cast<AbstractPattern *>(
        selected.indexes().back().internalPointer());
  if (pattern)
    emit canEdit(true);

  if (! pattern->is<StructuredPattern>()) {
    emit canAddSparse(false);
    emit canAddBlock(false);
    emit canAddFixed(false);
    emit canRemove(true);
    return;
  }

  if (pattern->is<CodeplugPattern>()) {
    emit canAddSparse(true);
    emit canAddBlock(true);
    emit canAddFixed(true);
    emit canRemove(false);
  } else if (pattern->is<RepeatPattern>()) {
    emit canAddSparse(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canAddBlock(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canAddFixed(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canRemove(true);
  } else if (pattern->is<BlockRepeatPattern>() || pattern->is<FixedRepeatPattern>()) {
    emit canAddSparse(false);
    emit canAddBlock(false);
    emit canAddFixed(0 == pattern->as<StructuredPattern>()->numChildPattern());
    emit canRemove(true);
  } else if (pattern->is<FixedPattern>() || pattern->is<BlockPattern>()) {
    emit canAddSparse(false);
    emit canAddBlock(false);
    emit canAddFixed(true);
    emit canRemove(true);
  }
}

void
PatternView::onShowContextMenu(const QPoint &point) {
  Application *app = qobject_cast<Application *>(Application::instance());

  QMenu contextMenu(this);
  contextMenu.addAction(app->findObject<QAction>("actionEdit_pattern"));
  contextMenu.addSeparator();

  QMenu *repeatMenu = new QMenu(tr("Add repeat"));
  repeatMenu->addAction(app->findObject<QAction>("actionAdd_sparse_repeat"));
  repeatMenu->addAction(app->findObject<QAction>("actionAdd_block_repeat"));
  repeatMenu->addAction(app->findObject<QAction>("actionAdd_fixed_repeat"));
  contextMenu.addMenu(repeatMenu);

  contextMenu.addAction(app->findObject<QAction>("actionAdd_element"));

  QMenu *intMenu = new QMenu(tr("Add integer"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_bit"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_uint8"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_int8"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_uint16"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_int16"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_uint32"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_int32"));
  intMenu->addAction(app->findObject<QAction>("actionAdd_BCD8"));
  contextMenu.addMenu(intMenu);

  contextMenu.addAction(app->findObject<QAction>("actionAdd_enum"));
  contextMenu.addAction(app->findObject<QAction>("actionAdd_unused"));
  contextMenu.addAction(app->findObject<QAction>("actionAdd_unknown"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionDelete_pattern"));

  contextMenu.exec(mapToGlobal(point));
}
