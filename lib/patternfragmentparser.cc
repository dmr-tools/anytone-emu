#include "patternfragmentparser.hh"
#include "pattern.hh"


PatterFragmentParser::PatterFragmentParser(QObject *parent)
  : PatternParser{parent}
{
  // pass...
}

bool
PatterFragmentParser::endDocument() {
  if (! PatternParser::endDocument())
    return false;

  if (1 != _stack.size()) {
    raiseError("Cannot parse fragment pattern. No fragment is left on the stack");
    return false;
  }

  if (! topIs<PatternFragment>()) {
    raiseError("Cannot parse fragmetn pattern. No fragment is left on the stack.");
    return false;
  }

  return true;
}


bool
PatterFragmentParser::beginFragmentElement(const QXmlStreamAttributes &attributes) {
  if (0 != _stack.size()) {
    raiseError("<fragment> must be the root elment.");
    return false;
  }

  push(new PatternFragment());

  return true;
}

bool
PatterFragmentParser::endFragmentElement() {
  // Do nothing to keep the fragment on the stack.
  return true;
}


