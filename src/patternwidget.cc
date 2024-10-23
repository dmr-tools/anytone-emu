#include "patternwidget.hh"
#include "ui_patternwidget.h"

#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QClipboard>

#include "application.hh"
#include "pattern.hh"
#include "patternmimedata.hh"


PatternWidget::PatternWidget(QWidget *parent)
  : QWidget(parent), ui(new Ui::patternWidget)
{
  QSettings settings;
  Application *app = qobject_cast<Application*>(Application::instance());

  ui->setupUi(this);

  if (settings.contains("layout/patternsHeaderState"))
    ui->patterns->header()->restoreState(settings.value("layout/patternsHeaderState").toByteArray());

  auto toolBar = new QToolBar();
  int defaultSize = app->style()->pixelMetric(QStyle::PM_ToolBarIconSize),
      toolBarIconSize = 2*defaultSize/3;
  toolBar->setIconSize(QSize(toolBarIconSize, toolBarIconSize));

  toolBar->addAction(ui->actionSavePattern);
  ui->actionViewElement->setIcon(QIcon::fromTheme("inspect"));
  toolBar->addAction(ui->actionViewElement);
  ui->actionEditPattern->setIcon(QIcon::fromTheme("edit"));
  toolBar->addAction(ui->actionEditPattern);
  auto insertMenu = new QMenu();
  ui->actionAppendNewPattern->setIcon(QIcon::fromTheme("insert-child"));
  insertMenu->addAction(ui->actionAppendNewPattern);
  ui->actionInsertNewPatternAbove->setIcon(QIcon::fromTheme("insert-above"));
  insertMenu->addAction(ui->actionInsertNewPatternAbove);
  ui->actionSplitUnknownField->setIcon(QIcon::fromTheme("split-unknown"));
  insertMenu->addAction(ui->actionSplitUnknownField);
  ui->actionInsertNewPatternBelow->setIcon(QIcon::fromTheme("insert-below"));
  insertMenu->addAction(ui->actionInsertNewPatternBelow);
  auto insertAction = new QAction(QIcon::fromTheme("document-new"), tr("Insert new pattern"));
  insertAction->setMenu(insertMenu);
  toolBar->addAction(insertAction);
  ui->actionCopyPattern->setIcon(QIcon::fromTheme("edit-copy"));
  toolBar->addAction(ui->actionCopyPattern);
  auto pasteMenu = new QMenu();
  ui->actionPastePatternAsChild->setIcon(QIcon::fromTheme("insert-child"));
  pasteMenu->addAction(ui->actionPastePatternAsChild);
  ui->actionPastePatternAbove->setIcon(QIcon::fromTheme("insert-above"));
  pasteMenu->addAction(ui->actionPastePatternAbove);
  ui->actionPastePatternBelow->setIcon(QIcon::fromTheme("insert-below"));
  pasteMenu->addAction(ui->actionPastePatternBelow);
  auto pasteAction = new QAction(QIcon::fromTheme("edit-paste"), tr("Paste copied pattern"));
  pasteAction->setMenu(pasteMenu);
  toolBar->addAction(pasteAction);
  toolBar->addSeparator();
  ui->actionMarkFieldAsUnknown->setIcon(QIcon::fromTheme("edit-erase"));
  toolBar->addAction(ui->actionMarkFieldAsUnknown);
  toolBar->addAction(ui->actionDeletePattern);


  qobject_cast<QVBoxLayout*>(layout())->insertWidget(0, toolBar);

  connect(ui->patterns, &PatternView::canEdit, ui->actionEditPattern, &QAction::setEnabled);
  connect(ui->actionEditPattern, &QAction::triggered, ui->patterns, &PatternView::editPattern);
  connect(ui->patterns, &PatternView::canView, ui->actionViewElement, &QAction::setEnabled);
  connect(ui->actionViewElement, &QAction::triggered, this, &PatternWidget::onViewPattern);

  connect(ui->patterns, &PatternView::canAppendPattern, ui->actionAppendNewPattern, &QAction::setEnabled);
  connect(ui->actionAppendNewPattern, &QAction::triggered, ui->patterns, &PatternView::appendNewPattern);
  connect(ui->patterns, &PatternView::canAppendPattern, ui->actionPastePatternAsChild, &QAction::setEnabled);
  connect(ui->actionPastePatternAsChild, &QAction::triggered, ui->patterns, &PatternView::pastePatternAsChild);

  connect(ui->patterns, &PatternView::canInsertPatternAbove, ui->actionInsertNewPatternAbove, &QAction::setEnabled);
  connect(ui->actionInsertNewPatternAbove, &QAction::triggered, ui->patterns, &PatternView::insertNewPatternAbove);
  connect(ui->patterns, &PatternView::canInsertPatternAbove, ui->actionPastePatternAbove, &QAction::setEnabled);
  connect(ui->actionPastePatternAbove, &QAction::triggered, ui->patterns, &PatternView::pastePatternAbove);

  connect(ui->patterns, &PatternView::canSplitFieldPattern, ui->actionSplitUnknownField, &QAction::setEnabled);
  connect(ui->actionSplitUnknownField, &QAction::triggered, ui->patterns, &PatternView::splitFieldPattern);
  ui->actionSplitUnknownField->setIcon(QIcon::fromTheme("edit-split-element"));

  connect(ui->patterns, &PatternView::canInsertPatternBelow, ui->actionInsertNewPatternBelow, &QAction::setEnabled);
  connect(ui->actionInsertNewPatternBelow, &QAction::triggered, ui->patterns, &PatternView::insertNewPatternBelow);
  connect(ui->patterns, &PatternView::canInsertPatternBelow, ui->actionPastePatternBelow, &QAction::setEnabled);
  connect(ui->actionPastePatternBelow, &QAction::triggered, ui->patterns, &PatternView::pastePatternBelow);

  connect(ui->patterns, &PatternView::canRemove, ui->actionDeletePattern, &QAction::setEnabled);
  connect(ui->actionDeletePattern, &QAction::triggered, ui->patterns, &PatternView::removeSelected);

  connect(ui->patterns, &PatternView::canRemove, ui->actionCopyPattern, &QAction::setEnabled);
  connect(ui->actionCopyPattern, &QAction::triggered, ui->patterns, &PatternView::copySelected);

  connect(ui->actionSavePattern, &QAction::triggered, ui->patterns, &PatternView::save);

  connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &PatternWidget::onClipboardDataChanged);
}

PatternWidget::~PatternWidget() {
  QSettings settings;
  settings.setValue("layout/patternsHeaderState", ui->patterns->header()->saveState());

  delete ui;
}


void
PatternWidget::onViewPattern() {
  AbstractPattern *pattern = ui->patterns->selectedPattern();
  if ((nullptr == pattern) || (! pattern->is<ElementPattern>()))
    return;

  emit viewPattern(pattern->as<ElementPattern>());
}


void
PatternWidget::onClipboardDataChanged() {
  auto mimeData = qobject_cast<const PatternMimeData *>(QGuiApplication::clipboard()->mimeData());
  if (nullptr == mimeData) {
    ui->actionPastePatternAsChild->setEnabled(false);
    ui->actionPastePatternAbove->setEnabled(false);
    ui->actionPastePatternBelow->setEnabled(false);
  } else {
    ui->actionPastePatternAsChild->setEnabled(true);
    ui->actionPastePatternAbove->setEnabled(true);
    ui->actionPastePatternBelow->setEnabled(true);
  }
}
