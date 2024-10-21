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
#include "logger.hh"
#include "logmessagelist.hh"
#include "patternmimedata.hh"
#include <QActionGroup>
#include <QSettings>
#include <QTextBrowser>
#include <QScrollArea>
#include <QStyleHints>
#include <QClipboard>
#include "aboutdialog.hh"
#include "elementpatterneditor.hh"
#include "questiondialog.hh"
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setWindowIcon(QIcon::fromTheme("application-anytone-emu"));

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
  if (settings.contains("layout/patternsHeaderState"))
    ui->patterns->header()->restoreState(settings.value("layout/patternsHeaderState").toByteArray());
  if (settings.contains("layout/imagesHeaderState"))
    ui->images->header()->restoreState(settings.value("layout/imagesHeaderState").toByteArray());

  ui->log->setModel(new LogMessageList());

  Application *app = qobject_cast<Application*>(Application::instance());
  ui->images->setModel(new CollectionWrapper(app->collection()));
  ui->images->setContextMenuPolicy(Qt::ActionsContextMenu);
  ui->images->addAction(ui->actionShowHexDump);
  ui->images->addAction(ui->actionShowHexDiff);

  ui->patterns->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->patterns, &PatternView::canEdit, ui->actionEdit_pattern, &QAction::setEnabled);
  connect(ui->actionEdit_pattern, &QAction::triggered, ui->patterns, &PatternView::editPattern);
  connect(ui->patterns, &PatternView::canView, ui->actionViewPattern, &QAction::setEnabled);
  connect(ui->actionViewPattern, &QAction::triggered, this, &MainWindow::onViewPattern);

  connect(ui->patterns, &PatternView::canAppendPattern, ui->actionAppendNewPattern, &QAction::setEnabled);
  connect(ui->actionAppendNewPattern, &QAction::triggered, ui->patterns, &PatternView::appendNewPattern);
  connect(ui->patterns, &PatternView::canAppendPattern, ui->actionPastePatternAsChild, &QAction::setEnabled);
  connect(ui->actionPastePatternAsChild, &QAction::triggered, ui->patterns, &PatternView::pastePatternAsChild);

  connect(ui->patterns, &PatternView::canInsertPatternAbove, ui->actionInsert_above, &QAction::setEnabled);
  connect(ui->actionInsert_above, &QAction::triggered, ui->patterns, &PatternView::insertNewPatternAbove);
  connect(ui->patterns, &PatternView::canInsertPatternAbove, ui->actionPastePatternAbove, &QAction::setEnabled);
  connect(ui->actionPastePatternAbove, &QAction::triggered, ui->patterns, &PatternView::pastePatternAbove);

  connect(ui->patterns, &PatternView::canSplitFieldPattern, ui->actionSplitUnknownField, &QAction::setEnabled);
  connect(ui->actionSplitUnknownField, &QAction::triggered, ui->patterns, &PatternView::splitFieldPattern);

  connect(ui->patterns, &PatternView::canInsertPatternBelow, ui->actionInsert_below, &QAction::setEnabled);
  connect(ui->actionInsert_below, &QAction::triggered, ui->patterns, &PatternView::insertNewPatternBelow);
  connect(ui->patterns, &PatternView::canInsertPatternBelow, ui->actionPastePatternBelow, &QAction::setEnabled);
  connect(ui->actionPastePatternBelow, &QAction::triggered, ui->patterns, &PatternView::pastePatternBelow);

  connect(ui->patterns, &PatternView::canRemove, ui->actionDelete_pattern, &QAction::setEnabled);
  connect(ui->actionDelete_pattern, &QAction::triggered, ui->patterns, &PatternView::removeSelected);

  connect(ui->patterns, &PatternView::canRemove, ui->actionCopyPattern, &QAction::setEnabled);
  connect(ui->actionCopyPattern, &QAction::triggered, ui->patterns, &PatternView::copySelected);

  connect(ui->actionSave_pattern, &QAction::triggered, ui->patterns, &PatternView::save);

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
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onShowAboutDialog);

  connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardDataChanged);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void
