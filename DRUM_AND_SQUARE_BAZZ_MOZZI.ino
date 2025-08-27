#define A0_meter 39
#define A1_meter 34
#define A2_meter 35
#define A3_meter 32
#define A4_meter 33
#define A5_meter 27

#define cross_button_up 13
#define cross_button_down 19
#define cross_button_left 23
#define cross_button_right 18

#define button_0 4
#define button_1 16

#define clock_out 12
#define run_stop 14

#include <Arduino.h>
#include <U8g2lib.h>
#define CONTROL_RATE 1024

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

void TaskBlink( void *pvParameters );


U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* cs=*/ 15, /* dc=*/ 2, /* reset=*/ 17);


#define SECONDS 4;


 boolean seq_drum[8][16] = {
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //kick
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //snare3
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //snare4
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //hihat1
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //hihat2
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //clap1
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  //water1
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}   //snap1
};
   int seq_melo[4][16] = { 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}, 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}, 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}   
};
   boolean seq_melo_trig[4][16] = { 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}, 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}, 
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0},  
 { 0, 0, 0, 0, 0, 0, 0, 0 ,0,0,0,0,0,0,0,0}   
};


String note_name[]={"C_","C#","D_","D#","E_","F_","F#","G_","G#","A_","A#","B_"};
String oct_name[]={"00","01","02","03","04","05","06","07","08","09","10","11","12"};


String seq_drum_kick="----------------";
String seq_drum_snare3="----------------";
String seq_drum_snare4="----------------";
String seq_drum_hihat1="----------------";
String seq_drum_hihat2="----------------";
String seq_drum_clap1="----------------";
String seq_drum_water1="----------------";
String seq_drum_snap1="----------------";

String seq_melo_oct0_8="";
String seq_melo_key0_8="";
String seq_melo_oct1_8="";
String seq_melo_key1_8="";
String seq_melo_oct2_8="";
String seq_melo_key2_8="";
String seq_melo_oct3_8="";
String seq_melo_key3_8="";

String seq_melo_oct0_16="";
String seq_melo_key0_16="";
String seq_melo_oct1_16="";
String seq_melo_key1_16="";
String seq_melo_oct2_16="";
String seq_melo_key2_16="";
String seq_melo_oct3_16="";
String seq_melo_key3_16="";

#include <MozziGuts.h>               // File header dasar Mozzi
#include <mozzi_rand.h>
#include <ADSR.h>                    // envelope

ADSR <AUDIO_RATE, AUDIO_RATE> envB0;
ADSR <AUDIO_RATE, AUDIO_RATE> envB1;
ADSR <AUDIO_RATE, AUDIO_RATE> envB2;
ADSR <AUDIO_RATE, AUDIO_RATE> envB3;

#include <Portamento.h> //Portamento <CONTROL_RATE, AUDIO_RATE> aPortamentoA;
#include <Line.h> // for smooth transitions
Line <long> aGliss;

#include <Smooth.h> // to smooth frequency for aSin1
const unsigned int AUDIO_STEPS_PER_CONTROL = AUDIO_RATE / CONTROL_RATE;
float smoothness = 0.01f;
Smooth <unsigned int> aSmooth(smoothness); 
volatile unsigned int freq1;

long audio_steps_per_gliss = AUDIO_RATE / 64; // ie. 4 glisses per second
long control_steps_per_gliss = CONTROL_RATE / 4;
long gliss_start;
long gliss_end;

