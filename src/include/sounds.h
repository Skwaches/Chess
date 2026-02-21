#ifndef SOUNDS_H
#define SOUNDS_H

bool Init_Audio(void);
void playMoveSound(void);
void playCaptureSound(void);
void playCastleSound(void);
void playCheckSound(void);
void playIllegalSound(void);
void playLowTimeAudio(void);
void playPremoveAudio(void);
void playPromoteAudio(void);
void playGameStartAudio(void);
void playGameEndAudio(void);
void playRightSound(bool checkStatus, int result);
void cleanAudio(void);

#endif
