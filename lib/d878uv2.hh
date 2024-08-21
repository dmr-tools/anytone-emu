#ifndef D878UV2_HH
#define D878UV2_HH

#include "device.hh"

/** Implements a AnyTone AT-D878UV II device emulator.
 * @ingroup device */
class D878UV2 : public Device
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D878UV2(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV2_HH