#include <tables/square_no_alias_2048_int8.h>
#include <Oscil.h> // Template osilator

Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu0(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu1(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu2(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSqu3(SQUARE_NO_ALIAS_2048_DATA); 

#include <kick9.h>
#include <snare3.h>
#include <snare4.h>
#include <hihattable.h>
#include <chihat2.h>
#include <clap2.h>
#include <water1.h>
#include <snap1.h>

#include <Sample.h> // Sample template
Sample <kick9_NUM_CELLS, AUDIO_RATE> kick9(kick9_DATA);
Sample <snare3_NUM_CELLS, AUDIO_RATE> snare3(snare3_DATA);
Sample <snare4_NUM_CELLS, AUDIO_RATE> snare4(snare4_DATA);
Sample <HIHATTABLE_NUM_CELLS, AUDIO_RATE> hihat1(HIHATTABLE_DATA);
Sample <chihat2_NUM_CELLS, AUDIO_RATE> hihat2(chihat2_DATA);
Sample <clap2_NUM_CELLS, AUDIO_RATE> clap(clap2_DATA);
Sample <water1_NUM_CELLS, AUDIO_RATE> water1(water1_DATA);
Sample <snap1_NUM_CELLS, AUDIO_RATE> snap1(snap1_DATA);

#include <EventDelay.h> // Header untuk pemicu (waktu pengucapan)
EventDelay kTriggerDelay; // Header untuk mengubah nilai dengan lancar

int master_count=0;
int tempo=0;
bool trigger;
byte check;       // Untuk menentukan naiknya pulse

int cursor_x_drum=0;
int cursor_y_drum=0;
int cursor_x_melo_12=0;
int cursor_y_melo_12=0;
int cursor_x_melo_34=0;
int cursor_y_melo_34=0;
int cursor_select=0;

int attack_time=0;
int decay_time=0;
int sustain_time=0;
int release_time=0;

void setup(){
 u8g2.begin();
 //  u8g2.setBusClock(400000000);
//   Wire.setClock(400000);
 startMozzi(CONTROL_RATE);
 kTriggerDelay.start(100); // Spesifikasi waktu pemicu (100ms)
 u8g2.setFontMode(1);
 u8g2.enableUTF8Print();  
 u8g2.setFont(u8g2_font_b10_t_japanese1); 
 u8g2.setFontDirection(0);
//aPortamentoA.setTime(300u);

   xTaskCreatePinnedToCore(
   TaskBlink
   ,  "TaskBlink"   // A name just for humans
   ,  16384  // This stack size can be checked & adjusted by reading the Stack Highwater
   ,  NULL
   ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
   ,  NULL 
   ,  0);
}


void updateControl(){
  
 attack_time=map(mozziAnalogRead(A3_meter),0,4095,500,5);
 decay_time=map(mozziAnalogRead(A4_meter),0,4095,500,5);
 sustain_time=map(mozziAnalogRead(A5_meter),0,4095,500,5);
 release_time=map(mozziAnalogRead(A2_meter),0,4095,500,5);
 

  if(kTriggerDelay.ready()){
    set_freq();
    digitalRead(run_stop);
    digitalWrite(clock_out, LOW);
      
  if ((digitalRead(run_stop))) {
    melo_part();
    drum_part();
    master_count+=1;
    tempo+=1;
  }
  
  if(master_count==16){
     master_count=0;
    }

    if(tempo==2){
     tempo=0;
     digitalWrite(clock_out, HIGH);
    }


     kTriggerDelay.start(map(mozziAnalogRead(A0_meter),0,4096,10,500));        
  }    //  kTrigger jika pernyataan berakhir
}     // batalkan updateControl() selesai　




void TaskBlink(void *pvParameters)  // This is a task.
{
 (void) pvParameters;

u8g2.begin();
set_pinmode();


 for (;;) // A Task shall never return or exit.
 {
cursor_select=map(mozziAnalogRead(A1_meter),0,4095,0,2);
switch(cursor_select){
case 0: 
drum_seq_disp();   
break;
case 1:
melo_seq_disp_12();
break;
case 2:
melo_seq_disp_34();
break;
}
vTaskDelay(10);
 }  // untuk (;;) akhir
}


void set_drum_seq(){

if(digitalRead(cross_button_up))cursor_y_drum+=-1;
if(digitalRead(cross_button_down))cursor_y_drum+=1;
if(digitalRead(cross_button_right))cursor_x_drum+=1;
if(digitalRead(cross_button_left))cursor_x_drum+=-1;
if(cursor_x_drum<0)cursor_x_drum=0;
if(cursor_x_drum>15)cursor_x_drum=15;
if(cursor_y_drum<0)cursor_y_drum=0;
if(cursor_y_drum>7)cursor_y_drum=7;
if(digitalRead(4)){
 seq_drum[cursor_y_drum][cursor_x_drum]=true;
 }
 if(digitalRead(16)){
 seq_drum[cursor_y_drum][cursor_x_drum]=false;
 } 

seq_drum_kick="";
seq_drum_snare3="";
seq_drum_snare4="";
seq_drum_hihat1="";
seq_drum_hihat2="";
seq_drum_clap1="";
seq_drum_water1="";
seq_drum_snap1="";

  for(int i=0;i<16;i++){

if(seq_drum[0][i]){
seq_drum_kick+="X";
}else if(seq_drum[0][i]==0){
seq_drum_kick+="."; 
 }
 
if(seq_drum[1][i]){
seq_drum_snare3+="X";
}else if(seq_drum[1][i]==0){
seq_drum_snare3+="-"; 
 }

if(seq_drum[2][i]){
seq_drum_snare4+="X";
}else if(seq_drum[2][i]==0){
seq_drum_snare4+="."; 
 }
if(seq_drum[3][i]){
seq_drum_hihat1+="X";
}else if(seq_drum[3][i]==0){
seq_drum_hihat1+="-"; 
 }
if(seq_drum[4][i]){
seq_drum_hihat2+="X";
}else if(seq_drum[4][i]==0){
seq_drum_hihat2+="."; 
 }
if(seq_drum[5][i]){
seq_drum_clap1+="X";
}else if(seq_drum[5][i]==0){
seq_drum_clap1+="-"; 
 }
if(seq_drum[6][i]){
seq_drum_water1+="X";
}else if(seq_drum[6][i]==0){
seq_drum_water1+="."; 
 }
if(seq_drum[7][i]){
seq_drum_snap1+="X";
}else if(seq_drum[7][i]==0){
seq_drum_snap1+="-"; 
 }
 }
 }


void drum_seq_disp(){
  u8g2.clearBuffer();
set_drum_seq();
u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f );  
u8g2.drawFrame(master_count*8,0,8,8);
u8g2.drawFrame(master_count*8,7,8,8);
u8g2.drawFrame(master_count*8,15,8,8);
u8g2.drawFrame(master_count*8,23,8,8);
u8g2.drawFrame(master_count*8,31,8,8);
u8g2.drawFrame(master_count*8,39,8,8);
u8g2.drawFrame(master_count*8,47,8,8);
u8g2.drawFrame(master_count*8,53,8,8);
u8g2.drawFrame(master_count*8,63,8,8);

u8g2.drawFrame(cursor_x_drum*8,cursor_y_drum*8,8,8);
// u8g2.setCursor(master_count*5,10);
// u8g2.print(master_count+1);
u8g2.setCursor(0,7);
u8g2.print(seq_drum_kick);
u8g2.setCursor(0,15);
u8g2.print(seq_drum_snare3);
u8g2.setCursor(0,23);
u8g2.print(seq_drum_snare4);
u8g2.setCursor(0,31);
u8g2.print(seq_drum_hihat1);
u8g2.setCursor(0,39);
u8g2.print(seq_drum_hihat2);
u8g2.setCursor(0,47);
u8g2.print(seq_drum_clap1);
u8g2.setCursor(0,53);
u8g2.print(seq_drum_water1);
u8g2.setCursor(0,63);
u8g2.print(seq_drum_snap1);
u8g2.sendBuffer();
 }


void drum_part(){
  if(seq_drum[0][master_count])  { 
   kick9.start();
  } 
  if(seq_drum[1][master_count])  {
   snare3.start();
  }
  if(seq_drum[2][master_count]) {   
   snare4.start();
  }
  if(seq_drum[3][master_count]) { 
   hihat1.start();
  }
  if(seq_drum[4][master_count]) { 
   hihat2.start();
  } 
  if(seq_drum[5][master_count])  {
   clap.start();
  }
  if(seq_drum[6][master_count])  {  
   water1.start();  
   } 
  if(seq_drum[7][master_count])  {  
   snap1.start();  
   } 
   }

void melo_part(){
 if(seq_melo_trig[0][master_count]){
     aSqu0.setFreq( mtof(seq_melo[0][master_count])  ) ; 
     envB0.setADLevels((byte)180, (byte)100);                    // tingkat envelope bass 0 ~ 255
     envB0.setTimes(attack_time,decay_time,sustain_time,release_time);     // waktu envelope bass
     envB0.noteOn();
  }
 if(seq_melo_trig[1][master_count]){
     aSqu1.setFreq( mtof(seq_melo[1][master_count])  ) ; 
     envB1.setADLevels((byte)180, (byte)100);                    // tingkat envelope bass 0 ~ 255
     envB1.setTimes(attack_time,decay_time,sustain_time,release_time);     // waktu envelope bass
     envB1.noteOn();
  }

    if(seq_melo_trig[2][master_count]){
     aSqu2.setFreq( mtof(seq_melo[2][master_count])  ) ; 
     envB2.setADLevels((byte)180, (byte)100);                    // tingkat envelope bass 0 ~ 255
     envB2.setTimes(attack_time,decay_time,sustain_time,release_time);     // waktu envelope bass
     envB2.noteOn();
  }

  if(seq_melo_trig[3][master_count]){
     aSqu3.setFreq( mtof(seq_melo[3][master_count])  ) ; 
     envB3.setADLevels((byte)180, (byte)100);                    // tingkat envelope bass 0 ~ 255
     envB3.setTimes(attack_time,decay_time,sustain_time,release_time);     // waktu envelope bass
     envB3.noteOn();
  }
  
 }
 

void set_melo_seq_12(){

if(digitalRead(cross_button_up)&&digitalRead(button_0)==0)cursor_y_melo_12+=-1;
if(digitalRead(cross_button_down)&&digitalRead(button_0)==0)cursor_y_melo_12+=1;
if(digitalRead(cross_button_right)&&digitalRead(button_0)==0)cursor_x_melo_12+=1;
if(digitalRead(cross_button_left)&&digitalRead(button_0)==0)cursor_x_melo_12+=-1;
if(cursor_x_melo_12<0)cursor_x_melo_12=0;
if(cursor_x_melo_12>7)cursor_x_melo_12=7;
if(cursor_y_melo_12<0)cursor_y_melo_12=0;
if(cursor_y_melo_12>3)cursor_y_melo_12=3;

if(cursor_y_melo_12<2){ 
if(digitalRead(button_0)&&digitalRead(cross_button_up)){
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]+=1;
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]=constrain(seq_melo[cursor_y_melo_12][cursor_x_melo_12], 0, 128);
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_down)){
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]+=-1;
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]=constrain(seq_melo[cursor_y_melo_12][cursor_x_melo_12], 0, 128);
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=true;
}
 if(digitalRead(button_0)&&digitalRead(cross_button_right)){
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]+=12;
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]=constrain(seq_melo[cursor_y_melo_12][cursor_x_melo_12], 0, 128);
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_left)){
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]+=-12;
 seq_melo[cursor_y_melo_12][cursor_x_melo_12]=constrain(seq_melo[cursor_y_melo_12][cursor_x_melo_12], 0, 128);
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=true;
 }
  if(digitalRead(button_1)){
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=false;
 } 
