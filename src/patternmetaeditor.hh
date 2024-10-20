#ifndef PATTERNMETAEDITOR_HH
#define PATTERNMETAEDITOR_HH

#include <QWidget>
class PatternMeta;
class CodeplugPattern;

namespace Ui {
  class PatternMetaEditor;
}


class PatternMetaEditor : public QWidget
{
  Q_OBJECT

public:
  explicit PatternMetaEditor(QWidget *parent = nullptr);
  ~PatternMetaEditor();

public slots:
  void setPatternMeta(PatternMeta *meta, const CodeplugPattern *codeplug);
  void apply();

private:
  Ui::PatternMetaEditor *ui;
  PatternMeta *_meta;
};

#endif // PATTERNMETAEDITOR_HH
