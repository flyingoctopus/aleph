/*
ctl.c
flry
aleph


set DAC values with gamepad + keys + encoders.

for each of the 4 dac channels, store a value for each state of the input (on or off)
also store a slew value for each state.

edit the value for the last touched channel/state using encoder 0.
edit the slew for the last touched channel/state from encoder 1.


 */

// std
#include <string.h>
// asf
#include "delay.h"
#include "print_funcs.h"
// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "control.h"
#include "interrupts.h"
// common
#include "fix.h"
#include "param_common.h"
// lppr
#include "ctl.h"
#include "render.h"
#include "util.h"

// blah
#define DSP_STRING "aleph-dacs"


//----------------------------------------
//---- static variables

//-- parameter values
// up and down values for each dac
// actual range in [0, PARAM_DAC_MAX]
static s32 dac[4][2] = { {0,0}, {0,0}, {0,0}, {0,0} };
// invert flags
static s32 inv[4];


//// TODO:
/*
// up and down values for each dac slew
// range in [0, FR32_MAX]
static s32 slew[4][2] = { {0,0}, {0,0}, {0,0}, {0,0} };

// for each channel, which state was last touched
static u8 state[4] = {0, 0, 0, 0};

// last touched channel
static u8 chan = 0;
*/


//----------------------------------------
//---- static functions
/// send dac value with inversion based on current state
static void send_dac(u8 ch) {
  u32 v;
  if(inv[ch]) {
    v = PARAM_DAC_MAX - dac[ch][0];
  } else {
    v = dac[ch][0];
  }
  ctl_param_change(eParam_dac0 + ch, v);
  render_dac(ch, v);
}

//-------------------------------------
//------ extern functions

// get parameter report from DSP
u8 ctl_report_params(void) {
  volatile char buf[64];
  volatile ParamDesc pdesc;
  u32 numParams;
  u8 i;
 
  bfin_get_num_params(&numParams);

  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams == 255) {
    print_dbg("\r\n report_params fail (too many)");
    return 0;
  }

  if(numParams > 0) {
    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);
      
      print_dbg("\r\n got pdesc : ");
      print_dbg((const char* )pdesc.label);
    }
  } else {
    print_dbg("\r\n report_params fail (none)");
    return 0;
  }
  
  delay_ms(100);

  print_dbg("\r\n checking module label ");
  // check module label
  bfin_get_module_name(buf);

  delay_ms(10);

  print_dbg("\r\n bfin module name: ");
  print_dbg((const char*)buf);
  if(strcmp((const char*)buf, DSP_STRING)) {
    print_dbg( "\r\n report_params fail (module name mismatch)" );
    return 0;
  } else {
    return 1;
  }
}

// set initial parameters
void ctl_init_params(void) {
  // dacs at 0
  ctl_param_change(eParam_dac0, 0);
  ctl_param_change(eParam_dac1, 0);
  ctl_param_change(eParam_dac2, 0);
  ctl_param_change(eParam_dac3, 0);

}



// set dac value
void  ctl_set_value(u8 ch, u16 val) {
  // param enum hack...
  dac[ch][0] = val;
  send_dac(ch);

  /* print_dbg("\r\n set dac, channel : "); */
  /* print_dbg_ulong(ch); */
  /* print_dbg(", value : 0x"); */
  /* print_dbg_hex(val); */
  
}

// increment dac value
void ctl_inc_value(u8 ch, s32 delta) {
  s32 tmp = dac[ch][0] + delta;

  if(tmp > PARAM_DAC_MAX) {    
    tmp = PARAM_DAC_MAX;
  }

  if(tmp < PARAM_DAC_MIN) {
    if (delta>0) { // overflow
      tmp = PARAM_DAC_MAX;
    } else {
      tmp = PARAM_DAC_MIN;
    }
  }
  ctl_set_value(ch, tmp);
}

// set slew
// increment slew


// button: invert and send
void ctl_but(u8 i, u8 val) {
  //  u32 v;
  inv[i] = val;
  send_dac(i);
  //  if(val) {
  //    v = 0xffff - dac[i][0];
  //    ctl_param_change(eParam_dac0 + ch, v);
  //    render_dac(ch, v);
  //  }
}

// joystick axis: change value and send
void ctl_joy(u8 ch, u8 val) {
  //  u32 v;
  // hack: lshift from u8 (joystick) to u16(dac)
  dac[ch][0] = val << 8;
  send_dac(ch);
  //  v = 0xffff - (dac[i][0] << 8);
}

// button in
// trigger in