if(digitalRead(button_0)){
 seq_melo_trig[cursor_y_melo_12][cursor_x_melo_12]=true;
 } 
}else{
  if(digitalRead(button_0)&&digitalRead(cross_button_up)){
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]+=1;
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]=constrain(seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8], 0, 128);
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_down)){
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]+=-1;
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]=constrain(seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8], 0, 128);
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=true;
}
 if(digitalRead(button_0)&&digitalRead(cross_button_right)){
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]+=12;
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]=constrain(seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8], 0, 128);
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=true;
 }
 
if(digitalRead(button_0)&&digitalRead(cross_button_left)){
seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]+=-12;
 seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8]=constrain(seq_melo[cursor_y_melo_12-2][cursor_x_melo_12+8], 0, 128);
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=true;
 }
 
if(digitalRead(button_1)){
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=false;
 } 
if(digitalRead(button_0)){
 seq_melo_trig[cursor_y_melo_12-2][cursor_x_melo_12+8]=true;
 } 
}  
    

seq_melo_oct0_8="";
seq_melo_key0_8="";
seq_melo_oct1_8="";
seq_melo_key1_8="";

seq_melo_oct0_16="";
seq_melo_key0_16="";
seq_melo_oct1_16="";
seq_melo_key1_16="";


  for(int i=0;i<16;i++){

if(i<8){
if(seq_melo_trig[0][i]){
seq_melo_oct0_8+=oct_name[(int)seq_melo[0][i]/12];
}else if(seq_melo_trig[0][i]==0){
seq_melo_oct0_8+="__"; 
 }
if(seq_melo_trig[0][i]){
seq_melo_key0_8+=note_name[(int)seq_melo[0][i]%12];
}else if(seq_melo_trig[0][i]==0){
seq_melo_key0_8+="__"; 
 }

if(seq_melo_trig[1][i]){
seq_melo_oct1_8+=oct_name[(int)seq_melo[1][i]/12];
}else if(seq_melo_trig[1][i]==0){
seq_melo_oct1_8+="__"; 
 }
if(seq_melo_trig[1][i]){
seq_melo_key1_8+=note_name[(int)seq_melo[1][i]%12];
}else if(seq_melo_trig[1][i]==0){
seq_melo_key1_8+="__"; 
 }
} else{  

if(seq_melo_trig[0][i]){
seq_melo_oct0_16+=oct_name[(int)seq_melo[0][i]/12];
}else if(seq_melo_trig[0][i]==0){
seq_melo_oct0_16+="__"; 
 }
if(seq_melo_trig[0][i]){
seq_melo_key0_16+=note_name[(int)seq_melo[0][i]%12];
}else if(seq_melo_trig[0][i]==0){
seq_melo_key0_16+="__"; 
 }

if(seq_melo_trig[1][i]){
seq_melo_oct1_16+=oct_name[(int)seq_melo[1][i]/12];
}else if(seq_melo_trig[1][i]==0){
seq_melo_oct1_16+="__"; 
 }
if(seq_melo_trig[1][i]){
seq_melo_key1_16+=note_name[(int)seq_melo[1][i]%12];
}else if(seq_melo_trig[1][i]==0){
seq_melo_key1_16+="__"; 
 }
}
} // untuk akhir


} // akhir set_melo_seq ()


