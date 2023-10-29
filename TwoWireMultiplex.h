#ifndef __TwoWireMultiplex__
#define __TwoWireMultiplex__

#include <Arduino.h>

typedef enum {
  I2C_BUS_CHANNEL_1 = 0,
  I2C_BUS_CHANNEL_2 = 1,
  I2C_BUS_CHANNEL_3 = 2,
  I2C_BUS_CHANNEL_4 = 3,
  I2C_BUS_CHANNEL_5 = 4,
  I2C_BUS_CHANNEL_6 = 5,
  I2C_BUS_CHANNEL_7 = 6,
  I2C_BUS_CHANNEL_8 = 7, // up to channel_8 (largest mux/switch)
  I2C_BUS_CHANNEL_ALL  = 14,
  I2C_BUS_CHANNEL_NONE = 15
} e_i2c_bus_channel;

#define I2C_BUS_CHANNEL_MAX 8

typedef void (*irqCallBackFunc)();
class TwoWireMultiplex;

class TwoWireMultiplex {
public:  
  TwoWireMultiplex(uint8_t maxChannels, uint8_t _addr)
    : nb_channels(maxChannels), addr(_addr), channel(I2C_BUS_CHANNEL_NONE)
    {
      childs= new TwoWireMultiplex * [maxChannels];
      for(byte k=0; k<maxChannels; k++) childs[k]=NULL; // initialize all childs to NULL, waiting for addNestDevice to assign one
    }  
    
  void printTo(Print& p);                 // print useful information (selected channel, nb channels, i2c mux addr)
  
  bool begin(uint32_t clock=400000uL);    // call Wire.begin() and check presence of multiplex device
  void addNestDevice(TwoWireMultiplex *mux, uint32_t channel);   // channel give the path to add new mux
  bool selectChannel(uint32_t chan, bool force=false);  
  
protected:
  union {
    uint8_t channel;
    e_i2c_bus_channel echannel;
  };
  const uint8_t nb_channels;
  const uint8_t addr;

  TwoWireMultiplex **childs; // array of pointers to muxdevices (one for each channel)

  // virtual method to be defined by derived class according to I2C mux device
  // allow to set/get the current channel of multiplexer (current NODE)
  // when no multiplexer is implemented (nb_channels=1), mux is bypassed and normal wire operation occur
  // when force boolean is true, device is always read/write, otherwise cached data is used    
  // these function are not intended to be used by class users, use multinode methods instead such as selectChannel()
  virtual bool setChannel(uint8_t subchan, bool force=false) =0;  // select the given channel for this level (called by selectChannel at each nesting level)
  virtual uint8_t getChannel(bool force=false)=0;
  virtual uint8_t getChannelIRQ() {return I2C_BUS_CHANNEL_NONE;};

  // support method for subchannel
  uint8_t validateChannel(uint32_t chan);  // check chan parameter and return chan if in proper range, otherwise return I2C_BUS_CHANNEL_NONE  
  
};

#define PCA9543A_ADDR(N) (0x70 + (N&3) )
#define PCA9543B_ADDR(N) (0x78 + (N&3) )

class TwoWireMultiplexPCA9543: public TwoWireMultiplex {
public:  
  TwoWireMultiplexPCA9543(uint8_t offset=0): TwoWireMultiplex(2,PCA9543A_ADDR(offset)) {}
  
  uint8_t getChannel(bool force=false) override;
  uint8_t getChannelIRQ() override;

protected:  
  bool setChannel(uint8_t subchan, bool force=false) override;
};

#if USE_TWOWIREMULTIPLEX_INTERFACE
class TwoWireMultiplexInterface {
public:
  TwoWireMultiplexInterface() {mux_drv=NULL; mux_channel=0;}
  void attachTwoWireMultiplex(TwoWireMultiplex *mux, uint32_t channel) {mux_drv=mux; mux_channel=channel;}
  TwoWireMultiplex * getAttachedTwoWireMultiplex() {return mux_drv;}
  uint32_t getAttachedTwoWireMultiplexChannel() {return mux_channel;}

  bool selectBus(bool verbose=false) {
    if (mux_drv) {
      if(verbose) {
        Serial.print("~~~ Query to select channel 0x"); Serial.println(mux_channel,HEX); 
      }
      return mux_drv->selectChannel(mux_channel,true); 
    }
    return true;
  }
protected:  
 	TwoWireMultiplex *mux_drv;
  uint32_t mux_channel; 
};
#endif

#endif
