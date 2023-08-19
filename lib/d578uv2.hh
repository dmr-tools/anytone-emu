#ifndef D578UV2_HH
#define D578UV2_HH

#include "device.hh"

class D578UV2 : public Device
{
  Q_OBJECT

public:
  explicit D578UV2(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D578UV2_HH
