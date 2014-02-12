#include "hphp/runtime/base/base-includes.h"
#include "hphp/runtime/base/stream-wrapper-registry.h"
#include <hphp/runtime/vm/event-hook.h>
#include "ExternalEventsRporter.h"
#include "hphp/runtime/monitor/monitor.h"

namespace HPHP
{
// Statics needed here
class monitorExtension : public Extension
{
public:
  monitorExtension() : Extension("monitor", "1.0 Release") {}
  virtual void moduleInit() {
    // HHVM_FE(hw_print);
    loadSystemlib();
    EventHook::Enable();

    // Initialize Zend event reporting
    Zend::InitializeReporting();
  }
  
  virtual void requestShutdown() {
    Zend::SendEvents("http://localhost/bla.php", "myscript.php");
  }
} s_monitor_extension;

// Uncomment for non-bundled module
HHVM_GET_MODULE(monitor);

//////////////////////////////////////////////////////////////////////////////
} // namespace HPHP
