#include "Arduino.h"
#include "TwoWireMultiplex.h"

TwoWireMultiplexPCA9543 dev1;
TwoWireMultiplexPCA9543 dev1_1(1);
TwoWireMultiplexPCA9543 dev1_2(1);
TwoWireMultiplexPCA9543 dev2_2(2);
TwoWireMultiplexPCA9543 dev3_1(3);

/*
 ROOT
  dev1
    |- dev1_1     0xfff0
      |- null
      |- null    
    |- dev1_2     0xfff1 
      |- null
      |- dev2_2   0xff11
        |- dev3_1 0xf011
          |- null
          |- null            
        |- null
*/

TwoWireMultiplex *root;

void setup()
{
  bool res=false;
  Serial.begin(115200);
  Serial.println("Building Multi node multiplexer");
  root = &dev1; 
  root->addNestDevice(&dev1_1,0xf0);
  root->addNestDevice(&dev1_2,0xf1);
  root->addNestDevice(&dev2_2,0xf11);
  root->addNestDevice(&dev3_1,0xf011);

  Serial.print("Show tree from Root: "); root->printTo(Serial);Serial.println();

  res= root->begin();

  Serial.print("Initialised: "); Serial.println((res)?"OK":"FAIL");

  Serial.print("Show tree after begin: "); root->printTo(Serial);Serial.println();
}

void loop()
{
  delay(1000);
}