#ifndef D868UV_HH
#define D868UV_HH

#include "device.hh"

class CodeplugPattern;


/** Implements a AnyTone AT-D868UV device emulator.
 * @ingroup device */
class D868UV : public Device
{
  Q_OBJECT

public:
  /** Constructor.
   * @param interface Specifies the interace to the CPS.
   * @param model Specifies the memory model for capturing codeplugs.
   * @param parent The QObject parent. */
  explicit D868UV(QIODevice *interface, Model *model = nullptr, QObject *parent = nullptr);

  QByteArray model() const;
  QByteArray hwVersion() const;
};

#endif // D868UV_HH
