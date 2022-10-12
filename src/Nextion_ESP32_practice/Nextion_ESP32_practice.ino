#include "Nextion.h"

NexPage p0 = NexPage(0,0,"page0");
NexPage p1 = NexPage(1,0,"page1");
// Page 0 values below here
NexText CurrTempVal = NexText(0,7,"tCurrTempVal");
NexText HighTempVal = NexText(0,8,"tHighTempVal");
NexText LowTempVal = NexText(0,9,"tLowTempVal");
NexText MotSenVal = NexText(0,10,"tMotSenVal");
NexText LightSenVal = NexText(0,11,"tLightSenVal");
NexNumber MyIntCount = NexNumber(0,12,"nMyIntCount");
NexNumber ADC_an_val = NexNumber(0,15,"nADCanval");
NexNumber ADC_v_val = NexNumber(0,17,"nADCvval");

// Page 1 values below here
NexText Terminal = NexText(1,3,"tTerminal");
NexTouch *nex_listen_list[] = {NULL};

uint32_t next, myInt = 0;

void do_every_so_often() {
  if (myInt == 10 or myInt == 30 or myInt == 50 or myInt == 70 or myInt == 90) {
    p1.show();
  } else if (myInt == 0 or myInt == 20 or myInt == 40 or myInt == 60 or myInt == 80) {
    p0.show();
  } else {}
  
  myInt = (myInt + 1) % 101;
  MyIntCount.setValue(myInt);
}

void setup() {
  // put your setup code here, to run once:
  nexInit();
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  digitalWrite(26, LOW);
  digitalWrite(27, LOW);
  analogReadResolution(12);

  CurrTempVal.setText("1");
  next = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  nexLoop(nex_listen_list); // what does this actually do if the nex_listen_list is empty
  if (millis() >= next) {
    next = millis()+500;
    do_every_so_often();
  }
  CurrTempVal.setText("1");
  
  int pot_analog_val = analogRead(33);
  int pot_analog_volts = analogReadMilliVolts(33);
  ADC_an_val.setValue(pot_analog_val);
  ADC_v_val.setValue(pot_analog_volts);
  Terminal.setText("The FitnessGram Pacer Test is a multistage \r\naerobic capacity test that aggressively gets \r\nmore difficult as it continues. \r\nThe 20 meter pacer test will begin \r\nin 30 seconds. Line up at the start. \r\nThe running speed starts slowly, \r\nbut gets faster each minute \r\nafter you hear this signal. \r\n[beep]. A single lap should \r\nbe completed each time you \r\nhear this sound. [ding]. \r\nRemember to run in a \r\nstraight line, and run as \r\nlong as possible. The \r\nsecond time you fail to \r\ncomplete a lap before the sound, \r\nyour test is over. The \r\ntest will begin on the word \r\nstart. On your mark, get \r\nready, start.");
  delay(100);
}
