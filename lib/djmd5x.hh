#ifndef DJMD5X_HH
#define DJMD5X_HH

#include "device.hh"

/** Implements a Alinco DJ-MD5X device emulator.
 * @ingroup device */
class DJMD5X : public AnyToneDevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit DJMD5X(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // DJMD5X_HH
