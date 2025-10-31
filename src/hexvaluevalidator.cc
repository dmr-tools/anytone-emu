#include "hexvaluevalidator.hh"

HexValueValidator::HexValueValidator(QObject *parent)
  : QRegularExpressionValidator(QRegularExpression(R"([0-9a-f]+\s*[h]?)",
                                                   QRegularExpression::CaseInsensitiveOption), parent)
{
  // pass...
}
