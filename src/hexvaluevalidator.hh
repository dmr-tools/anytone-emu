#ifndef HEXVALUEVALIDATOR_HH
#define HEXVALUEVALIDATOR_HH

#include <QRegularExpressionValidator>


class HexValueValidator : public QRegularExpressionValidator
{
  Q_OBJECT

public:
  explicit HexValueValidator(QObject *parent=nullptr);
};

#endif // HEXVALUEVALIDATOR_HH
