#ifndef D878UV_HH
#define D878UV_HH

#include "device.hh"

/** Implements a AnyTone AT-D878UV device emulator.
 * @ingroup device */
class D878UV : public AnyToneDevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D878UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV_HH
