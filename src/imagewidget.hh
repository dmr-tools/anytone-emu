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

  void canShowHexDiff(bool enable);
  void canShowHexDump(bool enable);
  void canAnnotate(bool enable);
  void canClearAnnotation(bool enable);
  void canDeleteImage(bool enable);

protected slots:
  void onSelectionChanged();

  void onShowHexDump();
  void onShowHexDiff();
  void onImageReceived(unsigned int idx);
  void onAnnotate();
  void onClearAnnotations();
  void onDeleteImage();

protected:
  QList<Image *> getSelectedImages();

private:
  Ui::ImageWidget *ui;
};

#endif // IMAGEWIDGET_HH
