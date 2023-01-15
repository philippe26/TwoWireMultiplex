#ifndef __TwoWireMultiplex__
#define __TwoWireMultiplex__

#include <Arduino.h>


#define I2C_BUS_CHANNEL_1   0
#define I2C_BUS_CHANNEL_2   1
#define I2C_BUS_CHANNEL_3   2
#define I2C_BUS_CHANNEL_4   3
#define I2C_BUS_CHANNEL_ALL   254
#define I2C_BUS_CHANNEL_NONE  255

class TwoWireMultiplex {
public:  
  TwoWireMultiplex(uint8_t maxChannels, uint8_t _addr): nb_channels(maxChannels), addr(_addr) 
    {channel=I2C_BUS_CHANNEL_NONE;}  
    
  // virtual method to be defined by derived class according to I2C mux device
  // allow to set/get the current channel of multiplexer
  // when no multiplexer is implemented (nb_channels=1), mux is bypassed and normal wire operation occur
  // when force boolean is true, device is always read/write, otherwise cached data is used
  virtual bool selectChannel(uint8_t chan, bool force=false) =0;
  virtual uint8_t getChannel(bool force=false)=0;

  void printTo(Print& p);
  uint8_t validateChannel(uint8_t chan) {
    switch (chan) {
      case I2C_BUS_CHANNEL_NONE: 
      case I2C_BUS_CHANNEL_ALL: 
        return chan;
      default:
        if (chan<nb_channels) return chan;        
    }
    return I2C_BUS_CHANNEL_NONE;
  }
protected:
  uint8_t channel;
  const uint8_t nb_channels;
  const uint8_t addr;  
};

#define PCA9543A_ADDR(N) (0x70 + (N&3) )
#define PCA9543B_ADDR(N) (0x78 + (N&3) )

class TwoWireMultiplexPCA9543: public TwoWireMultiplex {
public:  
  TwoWireMultiplexPCA9543(uint8_t _addr=PCA9543A_ADDR(0)): TwoWireMultiplex(2,_addr) {}
  bool selectChannel(uint8_t chan, bool force=false) override;
  uint8_t getChannel(bool force=false) override;
};

#endif
