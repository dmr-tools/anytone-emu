#ifndef DJMD5X_HH
#define DJMD5X_HH

#include "device.hh"

class DJMD5X : public Device
{
  Q_OBJECT

public:
  explicit DJMD5X(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // DJMD5X_HH
