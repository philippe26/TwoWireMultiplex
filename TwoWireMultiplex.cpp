#include "TwoWireMultiplex.h"
#include "Wire.h"


// *************************************************************************************************************
//       TwoWireMultiplex
// *************************************************************************************************************

void TwoWireMultiplex::printTo(Print& p) 
{
  p.print("Selected channel=");
  switch (channel)
  {
    case I2C_BUS_CHANNEL_NONE: p.print("NONE"); break;
    case I2C_BUS_CHANNEL_ALL: p.print("ALL"); break;      
    default:p.print(channel); break;      
  }  
  p.write(", nb="); p.print(nb_channels);
  p.print(" (dev_addr=0x"); p.print(addr,HEX); p.write(")");
  for(byte k=0; k<nb_channels; k++) {
    p.print(", Child"); p.print(k); p.print("="); 
    if (childs[k]) p.print("assigned"); else p.print("NUL");       
  }
  p.println();
  for(byte k=0; k<nb_channels; k++) {
     if (childs[k]) {
      p.print("Child");p.print(k); p.print("=> "); 
      childs[k]->printTo(p); 
      
     }
  }
}

uint8_t TwoWireMultiplex::validateChannel(uint32_t chan) 
{
  uint8_t subchan = chan & 0xF; // take 4 LSB only
  switch (subchan) {
    case I2C_BUS_CHANNEL_NONE: 
    case I2C_BUS_CHANNEL_ALL: 
      return subchan;
    default:
      if (subchan<nb_channels) return subchan;        
  }
  return I2C_BUS_CHANNEL_NONE;
}


// call setChannel() for each nested level (see next pointer)
bool TwoWireMultiplex::selectChannel(uint32_t chan, bool force) 
{
  bool result=false;
  uint8_t subChan=validateChannel(chan);
  TwoWireMultiplex *nestMux=NULL; 
  if(subChan<nb_channels)
    nestMux= childs[subChan];
  
  result=setChannel(subChan, force); // set current NODE
    
  if(nestMux) {      
    // if nested node, select it
    result = result && nestMux->selectChannel(chan>>4, force);
  }
  return result;
}

bool TwoWireMultiplex::begin(uint32_t clock) 
{
  if (nb_channels>I2C_BUS_CHANNEL_MAX) // erreur de constructeur, ne devrait jamais arriver
    return false;
  Wire.begin();
  Wire.setClock(clock);
  return selectChannel(0, true);  
}

// Add the mux device and return the nesting level
void TwoWireMultiplex::addNestDevice(TwoWireMultiplex *mux, uint32_t channel) 
{
  uint8_t subChan=validateChannel(channel);  
  if(subChan<nb_channels) {
    if (childs[subChan] && (validateChannel(channel>>4)!=I2C_BUS_CHANNEL_NONE))
      childs[subChan]->addNestDevice(mux, channel>>4);
    else
      childs[subChan]=mux;
  }    
}


// *************************************************************************************************************
//       TwoWireMultiplexPCA9543
// *************************************************************************************************************
bool TwoWireMultiplexPCA9543::setChannel(uint8_t subchan, bool force) 
{
  bool result=false;    
  if ((subchan!=channel) || force) {
    byte data=0;
    if (subchan<nb_channels) {
      data=1<<subchan;      
    } else if (subchan==(uint8_t)I2C_BUS_CHANNEL_ALL) {
      data=0x3; // select all channels
    }    
    //Serial.print("TwoWireMultiplexPCA9543.setChannel: addr=0x"); Serial.print(addr,HEX); Serial.print(", data=0x"); Serial.println(data,HEX); 
    // channel not yet selected, instruct the device
    Wire.beginTransmission(addr);
    Wire.write(data);
    result=(Wire.endTransmission()==0)?true:false;     
  } 
  channel=subchan;
  return result;
}
uint8_t TwoWireMultiplexPCA9543::getChannelIRQ()
{
  uint8_t data=0;
  if (Wire.requestFrom(addr,1u)) {
    data = Wire.read();
    data >>= 4;   // use bits 4-7
    data &= 0x3;  // mask 2 bits
  }
  switch(data){
    case 0: // no channel selected
      return  I2C_BUS_CHANNEL_NONE;
    case 1:// first channel selected
      return  I2C_BUS_CHANNEL_1;
    case 2: // second channel selected
      return  I2C_BUS_CHANNEL_2;      
    default: // both channels selected 
      return  I2C_BUS_CHANNEL_ALL;   
  }  
}
uint8_t TwoWireMultiplexPCA9543::getChannel(bool force)  
{  
  uint8_t data=0;
  if (force) {
    if (Wire.requestFrom(addr,1u)) 
      data = Wire.read() & 0x3;
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

