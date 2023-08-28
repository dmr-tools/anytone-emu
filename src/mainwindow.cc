#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "application.hh"
#include "device.hh"
#include "image.hh"
#include "hexdump.hh"
#include "heximagedumpdocument.hh"
#include "hexelementdumpdocument.hh"
#include "heximagediffdocument.hh"
#include "imagecollectionwrapper.hh"
#include "patternwrapper.hh"
#include "logger.hh"
#include "logmessagelist.hh"
#include <QActionGroup>
#include <QSettings>
#include <QTextBrowser>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  QSettings settings;

  if (settings.contains("layout/mainWindowSize"))
    restoreGeometry(settings.value("layout/mainWindowSize").toByteArray());
  if (settings.contains("layout/mainWindowState"))
    restoreState(settings.value("layout/mainWindowState").toByteArray());
  if (settings.contains("layout/horizontalSplitterState"))
    ui->horizontalSplitter->restoreState(settings.value("layout/horizontalSplitterState").toByteArray());
  if (settings.contains("layout/verticalSplitterState"))
    ui->verticalSplitter->restoreState(settings.value("layout/verticalSplitterState").toByteArray());
  if (settings.contains("layout/logHeaderState"))
    ui->log->horizontalHeader()->restoreState(settings.value("layout/logHeaderState").toByteArray());

  ui->log->setModel(new LogMessageList());

  Application *app = qobject_cast<Application*>(Application::instance());
  ui->images->setModel(new CollectionWrapper(app->collection()));
  ui->images->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->images->addAction(ui->actionShowHexDump);
  ui->images->addAction(ui->actionShowHexDiff);

  ui->patterns->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->patterns, &PatternView::canEdit, ui->actionEdit_pattern, &QAction::setVisible);
  connect(ui->actionEdit_pattern, &QAction::triggered, ui->patterns, &PatternView::editPattern);
  connect(ui->patterns, &PatternView::canAddFixed, ui->menuAdd_repeat, &QMenu::setEnabled);
  connect(ui->actionAdd_sparse_repeat, &QAction::triggered, ui->patterns, &PatternView::addSparseRepeat);
  connect(ui->patterns, &PatternView::canAddSparse, ui->actionAdd_sparse_repeat, &QAction::setVisible);
  connect(ui->actionAdd_block_repeat, &QAction::triggered, ui->patterns, &PatternView::addBlockRepeat);
  connect(ui->patterns, &PatternView::canAddBlock, ui->actionAdd_block_repeat, &QAction::setVisible);
  connect(ui->actionAdd_fixed_repeat, &QAction::triggered, ui->patterns, &PatternView::addFixedRepeat);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_fixed_repeat, &QAction::setVisible);
  connect(ui->actionAdd_element, &QAction::triggered, ui->patterns, &PatternView::addElement);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_element, &QAction::setVisible);
  connect(ui->patterns, &PatternView::canAddFixed, ui->menuAdd_integer, &QMenu::setEnabled);
  connect(ui->actionAdd_bit, &QAction::triggered, ui->patterns, &PatternView::addBit);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_bit, &QAction::setVisible);
  connect(ui->actionAdd_uint8, &QAction::triggered, ui->patterns, &PatternView::addUInt8);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_uint8, &QAction::setVisible);
  connect(ui->actionAdd_int8, &QAction::triggered, ui->patterns, &PatternView::addInt8);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_int8, &QAction::setVisible);
  connect(ui->actionAdd_uint16, &QAction::triggered, ui->patterns, &PatternView::addUInt16);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_uint16, &QAction::setVisible);
  connect(ui->actionAdd_int16, &QAction::triggered, ui->patterns, &PatternView::addInt16);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_int16, &QAction::setVisible);
  connect(ui->actionAdd_uint32, &QAction::triggered, ui->patterns, &PatternView::addUInt32);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_uint32, &QAction::setVisible);
  connect(ui->actionAdd_int32, &QAction::triggered, ui->patterns, &PatternView::addInt32);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_int32, &QAction::setVisible);
  connect(ui->actionAdd_BCD8, &QAction::triggered, ui->patterns, &PatternView::addBCD8);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_BCD8, &QAction::setVisible);
  connect(ui->actionAdd_enum, &QAction::triggered, ui->patterns, &PatternView::addEnum);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_enum, &QAction::setVisible);
  connect(ui->actionAdd_string, &QAction::triggered, ui->patterns, &PatternView::addString);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_string, &QAction::setVisible);
  connect(ui->actionAdd_unused, &QAction::triggered, ui->patterns, &PatternView::addUnused);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_unused, &QAction::setVisible);
  connect(ui->actionAdd_unknown, &QAction::triggered, ui->patterns, &PatternView::addUnknown);
  connect(ui->patterns, &PatternView::canAddFixed, ui->actionAdd_unknown, &QAction::setVisible);
  connect(ui->actionDelete_pattern, &QAction::triggered, ui->patterns, &PatternView::removeSelected);
  connect(ui->patterns, &PatternView::canRemove, ui->actionDelete_pattern, &QAction::setVisible);

  QActionGroup *viewGrp = new QActionGroup(this);
  viewGrp->addAction(ui->actionAutoViewNone);
  if ("none" == settings.value("action/autoShow", "none"))
    ui->actionAutoViewNone->setChecked(true);
  viewGrp->addAction(ui->actionAutoViewHexDump);
  if ("dump" == settings.value("action/autoShow", "none"))
    ui->actionAutoViewHexDump->setChecked(true);
  viewGrp->addAction(ui->actionAutoViewHexDiffFirst);
  if ("first" == settings.value("action/autoShow", "none"))
    ui->actionAutoViewHexDiffFirst->setChecked(true);
  viewGrp->addAction(ui->actionAutoViewHexDiffPrev);
  if ("prev" == settings.value("action/autoShow", "none"))
    ui->actionAutoViewHexDiffPrev->setChecked(true);
  viewGrp->setExclusive(true);

  connect(ui->actionRestart, &QAction::triggered, app, &QCoreApplication::quit);
  connect(ui->actionShowHexDump, &QAction::triggered, this, &MainWindow::onShowHexDump);
  connect(ui->actionShowHexDiff, &QAction::triggered, this, &MainWindow::onShowHexDiff);
  connect(ui->actionCloseTab, &QAction::triggered, this, &MainWindow::onCloseTab);
  connect(ui->actionAnnotate, &QAction::triggered, this, &MainWindow::onAnnotate);
  connect(ui->tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseTab);
  connect(app->collection(), &Collection::imageAdded, this, &MainWindow::onImageReceived);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void
