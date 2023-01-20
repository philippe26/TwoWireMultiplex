#include "TwoWireMultiplex.h"
#include "Wire.h"

void TwoWireMultiplex::printTo(Print& p) {
  p.print("channel=");
  p.print(channel);
  p.write("/");
  p.print(nb_channels);
  p.print(" (dev_addr=");
  p.print(addr);
  p.write(")");
}


bool TwoWireMultiplexPCA9543::selectChannel(uint8_t chan, bool force) 
{
  bool result=false;    
  chan=validateChannel(chan);
  if ((chan!=channel) || force) {
    byte data=0;
    if (chan<nb_channels) {
      data=1<<chan;      
    } else if (chan==I2C_BUS_CHANNEL_ALL) {
      data=0x3; // select all channels
    }    
    // channel not yet selected, instruct the device
    Wire.beginTransmission(addr);
    Wire.write(data);
    result=(Wire.endTransmission()==0)?true:false;     
  } 
  channel=chan;
  return result;
}

uint8_t TwoWireMultiplexPCA9543::getChannel(bool force)  
{  
  uint8_t data=0;
  if (force) {
    if (Wire.requestFrom(addr,1u) & 0x3) 
      data = Wire.read();
    switch(data){
      case 0: // no channel selected
        channel=I2C_BUS_CHANNEL_NONE;
        break;
      case 1:// first channel selected
        channel=I2C_BUS_CHANNEL_1;
        break;
      case 2: // second channel selected
        channel=I2C_BUS_CHANNEL_2;
        break;
      default: // both channels selected 
        channel=I2C_BUS_CHANNEL_ALL;
        break;
    }
  }
  return channel;
}