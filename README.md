# TwoWireMultiplex
Manage the I2C bus multiplexing through multiplex devices (TCA9543, TCA9448, etc)

## Usage
Create an instance of the class TwoWireMultiplex and attach it to all classes that use Wire library

For instance, pinDriver library

```C++
  TwoWireMultiplexPCA9543 busMux; // using a TCA9543 device for multiplexing 2 I2C bus
  pinDriverPCA9555 pinDrv;

  void setup() {
    Wire.begin();
    busMux.selectChannel(0, true);  // initialize/configure the device
    ...
    pinDrv.attachTwoWireMultiplex(&busMux, 0); // Attaching the mux and assign pinDrv to channel 0 (first bus of TCA9543)
  }
```

## Instrumentation of code to work with TwoWireMultiplex
Note: **All libraries based upon Manageable class are compatible with TwoWireMultiplex**

Exemple of base class using Wire

```C++
class BASE {
  public:
  void attachTwoWireMultiplex(TwoWireMultiplex *mux, uint8_t channel) {mux_drv=mux; mux_channel=channel;}
  inline bool selectBus() {if (mux_drv) return mux_drv->selectChannel(mux_channel); }
  private:
  TwoWireMultiplex *mux_drv;
  uint8_t mux_channel;
  const uint8_t i2caddr;
}

class A: public BASE {
  void func_write_reg() {
    if (selectBus()) {
      Wire.beginTransmission(i2caddr);
      Wire.write(2);
      Wire.write(17);
      Wire.endTransmission();
    }
  }
}


```
