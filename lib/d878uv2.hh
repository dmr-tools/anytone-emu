#ifndef D878UV2_HH
#define D878UV2_HH

#include "device.hh"

class D878UV2 : public Device
{
  Q_OBJECT

public:
  explicit D878UV2(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV2_HH
