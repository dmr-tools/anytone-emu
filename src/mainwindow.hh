#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

class Image;
class Element;
class ElementPattern;

namespace Ui {
  class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event);
  void changeEvent(QEvent *event);
  bool isDarkMode() const;

private slots:
  void onShowHexImage(const Image *img);
  void onShowHexElement(const Element *element);
  void onShowHexDiff(const Image *left, const Image *right);
  void onCloseTab(int index=-1);
  void onShowAboutDialog();
  void onViewPattern(ElementPattern *element);

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HH
