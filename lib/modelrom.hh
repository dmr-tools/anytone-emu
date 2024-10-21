#ifndef MODELROM_HH
#define MODELROM_HH

#include <cstdint>
#include <QByteArray>
#include <QVector>


/** Implementes some fixed memory read from an emulated model. */
class ModelRom
{
public:
  struct Segment {
    uint32_t address;
    QByteArray content;

    bool operator<(const Segment &other) const;
    bool operator<(uint32_t address) const;
    bool contains(uint32_t address, uint8_t size=0) const;
  };

  typedef QVector<Segment>::iterator iterator;
  typedef QVector<Segment>::const_iterator const_iterator;

public:
  ModelRom();
  ModelRom(const ModelRom &other) = default;

  ModelRom operator+(const ModelRom &other) const;
  const ModelRom &operator+=(const ModelRom &other);

  unsigned int segmentCount() const;

  void write(uint32_t address, const QByteArray &data);
  bool read(uint32_t address, uint8_t length, QByteArray &data) const;

  const_iterator begin() const;
  const_iterator end() const;
  iterator begin();
  iterator end();

protected:
  QVector<Segment> _content;
};



#endif // MODELROM_HH
