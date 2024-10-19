#ifndef MODEL_HH
#define MODEL_HH

#include <QObject>

class Image;


/** A memory model to collect and strore images.
 * @ingroup device */
class ImageCollector: public QObject
{
  Q_OBJECT

public:
  /** Default constructor.
   * @param parent A weak reference to the parent object. */
  explicit ImageCollector(QObject *parent = nullptr);

public:
  /** Read data from the model. */
  virtual bool read(uint32_t address, uint8_t length, QByteArray &payload);
  /** write data to the model. */
  virtual bool write(uint32_t address, const QByteArray &payload);

  /** Returns the number of received images. */
  unsigned int count() const;
  /** Returns the n-th image. */
  const Image *image(unsigned int n) const;

  /** Convenience function to return the first image.
   * Retruns @c nullptr if is are none. */
  const Image *first() const;
  /** Convenience function to return the last (most recent) image.
   * Retruns @c nullptr if is are none. */
  const Image *last() const;
  /** Convenience function to return the previous (2nd to last) image.
   * Retruns @c nullptr if is are none. */
  const Image *previous() const;

public slots:
  /** Gets called when the "radio programming" starts. */
  virtual void startProgram();
  /** Gets called when the "radio programming" end. */
  virtual void endProgram();

signals:
  /** Gets emitted, once an image is received. */
  void imageReceived();

protected:
  /** The images. */
  QVector<Image *> _images;
};


#endif // MODEL_HH
