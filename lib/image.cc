#include "image.hh"
#include "annotation.hh"
#include "pattern.hh"


/* ********************************************************************************************* *
 * Implementation of Element
 * ********************************************************************************************* */
Element::Element(const Address &address, uint32_t size, QObject *parent)
  : QObject{parent}, AnnotationCollection(), _address(address), _data(size, 0)
{
  // pass...
}

Element::Element(const Address &address, const QByteArray &data, QObject *parent)
  : QObject{parent}, AnnotationCollection(), _address(address), _data(data)
{
  // pass...
}

bool
Element::operator<=(const Element &other) const {
  return this->address() <= other.address();
}

bool
Element::operator<(const Element &other) const {
  return this->address() < other.address();
}

bool
Element::operator==(const Element &other) const {
  return (this->address() == other.address()) &&
      (this->data() == other.data());
}

bool
Element::operator!=(const Element &other) const {
  return (this->address() != other.address()) ||
      (this->data() != other.data());
}

const Address &
Element::address() const {
  return _address;
}

Size
Element::size() const {
  return Size::fromByte(_data.size());
}

bool
Element::extends(uint32_t address) const {
  return extends(Address::fromByte(address));
}

bool
Element::extends(const Address &address) const {
  return address == (this->address()+this->size());
}

bool
Element::contains(uint32_t address, uint32_t size) const {
  return (Address::fromByte(address) >= this->address()) &&
      ((Address::fromByte(address)+Size::fromByte(size)) <= (this->address()+this->size()));
}

bool
Element::contains(const Address &address, const Size &size) const {
  return (address >= this->address()) &&
      ((address + size) <= (this->address()+this->size()));
}

const QByteArray &
Element::data() const {
  return _data;
}

const uint8_t *
Element::data(const Address &address) const {
  if (! contains(address))
    return nullptr;
  Offset offset = address-this->address();
  return (uint8_t *)data().constData() + offset.byte();
}

void
Element::append(const QByteArray &data) {
  this->_data.append(data);
  emit modified(_address.byte());
}

void
Element::addAnnotation(AbstractAnnotation *annotation) {
  annotation->setParent(this);
  AnnotationCollection::addAnnotation(annotation);
}



/* ********************************************************************************************* *
 * Implementation of Image
 * ********************************************************************************************* */
Image::Image(const QString &label, QObject *parent)
  : QObject{parent}, _label(label), _elements()
{
  // pass...
}

unsigned int
Image::count() const {
  return _elements.size();
}

const Element *
Image::element(unsigned int n) const {
  return _elements.at(n);
}

const uint8_t *
Image::data(uint32_t address) const {
  return data(Address::fromByte(address));
}

const uint8_t *
Image::data(const Address &address) const {
  Element *pred = findPred(address);
  if (nullptr == pred)
    return nullptr;

  return pred->data(address);
}

void
Image::append(uint32_t address, const QByteArray &data) {
  append(Address::fromByte(address), data);
}

void
Image::append(const Address &address, const QByteArray &data) {
  Element *pred = findPred(address);
  if ((nullptr == pred) || (! pred->extends(address))) {
    add(new Element(address, data, this));
    return;
  }

  pred->append(data);
}

const QString &
Image::label() const {
  return _label;
}
void
Image::setLabel(const QString &label) {
  _label = label;
}

bool
Image::annotate(const CodeplugPattern *pattern) {
 bool ok = ImageAnnotator::annotate(this, pattern);
 if (ok)
   emit annotated(this);
 return ok;
}

void
Image::add(Element *el) {
  unsigned int idx = findInsertionIndex(el->address().byte(), 0, _elements.size());
  el->setParent(this);
  _elements.insert(idx, el);
  emit modified(idx, el->address().byte());
}

Element *
Image::find(const Address &address) const {
  if (_elements.isEmpty())
    return nullptr;

  unsigned int idx = findInsertionIndex(address, 0, _elements.size());
  // Chcek if we hit element
  if ((_elements.size() > idx) && _elements.at(idx)->contains(address))
    return _elements.at(idx);
  if ((0 < idx) && _elements.at(idx-1)->contains(address))
    return _elements.at(idx-1);
  return nullptr;
}

Element *
Image::findPred(const Address &address) const {
  if (_elements.isEmpty())
    return nullptr;

  unsigned int idx = findInsertionIndex(address, 0, _elements.size());
  // Chcek if we hit element
  if ((_elements.size() > idx) && (_elements.at(idx)->contains(address)))
    return _elements.at(idx);
  if (0 == idx)
    return nullptr;
  return _elements.at(idx-1);
}

unsigned int
Image::findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const {
  return findInsertionIndex(Address::fromByte(address), a, b);
}

unsigned int
Image::findInsertionIndex(const Address &address, unsigned int a, unsigned int b) const {

  if (a == b)
    return a;

  if (address <= _elements.at(a)->address())
    return a;

  if (address >= _elements.at(b-1)->address())
    return b;

  unsigned int m = (a+b)/2;
  if (address < _elements.at(m)->address())
    return findInsertionIndex(address, a, m);
  return findInsertionIndex(address, m, b);
}


/* ********************************************************************************************* *
 * Implementation of Collection
 * ********************************************************************************************* */
Collection::Collection(QObject *parent)
  : QObject{parent}, _images()
{
  // pass...
}

unsigned int
Collection::count() const {
  return _images.size();
}

const Image *
Collection::image(unsigned int idx) const {
  return _images[idx];
}

int
Collection::indexOf(const Image *img) const {
  return _images.indexOf(img);
}

void
Collection::append(Image *image) {
  _images.append(image);
  image->setParent(this);
  connect(image, &QObject::destroyed, this, &Collection::onImageDeleted);
  connect(image, &Image::annotated, this, &Collection::onImageAnnotated);
  emit imageAdded(_images.size()-1);
}

void
Collection::onImageDeleted(QObject *obj) {
  int idx = _images.indexOf(qobject_cast<Image*>(obj));
  if (idx < 0)
    return;
  _images.remove(idx);
  disconnect(qobject_cast<Image*>(obj), &Image::annotated, this, &Collection::onImageAnnotated);
  emit imageRemoved(idx);
}

void
Collection::onImageAnnotated(Image *img) {
  int idx = _images.indexOf(img);
  if (idx < 0)
    return;
  emit imageAnnotated(idx);
}