MainWindow::closeEvent(QCloseEvent *event) {
  if (qobject_cast<Application*>(QApplication::instance())->device()->pattern()->isModified()) {
    auto res = QuestionDialog::ask(
          "closeOnUnsavedPattern", tr("Discard changes?"),
          tr("Some changes to the codeplug pattern are not saved yet. Do you want to close the "
             "application and discard these changes?"));
    if (QMessageBox::Yes != res) {
      event->ignore();
      return;
    }
  }

  QSettings settings;
  settings.setValue("layout/mainWindowSize", saveGeometry());
  settings.setValue("layout/mainWindowState", saveState());
  settings.setValue("layout/verticalSplitterState", ui->verticalSplitter->saveState());
  settings.setValue("layout/horizontalSplitterState", ui->horizontalSplitter->saveState());
  settings.setValue("layout/logHeaderState", ui->log->horizontalHeader()->saveState());
  settings.setValue("layout/patternsHeaderState", ui->patterns->header()->saveState());
  settings.setValue("layout/imagesHeaderState", ui->images->header()->saveState());

  if (ui->actionAutoViewNone->isChecked())
    settings.setValue("action/autoShow", "none");
  if (ui->actionAutoViewHexDump->isChecked())
    settings.setValue("action/autoShow", "dump");
  if (ui->actionAutoViewHexDiffFirst->isChecked())
    settings.setValue("action/autoShow", "first");
  if (ui->actionAutoViewHexDiffPrev->isChecked())
    settings.setValue("action/autoShow", "prev");

  event->accept();

  QMainWindow::closeEvent(event);
}


void
MainWindow::changeEvent(QEvent *event) {
  QMainWindow::changeEvent(event);

  if (QEvent::ThemeChange == event->type()) {
    logDebug() << "Theme changed to " << (isDarkMode() ? "dark" : "light") << ".";
    if (isDarkMode())
      QIcon::setThemeName("dark");
    else
      QIcon::setThemeName("light");
    for (int i=0; i<ui->tabs->count(); i++) {
      if (auto view = qobject_cast<QTextBrowser *>(ui->tabs->widget(i))) {
        if (auto doc = qobject_cast<HexDocument *>(view->document()))
          doc->enableDarkMode(isDarkMode());
      }
    }
  }
}


bool
MainWindow::isDarkMode() const {
  return palette().window().color().lightness() < palette().windowText().color().lightness();
}


void
MainWindow::onShowHexDump() {
  QList<const QObject *> items;
  foreach (const QItemSelectionRange &range, ui->images->selectionModel()->selection()) {
    foreach (const QModelIndex &index, range.indexes()) {
      if (! index.isValid())
        continue;
      const QObject *obj = reinterpret_cast<const QObject *>(index.constInternalPointer());
      if (! items.contains(obj))
        items.append(obj);
    }
  }

  foreach (const QObject *obj, items) {
    if (const Image *img = qobject_cast<const Image *>(obj)) {
      QTextBrowser *view = new QTextBrowser();
      auto document = new HexImageDumpDocument(isDarkMode(), HexImage(img));
      document->enableDarkMode(isDarkMode());
      view->setDocument(document);
      ui->tabs->addTab(view, img->label());
    } else if (const Element *el = qobject_cast<const Element *>(obj)) {
      QTextBrowser *view = new QTextBrowser();
      auto document = new HexElementDumpDocument(isDarkMode(), HexElement(el));
      document->enableDarkMode(isDarkMode());
      view->setDocument(document);
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
    auto document = new HexImageDiffDocument(isDarkMode(), HexImage(items.at(i-1), items.at(i)));
    view->setDocument(document);
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
    view->setDocument(new HexImageDumpDocument(isDarkMode(), HexImage(last)));
    ui->tabs->addTab(view, last->label());
    return;
  }

  if (2 > app->collection()->count())
    return;

  if (ui->actionAutoViewHexDiffFirst->isChecked()) {
    const Image *first = app->collection()->image(0);
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDiffDocument(isDarkMode(), HexImage(first, last)));
    ui->tabs->addTab(view, QString("%1 vs. %2").arg(first->label()).arg(last->label()));
  } else if (ui->actionAutoViewHexDiffPrev->isChecked()) {
    const Image *prev = app->collection()->image(idx-1);
    QTextBrowser *view = new QTextBrowser();
    view->setDocument(new HexImageDiffDocument(isDarkMode(), HexImage(prev, last)));
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
        if (! images.contains(img))
          images.append(img);
    }
  }
  if (0 == images.size())
    logInfo() << "Select an image to annotate.";

  foreach (Image *img, images) {
    logDebug() << "Annotate image '" << img->label() << "'.";
    if (! img->annotate(app->device()->pattern())) {
      logError() << "Annotation failed.";
    }
  }
}

void
MainWindow::onClipboardDataChanged() {
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

void
MainWindow::onShowAboutDialog() {
  AboutDialog about;
  about.exec();
}

void
MainWindow::onViewPattern() {
  AbstractPattern *pattern = ui->patterns->selectedPattern();
  if ((nullptr == pattern) || (! pattern->is<ElementPattern>()))
    return;

  ElementPattern *element = pattern->as<ElementPattern>();
  auto view = new ElementPatternEditor(); view->setPattern(element);
  ui->tabs->addTab(view, element->meta().name());
}
