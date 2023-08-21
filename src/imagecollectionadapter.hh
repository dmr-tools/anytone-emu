#ifndef IMAGECOLLECTIONADAPTER_HH
#define IMAGECOLLECTIONADAPTER_HH

#include "model.hh"

class Collection;


class ImageCollectionAdapter: public Model
{
  Q_OBJECT

public:
  explicit ImageCollectionAdapter(Collection *collection, QObject *parent = nullptr);

public:
  virtual bool write(uint32_t address, const QByteArray &payload);

public slots:
  void startProgram();
  void endProgram();

protected:
  /** The current image (owned). */
  Image *_image;
  Collection *_collection;
};

#endif // IMAGECOLLECTIONADAPTER_HH
