#ifndef HEXDUMP_HH
#define HEXDUMP_HH

class Element;
class Image;
class ElementDifference;
class ImageDifference;
class QTextStream;

void hexdump(const Element *el, QTextStream &stream);
void hexdump(const Image *im, QTextStream &stream);

void hexdump(const ElementDifference *el, QTextStream &stream);
void hexdump(const ImageDifference *im, QTextStream &stream);

#endif // HEXDUMP_HH