void melo_seq_disp_12(){
  u8g2.clearBuffer();
set_melo_seq_12();
u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f );  
if(master_count<8){
u8g2.drawFrame(master_count*16,0,8,8);
u8g2.drawFrame(master_count*16,7,8,8);
u8g2.drawFrame(master_count*16,15,8,8);
u8g2.drawFrame(master_count*16,23,8,8);

}
if(master_count>=8){
u8g2.drawFrame((master_count-8)*16,31,8,8);
u8g2.drawFrame((master_count-8)*16,39,8,8);
u8g2.drawFrame((master_count-8)*16,47,8,8);
u8g2.drawFrame((master_count-8)*16,53,8,8);
}
u8g2.drawFrame(cursor_x_melo_12*16,cursor_y_melo_12*16,8,8);
// u8g2.setCursor(master_count*5,10);
// u8g2.print(master_count+1);
u8g2.setCursor(0,7);
u8g2.print(seq_melo_oct0_8);
u8g2.setCursor(0,15);
u8g2.print(seq_melo_key0_8);
u8g2.setCursor(0,23);
u8g2.print(seq_melo_oct1_8);
u8g2.setCursor(0,31);
u8g2.print(seq_melo_key1_8);
u8g2.setCursor(0,39);
u8g2.print(seq_melo_oct0_16);
u8g2.setCursor(0,47);
u8g2.print(seq_melo_key0_16);
u8g2.setCursor(0,53);
u8g2.print(seq_melo_oct1_16);
u8g2.setCursor(0,63);
u8g2.print(seq_melo_key1_16);
u8g2.sendBuffer();
 }

