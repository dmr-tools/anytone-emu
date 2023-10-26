#ifndef LIBRARYVIEW_HH
#define LIBRARYVIEW_HH

#include <QWidget>

namespace Ui {
  class LibraryView;
}

class QSortFilterProxyModel;
class PatternDefinitionLibrary;

class LibraryView : public QWidget
{
  Q_OBJECT

public:
  explicit LibraryView(PatternDefinitionLibrary *lib, QWidget *parent = nullptr);
  ~LibraryView();

protected slots:
  void onFilterChanged(const QString content);

private:
  Ui::LibraryView *ui;
  QSortFilterProxyModel *_filter;
};

#endif // LIBRARYVIEW_HH
