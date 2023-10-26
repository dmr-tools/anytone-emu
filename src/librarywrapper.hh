#ifndef LIBRARYWRAPPER_HH
#define LIBRARYWRAPPER_HH

#include <QAbstractItemModel>

class PatternDefinitionLibrary;

class LibraryWrapper : public QAbstractItemModel
{
public:
  explicit LibraryWrapper(PatternDefinitionLibrary *lib, QObject *parent = nullptr);

  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  QModelIndex parent(const QModelIndex &child) const;

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  QVariant data(const QModelIndex &index, int role) const;

protected:
  QVariant getName(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const;
  QVariant getIcon(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const;
  QVariant getTooltip(const PatternDefinitionLibrary *lib, int row, const QObject *obj) const;

protected slots:
  void onLibraryDeleted();

protected:
  PatternDefinitionLibrary *_lib;
};

#endif // LIBRARYWRAPPER_HH
