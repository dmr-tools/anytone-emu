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

class EnumItemsEditor : public QWidget
{
  Q_OBJECT

public:
  explicit EnumItemsEditor(QWidget *parent = nullptr);
  ~EnumItemsEditor() override;

private:
  Ui::EnumItemsEditor *ui;
};

#endif //ANYTONE_EMU_ENUMITEMSEDITOR_H
