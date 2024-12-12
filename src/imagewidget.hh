#ifndef IMAGEWIDGET_HH
#define IMAGEWIDGET_HH

#include <QWidget>

class Image;
class Element;

namespace Ui {
class ImageWidget;
}


class ImageWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ImageWidget(QWidget *parent = nullptr);
  ~ImageWidget();

signals:
  void showHexImage(const Image *img);
  void showHexElement(const Element *element);
  void showHexDiff(const Image *left, const Image *right);

protected slots:
  void onShowHexDump();
  void onShowHexDiff();
  void onImageReceived(unsigned int idx);
  void onAnnotate();
  void onClearAnnotations();

protected:
  QList<Image *> getSelectedImages();

private:
  Ui::ImageWidget *ui;
};

#endif // IMAGEWIDGET_HH
