#ifndef MODEL_HH
#define MODEL_HH

#include <QObject>

class Image;
class CodeplugPattern;


/** Interface for all memoy models. That is, the object that is read and written by the CPS
 * through the device emulation. The model also owns the codeplug pattern, that can be applied to
 * annotate any received images.
 * @ingroup device */
class Model : public QObject
{
  Q_OBJECT

  /** The codpelug pattern. */
  Q_PROPERTY(CodeplugPattern* pattern READ pattern WRITE setPattern)

protected:
  /** Hidden constructor.
   * @param pattern The codeplug pattern to annotate received images. The ownership is taken.
   * @param parent The QObject parent. **/
  explicit Model(CodeplugPattern *pattern = nullptr, QObject *parent = nullptr);

public:
  /** Read data from the model. */
  virtual bool read(uint32_t address, uint8_t length, QByteArray &payload);
  /** write data to the model. */
  virtual bool write(uint32_t address, const QByteArray &payload);

  /** Read data from ROM. */
  virtual bool readRom(uint32_t address, uint8_t length, QByteArray &payload) const;
  /** write data to ROM. */
  virtual bool writeRom(uint32_t address, const QByteArray &payload);

  /** Returns a weak reference to the codeplug pattern. */
  CodeplugPattern *pattern() const;
  /** Sets the codeplug pattern. The ownership is taken. */
  void setPattern(CodeplugPattern *pattern);

public slots:
  /** Gets called when the "radio programming" starts. */
  virtual void startProgram();
  /** Gets called when the "radio programming" end. */
  virtual void endProgram();

protected:
  /** The codeplug pattern, or @c nullptr if none is set. */
  CodeplugPattern *_pattern;
  /** Holds some portions of pre-defined memory (ROM). */
  QVector<QPair<uint32_t, QByteArray>> _rom;
};



/** A memory model to collect and strore images.
 * @ingroup device */
class ImageCollector: public Model
{
  Q_OBJECT

public:
  /** Default constructor.
   * @param pattern Specifies the codeplug pattern to annotate received images.
   *        If passed, the ownership is taken.
   * @param parent A weak reference to the parent object. */
  explicit ImageCollector(CodeplugPattern *pattern = nullptr, QObject *parent = nullptr);

public:
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
  void startProgram();
  /** Gets called when the "radio programming" end. */
  void endProgram();

signals:
  /** Gets emitted, once an image is received. */
  void imageReceived();

protected:
  /** The images. */
  QVector<Image *> _images;
};


#endif // MODEL_HH
