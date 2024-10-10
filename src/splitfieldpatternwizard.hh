#ifndef SPLITFIELDPATTERNWIZARD_HH
#define SPLITFIELDPATTERNWIZARD_HH

#include <QWizard>
#include "offset.hh"

class FieldPattern;
class FixedPattern;

namespace Ui {
class SplitFieldPatternWizard;
}

class SplitFieldPatternWizard : public QWizard
{
  Q_OBJECT

public:
  explicit SplitFieldPatternWizard(FieldPattern *field, QWidget *parent = nullptr);
  ~SplitFieldPatternWizard();

  Address address() const;
  FixedPattern *createPattern() const;

private:
  Ui::SplitFieldPatternWizard *ui;
  FieldPattern *_field;
  FixedPattern *_pattern;
};

#endif // SPLITFIELDPATTERNWIZARD_HH
