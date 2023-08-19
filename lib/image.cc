#include "image.hh"

/* ********************************************************************************************* *
 * Implementation of Element
 * ********************************************************************************************* */
Element::Element(uint32_t address, uint32_t size, QObject *parent)
  : QObject{parent}, _address(address), _data(size,0)
{
  // pass...
}

Element::Element(uint32_t address, const QByteArray &data, QObject *parent)
  : QObject{parent}, _address(address), _data(data)
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

uint32_t
Element::address() const {
  return _address;
}

uint32_t
Element::size() const {
  return _data.size();
}

bool
Element::extends(uint32_t address) const {
  return address == (this->address()+this->size());
}

const QByteArray &
Element::data() const {
  return _data;
}

const uint8_t *
Element::data(uint32_t address) const {
  if ((address < this->address()) || (address >= (this->address()+this->size())))
    return nullptr;
  uint32_t offset = address-this->address();
  return (uint8_t *)data().constData() + offset;
}

void
Element::append(const QByteArray &data) {
  this->_data.append(data);
  emit modified(_address);
}



/* ********************************************************************************************* *
 * Implementation of Image
 * ********************************************************************************************* */
Image::Image(QObject *parent)
  : QObject{parent}, _elements()
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
  Element *pred = findPred(address);
  if (nullptr == pred)
    return nullptr;

  return pred->data(address);
}

void
Image::append(uint32_t address, const QByteArray &data) {
  Element *pred = findPred(address);
  if ((nullptr == pred) || (! pred->extends(address))) {
    add(new Element(address, data, this));
    return;
  }

  pred->append(data);
}

void
Image::add(Element *el) {
  unsigned int idx = findInsertionIndex(el->address(), 0, _elements.size());
  el->setParent(this);
  _elements.insert(idx, el);
  emit modified(idx, el->address());
}

Element *
Image::findPred(uint32_t address) const {
  unsigned int idx = findInsertionIndex(address, 0, _elements.size());
  if (0 == idx)
    return nullptr;
  return _elements.at(idx-1);
}

unsigned int
Image::findInsertionIndex(uint32_t address, unsigned int a, unsigned int b) const {
  if (a == b)
    return a;

  if (address < _elements.at(a)->address())
    return a;

  if (address > _elements.at(b-1)->address())
    return b;

  unsigned int m = (a+b)/2;
  if (address < _elements.at(m)->address())
    return findInsertionIndex(address, a, m);
  return findInsertionIndex(address, m, b);
}
