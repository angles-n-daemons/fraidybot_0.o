// demo of Starter kit V2.0 - Grove - Light Sensor
// when the value of light sensor ledd than a certain value
// led will on, you can set this certain value via change thresholdvalue
// such as when you cover light sensor with your hand, you'll find led on


const int pinLight = A0;
const int pinTemp = A1;
const int pinSound = A2;

void setup() {
  Serial.begin(9600);
}

void loop()
{
    Serial.println("Output:");
    int brightness = analogRead(pinLight);    //the light sensor is attached to analog 0
    int temperature = analogRead(pinTemp);
    int loudness = analogRead(pinSound);
    delay(1000);
    Serial.println(brightness);
    Serial.println(temperature);
    Serial.println(loudness);
}
