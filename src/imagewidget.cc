#include "imagewidget.hh"
#include "ui_imagewidget.h"
#include <QSettings>
#include <QMenu>
#include <QActionGroup>
#include <QToolBar>

#include "device.hh"
#include "application.hh"
#include "image.hh"
#include "logger.hh"


ImageWidget::ImageWidget(QWidget *parent)
  : QWidget(parent), ui(new Ui::ImageWidget)
{
  QSettings settings;
  Application *app = qobject_cast<Application*>(Application::instance());

  ui->setupUi(this);

  auto toolBar = new QToolBar();
  int defaultSize = app->style()->pixelMetric(QStyle::PM_ToolBarIconSize),
      toolBarIconSize = 2*defaultSize/3;
  toolBar->setIconSize(QSize(toolBarIconSize, toolBarIconSize));

  auto autoShowMenu = new QMenu();
  ui->actionOnNewImageShowNone->setIcon(QIcon::fromTheme("none"));
  autoShowMenu->addAction(ui->actionOnNewImageShowNone);
  ui->actionOnNewImageShowHexDump->setIcon(QIcon::fromTheme("show-hexdump"));
  autoShowMenu->addAction(ui->actionOnNewImageShowHexDump);
  ui->actionOnNewImageShowHexDiffVsFirst->setIcon(QIcon::fromTheme("show-diff-single"));
  autoShowMenu->addAction(ui->actionOnNewImageShowHexDiffVsFirst);
  ui->actionOnNewImageShowHexDiffVsPrevious->setIcon(QIcon::fromTheme("show-diff-single"));
  autoShowMenu->addAction(ui->actionOnNewImageShowHexDiffVsPrevious);

  auto viewGrp = new QActionGroup(this);
  viewGrp->addAction(ui->actionOnNewImageShowNone);
  if ("none" == settings.value("action/autoShow", "none"))
    ui->actionOnNewImageShowNone->setChecked(true);
  viewGrp->addAction(ui->actionOnNewImageShowHexDump);
  if ("dump" == settings.value("action/autoShow", "none"))
    ui->actionOnNewImageShowHexDump->setChecked(true);
  viewGrp->addAction(ui->actionOnNewImageShowHexDiffVsFirst);
  if ("first" == settings.value("action/autoShow", "none"))
    ui->actionOnNewImageShowHexDiffVsFirst->setChecked(true);
  viewGrp->addAction(ui->actionOnNewImageShowHexDiffVsPrevious);
  if ("prev" == settings.value("action/autoShow", "none"))
    ui->actionOnNewImageShowHexDiffVsPrevious->setChecked(true);
  viewGrp->setExclusive(true);

  auto autoShowMenuButton = new QAction(QIcon::fromTheme("show-diff-multiple"), tr("On new codeplug"));
  autoShowMenuButton->setMenu(autoShowMenu);
  toolBar->addAction(autoShowMenuButton);

  ui->actionShowHexDiff->setIcon(QIcon::fromTheme("show-diff-single"));
  toolBar->addAction(ui->actionShowHexDiff);
  ui->actionShowHexDump->setIcon(QIcon::fromTheme("show-hexdump"));
  toolBar->addAction(ui->actionShowHexDump);
  toolBar->addSeparator();

  ui->actionAnnotate->setIcon(QIcon::fromTheme("edit-annotate"));
  toolBar->addAction(ui->actionAnnotate);
  ui->actionClearAnnotation->setIcon(QIcon::fromTheme("edit-clear"));
  toolBar->addAction(ui->actionClearAnnotation);
  toolBar->addSeparator();

  toolBar->addAction(ui->actionDeleteImage);

  qobject_cast<QVBoxLayout*>(layout())->insertWidget(0, toolBar);

  if (settings.contains("layout/imagesHeaderState"))
    ui->images->header()->restoreState(settings.value("layout/imagesHeaderState").toByteArray());

  connect(ui->actionShowHexDump, &QAction::triggered, this, &ImageWidget::onShowHexDump);
  connect(ui->actionShowHexDiff, &QAction::triggered, this, &ImageWidget::onShowHexDiff);
  connect(ui->actionAnnotate, &QAction::triggered, this, &ImageWidget::onAnnotate);
  connect(app->collection(), &Collection::imageAdded, this, &ImageWidget::onImageReceived);
}


ImageWidget::~ImageWidget() {
  QSettings settings;
  if (ui->actionOnNewImageShowNone->isChecked())
    settings.setValue("action/autoShow", "none");
  if (ui->actionOnNewImageShowHexDump->isChecked())
    settings.setValue("action/autoShow", "dump");
  if (ui->actionOnNewImageShowHexDiffVsFirst->isChecked())
    settings.setValue("action/autoShow", "first");
  if (ui->actionOnNewImageShowHexDiffVsPrevious->isChecked())
    settings.setValue("action/autoShow", "prev");
  settings.setValue("layout/imagesHeaderState", ui->images->header()->saveState());
  delete ui;
}



void
ImageWidget::onShowHexDump() {
  QList<const QObject *> items;
  auto selectionModel = ui->images->selectionModel();
  if (nullptr == selectionModel)
    return;
  foreach (const QItemSelectionRange &range, selectionModel->selection()) {
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
      emit showHexImage(img);
    } else if (const Element *el = qobject_cast<const Element *>(obj)) {
      emit showHexElement(el);
    }
  }
}


void
ImageWidget::onShowHexDiff() {
  QList<const Image *> items;

  auto selectionModel = ui->images->selectionModel();
  if (nullptr == selectionModel)
    return;

  foreach (const QItemSelectionRange &range, selectionModel->selection()) {
    foreach (const QModelIndex &index, range.indexes()) {
      if (! index.isValid())
        continue;
      if (const Image *img = qobject_cast<const Image *>(
            reinterpret_cast<const QObject *>(
              index.constInternalPointer())))
        items.append(img);
    }
  }

  for (int i=1; i<items.size(); i++) {
    emit showHexDiff(items.at(i-1), items.at(i));
  }
}


void
ImageWidget::onImageReceived(unsigned int idx) {
  if (ui->actionOnNewImageShowNone->isChecked())
    return;

  Application *app = qobject_cast<Application*>(Application::instance());
  const Image *last = app->collection()->image(idx);

  if (ui->actionOnNewImageShowHexDump->isChecked()) {
    emit showHexImage(last);
    return;
  }

  if (2 > app->collection()->count())
    return;

  if (ui->actionOnNewImageShowHexDiffVsFirst->isChecked()) {
    const Image *first = app->collection()->image(0);
    emit showHexDiff(first, last);
  } else if (ui->actionOnNewImageShowHexDiffVsPrevious->isChecked()) {
    const Image *prev = app->collection()->image(idx-1);
    emit showHexDiff(prev, last);
  }
}


void
ImageWidget::onAnnotate() {
  auto selectionModel = ui->images->selectionModel();
  if (nullptr == selectionModel)
    return;

  Application *app = qobject_cast<Application *>(Application::instance());
  if (! app->device()->pattern())
    return;

  QList<Image *> images;
  foreach (const QItemSelectionRange &range, selectionModel->selection()) {
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

