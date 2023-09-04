#include "codeplugpatternparser.hh"
#include "codeplugpattern.hh"

#include <QXmlStreamAttribute>

CodeplugPatternParser::CodeplugPatternParser(QObject *parent)
  : XmlParser(parent), _stack(), _state(State::None)
{
  // pass...
}

bool
CodeplugPatternParser::endDocument() {
  if (! XmlParser::endDocument())
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
CodeplugPatternParser::processText(const QStringView &content) {
  if (topIs<PatternMeta>()) {
    switch(_state) {
    case State::None: break;
    case State::MetaName: topAs<PatternMeta>()->setName(content.toString()); break;
    case State::MetaDescription: topAs<PatternMeta>()->setDescription(content.toString()); break;
    case State::MetaFWVersion: topAs<PatternMeta>()->setFirmwareVersion(content.toString()); break;
    }
    return true;
  }

  if (topIs<UnusedFieldPattern>()) {
    if (! topAs<UnusedFieldPattern>()->setContent(QByteArray::fromHex(content.toLatin1()))) {
      raiseError("Cannot set content of <unused> tag.");
      return false;
    }
    return true;
  }

  return true;
}

bool
CodeplugPatternParser::processDefaultArgs(const QXmlStreamAttributes &attributes) {
  Address addr;
  if (attributes.hasAttribute("at")) {
    addr = Address::fromString(attributes.value("at").toString());
    if (! addr.isValid()) {
      raiseError("Invalid address value.");
      return false;
    }
    topAs<AbstractPattern>()->setAddress(addr);
  }

  return true;
}


bool
CodeplugPatternParser::beginMetaElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<AbstractPattern>()) {
    raiseError("Unexpected <meta> tag.");
    return false;
  }
  push(new PatternMeta());
  return true;
}

bool
CodeplugPatternParser::endMetaElement() {
  PatternMeta *meta = popAs<PatternMeta>();
  if (! meta)
    return false;
  topAs<AbstractPattern>()->meta() = *meta;
  delete meta;
  return true;
}


bool
CodeplugPatternParser::beginNameElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <name> tag.");
    return false;
  }
  _state = State::MetaName;
  return true;
}

bool
CodeplugPatternParser::endNameElement() {
  _state = State::None;
  return true;
}

bool
CodeplugPatternParser::beginDescriptionElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <description> tag.");
    return false;
  }
  _state = State::MetaDescription;
  return true;
}

bool
CodeplugPatternParser::endDescriptionElement() {
  _state = State::None;
  return true;
}

bool
CodeplugPatternParser::beginVersionElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <version> tag.");
    return false;
  }
  _state = State::MetaFWVersion;
  return true;
}

bool
CodeplugPatternParser::endVersionElement() {
  _state = State::None;
  return true;
}

bool
CodeplugPatternParser::beginDoneElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <done> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::Done);
  return true;
}
bool
CodeplugPatternParser::endDoneElement() {
  return true;
}

bool
CodeplugPatternParser::beginNeedsReviewElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <needs-review> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::NeedsReview);
  return true;
}
bool
CodeplugPatternParser::endNeedsReviewElement() {
  return true;
}

