//
// Created by hannes on 19.06.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_EnumItemsEditor.h" resolved

#include "enumitemseditor.h"
#include "ui_EnumItemsEditor.h"

EnumItemsEditor::EnumItemsEditor(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::EnumItemsEditor) {
  ui->setupUi(this);
}

EnumItemsEditor::~EnumItemsEditor() {
  delete ui;
}