void set_melo_seq_34(){

if(digitalRead(cross_button_up)&&digitalRead(button_0)==0)cursor_y_melo_34+=-1;
if(digitalRead(cross_button_down)&&digitalRead(button_0)==0)cursor_y_melo_34+=1;
if(digitalRead(cross_button_right)&&digitalRead(button_0)==0)cursor_x_melo_34+=1;
if(digitalRead(cross_button_left)&&digitalRead(button_0)==0)cursor_x_melo_34+=-1;
if(cursor_x_melo_34<0)cursor_x_melo_34=0;
if(cursor_x_melo_34>7)cursor_x_melo_34=7;
if(cursor_y_melo_34<0)cursor_y_melo_34=0;
if(cursor_y_melo_34>3)cursor_y_melo_34=3;

if(cursor_y_melo_34<2){ 
if(digitalRead(button_0)&&digitalRead(cross_button_up)){
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]+=1;
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]=constrain(seq_melo[cursor_y_melo_34+2][cursor_x_melo_34], 0, 128);
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_down)){
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]+=-1;
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]=constrain(seq_melo[cursor_y_melo_34+2][cursor_x_melo_34], 0, 128);
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=true;
}
 if(digitalRead(button_0)&&digitalRead(cross_button_right)){
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]+=12;
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]=constrain(seq_melo[cursor_y_melo_34+2][cursor_x_melo_34], 0, 128);
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_left)){
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]+=-12;
 seq_melo[cursor_y_melo_34+2][cursor_x_melo_34]=constrain(seq_melo[cursor_y_melo_34+2][cursor_x_melo_34], 0, 128);
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=true;
 }
  if(digitalRead(button_1)){
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=false;
 } 
