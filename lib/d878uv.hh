#ifndef D878UV_HH
#define D878UV_HH

#include "device.hh"

class D878UV : public Device
{
  Q_OBJECT

public:
  explicit D878UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV_HH
