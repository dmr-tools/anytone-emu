#include "response.hh"
#include <QtEndian>

inline uint8_t crc8(const QByteArray &data) {
  uint8_t crc = 0;
  for (int i=0; i<data.length(); i++)
    crc += data[i];
  return crc;
}


