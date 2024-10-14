#ifndef PATTERNMIMEDATA_HH
#define PATTERNMIMEDATA_HH

#include <QMimeData>

class AbstractPattern;


/** Holds a copies/cut pattern. */
class PatternMimeData : public QMimeData
{
  Q_OBJECT

public:
  /** Constructor from pattern and takes its ownership. */
  explicit PatternMimeData(AbstractPattern *pattern);

  /** Retunrs the formats supported by this container. */
  QStringList formats() const;
  /** Retunrs @c true, if the container hold data for the given MIME type. */
  bool hasFormat(const QString &mimetype) const;

  /** Returns the pattern held. */
  AbstractPattern *pattern() const;
  /** Takes the held pattern. Transfers ownership to the caller. */
  AbstractPattern *takePattern();

protected:
  /** Retunrs the data for the given MIME type as the specified data type. */
  QVariant retrieveData(const QString &mimetype, QMetaType preferredType) const;

protected:
  /** Holds the pattern. */
  AbstractPattern *_pattern;
};

#endif // PATTERNMIMEDATA_HH
