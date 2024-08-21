#ifndef DJMD5_HH
#define DJMD5_HH

#include "device.hh"

/** Implements a Alinco DJ-MD5 device emulator.
 * @ingroup device */
class DJMD5 : public Device
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit DJMD5(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D878UV_HH
