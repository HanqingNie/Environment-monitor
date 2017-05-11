#include<LiquidCrystal.h>

//state value
const int ST=0;
const int TH=1;
const int TN=2;
const int TL=3;
const int LH=4;
const int LN=5;
const int LL=6;

//pin value
const int tmp_pin=0;
const int light_pin=1;
const int led_red=12;
const int led_green=4;
const int piezo=10;
const int x=2;
const int y=3;
const int redPin=11;
const int greenPin=12;
const int bluePin=13;

//measured voltage
float voltage_t;
float voltage_l;

//full color led value
int red;
int green;
int blue;

//temperature and light
float tmp;
float light;

//joystick value
float xx;
float yy;
int ssww;

//confortable value
float tmp_c=28;
float light_c=450;

//lcd pin
LiquidCrystal lcd(9,8,7,6,5,4);

//interrupt
const byte setCurrent=2;
const byte setByHand=3;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  pinMode(tmp_pin,INPUT);
  pinMode(light_pin,INPUT);
  pinMode(piezo,OUTPUT);
  pinMode(x,INPUT);
  pinMode(y,INPUT);
  pinMode(A5,INPUT);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);
  //interrupt
  attachInterrupt(digitalPinToInterrupt(setCurrent),current,RISING);
  attachInterrupt(digitalPinToInterrupt(setByHand),byHand,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  //measure voltages
  voltage_t=(analogRead(tmp_pin)*0.004882814);
  voltage_l=analogRead(light_pin);

  //read joystick value
  xx=analogRead(x);
  yy=analogRead(y);
  ssww=analogRead(A5);
  
  //calculate temperature in degree
  tmp=(voltage_t-0.5)*100;
  light=voltage_l;

  //determine the color of the led
  red=255*(tmp-tmp_c+5)/10;
  blue=255-red;
  green=255-abs(2*red-255);

  //light up led
  analogWrite(redPin,red);
  analogWrite(greenPin,green);
  analogWrite(bluePin,blue);
  
  Serial.print("T:");
  Serial.print(tmp);
  Serial.print("     L:");
  Serial.print(light);
  Serial.print("     T_c:");
  Serial.print(tmp_c);
  Serial.print("     L_c:");
  Serial.print(light_c);
  Serial.print("    sw:");
  Serial.println(ssww);
  
  static int state=ST;
  switch(state)
  {
    case ST:
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tmp:");
      lcd.print(tmp);
      lcd.setCursor(0,1);
      lcd.print("Light:");
      lcd.print(light);

      delay(1000);
      if(tmp-tmp_c>5)
      {
        state=TH;
      }
      else if(tmp_c-tmp>5)
      {
        state=TL;
      }
      else
      {
        state=TN;
      }
      break;
    }
    
    case TH:
    {
      tmp_warn();
      if(light-light_c>100)
      {
        light_warn();
        state=LH;
      }
      else if(light_c-light>100)
      {
        light_warn;
        state=LL;
      }
      else
      {
        state=LN;
      }
      break;
    }

    case TN:
    {
      if(light-light_c>100)
      {
        state=LH;
      }
      else if(light_c-light>100)
      {
        state=LL;
      }
      else
      {
        state=LN;
      }
      break;
    }

    case TL:
    {
      tmp_warn();
      if(light-light_c>100)
      {
        state=LH;
      }
      else if(light_c-light>100)
      {
        state=LL;
      }
      else
      {
        state=LN;
      }
      break;
    }

    case LH:
    {
      light_warn();
      state=ST;
      break;
    }
    case LN:
    {
      state=ST;
      break;
    }
    case LL:
    {
      light_warn();
      state=ST;
    }
    break;
  }
}

void tmp_warn()
{
  Serial.println("Tmp warning!!!");
  tone(piezo,1000);
  delay(1000);
  noTone(piezo);
  delay(1000);
}

void light_warn()
{
  Serial.println("Light warning!!!");
  tone(piezo,1000);
  delay(1000);
  noTone(piezo);
  delay(1000);
}

void current()
{
  tmp_c=tmp;
  light_c=light;
}

void byHand()
{
  
  while(ssww>15)
  {
    ssww=analogRead(A5);
    xx=analogRead(x);
    yy=analogRead(y);
    Serial.print("     T_c:");
  Serial.print(tmp_c);
  Serial.print("     L_c:");
  Serial.print(light_c);
  Serial.print("    sw:");
  Serial.println(ssww);
    if(xx>700)
    {
      tmp_c=tmp_c+0.1;  
    }
    else if(xx<200)
    {
      tmp_c=tmp_c-0.1;
    }
    if(yy>700)
    {
      light_c=light_c+10;
    }
    else if(yy<200)
    {
      light_c=light_c-10;
    }
  }
}

