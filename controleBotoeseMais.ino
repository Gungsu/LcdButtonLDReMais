#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "ESP32TimerInterrupt.h"

LiquidCrystal_I2C lcd(0x27,21,22);

#define B1 32
#define B2 33
#define B3 27
#define B4 14
#define ANPOTENCIOMETRO 25
#define LDR 26
#define LD 19

uint64_t timeB_hold_cont = 0;
uint16_t analogVal_P,analogVal_L, cont, anologSum_P,anologSum_L;
uint16_t anP_map, anL_map;
uint16_t anP_map_old, anL_map_old;

String ldrMsg[] = {"Escuro", "Medio ", "Claro "};

#define TIMER0_INTERVAL_MS        1000
ESP32Timer ITimer0(0);
bool blink, stopAn, newDisp0, newDisp1;

bool IRAM_ATTR TimerHandler0(void * timerNo)
{
  anologSum_L += analogRead(LDR);
  anologSum_P += analogRead(ANPOTENCIOMETRO);
  cont++;
  if (cont >= 10) {
    blink = !blink;
    analogVal_L = anologSum_L/10;
    anologSum_L = 0;
    analogVal_P = anologSum_P/10;
    anologSum_P = 0;
    cont = 0;
  }
	return true;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  pinMode(B4, INPUT_PULLUP);
  pinMode(LD, OUTPUT);
  analogRead(LDR);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Cmds de botoes!");
  lcd.setCursor(2,1);
  lcd.print("by Amauri!");
  newDisp0 = true;
  newDisp1 = true;
  ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 10, TimerHandler0);

  Serial.begin(9600);
  delay(2000);
  lcd.clear();
}

int readButtons() {
  uint16_t resultValue = 0;
  if (!digitalRead(B1)) {
    resultValue += 1;
  }
  if (!digitalRead(B2)) {
    resultValue += 2;
  }
  if (!digitalRead(B3)) {
    resultValue += 4;
  }
  if (!digitalRead(B4)) {
    resultValue += 8;
  }
  return resultValue;
}

void acoesAnalog(){
  if (stopAn) return;
  anP_map = map(analogVal_P,0,4095,0,3300);
  anL_map = map(analogVal_L,0,1310,0,2);
  if (anP_map != anP_map_old || anL_map != anL_map_old) {
    newDisp1 = true;
    anL_map_old = anL_map;
    anP_map_old = anP_map;
  }
  if (anL_map == 0)
    digitalWrite(LD, 1);
  else
    digitalWrite(LD, 0);
}

void display(int val){
  if (val == 0) {
    if (newDisp0) {
      lcd.setCursor(0,0);
      lcd.print("Analogic:");
      newDisp0 = false;
    }
    if (newDisp1) {
      lcd.setCursor(0,1);
      lcd.print(ldrMsg[anL_map]);
      lcd.setCursor(12,1);
      lcd.print("    ");
      lcd.setCursor(12,1);
      lcd.print(anP_map);
      newDisp1 = false;
    }
  } else {
    newDisp0 = true;
    lcd.clear();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (readButtons())
  {
    case 0: //Nenhum botão
      stopAn = false;
      timeB_hold_cont = 0;
      display(readButtons());
      acoesAnalog();
      break;
    case 1:  //B1
      display(readButtons());
      lcd.setCursor(0,0);
      lcd.print("B1");
      lcd.setCursor(0,1);
      lcd.print(timeB_hold_cont);
      timeB_hold_cont++;
      break;
    case 2: //B2
      display(readButtons());
      lcd.setCursor(0,0);
      lcd.print("B2");
      lcd.setCursor(0,1);
      lcd.print(timeB_hold_cont);
      timeB_hold_cont++;
      break;
    case 4: //B3
      display(readButtons());
      lcd.setCursor(0,0);
      lcd.print("B3");
      lcd.setCursor(0,1);
      lcd.print(timeB_hold_cont);
      timeB_hold_cont++;
      break;
    case 8: //B4
      display(readButtons());
      lcd.setCursor(0,0);
      lcd.print("B4");
      lcd.setCursor(0,1);
      lcd.print(timeB_hold_cont);
      timeB_hold_cont++;
      break;
    case 9: //B4 + B1
      display(readButtons());
      stopAn = true;
      lcd.setCursor(0,0);
      lcd.print("B1 + B4");
      digitalWrite(LD, blink);
      break;
    default:
      break;
  }
  delay(10);
}