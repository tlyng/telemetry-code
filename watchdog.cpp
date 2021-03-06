#include "logging.hpp"
#include "watchdog.hpp"
#include <algorithm>

// https://forum.arduino.cc/index.php?topic=366836.0

namespace
{
  Logger& logger = logging::get("watchdog");
  
  static void   WDTsync() {
    while (WDT->STATUS.bit.SYNCBUSY == 1); //Just wait till WDT is free
  }
  
  //============= resetWDT ===================================================== resetWDT ============
  void resetWDT() {
    // reset the WDT watchdog timer.
    // this must be called before the WDT resets the system
    WDT->CLEAR.reg= 0xA5; // reset the WDT
    WDTsync(); 
  }
  
  //============= systemReset ================================================== systemReset ============
  void systemReset() {
    // use the WDT watchdog timer to force a system reset.
    // WDT MUST be running for this to work
    WDT->CLEAR.reg= 0x00; // system reset via WDT
    WDTsync(); 
  }
  
  //============= setupWDT ===================================================== setupWDT ============
  void setupWDT(int period) {
    // initialize the WDT watchdog timer
  
    WDT->CTRL.reg = 0; // disable watchdog
    WDTsync(); // sync is required
  
    WDT->CONFIG.reg = std::min(period, 11); // see Table 17-5 Timeout Period (valid values 0-11)
  
    WDT->CTRL.reg = WDT_CTRL_ENABLE; //enable watchdog
    WDTsync(); 
  }

  void stopWDT() {
    WDT->CTRL.reg = 0; // disable watchdog
    WDTsync();    
  }
}

namespace watchdog
{
  bool inited = false;
  
  void begin() {
    setupWDT( 11 ); // initialize and activate WDT with maximum period 
    inited = true;
    tickle();
  }
  
  void tickle()
  {
    if (inited) resetWDT();
  }
  
  void reboot()
  {
    logger.println("Rebooting");
    logger.flush();
    begin();
    systemReset();    
  }

}

