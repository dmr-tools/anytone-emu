#include "patternparser.hh"
#include "patterndefinition.hh"

#include <QXmlStreamAttribute>

PatternDefinitionParser::PatternDefinitionParser(QObject *parent)
  : XmlParser(parent), _stack(), _state(State::None)
{
  // pass...
}

bool
PatternDefinitionParser::endDocument() {
  if (! XmlParser::endDocument())
    return false;

  if (1 != _stack.size()) {
    raiseError("Cannot parse codeplug pattern. No codeplug is left on the stack");
    return false;
  }

  if (! topIs<AbstractPatternDefinition>()) {
    raiseError("Cannot parse pattern. No pattern definition is left on the stack.");
    return false;
  }

  return true;
}

bool
PatternDefinitionParser::processText(const QStringView &content) {
  if (topIs<PatternMeta>()) {
    switch(_state) {
    case State::None: break;
    case State::MetaName: topAs<PatternMeta>()->setName(content.toString()); break;
    case State::MetaDescription: topAs<PatternMeta>()->setDescription(content.toString()); break;
    case State::MetaFWVersion: topAs<PatternMeta>()->setFirmwareVersion(content.toString()); break;
    }
    return true;
  }

  if (topIs<UnusedFieldPatternDefinition>()) {
    if (! topAs<UnusedFieldPatternDefinition>()->setContent(QByteArray::fromHex(content.toLatin1()))) {
      raiseError("Cannot set content of <unused> tag.");
      return false;
    }
    return true;
  }

  return true;
}

bool
PatternDefinitionParser::processDefaultArgs(const QXmlStreamAttributes &attributes) {
  Address addr;
  if (attributes.hasAttribute("at")) {
    addr = Address::fromString(attributes.value("at").toString());
    if (! addr.isValid()) {
      raiseError("Invalid address value.");
      return false;
    }
    topAs<AbstractPatternDefinition>()->setAddress(addr);
  }

  return true;
}


bool
PatternDefinitionParser::beginMetaElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<AbstractPatternDefinition>()) {
    raiseError("Unexpected <meta> tag.");
    return false;
  }
  push(new PatternMeta());
  return true;
}

bool
PatternDefinitionParser::endMetaElement() {
  PatternMeta *meta = popAs<PatternMeta>();
  if (! meta)
    return false;
  topAs<AbstractPatternDefinition>()->meta() = *meta;
  delete meta;
  return true;
}


bool
PatternDefinitionParser::beginNameElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <name> tag.");
    return false;
  }
  _state = State::MetaName;
  return true;
}

bool
PatternDefinitionParser::endNameElement() {
  _state = State::None;
  return true;
}

bool
PatternDefinitionParser::beginDescriptionElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <description> tag.");
    return false;
  }
  _state = State::MetaDescription;
  return true;
}

bool
PatternDefinitionParser::endDescriptionElement() {
  _state = State::None;
  return true;
}

bool
PatternDefinitionParser::beginVersionElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <version> tag.");
    return false;
  }
  _state = State::MetaFWVersion;
  return true;
}

bool
PatternDefinitionParser::endVersionElement() {
  _state = State::None;
  return true;
}

bool
PatternDefinitionParser::beginDoneElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <done> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::Done);
  return true;
}
bool
PatternDefinitionParser::endDoneElement() {
  return true;
}

bool
PatternDefinitionParser::beginNeedsReviewElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <needs-review> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::NeedsReview);
  return true;
}
bool
PatternDefinitionParser::endNeedsReviewElement() {
  return true;
}

bool
PatternDefinitionParser::beginIncompleteElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <incomplete> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::Incomplete);
  return true;
}
bool
PatternDefinitionParser::endIncompleteElement() {
  return true;
}


bool
PatternDefinitionParser::beginCodeplugElement(const QXmlStreamAttributes &attributes) {
  if (0 != _stack.size()) {
    raiseError("<codeplug> must be the root elment.");
    return false;
  }

  push(new CodeplugPatternDefinition());

  return true;
}

bool
PatternDefinitionParser::endCodeplugElement() {
  // Do nothing to keep the codeplug on the stack.
  return true;
}


