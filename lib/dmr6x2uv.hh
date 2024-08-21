#ifndef DMR6X2UV_HH
#define DMR6X2UV_HH

#include "device.hh"

/** Implements a Btech DMR-6X2UV device emulator.
 * @ingroup device */
class DMR6X2UV : public Device
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit DMR6X2UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // DMR6X2UV_HH
