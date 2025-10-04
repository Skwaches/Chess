
#include <SDL3/SDL.h>
#include "Linkers/funcs.h"
#include "Linkers/sounds.h"

static MIX_Mixer *ChessMixer;
static MIX_Track *sfxTrack;
static MIX_Audio *captureAudio,
    *moveAudio, *castleAudio,
    *checkAudio, *illegalAudio,
    *lowtimeAudio, *premoveAudio, *promoteAudio,
    *gameStartAudio, *gameEndAudio;
char **soundEffects;

bool loadSound(char *soundEffect, const char *fileName, MIX_Audio **soundVar)
{
    if (SDL_strcmp(soundEffect, fileName))
    {
        char buffer[MAX_ASSET_PATH];
        SDL_snprintf(buffer, sizeof(buffer), "%s/%s", SOUNDS_PATH, fileName);
        SDL_IOStream *soundIO = SDL_IOFromFile(buffer, "rb");
        if (soundIO == NULL)
        {
            SDL_Log("%s Audio :%s\n", fileName, SDL_GetError());
            MIX_DestroyTrack(sfxTrack);
            MIX_DestroyMixer(ChessMixer);
            return false;
        }
        *soundVar = MIX_LoadAudio_IO(ChessMixer, soundIO, true, true);
        if (captureAudio == NULL)
        {
            SDL_Log("%s Audio :%s\n", fileName, SDL_GetError());
            MIX_DestroyTrack(sfxTrack);
            MIX_DestroyMixer(ChessMixer);
            MIX_Quit();
            return false;
        }
    }
    return true;
}

bool loadAllSounds(void)
{
    int effectsNO = 0;
    soundEffects = SDL_GlobDirectory(SOUNDS_PATH, NULL, 0, &effectsNO);
    for (int k = 0; k < effectsNO; k++)
    {
        if (!loadSound(soundEffects[k], CAPTURE_SOUND_FILE_NAME, &captureAudio))
            return false;
        else if (!loadSound(soundEffects[k], MOVE_SOUND_FILE_NAME, &moveAudio))
            return false;
        else if (!loadSound(soundEffects[k], CASTLE_SOUND_FILE_NAME, &castleAudio))
            return false;
        else if (!loadSound(soundEffects[k], CHECK_SOUND_FILE_NAME, &checkAudio))
            return false;
        else if (!loadSound(soundEffects[k], ILLEGAL_SOUND_FILE_NAME, &illegalAudio))
            return false;
        else if (!loadSound(soundEffects[k], LOWTIME_SOUND_FILE_NAME, &lowtimeAudio))
            return false;
        else if (!loadSound(soundEffects[k], PREMOVE_SOUND_FILE_NAME, &premoveAudio))
            return false;
        else if (!loadSound(soundEffects[k], PROMOTE_SOUND_FILE_NAME, &promoteAudio))
            return false;
        else if (!loadSound(soundEffects[k], GAMESTART_SOUND_FILE_NAME, &gameStartAudio))
            return false;
        else if (!loadSound(soundEffects[k], GAMEEND_SOUND_FILE_NAME, &gameEndAudio))
            return false;
    }
    return true;
}

bool Init_Audio(void)
{

    if (!MIX_Init())
    {
        SDL_Log("Error Initialising....:%s", SDL_GetError());
        return false;
    }
    ChessMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (ChessMixer == NULL)
    {
        SDL_Log("Chess Sound Mixer : %s\n", SDL_GetError());
        MIX_Quit();
        return false;
    }
    sfxTrack = MIX_CreateTrack(ChessMixer);
    if (sfxTrack == NULL)
    {
        SDL_Log("Sound Effects Error : %s\n", SDL_GetError());
        MIX_DestroyMixer(ChessMixer);
        MIX_Quit();
        return false;
    }
    if (!loadAllSounds())
    {
        SDL_free(soundEffects);
        return false;
    }

    return true;
}

// Returns true for successful play
bool playSound(MIX_Audio *sound)
{
    if (!MIX_SetTrackAudio(sfxTrack, sound))
    {
        SDL_Log("Error Playing :%s\n", SDL_GetError());
        return false;
    }
    if (!MIX_PlayTrack(sfxTrack, 0))
    {
        SDL_Log("Error Playing :%s\n", SDL_GetError());
        return false;
    }
    return true;
}

// Wrapper on playSound function:
// Returns true for successful play
void playMoveSound(void)
{
    if (!playSound(moveAudio))
    {
        SDL_Log("Something is wrong with the Move sound\n");
    }
    return;
}
void playCaptureSound(void)
{
    if (!playSound(captureAudio))
    {
        SDL_Log("Something is wrong with the Capture sound\n");
    }
    return;
}
void playCastleSound(void)
{
    if (!playSound(castleAudio))
    {
        SDL_Log("Something is wrong with the Castle sound\n");
    }
    return;
}
void playCheckSound(void)
{
    if (!playSound(checkAudio))
    {
        SDL_Log("Something is wrong with the Check sound\n");
    }
    return;
}
void playIllegalSound(void)
{
    if (!playSound(illegalAudio))
    {
        SDL_Log("Something is wrong with the Check sound\n");
    }
    return;
}
void playLowTimeAudio(void)
{
    if (!playSound(lowtimeAudio))
    {
        SDL_Log("Something is wrong with the Low Time sound\n");
    }
    return;
}
void playPremoveAudio(void)
{
    if (!playSound(premoveAudio))
    {
        SDL_Log("Something is wrong with the Premove sound\n");
    }
    return;
}
void playPromoteAudio(void)
{
    if (!playSound(promoteAudio))
    {
        SDL_Log("Something is wrong with the Promote sound\n");
    }
    return;
}
void playGameStartAudio(void)
{
    if (!playSound(gameStartAudio))
    {
        SDL_Log("Something is wrong with the game start sound\n");
    }
    return;
}
void playGameEndAudio(void)
{
    if (!playSound(gameEndAudio))
    {
        SDL_Log("Something is wrong with the game end sound\n");
    }
    return;
}

// Easy clean up for audio
// A list of audios would be smarter.
void cleanAudio(void)
{
    MIX_DestroyAudio(captureAudio);
    MIX_DestroyAudio(moveAudio);
    MIX_DestroyAudio(castleAudio);
    MIX_DestroyAudio(checkAudio);
    MIX_DestroyAudio(illegalAudio);
    MIX_DestroyAudio(lowtimeAudio);
    MIX_DestroyAudio(premoveAudio);
    MIX_DestroyAudio(promoteAudio);
    MIX_DestroyAudio(gameStartAudio);
    MIX_DestroyAudio(gameEndAudio);
    MIX_DestroyTrack(sfxTrack);
    MIX_DestroyMixer(ChessMixer);
    MIX_Quit();
}