bool
PatternDefinitionParser::beginRepeatElement(const QXmlStreamAttributes &attributes) {
  bool ok;

  Offset step;
  if (attributes.hasAttribute("step")) {
    step = Offset::fromString(attributes.value("step").toString());
    if (! step.isValid()) {
      raiseError("Invalid step value.");
      return false;
    }
  }

  unsigned int min = std::numeric_limits<unsigned int>().max();
  if (attributes.hasAttribute("min")) {
    min = attributes.value("min").toUInt(&ok);
    if (! ok) {
      raiseError("Invalid minimum repeat value.");
      return false;
    }
  }

  unsigned int max = std::numeric_limits<unsigned int>().max();
  if (attributes.hasAttribute("max")) {
    max = attributes.value("max").toUInt(&ok);
    if (! ok) {
      raiseError("Invalid maximum repeat value.");
      return false;
    }
  }

  unsigned int n = std::numeric_limits<unsigned int>().max();
  if (attributes.hasAttribute("n")) {
    n = attributes.value("n").toUInt(&ok);
    if (! ok) {
      raiseError("Invalid repeat-count value.");
      return false;
    }
  }

  // Dispatch by attributes
  if (step.isValid()) { // -> RepeatPattern
    // Check parent element
    if ((!_stack.isEmpty()) && (! topIs<GroupPatternDefinition>())) {
      raiseError("Unexpected <repeat> tag.");
      return false;
    }

    // Check if max-repeat is there
    if (std::numeric_limits<unsigned int>().max() == max) {
      raiseError("<repeat> requires a 'max' attribute.");
      return false;
    }
    // Check if min-repeat is there
    if (std::numeric_limits<unsigned int>().max() == min) {
      raiseError("<repeat> requires a 'min' attribute.");
      return false;
    }

    // Assemble with mandatory settings
    auto *pattern = new RepeatPatternDefinition();
    pattern->setStep(step);
    pattern->setMaxRepetition(max);
    pattern->setMinRepetition(min);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  if (std::numeric_limits<unsigned int>().max() != max) { // -> BlockRepeatPattern
    // Check parent element
    if ((!_stack.isEmpty()) && (! topIs<GroupPatternDefinition>())) {
      raiseError("Unexpected <repeat> tag.");
      return false;
    }

    // Check if min-repeat is there
    if (std::numeric_limits<unsigned int>().max() == min) {
      raiseError("<repeat> requires a 'min' attribute.");
      return false;
    }

    // Assemble with mandatory settings
    auto *pattern = new BlockRepeatPatternDefinition();
    pattern->setMaxRepetition(max);
    pattern->setMinRepetition(min);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  if (std::numeric_limits<unsigned int>().max() != n) { // -> FixedRepeatPattern
    // Assemble with mandatory settings
    auto *pattern = new FixedRepeatPatternDefinition();
    pattern->setRepetition(n);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  raiseError("<repeat> requires either 'min'/'max' or 'n' attribute.");
  return false;
}

bool
PatternDefinitionParser::endRepeatElement() {
  if (1 == _stack.size())
    return true;

  AbstractPatternDefinition *rep = popAs<AbstractPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <repeat> element to parent, not a stuctured pattern.");
    delete rep;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(rep)) {
    raiseError("Cannot add <repeat> element to parent, parent rejects child.");
    delete rep;
    return false;
  }

  return true;
}


bool
PatternDefinitionParser::beginElementElement(const QXmlStreamAttributes &attributes) {
  push(new ElementPatternDefinition());
  return processDefaultArgs(attributes);
;
}

bool
PatternDefinitionParser::endElementElement() {
  if (1 == _stack.size())
    return true;

  ElementPatternDefinition *el = popAs<ElementPatternDefinition>();

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(el)) {
    raiseError("Cannot add element to parent.");
    delete el;
    return false;
  }

  return true;
}

bool
PatternDefinitionParser::beginUnusedElement(const QXmlStreamAttributes &attributes) {
  UnusedFieldPatternDefinition *pattern = new UnusedFieldPatternDefinition();

  if (attributes.hasAttribute("width")) {
    Size size = Size::fromString(attributes.value("width").toString());
    if (! size.isValid()) {
      raiseError(QString("Invalid width value '%1' for <unsed> tag.")
                 .arg(attributes.value("width")));
      delete pattern;
      return false;
    }
    pattern->setWidth(size);
  }

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
PatternDefinitionParser::endUnusedElement() {
  if (1 == _stack.size())
    return true;

  UnusedFieldPatternDefinition *pattern = popAs<UnusedFieldPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <unused> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(pattern)) {
    raiseError("Cannot add <unused> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
PatternDefinitionParser::beginUnknownElement(const QXmlStreamAttributes &attributes) {
  Offset size;

  if (attributes.hasAttribute("width")) {
    size = Offset::fromString(attributes.value("width").toString());
    if (! size.isValid()) {
      raiseError("Cannot parse <unkonwn>, invalid 'width' attribute.");
      return false;
    }
  }

  auto *pattern = new UnknownFieldPatternDefinition();
  pattern->setWidth(size);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
PatternDefinitionParser::endUnknownElement() {
  if (1 == _stack.size())
    return true;

  UnknownFieldPatternDefinition *pattern = popAs<UnknownFieldPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <unknown> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(pattern)) {
    raiseError("Cannot add <unkown> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
PatternDefinitionParser::beginIntElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("width")) {
    raiseError("<int> tag requires 'width' attribute.");
    return false;
  }

  Offset size = Offset::fromString(attributes.value("width").toString());
  if (! size.isValid()) {
    raiseError("'width' attribute of <int> tag is invalid.");
    return false;
  }

  if ((1<size.byte()) && (! attributes.hasAttribute("endian"))) {
    raiseError("<int> tag requires 'endian' attribute.");
    return false;
  }

  IntegerFieldPattern::Format format = IntegerFieldPattern::Format::Signed;
  if (attributes.hasAttribute("format")) {
    if ("unsigned" == attributes.value("format").toString()) {
      format = IntegerFieldPattern::Format::Unsigned;
    } else if ("signed" == attributes.value("format").toString()) {
      format = IntegerFieldPattern::Format::Signed;
    } else if ("bcd" == attributes.value("format").toString()) {
      format = IntegerFieldPattern::Format::BCD;
    } else {
      raiseError(QString("Unknown integer format '%1'.").arg(attributes.value("format")));
      return false;
    }
  }

  IntegerFieldPattern::Endian endian = IntegerFieldPattern::Endian::Little;
  if (attributes.hasAttribute("endian")) {
    if ("little" == attributes.value("endian").toString()) {
      endian = IntegerFieldPattern::Endian::Little;
    } else if ("big" == attributes.value("endian").toString()) {
      endian = IntegerFieldPattern::Endian::Big;
    } else {
      raiseError(QString("Unknown endian '%1'.").arg(attributes.value("endian")));
      return false;
    }
  }

  IntegerFieldPatternDefinition *pattern = new IntegerFieldPatternDefinition();
  pattern->setWidth(size);
  pattern->setFormat(format);
  pattern->setEndian(endian);

  push(pattern);

  bool ok;
  if (attributes.hasAttribute("min")) {
    pattern->setMinValue(attributes.value("min").toLongLong(&ok));
    if (! ok) {
      raiseError(QString("Invalid 'min' value.").arg(attributes.value("min")));
      return false;
    }
  }

  if (attributes.hasAttribute("max")) {
    pattern->setMinValue(attributes.value("min").toLongLong(&ok));
    if (! ok) {
      raiseError(QString("Invalid 'max' value.").arg(attributes.value("max")));
      return false;
    }
  }

  if (attributes.hasAttribute("default")) {
    pattern->setDefaultValue(attributes.value("default").toLongLong(&ok));
    if (! ok) {
      raiseError(QString("Invalid 'default' value.").arg(attributes.value("max")));
      return false;
    }
  }

  return processDefaultArgs(attributes);
}

bool
PatternDefinitionParser::endIntElement() {
  if (1 == _stack.size())
    return true;

  IntegerFieldPatternDefinition *pattern = popAs<IntegerFieldPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <int> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(pattern)) {
    raiseError("Cannot add <int> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}


bool
PatternDefinitionParser::beginBitElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "unsigned");
  attrs.append("width", ":1");
  return beginIntElement(attrs);
}

bool
PatternDefinitionParser::endBitElement() {
  return endIntElement();
}

bool
PatternDefinitionParser::beginBcdElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "bcd");
  return beginIntElement(attrs);
}

bool
PatternDefinitionParser::endBcdElement() {
  return endIntElement();
}

bool
PatternDefinitionParser::beginBcd8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  return beginBcdElement(attrs);
}

bool
PatternDefinitionParser::endBcd8Element() {
  return endBcdElement();
}

bool
PatternDefinitionParser::beginUintElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "unsigned");
  return beginIntElement(attrs);
}

