#ifndef D578UV_HH
#define D578UV_HH

#include "device.hh"

class D578UV : public Device
{
  Q_OBJECT

public:
  explicit D578UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D578UV_HH
