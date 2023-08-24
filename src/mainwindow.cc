#include "mainwindow.hh"
#include "ui_mainwindow.h"
#include "application.hh"
#include "image.hh"
#include "hexdump.hh"
#include "heximagedumpdocument.hh"
#include "hexelementdumpdocument.hh"
#include "heximagediffdocument.hh"
#include "imagecollectionwrapper.hh"
#include <QActionGroup>
#include <QSettings>

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

  Application *app = qobject_cast<Application*>(Application::instance());
  ui->images->setModel(new CollectionWrapper(app->collection()));
  ui->images->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->images->addAction(ui->actionShowHexDump);
  ui->images->addAction(ui->actionShowHexDiff);

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
