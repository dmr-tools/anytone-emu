#include "libraryview.hh"
#include "ui_libraryview.h"
#include "librarywrapper.hh"
#include <QSortFilterProxyModel>

LibraryView::LibraryView(PatternDefinitionLibrary *lib, QWidget *parent)
  : QWidget(parent), ui(new Ui::LibraryView), _filter(new QSortFilterProxyModel(this))
{
  ui->setupUi(this);

  _filter->setSourceModel(new LibraryWrapper(lib));
  _filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _filter->setFilterKeyColumn(0);
  _filter->setFilterRole(Qt::DisplayRole);
  _filter->setFilterRegularExpression("");
  _filter->setDynamicSortFilter(true);
  _filter->setRecursiveFilteringEnabled(true);

  ui->libView->setModel(_filter);

  connect(ui->filter, &QLineEdit::textChanged, this, &LibraryView::onFilterChanged);
}

LibraryView::~LibraryView()
{
  delete ui;
}

void
LibraryView::onFilterChanged(const QString content) {
  _filter->setFilterRegularExpression(content);
}
