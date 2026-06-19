//
// Created by hannes on 19.06.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_EnumItemsEditor.h" resolved

#include "enumitemseditor.hh"
#include "ui_enumitemseditor.h"
#include <QMessageBox>
#include <QSettings>
#include "enumfieldpatternwrapper.hh"
#include <QStyledItemDelegate>
#include <QStandardItemEditorCreator>
#include "logger.hh"


EnumItemsEditor::EnumItemsEditor(QWidget *parent)
  : QWidget(parent) , ui(new Ui::EnumItemsEditor)
{
  ui->setupUi(this);


  auto *delegate = new QStyledItemDelegate(ui->itemsView);
  auto *factory = new QItemEditorFactory;
  factory->registerEditor(
        QMetaType::fromType<PatternMeta::Flags>().id(),
        new QStandardItemEditorCreator<PatternMetaFlagsEditor>());
  delegate->setItemEditorFactory(factory);
  ui->itemsView->setItemDelegateForColumn(4, delegate);

  connect(ui->addButton, &QPushButton::clicked, this, &EnumItemsEditor::onAddItem);
  connect(ui->deleteButton, &QPushButton::clicked, this, &EnumItemsEditor::onDelItem);
}


EnumItemsEditor::~EnumItemsEditor() {
  delete ui;
}


void
EnumItemsEditor::done(int res) {
  QSettings settings;

  if (nullptr != ui->itemsView->model())
    settings.setValue("layout/enumFieldDialogItemsHeaderState",
                      ui->itemsView->horizontalHeader()->saveState());
}


void
EnumItemsEditor::setPattern(AbstractEnumFieldPattern *pattern, const CodeplugPattern *codeplug) {
  _pattern = pattern;
  ui->itemsView->setModel(new EnumFieldPatternWrapper(_pattern));

  QSettings settings;
  if (settings.contains("layout/enumFieldDialogItemsHeaderState"))
    ui->itemsView->horizontalHeader()->restoreState(
          settings.value("layout/enumFieldDialogItemsHeaderState").toByteArray());

}


void
EnumItemsEditor::onAddItem() {
  unsigned int value = 0;
  if (_pattern->numItems()) {
    value = _pattern->item(_pattern->numItems()-1)->value()+1;
  }

  auto item = new EnumFieldPatternItem();
  item->setName("new item");
  item->setValue(value);

  if (! _pattern->addItem(item)) {
    logError() << "Could not add new item to enum: Enum rejected item.";
    delete item;
  }
}

void
EnumItemsEditor::onDelItem() {
  QItemSelection selection = ui->itemsView->selectionModel()->selection();
  if ((1 != selection.size()) || (1 != selection.back().indexes().size())) {
    QMessageBox::information(nullptr, tr("Select an item first"),
                             tr("Select a single item fist."));
    return;
  }

  QModelIndex index = selection.back().indexes().back();
  if (! _pattern->deleteItem(index.row())) {
    QMessageBox::warning(nullptr, tr("Cannot delete item."),
                         tr("Cannot delete item for some reason."));
  }
}
