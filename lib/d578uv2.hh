#ifndef D578UV2_HH
#define D578UV2_HH

#include "device.hh"

/** Implements a AnyTone AT-D578UV II device emulator.
 * @ingroup device */
class D578UV2 : public AnyToneDevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D578UV2(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D578UV2_HH