MainWindow::closeEvent(QCloseEvent *event) {
  QSettings settings;
  settings.setValue("layout/mainWindowSize", saveGeometry());
  settings.setValue("layout/mainWindowState", saveState());
  settings.setValue("layout/verticalSplitterState", ui->verticalSplitter->saveState());
  settings.setValue("layout/horizontalSplitterState", ui->horizontalSplitter->saveState());
  settings.setValue("layout/logHeaderState", ui->log->horizontalHeader()->saveState());

  if (ui->actionAutoViewNone->isChecked())
    settings.setValue("action/autoShow", "none");
  if (ui->actionAutoViewHexDump->isChecked())
    settings.setValue("action/autoShow", "dump");
  if (ui->actionAutoViewHexDiffFirst->isChecked())
    settings.setValue("action/autoShow", "first");
  if (ui->actionAutoViewHexDiffPrev->isChecked())
    settings.setValue("action/autoShow", "prev");
  QMainWindow::closeEvent(event);
}

void
MainWindow::onShowHexDump() {
  QList<const QObject *> items;
  foreach (const QItemSelectionRange &range, ui->images->selectionModel()->selection()) {
    foreach (const QModelIndex &index, range.indexes()) {
      if (! index.isValid())
        continue;
      items.append(reinterpret_cast<const QObject *>(index.constInternalPointer()));
    }
  }

  foreach (const QObject *obj, items) {
    if (const Image *img = qobject_cast<const Image *>(obj)) {
      QTextBrowser *view = new QTextBrowser();
      view->setDocument(new HexImageDumpDocument(HexImage(img)));
      ui->tabs->addTab(view, img->label());
    } else if (const Element *el = qobject_cast<const Element *>(obj)) {
      QTextBrowser *view = new QTextBrowser();
      view->setDocument(new HexElementDumpDocument(HexElement(el)));
      ui->tabs->addTab(view, QString("Element @ %1h").arg(el->address().byte(), 0, 16));
    }
  }
}


void
MainWindow::onShowHexDiff() {
  QList<const Image *> items;
  foreach (const QItemSelectionRange &range, ui->images->selectionModel()->selection()) {
    foreach (const QModelIndex &index, range.indexes()) {
      if (! index.isValid())
        continue;
      if (const Image *img = qobject_cast<const Image *>(reinterpret_cast<const QObject *>(index.constInternalPointer())))
        items.append(img);
    }
  }
  for (int i=1; i<items.size(); i++) {
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDiffDocument(HexImage(items.at(i-1), items.at(i))));
    ui->tabs->addTab(view, QString("%1 vs. %2").arg(items.at(i-1)->label()).arg(items.at(i)->label()));
  }
}

void
MainWindow::onCloseTab(int index) {
  if (0 <= index) {
    QWidget *view = ui->tabs->widget(index);
    ui->tabs->removeTab(ui->tabs->currentIndex());
    view->deleteLater();
  } else if (QWidget *view = ui->tabs->currentWidget()) {
    ui->tabs->removeTab(ui->tabs->currentIndex());
    view->deleteLater();
  }
}

void
MainWindow::onImageReceived(unsigned int idx) {
  if (ui->actionAutoViewNone->isChecked())
    return;

  Application *app = qobject_cast<Application*>(Application::instance());
  const Image *last = app->collection()->image(idx);

  if (ui->actionAutoViewHexDump->isChecked()) {
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDumpDocument(HexImage(last)));
    ui->tabs->addTab(view, last->label());
    return;
  }

  if (2 > app->collection()->count())
    return;

  if (ui->actionAutoViewHexDiffFirst->isChecked()) {
    const Image *first = app->collection()->image(0);
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDiffDocument(HexImage(first, last)));
    ui->tabs->addTab(view, QString("%1 vs. %2").arg(first->label()).arg(last->label()));
  } else if (ui->actionAutoViewHexDiffPrev->isChecked()) {
    const Image *prev = app->collection()->image(idx-1);
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDiffDocument(HexImage(prev, last)));
    ui->tabs->addTab(view, QString("%1 vs. %2").arg(prev->label()).arg(last->label()));
  }
}


void
MainWindow::onAnnotate() {
  Application *app = qobject_cast<Application *>(Application::instance());
  if (! app->device()->pattern())
    return;

  QList<Image *> images;
  foreach (const QItemSelectionRange &range, ui->images->selectionModel()->selection()) {
    foreach (const QModelIndex &index, range.indexes()) {
      if (! index.isValid())
        continue;
      QObject *obj = reinterpret_cast<QObject *>(index.internalPointer());
      if (Image *img = qobject_cast<Image *>(obj))
        images.append(img);
    }
  }

  foreach (Image *img, images) {
    if (! img->annotate(app->device()->pattern())) {
      logError() << "Annotation failed.";
    }
  }
}
