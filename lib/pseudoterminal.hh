#ifndef PSEUDOTERMINAL_HH
#define PSEUDOTERMINAL_HH

#include <QtGlobal>

#if defined(Q_OS_WINDOWS)
#include "pseudotermimalwindows.hh"
#elif defined(Q_OS_UNIX)
#include "pseudoterminalposix.hh"
#endif

#endif // PSEUDOTERMINAL_HH
