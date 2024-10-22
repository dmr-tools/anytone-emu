#ifndef D868UVE_HH
#define D868UVE_HH

#include "device.hh"

/** Implements a AnyTone AT-D868UVE device emulator.
 * @ingroup device */
class D868UVE : public AnyToneDevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D868UVE(QIODevice *interface, Model* model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D868UVE_HH
