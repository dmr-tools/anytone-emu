#include "patternview.hh"
#include "codeplugpattern.hh"
#include "device.hh"
#include "application.hh"
#include "patternwrapper.hh"
#include "logger.hh"

#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include "sparserepeatdialog.hh"
#include "blockrepeatdialog.hh"
#include "elementdialog.hh"
#include "integerfielddialog.hh"
#include "enumfielddialog.hh"
#include "stringfielddialog.hh"
#include "unusedfielddialog.hh"


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
PatternView::addSparseRepeat() {
  AbstractPattern *parent = selectedPattern();
  if ((nullptr == parent) || (! parent->is<GroupPattern>())) {
    QMessageBox::information(nullptr, tr("Select a group pattern first."),
                             tr("To add a sparse repeat pattern, select a group pattern first."));
    return;
  }

  StructuredPattern *structure = parent->as<StructuredPattern>();
  auto sr = new RepeatPattern();
  sr->meta().setName("New sparse repeat");
  sr->meta().setFlags(PatternMeta::Flags::Incomplete);

  if (! structure->addChildPattern(sr)) {
    logWarn() << "Cannot add sparse repeat pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete sr;
  }
}

void
PatternView::addBlockRepeat() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new BlockRepeatPattern();
  pattern->meta().setName("New block repeat");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add block-repeat pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addFixedRepeat() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new FixedRepeatPattern();
  pattern->meta().setName("New fixed repeat");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add fixed repeat pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addElement() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new ElementPattern();
  pattern->meta().setName("New element");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add element pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addInteger() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New integer");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add integer pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addBit() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New bit");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(1));
  pattern->setFormat(IntegerFieldPattern::Format::Unsigned);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add bit pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addUInt8() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New uint8");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(8));
  pattern->setFormat(IntegerFieldPattern::Format::Unsigned);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add uint8 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addInt8() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New int8");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(8));
  pattern->setFormat(IntegerFieldPattern::Format::Signed);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add int8 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addUInt16() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New uint16");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(16));
  pattern->setFormat(IntegerFieldPattern::Format::Unsigned);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add uint16 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addInt16() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New int16");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(16));
  pattern->setFormat(IntegerFieldPattern::Format::Signed);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add int16 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addUInt32() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New uint32");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(32));
  pattern->setFormat(IntegerFieldPattern::Format::Unsigned);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add uint32 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addInt32() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New int32");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(32));
  pattern->setFormat(IntegerFieldPattern::Format::Signed);
  pattern->setEndian(IntegerFieldPattern::Endian::Little);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add int32 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addBCD8() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new IntegerFieldPattern();
  pattern->meta().setName("New BCD8");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  pattern->setWidth(Size::fromBits(32));
  pattern->setFormat(IntegerFieldPattern::Format::BCD);
  pattern->setEndian(IntegerFieldPattern::Endian::Big);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add bcd8 pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addEnum() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new EnumFieldPattern();
  pattern->meta().setName("New enum");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add enum pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addString() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new StringFieldPattern();
  pattern->meta().setName("New string");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add string pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addUnused() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new UnusedFieldPattern();
  pattern->meta().setName("New unused data pattern");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add unused data pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
    return;
  }
}

void
PatternView::addUnknown() {
  if (! selectedPattern()->is<StructuredPattern>())
    return;

  auto *pattern = new UnusedFieldPattern();
  pattern->meta().setName("New unknown data pattern");
  pattern->meta().setFlags(PatternMeta::Flags::Incomplete);
  if (! selectedPattern()->as<StructuredPattern>()->addChildPattern(pattern)) {
    logWarn() << "Cannot add unknonw data pattern to " << selectedPattern()->metaObject()->className()
              << " '" << selectedPattern()->meta().name() << "'.";
    delete pattern;
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
  contextMenu.addAction(app->findObject<QAction>("actionAdd_string"));
  contextMenu.addAction(app->findObject<QAction>("actionAdd_unused"));
  contextMenu.addAction(app->findObject<QAction>("actionAdd_unknown"));
  contextMenu.addSeparator();
  contextMenu.addAction(app->findObject<QAction>("actionDelete_pattern"));

  contextMenu.exec(mapToGlobal(point));
}
