/*TSL201R_Scanning.ino Arduining 26 MAR 2014

Linear Array Sensor TSL201R (TAOS) is now AMS:
  -64 x 1 Linear Sensor Array 200 DPI

-Sketch used with the Arduino NANO.
  Data is Send at 115200 bauds (ASCII messages ending with CR and LF)
  One frame (64x1 image) has the followinf format:
      >5890    Indicates the integration period in microseconds.
      843      this value is discarded.
      234      value of the pixel 1
      242      value of the pixel 2
       .         .
       .         .
      245      value of the pixel 64

-Reading only one pixel in each scan.
-The image is completed after 1+64 scans.(first is discarded)
-First scan is repeated to permit a complete integration period). 
-Pixel integration Period is adjustable (potentiometer in analog channel 1).

*/

#define CLK     A7
#define SI      A9
#define VOUT    A3   //pixel intensity in the analog channel 0
#define INTVAL  A2   //integration time adjust in the analog channel 1.
#define PIXELS  128

int intDelay;      //Integration Period = (intDelay + 535 ) microseconds.
int Value;         //pixel intensity value.

void setup(){
  pinMode(CLK, OUTPUT);
  pinMode(SI, OUTPUT);
  digitalWrite(CLK, LOW);
  digitalWrite(SI, LOW);

//  Serial.begin(9600);
  Serial.begin(9600);
  Serial.flush();
}



void loop(){
  intDelay=1+analogRead(INTVAL)*10;  //read integration time from potentiometer.
  Serial.print("-------------------------------------------------------------------------------------------------------------");                //Mark the start of a new frame
  //Serial.println(intDelay + 535);   //Send Integration Period in microseconds.
  delay(8);     // used with 9600 bauds
  //delay(2);     // used with 115200 bauds

  readPixel(0);                     //the first reading will be discarded.
  delayMicroseconds(intDelay);

  for(int i=0;i<PIXELS;i++){
    readPixel(i);
    delayMicroseconds(intDelay);    //Delay added to the integration period.
  }
}

//------------------ Send the intensity of the pixel serially -----------------
void readPixel(int pixel){
  digitalWrite(CLK, LOW);
  digitalWrite(SI, HIGH);         
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
 

  for(int i=0;i<pixel; i++){    //Clock pulses before pixel reading
    digitalWrite(CLK, LOW);
    digitalWrite(CLK, HIGH);    //Select next pixel and reset integrator of actual pixel.
  }

  Value = analogRead(VOUT); 
  Serial.println(Value);
  delay(8);       //used with 9600 bauds
//  delay(1);         //used with 115200 bauds
  for(int i=0;i<=(PIXELS-pixel); i++){    //Clock pulses after pixel reading.
    digitalWrite(CLK, LOW);
    digitalWrite(CLK, HIGH);    //Select next pixel and reset integrator of actual pixel.
  }
}
