//
// Created by hannes on 19.06.26.
//

#ifndef ANYTONE_EMU_ENUMITEMSEDITOR_H
#define ANYTONE_EMU_ENUMITEMSEDITOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class EnumItemsEditor;
}
QT_END_NAMESPACE

class AbstractEnumFieldPattern;
class CodeplugPattern;

class EnumItemsEditor : public QWidget
{
  Q_OBJECT

public:
  explicit EnumItemsEditor(QWidget *parent = nullptr);
  ~EnumItemsEditor() override;

public slots:
  void setPattern(AbstractEnumFieldPattern *pattern, const CodeplugPattern *codeplug);
  void done(int res);

private slots:
  void onAddItem();
  void onDelItem();

private:
  Ui::EnumItemsEditor *ui;
  AbstractEnumFieldPattern *_pattern;
};

#endif //ANYTONE_EMU_ENUMITEMSEDITOR_H
