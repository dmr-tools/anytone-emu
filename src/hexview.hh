#ifndef HEXVIEW_HH
#define HEXVIEW_HH

#include <QWidget>
#include <QFontMetrics>
#include "hexdump.hh"



class HexView : public QWidget
{
  Q_OBJECT

public:
  class Style
  {
  public:
    explicit Style();

    const QFont &headerFont() const;
    const QFont &blockHeaderFont() const;
    const QFont &codeFont() const;

    const QColor &defaultColor() const;
    const QColor &keepColor() const;
    const QColor &addColor() const;
    const QColor &removeColor() const;

    bool showOnlyDiffs() const;

  protected:
    QFont _headerFont;
    QFont _blockHeaderFont;
    QFont _codeFont;
    QColor _defaultColor;
    QColor _keepColor;
    QColor _addColor;
    QColor _removeColor;
    bool _showOnlyDiffs;
  };

  class Metric
  {
  public:
    Metric(const Style &style, const QPaintDevice *device);

    const QFontMetrics &headerMetrics() const;
    const QFontMetrics &blockHeaderMetrics() const;
    const QFontMetrics &codeMetrics() const;

    int blockHeaderHeight() const;
    int blockSkip() const;
    int lineSpacing() const;
    int charWidth() const;
    int addressSkip() const;
    int codeSkip() const;
    int byteSkip() const;

    int addressWidth() const;
    int hexBlockWidth() const;
    int asciiBlockWidth() const;

  protected:
    QFontMetrics _headerMetrics;
    QFontMetrics _blockHeaderMetrics;
    QFontMetrics _codeMetrics;
    int _blockSkip;
    int _lineSpacing;
    int _charWidth;
    int _addressSkip;
    int _codeSkip;
    int _byteSkip;
  };

public:
  explicit HexView(HexImage *document, QWidget *parent = nullptr);
  virtual ~HexView();

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

public slots:
  void setDocument(HexImage *document);

protected slots:
  void setHexViewStyle(const HexView::Style &style);

protected:
  void paintEvent(QPaintEvent *event) override;

  void paintBlock(const HexElement &block, QPoint &pos, const QRect &region, QPainter &painter);
  void paintLine(const HexLine &line, QPoint &pos, const QRect &region, QPainter &painter);

private:
  HexImage *_document;
  Style _style;
  Metric _metric;
};


#endif // HEXVIEW_HH
