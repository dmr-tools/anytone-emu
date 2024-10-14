#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>

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
  void onShowHexDump();
  void onShowHexDiff();
  void onCloseTab(int index=-1);
  void onImageReceived(unsigned int idx);
  void onAnnotate();
  void onShowAboutDialog();
  void onViewPattern();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HH
