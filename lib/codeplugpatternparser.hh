#ifndef CODEPLUGPATTERNPARSER_HH
#define CODEPLUGPATTERNPARSER_HH

#include "patternparser.hh"

/** Parses an entire codeplug. */
class CodeplugPatternParser : public PatternParser
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit CodeplugPatternParser(QObject *parent = nullptr);

protected:
  bool endDocument();

protected slots:
  /** Handler for start of codeplug element. */
  bool beginCodeplugElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of codeplug element. */
  bool endCodeplugElement();

};

#endif // CODEPLUGPATTERNPARSER_HH
