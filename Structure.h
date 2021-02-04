#pragma once
#include <Arduino.h>

#define N_B0  31
#define N_C1  33
#define N_CS1 35
#define N_D1  37
#define N_DS1 39
#define N_E1  41
#define N_F1  44
#define N_FS1 46
#define N_G1  49
#define N_GS1 52
#define N_A1  55
#define N_AS1 58
#define N_B1  62
#define N_C2  65
#define N_CS2 69
#define N_D2  73
#define N_DS2 78
#define N_E2  82
#define N_F2  87
#define N_FS2 93
#define N_G2  98
#define N_GS2 104
#define N_A2  110
#define N_AS2 117
#define N_B2  123
#define N_C3  131
#define N_CS3 139
#define N_D3  147
#define N_DS3 156
#define N_E3  165
#define N_F3  175
#define N_FS3 185
#define N_G3  196
#define N_GS3 208
#define N_A3  220
#define N_AS3 233
#define N_B3  247
#define N_C4  262
#define N_CS4 277
#define N_D4  294
#define N_DS4 311
#define N_E4  330
#define N_F4  349
#define N_FS4 370
#define N_G4  392
#define N_GS4 415
#define N_A4  440
#define N_AS4 466
#define N_B4  494
#define N_C5  523
#define N_CS5 554
#define N_D5  587
#define N_DS5 622
#define N_E5  659
#define N_F5  698
#define N_FS5 740
#define N_G5  784
#define N_GS5 831
#define N_A5  880
#define N_AS5 932
#define N_B5  988
#define N_C6  1047
#define N_CS6 1109
#define N_D6  1175
#define N_DS6 1245
#define N_E6  1319
#define N_F6  1397
#define N_FS6 1480
#define N_G6  1568
#define N_GS6 1661
#define N_A6  1760
#define N_AS6 1865
#define N_B6  1976
#define N_C7  2093
#define N_CS7 2217
#define N_D7  2349
#define N_DS7 2489
#define N_E7  2637
#define N_F7  2794
#define N_FS7 2960
#define N_G7  3136
#define N_GS7 3322
#define N_A7  3520
#define N_AS7 3729
#define N_B7  3951
#define N_C8  4186
#define N_CS8 4435
#define N_D8  4699
#define N_DS8 4978

const unsigned int MEL_1_COUNT = 2;
const unsigned int PROGMEM MEL_1_TONE_ARR[MEL_1_COUNT] = {N_A4, 0};
const unsigned int PROGMEM MEL_1_DELAY_ARR[MEL_1_COUNT] = {250, 250};

const unsigned int MEL_2_COUNT = 4;
const unsigned int PROGMEM MEL_2_TONE_ARR[MEL_2_COUNT] = {N_AS7, 0, N_AS7, 0};
const unsigned int PROGMEM MEL_2_DELAY_ARR[MEL_2_COUNT] = {50, 100, 50, 800};

const unsigned int MEL_3_COUNT = 18;
const unsigned int PROGMEM MEL_3_TONE_ARR[MEL_3_COUNT] =   {N_E5, 0,   N_G5, 0,   N_G5, N_E5, N_C5, 0,    N_A4, 0,   N_C5, 0,  N_A4, 0,  N_E5, 0,  N_D5, 0};
const unsigned int PROGMEM MEL_3_DELAY_ARR[MEL_3_COUNT] =  {400,     100, 150,     200, 150,     200,     200,     500, 400,     100, 300,     50, 100,     50, 150,      100, 150,     600};

const unsigned int MEL_4_COUNT = 7;
const unsigned int PROGMEM MEL_4_TONE_ARR[MEL_4_COUNT] =   {N_DS7, N_DS6, N_AS6, N_GS6, N_DS7, N_AS6, 0};
const unsigned int PROGMEM MEL_4_DELAY_ARR[MEL_4_COUNT] =  {300, 200, 300, 500, 300, 300, 800};

struct DisplayData {
  byte d[6];
};

// Ascii lookup table, but with numbers at beginning
//                                                                                n= 0,1,2,3,4,5,6,7,8,9,
const byte c[] = {0x7B, 0x41, 0x37, 0x67, 0x4D, 0x6E, 0x7E, 0x43, 0x7F, 0x4F, //n  - 0,1,2,3,4,5,6,7,8,9,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //1n -  , , , , , , , , , ,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //2n -  , , , , , , , , , ,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //3n -  , , ,!,",#,$,%,&,',
                  0x00, 0x00, 0x0F, 0x00, 0x00, 0x04, 0x80, 0x00, 0x7B, 0x41, //4n - (,),*,+,,,-,.,/,0,1,
                  0x37, 0x67, 0x4D, 0x6E, 0x7E, 0x43, 0x7F, 0x4F, 0x00, 0x00, //5n - 2,3,4,5,6,7,8,9,:,;,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0x7F, 0x3A, 0x7B, 0x00, //6n - <,=,>,?,@,A,B,C,D,E,
                  0x1E, 0x00, 0x5D, 0x00, 0x00, 0x5D, 0x00, 0x5B, 0x00, 0x7B, //7n - F,G,H,I,J,K,L,M,N,O,
                  0x1F, 0x00, 0x00, 0x6E, 0x1A, 0x00, 0x00, 0x79, 0x00, 0x1D, //8n - P,Q,R,S,T,U,V,W,X,Y,
                  0x00, 0x00, 0x00, 0x00, 0x0B, 0x20, 0x00, 0x74, 0x00, 0x34, //9n - Z,[,\,],^,_,`,a,b,c,
                  0x75, 0x3F, 0x1E, 0x00, 0x5C, 0x41, 0x00, 0x5D, 0x18, 0x54, //10n- d,e,f,g,h,i,j,k,l,m,
                  0x54, 0x74, 0x1F, 0x00, 0x14, 0x00, 0x3C, 0x70, 0x00, 0x00, //11n- n,o,p,q,r,s,t,u,v,w,
                  0x00, 0x6D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00              //12n- x,y,z,{,|,},~, ,
                 };
