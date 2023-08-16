#ifndef D868UVE_HH
#define D868UVE_HH

#include "device.hh"

class D868UVE : public Device
{
  Q_OBJECT

public:
  explicit D868UVE(QIODevice *interface, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D868UVE_HH
