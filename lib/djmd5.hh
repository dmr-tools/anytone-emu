#ifndef DJMD5_HH
#define DJMD5_HH

#include "device.hh"

class DJMD5 : public Device
{
  Q_OBJECT

public:
  explicit DJMD5(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV_HH
