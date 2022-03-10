/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2022 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _X1_010_H
#define _X1_010_H

#include <queue>
#include "../dispatch.h"
#include "../engine.h"
#include "../macroInt.h"
#include "sound/x1_010/x1_010.hpp"

class DivX1_010Interface: public x1_010_mem_intf {
  public:
    DivEngine* parent;
    int sampleBank;
    virtual u8 read_byte(u32 address) override {
      if (parent->x1_010Mem==NULL) return 0;
      return parent->x1_010Mem[address & 0xfffff];
    }
    DivX1_010Interface(): parent(NULL), sampleBank(0) {}
};

class DivPlatformX1_010: public DivDispatch {
  struct Channel {
    struct Envelope {
      struct EnvFlag {
        unsigned char envEnable : 1;
        unsigned char envOneshot : 1;
        unsigned char envSplit : 1;
        unsigned char envHinvR : 1;
        unsigned char envVinvR : 1;
        unsigned char envHinvL : 1;
        unsigned char envVinvL : 1;
        void reset() {
          envEnable=0;
          envOneshot=0;
          envSplit=0;
          envHinvR=0;
          envVinvR=0;
          envHinvL=0;
          envVinvL=0;
        }
        EnvFlag():
          envEnable(0),
          envOneshot(0),
          envSplit(0),
          envHinvR(0),
          envVinvR(0),
          envHinvL(0),
          envVinvL(0) {}
      };
      int shape, period, slide, slidefrac;
      EnvFlag flag;
      void reset() {
        shape=-1;
        period=0;
        flag.reset();
      }
      Envelope():
        shape(-1),
        period(0),
        slide(0),
        slidefrac(0) {}
    };
    int freq, baseFreq, pitch, note;
    int wave, sample, ins;
    unsigned char pan, autoEnvNum, autoEnvDen;
    bool active, insChanged, envChanged, freqChanged, keyOn, keyOff, inPorta, furnacePCM, pcm;
    int vol, outVol, lvol, rvol;
    unsigned char waveBank;
    Envelope env;
    DivMacroInt std;
    void reset() {
        freq = baseFreq = pitch = note = 0;
        wave = sample = ins = -1;
        pan = 255;
        autoEnvNum = autoEnvDen = 0;
        active = false;
        insChanged = envChanged = freqChanged = true;
        keyOn = keyOff = inPorta = furnacePCM = pcm = false;
        vol = outVol = lvol = rvol = 15;
        waveBank = 0;
    }
    Channel():
      freq(0), baseFreq(0), pitch(0), note(0),
      wave(-1), sample(-1), ins(-1),
      pan(255), autoEnvNum(0), autoEnvDen(0),
      active(false), insChanged(true), envChanged(true), freqChanged(false), keyOn(false), keyOff(false), inPorta(false), furnacePCM(false), pcm(false),
      vol(15), outVol(15), lvol(15), rvol(15),
      waveBank(0) {}
  };
  Channel chan[16];
  bool isMuted[16];
  bool stereo=false;
  unsigned char sampleBank;
  DivX1_010Interface intf;
  x1_010_core* x1_010;
  unsigned char regPool[0x2000];
  double NoteX1_010(int ch, int note);
  void updateWave(int ch);
  void updateEnvelope(int ch);
  friend void putDispatchChan(void*,int,int);
  public:
    void acquire(short* bufL, short* bufR, size_t start, size_t len);
    int dispatch(DivCommand c);
    void* getChanState(int chan);
    unsigned char* getRegisterPool();
    int getRegisterPoolSize();
    void reset();
    void forceIns();
    void tick();
    void muteChannel(int ch, bool mute);
    bool isStereo();
    bool keyOffAffectsArp(int ch);
    void setFlags(unsigned int flags);
    void notifyWaveChange(int wave);
    void notifyInsDeletion(void* ins);
    void poke(unsigned int addr, unsigned short val);
    void poke(std::vector<DivRegWrite>& wlist);
    const char** getRegisterSheet();
    const char* getEffectName(unsigned char effect);
    int init(DivEngine* parent, int channels, int sugRate, unsigned int flags);
    void quit();
    ~DivPlatformX1_010();
};

#endif
