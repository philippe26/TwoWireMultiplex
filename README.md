# TwoWireMultiplex
Manage the I2C bus multiplexing through multiplex devices (TCA9543, TCA9448, etc)

## Usage
Create an instance of the class TwoWireMultiplex and attach it to all classes that use Wire library

For instance, pinDriver library

TwoWireMultiplexPCA9543 busMux; // using a TCA9543 device for multiplexing 2 I2C bus
pinDriverPCA9555 pinDrv;

void setup() {
  Wire.begin();
  busMux.selectChannel(0, true);  // initialize/configure the device
  ...
  pinDrv.attachTwoWireMultiplex(&busMux, 0); // Attaching the mux and assign pinDrv to channel 0 (first bus of TCA9543)
}