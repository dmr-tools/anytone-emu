#ifndef D868UV_HH
#define D868UV_HH

#include "device.hh"

class D868UV : public Device
{
  Q_OBJECT

public:
  explicit D868UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D868UV_HH
