#ifndef PATTERNFRAGMENTPARSER_HH
#define PATTERNFRAGMENTPARSER_HH

#include "patternparser.hh"


/** Simple XML parser, reading pattern fragments. */
class PatterFragmentParser : public PatternParser
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit PatterFragmentParser(QObject *parent = nullptr);

protected:
  bool endDocument();

protected slots:
  /** Handler for start of fragment element. */
  bool beginFragmentElement(const QXmlStreamAttributes &attributes);
  /** Handler for end of fragment element. */
  bool endFragmentElement();
};

#endif // PATTERNFRAGMENTPARSER_HH
