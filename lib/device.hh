/** @defgroup device Device emulation
 * This module collects all classes that implement the device emulation. This includes the interface
 * implementation. */

#ifndef DEVICE_HH
#define DEVICE_HH

#include "modelrom.hh"
#include <QObject>
#include <QHash>
#include <QPair>

class Model;
class QIODevice;
class ImageCollector;
class CodeplugPattern;


class Device: public QObject
{
  Q_OBJECT

protected:
  explicit Device(CodeplugPattern *pattern, ImageCollector *handler, QObject *parent=nullptr);

public:
  /** Reads some data from the device and stores it @c payload. */
  virtual bool read(uint32_t addr, uint16_t len, QByteArray &payload);
  /** Write some data. */
  virtual bool write(uint32_t addr, const QByteArray &data);

  virtual ImageCollector *handler() const;
  void setHandler(ImageCollector *handler);

  /** Returns the pattern associated with this device. */
  virtual CodeplugPattern *pattern() const;

  const ModelRom &rom() const;
  ModelRom &rom();

signals:
  /** Gets emitted once the programming started. */
  void startProgram();
  /** Gets emitted once the programming ended. */
  void endProgram();

protected:
  CodeplugPattern *_pattern;
  ImageCollector *_handler;
  ModelRom _rom;
};




#endif // DEVICE_HH
