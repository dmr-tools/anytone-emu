#ifndef PATTERNMETAEDITOR_HH
#define PATTERNMETAEDITOR_HH

#include <QTabWidget>
#include "pattern.hh"

namespace Ui {
class metaEditor;
}


class PatternMetaEditor : public QTabWidget
{
  Q_OBJECT

public:
  explicit PatternMetaEditor(QWidget *parent = nullptr);
  virtual ~PatternMetaEditor();

  void setPatternMeta(PatternMeta *meta, const CodeplugPattern *codeplug);

public:
  void apply();

private:
  PatternMeta *_meta;
  Ui::metaEditor *ui;
};

#endif // PATTERNMETAEDITOR_HH