if(digitalRead(button_0)){
 seq_melo_trig[cursor_y_melo_34+2][cursor_x_melo_34]=true;
 } 
}else{
  if(digitalRead(button_0)&&digitalRead(cross_button_up)){
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]+=1;
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]=constrain(seq_melo[cursor_y_melo_34][cursor_x_melo_34+8], 0, 128);
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_down)){
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]+=-1;
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]=constrain(seq_melo[cursor_y_melo_34][cursor_x_melo_34+8], 0, 128);
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=true;
}
 if(digitalRead(button_0)&&digitalRead(cross_button_right)){
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]+=12;
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]=constrain(seq_melo[cursor_y_melo_34][cursor_x_melo_34+8], 0, 128);
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=true;
 }
if(digitalRead(button_0)&&digitalRead(cross_button_left)){
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]+=-12;
 seq_melo[cursor_y_melo_34][cursor_x_melo_34+8]=constrain(seq_melo[cursor_y_melo_34][cursor_x_melo_34+8], 0, 128);
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=true;
 }
if(digitalRead(button_1)){
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=false;
 } 
if(digitalRead(button_0)){
 seq_melo_trig[cursor_y_melo_34][cursor_x_melo_34+8]=true;
 } 
}  
    

seq_melo_oct2_8="";
seq_melo_key2_8="";
seq_melo_oct3_8="";
seq_melo_key3_8="";

seq_melo_oct2_16="";
seq_melo_key2_16="";
seq_melo_oct3_16="";
seq_melo_key3_16="";


  for(int i=0;i<16;i++){

if(i<8){
if(seq_melo_trig[2][i]){
seq_melo_oct2_8+=oct_name[(int)seq_melo[2][i]/12];
}else if(seq_melo_trig[2][i]==0){
seq_melo_oct2_8+="__"; 
 }
if(seq_melo_trig[2][i]){
seq_melo_key2_8+=note_name[(int)seq_melo[2][i]%12];
}else if(seq_melo_trig[2][i]==0){
seq_melo_key2_8+="__"; 
 }

if(seq_melo_trig[3][i]){
seq_melo_oct3_8+=oct_name[(int)seq_melo[3][i]/12];
}else if(seq_melo_trig[3][i]==0){
seq_melo_oct3_8+="__"; 
 }
if(seq_melo_trig[3][i]){
seq_melo_key3_8+=note_name[(int)seq_melo[3][i]%12];
}else if(seq_melo_trig[3][i]==0){
seq_melo_key3_8+="__"; 
 }
} else{  

if(seq_melo_trig[2][i]){
seq_melo_oct2_16+=oct_name[(int)seq_melo[2][i]/12];
}else if(seq_melo_trig[2][i]==0){
seq_melo_oct2_16+="__"; 
 }
if(seq_melo_trig[2][i]){
seq_melo_key2_16+=note_name[(int)seq_melo[2][i]%12];
}else if(seq_melo_trig[2][i]==0){
seq_melo_key2_16+="__"; 
 }

if(seq_melo_trig[3][i]){
seq_melo_oct3_16+=oct_name[(int)seq_melo[3][i]/12];
}else if(seq_melo_trig[3][i]==0){
seq_melo_oct3_16+="__"; 
 }
if(seq_melo_trig[3][i]){
seq_melo_key3_16+=note_name[(int)seq_melo[3][i]%12];
}else if(seq_melo_trig[3][i]==0){
seq_melo_key3_16+="__"; 
 }
}
} // untuk akhir


} // akhir dari set_melo_seq()

