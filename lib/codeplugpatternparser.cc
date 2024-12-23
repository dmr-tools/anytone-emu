#include "codeplugpatternparser.hh"
#include "pattern.hh"


CodeplugPatternParser::CodeplugPatternParser(QObject *parent)
  : PatternParser{parent}
{

}

bool
CodeplugPatternParser::endDocument() {
  if (! PatternParser::endDocument())
    return false;

  if (1 != _stack.size()) {
    raiseError("Cannot parse codeplug pattern. No codeplug is left on the stack");
    return false;
  }

  if (! topIs<CodeplugPattern>()) {
    raiseError("Cannot parse codeplug pattern. No codeplug is left on the stack.");
    return false;
  }

  return true;
}


bool
CodeplugPatternParser::beginCodeplugElement(const QXmlStreamAttributes &attributes) {
  if (0 != _stack.size()) {
    raiseError("<codeplug> must be the root elment.");
    return false;
  }

  push(new CodeplugPattern());

  return true;
}

bool
CodeplugPatternParser::endCodeplugElement() {
  // Do nothing to keep the codeplug on the stack.
  return true;
}


