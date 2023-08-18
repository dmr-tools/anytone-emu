#ifndef MODEL_HH
#define MODEL_HH

#include <QObject>
class Image;

class Model : public QObject
{
  Q_OBJECT

protected:
  explicit Model(QObject *parent = nullptr);

public:
  virtual bool read(uint32_t address, uint8_t length, QByteArray &payload);
  virtual bool write(uint32_t address, const QByteArray &payload);

public slots:
  virtual void startProgram();
  virtual void endProgram();
};


class ImageCollector: public Model
{
  Q_OBJECT

public:
  explicit ImageCollector(QObject *parent = nullptr);

public:
  virtual bool write(uint32_t address, const QByteArray &payload);

  unsigned int count() const;
  const Image *image(unsigned int n) const;

  const Image *first() const;
  const Image *last() const;
  const Image *previous() const;

public slots:
  void startProgram();
  void endProgram();

signals:
  void imageReceived();

protected:
  QVector<Image *> _images;
};


#endif // MODEL_HH