bool
PatternDefinitionParser::endUintElement() {
  return endIntElement();
}

bool
PatternDefinitionParser::beginInt8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "signed");
  attrs.append("width", ":10");
  return beginIntElement(attrs);
}

bool
PatternDefinitionParser::endInt8Element() {
  return endIntElement();
}

bool
PatternDefinitionParser::beginUint8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":10");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint8Element() {
  return endUintElement();
}


bool
PatternDefinitionParser::beginUint16Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint16Element() {
  return endUintElement();
}

bool
PatternDefinitionParser::beginUint16leElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  attrs.append("endian", "little");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint16leElement() {
  return endUintElement();
}

bool
PatternDefinitionParser::beginUint16beElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  attrs.append("endian", "big");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint16beElement() {
  return endUintElement();
}


bool
PatternDefinitionParser::beginUint32Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint32Element() {
  return endUintElement();
}

bool
PatternDefinitionParser::beginUint32leElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  attrs.append("endian", "little");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint32leElement() {
  return endUintElement();
}

bool
PatternDefinitionParser::beginUint32beElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  attrs.append("endian", "big");
  return beginUintElement(attrs);
}

bool
PatternDefinitionParser::endUint32beElement() {
  return endUintElement();
}


bool
PatternDefinitionParser::beginEnumElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("width")) {
    raiseError("<enum> element requires a 'width' attribute.");
    return false;
  }

  Size width = Size::fromString(attributes.value("width").toString());
  if (! width.isValid()) {
    raiseError(QString("<enum> element has invalid 'width' attribute '%1'.")
               .arg(attributes.value("width")));
    return false;
  }

  EnumFieldPatternDefinition *pattern = new EnumFieldPatternDefinition();
  pattern->setWidth(width);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
