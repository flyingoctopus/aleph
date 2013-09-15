#ifndef _ALEPH_DSP_DRUMSYN_H_
#define _ALEPH_DSP_DRUMSYN_H_

#include "env_int.h"
#include "filter_svf.h"
#include "noise.h"

#define DRUMSYN_NVOICES 1

typedef struct _drumsynVoice {
  filter_svf* svf;
  u8 gate;
  fract32 amp;
  env_int* envAmp;
  env_int* envFreq;
  env_int* envRes;
  //  fract32 envOffAmp; // offset 
  //  fract32 envMulAmp; // multiplier
  fract32 envAddFreq; // offset 
  fract32 envMulFreq; // multiplier
  fract32 envAddRes; // offset 
  fract32 envMulRes; // multiplier
  lcprng* rngH;
  lcprng* rngL;
} drumsynVoice;

extern drumsynVoice* voices[DRUMSYN_NVOICES];

//////
///
/// TESTing 
// envelope is nuts? try just a raw integrator
extern filter_1p_fr32 ampLp;
//
/////


#endif // h guard
