#include "Nextion.h"

// Declare the Nextion objects for communicating with the Nextion Display.
// all of them follow the same hierarchy in these declarations.
// Format is Nex<obj> <variable_name> = Nex<obj>(pid,cid,name)
// pid,cid,name all must match values from the Nextion Editor application that
// formats the GUI on the Nextion Display
// pid -> the page the object is on (int)
// cid -> "id" value of the object seen in attributes (int)
// name -> "objname" value of the object seen in attributes (string)

// Pages here
NexPage p0 = NexPage(0,0,"page0"); // Page with sensor values
NexPage p1 = NexPage(1,0,"page1"); // Page with big text field for printing (news articles?)
NexPage p2 = NexPage(2,0,"page2"); // Page with Connecting to Wifi screen

// Page 0 objects below here
NexText CurrTempVal = NexText(0,7,"tCurrTempVal");
NexText HighTempVal = NexText(0,8,"tHighTempVal");
NexText LowTempVal = NexText(0,9,"tLowTempVal");
NexText MotSenVal = NexText(0,10,"tMotSenVal");
NexText LightSenVal = NexText(0,11,"tLightSenVal");
NexText WeatherVal = NexText(0,13,"tWeathVal");
NexText TempSenVal = NexText(0,15,"tTempSenVal");
// Page 1 objects below here
NexText Terminal = NexText(1,3,"tTerminal");
NexText Terminal_label = NexText(1,4,"tTermLab");
// Page 2 objects below here
NexText DebugPage = NexText(2,2,"tDebug");


NexTouch *nex_listen_list[] = {NULL};
uint32_t next, myInt = 0;

void setup() {
  // put your setup code here, to run once:
  nexInit();
  analogReadResolution(12);
}

void loop() {
  // put your main code here, to run repeatedly:
  nexLoop(nex_listen_list); // what does this actually do if the nex_listen_list is empty, should be nothing

  // Connected to 32 (Temp sensor), 33 (Light sensor), 34 (Motion sensor)
  int Light_Sensor_volts = analogReadMilliVolts(33); // could alternatively use   //int Light_Sensor_Analog = analogRead(XX);
  int Motion_Sensor_volts = analogReadMilliVolts(34); // could alternatively use //int Motion_Sensor_Analog = analogRead(XX);
  int Temp_Sensor_volts = analogReadMilliVolts(32); // could alternatively use //int Motion_Sensor_Analog = analogRead(XX);
  char Light_str[16];
  char Motion_str[16];
  char Temp_str[16];
  sprintf(Light_str, "%04d", Light_Sensor_volts);
  sprintf(Motion_str, "%04d", Motion_Sensor_volts);
  sprintf(Temp_str, "%04d", Temp_Sensor_volts);
  LightSenVal.setText(Light_str);
  MotSenVal.setText(Motion_str);
  TempSenVal.setText(Temp_str);
  
  Terminal.setText("The FitnessGram Pacer Test is a multistage \r\naerobic capacity test that aggressively gets \r\nmore difficult as it continues. \r\nThe 20 meter pacer test will begin \r\nin 30 seconds. Line up at the start. \r\nThe running speed starts slowly, \r\nbut gets faster each minute \r\nafter you hear this signal. \r\n[beep]. A single lap should \r\nbe completed each time you \r\nhear this sound. [ding]. \r\nRemember to run in a \r\nstraight line, and run as \r\nlong as possible. The \r\nsecond time you fail to \r\ncomplete a lap before the sound, \r\nyour test is over. The \r\ntest will begin on the word \r\nstart. On your mark, get \r\nready, start.");
  delay(2000);
  p1.show();
  Terminal.setText("The FitnessGram Pacer Test is a multistage \r\naerobic capacity test that aggressively gets \r\nmore difficult as it continues. \r\nThe 20 meter pacer test will begin \r\nin 30 seconds. Line up at the start. \r\nThe running speed starts slowly, \r\nbut gets faster each minute \r\nafter you hear this signal. \r\n[beep]. A single lap should \r\nbe completed each time you \r\nhear this sound. [ding]. \r\nRemember to run in a \r\nstraight line, and run as \r\nlong as possible. The \r\nsecond time you fail to \r\ncomplete a lap before the sound, \r\nyour test is over. The \r\ntest will begin on the word \r\nstart. On your mark, get \r\nready, start.");
  delay(2000);
  p2.show();
  delay(2000);
  p0.show();
}
