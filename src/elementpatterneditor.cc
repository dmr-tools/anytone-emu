#include "elementpatterneditor.hh"
#include "ui_elementpatterneditor.h"
#include <QMenu>
#include <QMessageBox>

#include "pattern.hh"
#include "patternview.hh"
#include "splitfieldpatterndialog.hh"
#include "questiondialog.hh"


ElementPatternEditor::ElementPatternEditor(QWidget *parent)
  : QWidget(parent), ui(new Ui::ElementPatternEditor)
{
  ui->setupUi(this);

  ui->editor->addActions({ ui->actionElementEditorEditPattern,
                           ui->actionElementEditorSplitUnknownField,
                           ui->actionElementEditorMarkAsUnknown });

  ui->actionElementEditorEditPattern->setIcon(QIcon::fromTheme("edit"));
  ui->actionElementEditorSplitUnknownField->setIcon(QIcon::fromTheme("edit-split-unknown"));
  ui->actionElementEditorMarkAsUnknown->setIcon(QIcon::fromTheme("edit-erase"));

  connect(ui->editor, &ElementPatternView::selectionChanged,
          this, &ElementPatternEditor::onSelectionChanged);
  connect(ui->editor, &ElementPatternView::doubleClicked,
          this, &ElementPatternEditor::onDoubleClick);

  connect(ui->actionElementEditorEditPattern, &QAction::triggered,
          this, &ElementPatternEditor::onEditPattern);
  connect(ui->actionElementEditorSplitUnknownField, &QAction::triggered,
          this, &ElementPatternEditor::onSplitField);
  connect(ui->actionElementEditorMarkAsUnknown, &QAction::triggered,
          this, &ElementPatternEditor::onMarkAsUnknown);
}

ElementPatternEditor::~ElementPatternEditor() {
  delete ui;
}

const ElementPattern *
ElementPatternEditor::pattern() const {
  return ui->editor->pattern();
}

void
ElementPatternEditor::setPattern(ElementPattern *pattern) {
  //ui->titleLabel->setText(QString("<h1>%1</h1>").arg(pattern->meta().name()));
  ui->editor->setPattern(pattern);
}


void
ElementPatternEditor::onSelectionChanged(FixedPattern *selected) {
  ui->editor->setContextMenuPolicy((nullptr == selected) ? Qt::NoContextMenu : Qt::DefaultContextMenu);
  ui->actionElementEditorEditPattern->setEnabled(selected);
  ui->actionElementEditorSplitUnknownField->setEnabled(selected && selected->is<UnknownFieldPattern>());
  ui->actionElementEditorMarkAsUnknown->setEnabled(selected && (!selected->is<UnknownFieldPattern>()));
}

void
ElementPatternEditor::onDoubleClick(FixedPattern *pattern) {
  PatternView::showPatternEditor(pattern);
}

void
ElementPatternEditor::onEditPattern() {
  FixedPattern *pattern = ui->editor->selectedPattern();
  if (pattern)
    PatternView::showPatternEditor(pattern);
}

void
ElementPatternEditor::onSplitField() {
  if (nullptr == ui->editor->selectedPattern())
    return;

  auto replaced = ui->editor->selectedPattern()->as<UnknownFieldPattern>();
  if (nullptr == replaced)
    return;

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
  if (! PatternView::showPatternEditor(inserted, parent->codeplug())) {
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
ElementPatternEditor::onMarkAsUnknown() {
  if (nullptr == ui->editor->selectedPattern())
    return;
  auto replaced = ui->editor->selectedPattern();

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

  auto res = QuestionDialog::ask(
        "elementPatternEditorMarkUnknown", tr("Replace pattern?"),
        tr("You are about to replace the pattern '%1' with an unknown field pattern of the same "
           "size. This cannot be undone. Do you want to proceed?").arg(replaced->meta().name()));
  if (QMessageBox::Yes != res)
    return;

  auto inserted = new UnknownFieldPattern();
  inserted->meta().setName("Unused data");
  if (parentElement->codeplug() && parentElement->codeplug()->meta().hasFirmwareVersion())
    inserted->meta().setFirmwareVersion(
          parentElement->codeplug()->meta().firmwareVersion());
  inserted->setWidth(originalSize);

  // Remove old pattern;
  parentElement->deleteChild(insertionIndex);
  parentElement->insertChildPattern(inserted, insertionIndex++);

  // done.
}