bool
CodeplugPatternParser::beginIncompleteElement(const QXmlStreamAttributes &attributes) {
  if (! topIs<PatternMeta>()) {
    raiseError("Unexpected <incomplete> tag.");
    return false;
  }
  topAs<PatternMeta>()->setFlags(PatternMeta::Flags::Incomplete);
  return true;
}
bool
CodeplugPatternParser::endIncompleteElement() {
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


bool
CodeplugPatternParser::beginRepeatElement(const QXmlStreamAttributes &attributes) {
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
    if ((! topIs<GroupPattern>())) {
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
    RepeatPattern *pattern = new RepeatPattern();
    pattern->setStep(step);
    pattern->setMaxRepetition(max);
    pattern->setMinRepetition(min);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  if (std::numeric_limits<unsigned int>().max() != max) { // -> BlockRepeatPattern
    // Check parent element
    if ((! topIs<GroupPattern>())) {
      raiseError("Unexpected <repeat> tag.");
      return false;
    }

    // Check if min-repeat is there
    if (std::numeric_limits<unsigned int>().max() == min) {
      raiseError("<repeat> requires a 'min' attribute.");
      return false;
    }

    // Assemble with mandatory settings
    BlockRepeatPattern *pattern = new BlockRepeatPattern();
    pattern->setMaxRepetition(max);
    pattern->setMinRepetition(min);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  if (std::numeric_limits<unsigned int>().max() != n) { // -> FixedRepeatPattern
    // Assemble with mandatory settings
    FixedRepeatPattern *pattern = new FixedRepeatPattern();
    pattern->setRepetition(n);

    push(pattern);

    return processDefaultArgs(attributes);
  }

  raiseError("<repeat> requires either 'min'/'max' or 'n' attribute.");
  return false;
}

bool
CodeplugPatternParser::endRepeatElement() {
  AbstractPattern *rep = popAs<AbstractPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <repeat> element to parent, not a stuctured pattern.");
    delete rep;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(rep)) {
    raiseError("Cannot add <repeat> element to parent, parent rejects child.");
    delete rep;
    return false;
  }

  return true;
}


bool
CodeplugPatternParser::beginElementElement(const QXmlStreamAttributes &attributes) {
  push(new ElementPattern());
  return processDefaultArgs(attributes);
;
}

bool
CodeplugPatternParser::endElementElement() {
  ElementPattern *el = popAs<ElementPattern>();

  if (! topAs<StructuredPattern>()->addChildPattern(el)) {
    raiseError("Cannot add element to parent.");
    delete el;
    return false;
  }

  return true;
}

bool
CodeplugPatternParser::beginUnusedElement(const QXmlStreamAttributes &attributes) {
  UnusedFieldPattern *pattern = new UnusedFieldPattern();

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
CodeplugPatternParser::endUnusedElement() {
  UnusedFieldPattern *pattern = popAs<UnusedFieldPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <unused> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(pattern)) {
    raiseError("Cannot add <unused> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
CodeplugPatternParser::beginUnknownElement(const QXmlStreamAttributes &attributes) {
  Offset size;

  if (attributes.hasAttribute("width")) {
    size = Offset::fromString(attributes.value("width").toString());
    if (! size.isValid()) {
      raiseError("Cannot parse <unkonwn>, invalid 'width' attribute.");
      return false;
    }
  }

  UnknownFieldPattern *pattern = new UnknownFieldPattern();
  pattern->setWidth(size);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
CodeplugPatternParser::endUnknownElement() {
  UnknownFieldPattern *pattern = popAs<UnknownFieldPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <unknown> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(pattern)) {
    raiseError("Cannot add <unkown> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
CodeplugPatternParser::beginIntElement(const QXmlStreamAttributes &attributes)
{
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

  IntegerFieldPattern *pattern = new IntegerFieldPattern();
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
CodeplugPatternParser::endIntElement() {
  IntegerFieldPattern *pattern = popAs<IntegerFieldPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <int> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(pattern)) {
    raiseError("Cannot add <int> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}


bool
CodeplugPatternParser::beginBitElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "unsigned");
  attrs.append("width", ":1");
  return beginIntElement(attrs);
}

bool
CodeplugPatternParser::endBitElement() {
  return endIntElement();
}

bool
CodeplugPatternParser::beginBcdElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "bcd");
  return beginIntElement(attrs);
}

bool
CodeplugPatternParser::endBcdElement() {
  return endIntElement();
}

bool
CodeplugPatternParser::beginBcd8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  return beginBcdElement(attrs);
}

bool
CodeplugPatternParser::endBcd8Element() {
  return endBcdElement();
}

bool
CodeplugPatternParser::beginUintElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "unsigned");
  return beginIntElement(attrs);
}

bool
CodeplugPatternParser::endUintElement() {
  return endIntElement();
}

bool
CodeplugPatternParser::beginInt8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("format", "signed");
  attrs.append("width", ":10");
  return beginIntElement(attrs);
}

bool
CodeplugPatternParser::endInt8Element() {
  return endIntElement();
}

bool
CodeplugPatternParser::beginUint8Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":10");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint8Element() {
  return endUintElement();
}


bool
CodeplugPatternParser::beginUint16Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint16Element() {
  return endUintElement();
}

bool
CodeplugPatternParser::beginUint16leElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  attrs.append("endian", "little");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint16leElement() {
  return endUintElement();
}

bool
CodeplugPatternParser::beginUint16beElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":20");
  attrs.append("endian", "big");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint16beElement() {
  return endUintElement();
}


bool
CodeplugPatternParser::beginUint32Element(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint32Element() {
  return endUintElement();
}

bool
CodeplugPatternParser::beginUint32leElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  attrs.append("endian", "little");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint32leElement() {
  return endUintElement();
}

bool
CodeplugPatternParser::beginUint32beElement(const QXmlStreamAttributes &attributes) {
  QXmlStreamAttributes attrs(attributes);
  attrs.append("width", ":40");
  attrs.append("endian", "big");
  return beginUintElement(attrs);
}

bool
CodeplugPatternParser::endUint32beElement() {
  return endUintElement();
}


bool
CodeplugPatternParser::beginEnumElement(const QXmlStreamAttributes &attributes) {
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

  EnumFieldPattern *pattern = new EnumFieldPattern();
  pattern->setWidth(width);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
CodeplugPatternParser::endEnumElement() {
  EnumFieldPattern *pattern = popAs<EnumFieldPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <enum> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(pattern)) {
    raiseError("Cannot add <enum> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}

bool
CodeplugPatternParser::beginItemElement(const QXmlStreamAttributes &attributes) {
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

  EnumFieldPatternItem *item = new EnumFieldPatternItem();
  item->setValue(value);

  push(item);

  return true;
}

bool
CodeplugPatternParser::endItemElement() {
  EnumFieldPatternItem *item = popAs<EnumFieldPatternItem>();
  topAs<EnumFieldPattern>()->addItem(item);
  return true;
}



bool
CodeplugPatternParser::beginStringElement(const QXmlStreamAttributes &attributes) {
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

  auto pattern = new StringFieldPattern();
  pattern->setFormat(format);
  pattern->setNumChars(numChars);
  pattern->setPadValue(padValue);

  push(pattern);

  return processDefaultArgs(attributes);
}

bool
CodeplugPatternParser::endStringElement() {
  auto pattern = popAs<StringFieldPattern>();

  if (! topIs<StructuredPattern>()) {
    raiseError("Cannot add <string> field to parent, parent is not structured.");
    delete pattern;
    return false;
  }

  if (! topAs<StructuredPattern>()->addChildPattern(pattern)) {
    raiseError("Cannot add <string> field to parent, parent rejected it.");
    delete pattern;
    return false;
  }

  return true;
}


void
CodeplugPatternParser::push(QObject *el) {
  _stack.append(el);
}
QObject *
CodeplugPatternParser::pop() {
  if (_stack.isEmpty())
    return nullptr;

  QObject *obj = _stack.back(); _stack.pop_back();
  obj->setParent(nullptr);

  return obj;
}
