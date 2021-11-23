#include <SPI.h>
#include <SdFat.h>
#include <SFEMP3Shield.h>

int play_track = 0;
int play_type = 0;
int list_size = 17;

SdFat sd;
SFEMP3Shield MP3player;

// 버튼으로 조종하는 MP3

void setup(){
   Serial.begin(115200);
   randomSeed(analogRead(0));
   sd.begin(SD_SEL, SPI_HALF_SPEED);
   MP3player.begin();
   pinMode(A0,INPUT_PULLUP);
   pinMode(A1,INPUT_PULLUP);
   pinMode(A2,INPUT_PULLUP);
   pinMode(A3,INPUT_PULLUP);
   pinMode(A4,INPUT_PULLUP);
}

void loop(){  
  int choice_style = digitalRead(A1);
  int sound_up = digitalRead(A0);
  int sound_down = digitalRead(A4);
  int to_pause = digitalRead(A2);
  int playing_track = digitalRead(A3);

  if(to_pause == 0) {
    
    pausing();
    delay(100);
    
  } else if(sound_up == 0) {
    
    sound(1);
    delay(100);
    
  } else if(sound_down == 0) {
    
    sound(0);
    delay(100);
    
  } else if(choice_style == 0) {
    
    play_type++;
    if(play_type > 2){
      play_type = 0;
    }
    Serial.print("play type : ");
    Serial.println(play_type);
    delay(100);
    
  } else if(playing_track == 0) {
    MP3player.stopTrack();
    play_track++;
    MP3player.playTrack(play_track);
    Serial.print("play track : ");
    Serial.println(play_track);
    delay(100);
  }

  // 오류 방지용 (순서 중요)
  if(play_track > list_size) {
    play_track = 0;
  }
  if(play_track == 0 && play_type == 2) {
    play_track = 1;
  }
  if(MP3player.isPlaying() != 1) {
    style();
  }
  
  delay(100);
}

void style() {
  if(play_type == 1) // 한 개 반복
  {
    MP3player.playTrack(play_track);
      
  } else if(play_type == 2) // 무작위로 전체 반복
  {
    play_track = random(1,list_size+1);
    MP3player.playTrack(play_track);
      
  } else // 순서대로 전체 반복
  {
    play_track++;
    MP3player.playTrack(play_track);
    
  }
  Serial.print("play track : ");
  Serial.println(play_track);
}

// 소리 조절
void sound(int a) {
  union twobyte mp3_vol;
  mp3_vol.word = MP3player.getVolume();
  
  if(a == 0) {
    Serial.print("down ");
    if(mp3_vol.byte[1] >= 254) {
      mp3_vol.byte[1] = 254;
    } else {
      mp3_vol.byte[1] += 2;
    }
  } else {
    Serial.print("up ");
    if(mp3_vol.byte[1] <= 2) { // range check
      mp3_vol.byte[1] = 2;
    } else {
      mp3_vol.byte[1] -= 2;
    }
  }
  Serial.println(mp3_vol.byte[1]>>1, 1);
  MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]);
}

// 일시정지
void pausing() {
  if( MP3player.getState() == playback) {
      Serial.println("to pause");
      MP3player.pauseMusic();
    } else if( MP3player.getState() == paused_playback) {
      Serial.println("again");
      MP3player.resumeMusic();
    }
}