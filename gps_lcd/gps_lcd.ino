#include <Wire.h>
#include <LGPS.h>
#include "rgb_lcd.h"

gpsSentenceInfoStruct info;
char buff[256];

rgb_lcd lcd;

const int colorR = 128;
const int colorG = 128;
const int colorB = 128;

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0;
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static void readLatLng(const char *s, double *latlng) {
  char buf[10];
  
  int i = getComma(1, s);
  strncpy(buf, s, 2);
  buf[2] = 0;
  double val = atof(buf);
  strncpy(buf, s+2, i-2);
  buf[i-2] = 0;
  val += atof(buf)/60.0;
  val *= (double)(s[i] == 'S' ? -1 : 1);
  latlng[0] = val;

  int lngOffset = getComma(2, s);
  s = s+lngOffset;
  i = getComma(1, s);
  strncpy(buf, s, 3);
  buf[3] = 0;
  val = atof(buf);
  strncpy(buf, s+3, i-3);
  buf[i-3] = 0;
  val += atof(buf)/60.0;
  Serial.print(s[i+1]);
  val *= (double)(s[i] == 'W' ? -1 : 1);
  latlng[1] = val;
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    int latPos = tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff);

    double latlng[2];
    readLatLng(&GPGGAstr[latPos], latlng);
    lcd.setCursor(0,0);
    lcd.print("Lat: ");
    sprintf(buff, "%4.4f", latlng[0]);
    lcd.print(buff);
    lcd.setCursor(0,1);
    lcd.print("Lng: ");
    sprintf(buff, "%4.4f", latlng[1]);
    lcd.print(buff);
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff);

    tmp = getComma(9, GPGGAstr);
    double altitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "altitude = %f", altitude);
    Serial.println(buff);
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void setup() 
{
    // set up the LCD's number of columns and rows:
    Serial.begin(115200);
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    
    LGPS.powerOn();
    lcd.print("LGPS Power on"); 
    delay(3000);
    lcd.clear();
}

void loop() 
{
    lcd.setCursor(0, 1);
    LGPS.getData(&info);
    Serial.print((const char*)info.GPGGA);
    parseGPGGA((const char*)info.GPGGA);

    delay(2000);
}
