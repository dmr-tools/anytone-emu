#include "enumfielddialog.hh"
#include "ui_enumfielddialog.h"
#include "codeplugpattern.hh"
#include "enumfieldpatternwrapper.hh"
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QItemEditorFactory>
#include <QStandardItemEditorCreator>
#include <QSettings>
#include <QMessageBox>
#include "logger.hh"


EnumFieldDialog::EnumFieldDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::EnumFieldDialog), _pattern(nullptr)
{
  ui->setupUi(this);

  QSettings settings;
  if (settings.contains("layout/enumFieldDialogSize"))
    restoreGeometry(settings.value("layout/enumFieldDialogSize").toByteArray());
  if (settings.contains("layout/enumFieldDialogItemsHeaderState"))
    ui->items->horizontalHeader()->restoreState(
          settings.value("layout/enumFieldDialogItemsHeaderState").toByteArray());

  QStyledItemDelegate *delegate = new QStyledItemDelegate(ui->items);
  QItemEditorFactory *factory = new QItemEditorFactory;
  factory->registerEditor(
        QMetaType::fromType<PatternMeta::Flags>().id(),
        new QStandardItemEditorCreator<PatternMetaFlagsEditor>());
  delegate->setItemEditorFactory(factory);
  ui->items->setItemDelegateForColumn(4, delegate);

  connect(ui->addItem, &QPushButton::clicked, this, &EnumFieldDialog::onAddItem);
  connect(ui->deleteItem, &QPushButton::clicked, this, &EnumFieldDialog::onDelItem);
}

EnumFieldDialog::~EnumFieldDialog()
{
  delete ui;
}


void
EnumFieldDialog::done(int res) {
  QSettings settings;
  settings.setValue("layout/enumFieldDialogSize", saveGeometry());
  settings.setValue("layout/enumFieldDialogItemsHeaderState",
                    ui->items->horizontalHeader()->saveState());
  QDialog::done(res);
}


void
EnumFieldDialog::setPattern(EnumFieldPattern *pattern) {
  _pattern = pattern;
  if (pattern->hasImplicitAddress() && !pattern->hasAddress()) {
    ui->address->setText("implicit");
    ui->address->setEnabled(false);
  } else if (pattern->hasImplicitAddress()) {
    ui->address->setText(pattern->address().toString());
    ui->address->setEnabled(false);
  } else {
    ui->address->setText(pattern->address().toString());
  }

  ui->width->setValue(_pattern->size().bits());
  ui->items->setModel(new EnumFieldPatternWrapper(pattern));

  ui->metaEdit->setPatternMeta(&pattern->meta());
}


void
EnumFieldDialog::accept() {
  Address addr = Address::fromString(ui->address->text());
  if (! addr.isValid()) {
    QMessageBox::critical(nullptr, tr("Invalid address format."),
                          tr("Invalid address format '%1'.").arg(ui->address->text()));
    return;
  }

  _pattern->setAddress(addr);
  _pattern->setWidth(Offset::fromBits(ui->width->value()));

  ui->metaEdit->apply();

  QDialog::accept();
}


void
EnumFieldDialog::onAddItem() {
  if (nullptr == _pattern)
    return;

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
EnumFieldDialog::onDelItem() {
  QItemSelection selection = ui->items->selectionModel()->selection();
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
