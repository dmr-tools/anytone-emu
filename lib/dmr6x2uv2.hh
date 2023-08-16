#ifndef DMR6X2UV2_HH
#define DMR6X2UV2_HH

#include "device.hh"

class DMR6X2UV2 : public Device
{
  Q_OBJECT

public:
  explicit DMR6X2UV2(QIODevice *interface, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // DMR6X2UV2_HH
