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
#include <QActionGroup>
#include <QSettings>
#include <QTextBrowser>
#include <QScrollArea>
#include <QStyleHints>
#include <QClipboard>
#include "aboutdialog.hh"
#include "patternwidget.hh"
#include "elementpatterneditor.hh"
#include "questiondialog.hh"

#include <QCloseEvent>
#include <QScrollBar>



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

  ui->toolBox->setCurrentIndex(settings.value("layout/toolBoxIndex", 0).toUInt());
  ui->actionShow_Log->setChecked(settings.value("layout/showLog", true).toBool());
  ui->log->setVisible(settings.value("layout/showLog", true).toBool());
  ui->log->setModel(new LogMessageList());

  Application *app = qobject_cast<Application*>(Application::instance());

  ui->toolBox->setItemIcon(0, QIcon::fromTheme("camera-photo"));
  ui->toolBox->setItemIcon(1, QIcon::fromTheme("pattern-element"));

  ui->actionShow_Log->setIcon(QIcon::fromTheme("show-log"));
  connect(ui->actionShow_Log, &QAction::toggled, ui->log, &QWidget::setVisible);

  auto spacer = new QWidget();
  spacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  ui->toolBar->insertWidget(ui->actionRestart, spacer);

  connect(ui->actionRestart, &QAction::triggered, app, &QCoreApplication::quit);
  connect(ui->imageWidget, &ImageWidget::showHexImage, this, &MainWindow::onShowHexImage);
  connect(ui->imageWidget, &ImageWidget::showHexElement, this, &MainWindow::onShowHexElement);
  connect(ui->imageWidget, &ImageWidget::showHexDiff, this, &MainWindow::onShowHexDiff);
  connect(ui->patternPage, &PatternWidget::viewPattern, this, &MainWindow::onViewPattern);
  connect(ui->tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseTab);
  connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onShowAboutDialog);
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
  settings.setValue("layout/showLog", ui->actionShow_Log->isChecked());
  settings.setValue("layout/toolBoxIndex", ui->toolBox->currentIndex());

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
MainWindow::onShowHexImage(const Image *img) {
  QTextBrowser *view = new QTextBrowser();
  auto document = new HexImageDumpDocument(isDarkMode(), HexImage(img));
  document->enableDarkMode(isDarkMode());
  view->setDocument(document);
  ui->tabs->addTab(view, img->label());
}

void
MainWindow::onShowHexElement(const Element *el) {
  QTextBrowser *view = new QTextBrowser();
  auto document = new HexElementDumpDocument(isDarkMode(), HexElement(el));
  document->enableDarkMode(isDarkMode());
  view->setDocument(document);
  ui->tabs->addTab(view, QString("Element @ %1h").arg(el->address().byte(), 0, 16));
}

void
MainWindow::onShowHexDiff(const Image *left, const Image *right) {
  QTextBrowser *view = new QTextBrowser();
  auto document = new HexImageDiffDocument(isDarkMode(), HexImage(left, right));
  view->setDocument(document);
  ui->tabs->addTab(view, QString("%1 vs. %2").arg(left->label()).arg(right->label()));
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
MainWindow::onShowAboutDialog() {
  AboutDialog about;
  about.exec();
}

void
MainWindow::onViewPattern(ElementPattern *element) {
  auto view = new ElementPatternEditor(); view->setPattern(element);
  ui->tabs->addTab(view, element->meta().name());
}

