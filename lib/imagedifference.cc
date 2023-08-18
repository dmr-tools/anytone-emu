#include "imagedifference.hh"


/* ********************************************************************************************* *
 * Implementation of ElementDifference
 * ********************************************************************************************* */
ElementDifference::ElementDifference(const Element* left, const Element* right, QObject *parent)
  : QObject{parent}, _left(left), _right(right)
{
  if (_left)
    connect(_left, &QObject::destroyed, this, &ElementDifference::onElementDeleted);
  if (_right)
    connect(_right, &QObject::destroyed, this, &ElementDifference::onElementDeleted);
}

void
ElementDifference::onElementDeleted(QObject *obj) {
  if (_left == obj)
    _left = nullptr;
  if (_right == obj)
    _right = nullptr;
}


/* ********************************************************************************************* *
 * Implementation of AddElement
 * ********************************************************************************************* */
AddElement::AddElement(const Element *b, QObject *parent)
  : ElementDifference{nullptr, b, parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of DelElement
 * ********************************************************************************************* */
DelElement::DelElement(const Element *a, QObject *parent)
  : ElementDifference{a, nullptr, parent}
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ModifyElement::Modification
 * ********************************************************************************************* */
ModifyElement::Modification
ModifyElement::Modification::add(uint32_t at, uint8_t what) {
  return {.type=Type::Add, .offset=at, .added=what, .removed=0};
}

ModifyElement::Modification
ModifyElement::Modification::rem(uint32_t at, uint8_t what) {
  return {.type=Type::Del, .offset=at, .added=0, .removed=what};
}

ModifyElement::Modification
ModifyElement::Modification::mod(uint32_t at, uint8_t from, uint8_t to) {
  return {.type=Type::Del, .offset=at, .added=to, .removed=from};
}


/* ********************************************************************************************* *
 * Implementation of ModifyElement
 * ********************************************************************************************* */
ModifyElement::ModifyElement(const Element *a, const Element *b, QObject *parent)
  : ElementDifference{a, b, parent}
{
  for (unsigned int i=0, j=0; (i < (unsigned int)a->data().size()) || (j < (unsigned int)b->data().size()); ) {
    if ((i < (unsigned int)a->data().size()) && (j < (unsigned int)b->data().size())) {
      if (a->data().at(i) != b->data().at(j))
        _changes.append(Modification::mod(i, a->data().at(i), b->data().at(j)));
      i++; j++;
    } else if (i < (unsigned int)a->data().size()) {
      _changes.append(Modification::rem(i, a->data().at(i)));
      i++;
    } else if (j < (unsigned int)b->data().size()) {
      _changes.append(Modification::add(j, b->data().at(j)));
      j++;
    }
  }
}


/* ********************************************************************************************* *
 * Implementation of ImageDifference
 * ********************************************************************************************* */
ImageDifference::ImageDifference(const Image* a, const Image* b, QObject *parent)
  : QObject{parent}, _a(a), _b(b), _differences()
{
  connect(a, &QObject::destroyed, this, &ImageDifference::onImageDeleted);
  connect(b, &QObject::destroyed, this, &ImageDifference::onImageDeleted);

  for (unsigned int i=0,j=0; (i<_a->count()) || (j<_b->count()); ) {
    if ((i<_a->count()) && (j<_b->count())) {
      if (_a->element(i)->address() < _b->element(j)->address()) {
        _differences.append(new DelElement(_a->element(i), this));
        i++;
      } else if (_a->element(i)->address() > _b->element(j)->address()) {
        _differences.append(new AddElement(_b->element(j), this));
        j++;
      } else {
        if ((*(_a->element(i))) != (*(_b->element(j))))
          _differences.append(new ModifyElement(_a->element(i), _b->element(j), this));
        i++; j++;
      }
    } else if (i < _a->count()) {
      _differences.append(new DelElement(_a->element(i), this));
      i++;
    } else if (j < _b->count()) {
      _differences.append(new AddElement(_b->element(j), this));
      j++;
    }
  }
}

unsigned int
ImageDifference::count() const {
  return _differences.count();
}

const ElementDifference *
ImageDifference::diff(unsigned int n) const {
  return _differences.at(n);
}

void
ImageDifference::onImageDeleted(QObject *obj) {
  if (_a == obj) {
    _a = nullptr;
    if (_b)
      disconnect(_b, &QObject::destroyed, this, &ImageDifference::onImageDeleted);
    _b = nullptr;
  } else if (_b == obj) {
    _b = nullptr;
    if (_a)
      disconnect(_a, &QObject::destroyed, this, &ImageDifference::onImageDeleted);
    _a = nullptr;
  }
}
