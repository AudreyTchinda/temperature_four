#include <EEPROM.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7,6}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
#define relais 9
#define buz 10
#define B A1
#define V A2
#define R A3
int temp_min;
byte customChar[] = {
  B11000,
  B11000,
  B00000,
  B00111,
  B01000,
  B01000,
  B01000,
  B00111
};
byte customChar1[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte customChar2[] = {
  B01000,
  B01100,
  B01110,
  B11111,
  B11111,
  B01110,
  B01100,
  B01000
};
byte customChar3[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
};
int temperature;
unsigned long temps;
boolean scroll;
byte scroll1;
char customKey;
int menu;
byte suiv;
int temperature_ent[4] = {0, 0, 0, 0};
int temps_ent[4] = {0, 0, 0, 0};
int nombre;
int i, j, temps_fin, temper_fin;
int save_i, save_j;
long long temps_convert;
unsigned long temps2;
unsigned long t1;
void setup() {
    pinMode(relais,OUTPUT);
   pinMode(buz,OUTPUT);
    pinMode(R,OUTPUT);
     pinMode(V,OUTPUT);
      pinMode(B,OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  // lcd.home();
  // lcd.write(0);
  lcd.setCursor(6, 0);
  lcd.print("Hello");
  digitalWrite(buz,1);
  delay(2000);
   digitalWrite(buz,0);
  lcd.clear();
}

void loop() {
  //read_temperature();
  key_pad();
  lcd.clear();
  while (menu == 0) {
    key_pad();
    if (millis() > (temps + 1000)) {
      lcd.clear(); temps = millis(); scroll = !scroll;
      lcd.setCursor(3, scroll);
      if (scroll)lcd.print("APPUYEZ SUR #");
      else lcd.print("PRESS # ");
    }
    digitalWrite(B,1);
    digitalWrite(R,0);
    digitalWrite(V,0);
  }
  while (menu == 1) {
    lcd.setCursor(0, 0);
    lcd.print("ENTREZ LE TEMPS");
    lcd.setCursor(0, 1);
    lcd.print("& LA TEMPERATURE");
    if (millis() > (temps + 1000)) {
      lcd.clear(); temps = millis(); delay(200); suiv += 1;
    }
    if (suiv == 3) menu = 2;
  }
  lcd.clear();
  while (menu == 2) {
     digitalWrite(B,0);
    digitalWrite(R,1);
    digitalWrite(V,0);
    key_pad();
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE");
    lcd.setCursor(11, scroll);
    lcd.write(2);
    lcd.setCursor(12, 0);
    if ((i == 0) and save_i == 0) {
      lcd.write(3);
      lcd.write(3);
      lcd.write(3);
    }
    else {
      lcd.setCursor(12, 0); lcd.print(temper_fin);
    }
    lcd.setCursor(15, 0);
    lcd.write(0);
    lcd.setCursor(0, 1);
    lcd.print("TEMPS(Min) ");
    lcd.setCursor(15, 1);
    lcd.write(1);
    if ((j == 0) and (save_j == 0)) {
      lcd.setCursor(12, 1);
      lcd.write(3);
      lcd.write(3);
      lcd.write(3);
    }
    else {
      lcd.setCursor(12, 1);
      lcd.print(temps_fin);
    }
  }
  while (menu == 3) {
     digitalWrite(B,0);
    digitalWrite(R,0);
    digitalWrite(V,1);
    
    temps = millis();
    lcd.clear();
    while (millis() < (temps + 3000)) {
      com();
      lcd.setCursor(0, 0);
      lcd.print("T demand :");
      lcd.print(temper_fin);

      lcd.write(0);
      lcd.setCursor(0, 1);
      lcd.print("T actuel :");
      lcd.print(temperature);
      lcd.write(0);
    }
    temps = millis();
    lcd.clear();
    while (millis() < (temps + 3000)) {
      com();
      lcd.setCursor(0, 0);
      lcd.print("t demand :");
      lcd.print(temps_fin);
      lcd.print("Min");
      lcd.setCursor(0, 1);
      lcd.print("t resten :");
      lcd.print(temp_min);
      lcd.print("Min");
    }
  }



}
void key_pad() {
  customKey = customKeypad.getKey();
  Serial.println(customKey);
  if (customKey) {
    //Serial.println(customKey);
    convert_char();
    if (customKey == '#') {
      menu += 1;
      lcd.clear();
      if (menu >= 3) {
        menu = 2; scroll = !scroll;
        if (j != 0)save_j = j;
        if (i != 0)save_i = i;

      }
    }
    if (customKey == '*') {
      menu = 3;
      delay(1000);
      j = 0; save_i = 0;
      i = 0; save_j = 0;
      EEPROM.update(0,menu);
      EEPROM.update(1,temper_fin);
      temp_min = temps_fin;
      EEPROM.update(2,temps_fin);
      EEPROM.update(3,temp_min);
      temps2=millis();
      for (int p ; p < 4; p++) {
        temperature_ent[p] = 0;
        temps_ent[p] = 0;
      }
      //Serial.println(menu);
    }
  }
}
void convert_char() {

  if ((customKey != '*') and (customKey != '#') and (menu == 2)) {
    if (customKey == '0') nombre = 0;
    if (customKey == '1') nombre = 1;
    if (customKey == '2') nombre = 2;
    if (customKey == '3') nombre = 3;
    if (customKey == '4') nombre = 4;
    if (customKey == '5') nombre = 5;
    if (customKey == '6') nombre = 6;
    if (customKey == '7') nombre = 7;
    if (customKey == '8') nombre = 8;
    if (customKey == '9') nombre = 9;
    if (scroll == 0) {
      lcd.clear();
      lcd.setCursor(12, 0);
      j = 0;
      if (i == 3)for (int p ; p < 4; p++) {
          temperature_ent[p] = 0;
          i = 0;
        }
      temperature_ent[i] = nombre;
      if (i == 1) {
        temper_fin = (temperature_ent[0] * 10) + temperature_ent[1];
        lcd.print(temper_fin);
        lcd.write(3);
      }
      else if (i == 2) {
        temper_fin = (temperature_ent[0] * 100) + (temperature_ent[1] * 10) + temperature_ent[2];
        lcd.print(temper_fin);
      }
      else {
        temper_fin = temperature_ent[0];
        lcd.print(temper_fin);
        lcd.write(3);
        lcd.write(3);

      }
      Serial.println(temper_fin);
      i += 1;

    }
    if (scroll == 1) {
      i = 0;
      lcd.clear();
      lcd.setCursor(12, 1);
      if (j == 3)for (int p ; p < 4; p++) {
          temps_ent[p] = 0;
          j = 0;
        }
      temps_ent[j] = nombre;
      if (j == 1) {
        temps_fin = (temps_ent[0] * 10) + temps_ent[1];
        lcd.print(temps_fin);;
        lcd.write(3);
      }
      else if (j == 2) {
        temps_fin = (temps_ent[0] * 100) + (temps_ent[1] * 10) + temps_ent[2];
        lcd.print(temps_fin);
      }
      else   {
        temps_fin = temps_ent[0];
        lcd.print(temps_fin);
        lcd.write(3);
        lcd.write(3);
      }
      Serial.println(temps_fin);
      j += 1;
    }
  }
}
void com() {

  if (temperature >= temper_fin) {
    digitalWrite(relais, 0);
    if (millis() > (temps2 + 60000)) {
      temps2 = millis();
      t1 +=1;
      temps_convert =(temps_fin -t1);
      Serial.println(t1);
      temp_min = temps_convert;
      if(t1%2==0){EEPROM.update(3,temp_min); EEPROM.update(4,t1);}
    }
  }
  else digitalWrite(relais, 1);
  customKey = customKeypad.getKey();
  if(temp_min<=0){
    digitalWrite(relais, 0);
    digitalWrite(buz,1);
     digitalWrite(B,0);
    digitalWrite(R,0);
    digitalWrite(V,1);
    delay(1000);
    digitalWrite(V,0);
    delay(1000);
    digitalWrite(V,1);
    delay(1000);
    digitalWrite(V,0);
    delay(1000);
    digitalWrite(V,1);
    delay(1000);
    digitalWrite(V,0);
    digitalWrite(buz,0);
    temps_fin=0;
    temper_fin=0;
    temp_min=0;
    menu=0;
    t1=0;
     EEPROM.update(0,menu);
      EEPROM.update(1,temper_fin);
      EEPROM.update(2,temps_fin);
      EEPROM.update(3,temp_min);
      EEPROM.update(4,t1);
    
    
    
    }
   if(customKey=='*'){
     temps_fin=0;
    temper_fin=0;
    temp_min=0;
    menu=0;
    t1=0;
     EEPROM.update(0,menu);
      EEPROM.update(1,temper_fin);
      EEPROM.update(2,temps_fin);
      EEPROM.update(3,temp_min);
      EEPROM.update(4,t1);
    } 
}