PatternDefinitionParser::endEnumElement() {
  if (1 == _stack.size())
    return true;

  EnumFieldPatternDefinition *pattern = popAs<EnumFieldPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <enum> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(pattern)) {
    raiseError("Cannot add <enum> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
PatternDefinitionParser::beginItemElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("value")) {
    raiseError(QString("<item> element requires a 'value' attribute."));
    return false;
  }
  bool ok;
  unsigned int value = attributes.value("value").toUInt(&ok);
  if (! ok) {
    raiseError(QString("Invalid value '%1' for 'value' attribute.").arg(attributes.value("value")));
    return false;
  }

  EnumFieldPatternItemDefinition *item = new EnumFieldPatternItemDefinition();
  item->setValue(value);

  push(item);

  return true;
}

bool
PatternDefinitionParser::endItemElement() {
  EnumFieldPatternItemDefinition *item = popAs<EnumFieldPatternItemDefinition>();
  topAs<EnumFieldPatternDefinition>()->addItem(item);
  return true;
}



bool
PatternDefinitionParser::beginStringElement(const QXmlStreamAttributes &attributes) {
  if (! attributes.hasAttribute("width")) {
    raiseError("<string> element requires a 'chars' attribute.");
    return false;
  }

  bool ok;
  unsigned int numChars = attributes.value("width").toUInt(&ok);
  if (! ok) {
    raiseError(QString("Invalid 'width' value '%1' for <string>.")
               .arg(attributes.value("chars")));
    return false;
  }

  StringFieldPattern::Format format = StringFieldPattern::Format::ASCII;
  if (attributes.hasAttribute("format")) {
    if ("ascii" == attributes.value("format").toString())
      format = StringFieldPattern::Format::ASCII;
    else if ("unicode" == attributes.value("format").toString())
      format = StringFieldPattern::Format::Unicode;
    else {
      raiseError(QString("Unknown format '%1' attribute for <string>.")
                 .arg(attributes.value("format")));
      return false;
    }
  }

  unsigned int padValue = 0;
  if (attributes.hasAttribute("pad")) {
    padValue = attributes.value("pad").toUInt(&ok);
    if (! ok) {
      raiseError(QString("Invalid value '%1' for 'pad' attribute of <string>.")
                 .arg(attributes.value("pad")));
      return false;
    }
  }

  auto pattern = new StringFieldPatternDefinition();
  pattern->setFormat(format);
  pattern->setNumChars(numChars);
  pattern->setPadValue(padValue);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
PatternDefinitionParser::endStringElement() {
  if (1 == _stack.size())
    return true;

  auto pattern = popAs<StringFieldPatternDefinition>();

  if (! topIs<StructuredPatternDefinition>()) {
    raiseError("Cannot add <string> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPatternDefinition>()->addChildPattern(pattern)) {
    raiseError("Cannot add <string> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}


void
PatternDefinitionParser::push(QObject *el) {
  _stack.append(el);
}
QObject *
PatternDefinitionParser::pop() {
  if (_stack.isEmpty())
    return nullptr;

  QObject *obj = _stack.back(); _stack.pop_back();
  obj->setParent(nullptr);

  return obj;
}