void melo_seq_disp_34(){
  u8g2.clearBuffer();
set_melo_seq_34();
u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f );  
if(master_count<8){
u8g2.drawFrame(master_count*16,0,8,8);
u8g2.drawFrame(master_count*16,7,8,8);
u8g2.drawFrame(master_count*16,15,8,8);
u8g2.drawFrame(master_count*16,23,8,8);

}
if(master_count>=8){
u8g2.drawFrame((master_count-8)*16,31,8,8);
u8g2.drawFrame((master_count-8)*16,39,8,8);
u8g2.drawFrame((master_count-8)*16,47,8,8);
u8g2.drawFrame((master_count-8)*16,53,8,8);
}
u8g2.drawFrame(cursor_x_melo_34*16,cursor_y_melo_34*16,8,8);
// u8g2.setCursor(master_count*5,10);
// u8g2.print(master_count+1);
u8g2.setCursor(0,7);
u8g2.print(seq_melo_oct2_8);
u8g2.setCursor(0,15);
u8g2.print(seq_melo_key2_8);
u8g2.setCursor(0,23);
u8g2.print(seq_melo_oct3_8);
u8g2.setCursor(0,31);
u8g2.print(seq_melo_key3_8);
u8g2.setCursor(0,39);
u8g2.print(seq_melo_oct2_16);
u8g2.setCursor(0,47);
u8g2.print(seq_melo_key2_16);
u8g2.setCursor(0,53);
u8g2.print(seq_melo_oct3_16);
u8g2.setCursor(0,63);
u8g2.print(seq_melo_key3_16);
u8g2.sendBuffer();
 }
 

void set_freq(){
   kick9.setFreq( (float)6*(float)kick9_SAMPLERATE/(float)kick9_NUM_CELLS );
   snare3.setFreq( 6*(float)snare3_SAMPLERATE/(float)snare3_NUM_CELLS );
   snare4.setFreq( 6*(float)snare4_SAMPLERATE/(float)snare4_NUM_CELLS );
   hihat1.setFreq( 16*(float)HIHATTABLE_SAMPLERATE/(float)HIHATTABLE_NUM_CELLS );
   hihat2.setFreq( (float)7*(float)chihat2_SAMPLERATE/(float)chihat2_NUM_CELLS );
   clap.setFreq( 6*(float)clap2_SAMPLERATE/(float)clap2_NUM_CELLS );
   water1.setFreq( 5*(float)water1_SAMPLERATE/(float)water1_NUM_CELLS );  
   snap1.setFreq( (float)6*(float)snap1_SAMPLERATE/(float)snap1_NUM_CELLS ); 
 }


void set_pinmode(){
pinMode(cross_button_up,INPUT);
pinMode(cross_button_down,INPUT);
pinMode(cross_button_left,INPUT);
pinMode(cross_button_right,INPUT);
pinMode(button_0,INPUT);
pinMode(button_1,INPUT);
pinMode(clock_out,OUTPUT);
pinMode(run_stop,INPUT_PULLUP);
 
gpio_set_pull_mode(GPIO_NUM_13, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(GPIO_NUM_23, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(GPIO_NUM_19, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(GPIO_NUM_18, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(GPIO_NUM_16, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLDOWN_ONLY);

} 

int updateAudio(){
  envB0.update();
  envB1.update();
  envB2.update();
  envB3.update();
//aSaw1.setPhaseInc(aGliss.next());
int smoothedFreq = aSmooth.next(freq1); 

 int code=envB0.next()*(aSqu0.next()>>8)+envB1.next()*(aSqu1.next()>>8)+envB2.next()*(aSqu2.next()>>8)+envB3.next()*(aSqu3.next()>>8);
    return ( (code>>2)+ snare3.next()+snare4.next() +snap1.next()+ +kick9.next()+(hihat1.next())+(hihat2.next()) +clap.next() +water1.next() )>>2;
    
}

void loop(){
 audioHook();
} 
