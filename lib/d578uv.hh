#ifndef D578UV_HH
#define D578UV_HH

#include "device.hh"

/** Implements a AnyTone AT-D578UV device emulator.
 * @ingroup device */
class D578UV : public Device
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D578UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D578UV_HH
