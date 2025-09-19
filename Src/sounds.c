
#include <SDL3/SDL.h>
#include "Linkers/funcs.h"
#include "Linkers/sounds.h"

static MIX_Mixer *ChessMixer;
static MIX_Track *sfxTrack;
static MIX_Audio *captureAudio;
static MIX_Audio *moveAudio;
static MIX_Audio *castleAudio;

bool Init_Audio(void)
{
    int effectsNO = 0;
    char **soundEffects;
    char buffer[MAX_ASSET_PATH];
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
    soundEffects = SDL_GlobDirectory(SOUNDS_PATH, NULL, 0, &effectsNO);

    SDL_IOStream *soundIO;

    // FIX THIS SHIT. #HolyDogWater
    for (int k = 0; k < effectsNO; k++)
    {
        if (SDL_strcmp(soundEffects[k], CAPTURE_SOUND_FILE_NAME))
        {
            SDL_snprintf(buffer, sizeof(buffer), "%s/%s", SOUNDS_PATH, CAPTURE_SOUND_FILE_NAME);
            soundIO = SDL_IOFromFile(buffer, "rb");
            if (soundIO == NULL)
            {
                SDL_Log("Capture Audio IO :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                return false;
            }
            captureAudio = MIX_LoadAudio_IO(ChessMixer, soundIO, true, true);
            if (captureAudio == NULL)
            {
                SDL_Log("Capture Audio :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                MIX_Quit();
                return false;
            }
        }
        if (SDL_strcmp(soundEffects[k], MOVE_SOUND_FILE_NAME))
        {
            SDL_snprintf(buffer, sizeof(buffer), "%s/%s", SOUNDS_PATH, MOVE_SOUND_FILE_NAME);
            soundIO = SDL_IOFromFile(buffer, "rb");
            if (soundIO == NULL)
            {
                SDL_Log("Move Audio IO :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                return false;
            }
            moveAudio = MIX_LoadAudio_IO(ChessMixer, soundIO, true, true);
            if (moveAudio == NULL)
            {
                SDL_Log("move Audio :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                MIX_Quit();
                return false;
            }
        }
        if (SDL_strcmp(soundEffects[k], CASTLE_SOUND_FILE_NAME))
        {
            SDL_snprintf(buffer, sizeof(buffer), "%s/%s", SOUNDS_PATH, CASTLE_SOUND_FILE_NAME);
            soundIO = SDL_IOFromFile(buffer, "rb");
            if (soundIO == NULL)
            {
                SDL_Log("Capture Audio IO :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                return false;
            }
            castleAudio = MIX_LoadAudio_IO(ChessMixer, soundIO, true, true);
            if (castleAudio == NULL)
            {
                SDL_Log("Castle Audio :%s\n", SDL_GetError());
                SDL_free(soundEffects);
                MIX_DestroyTrack(sfxTrack);
                MIX_DestroyMixer(ChessMixer);
                MIX_Quit();
                return false;
            }
        }
    }
    SDL_Log("Sounds Loaded!\n");
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

// WRAPPERS ON playSOUND
bool playMoveSound(void)
{
    return playSound(moveAudio);
}
bool playCaptureSound(void)
{
    return playSound(captureAudio);
}
bool playCastleSound(void)
{
    return playSound(castleAudio);
}

// Easy clean up;
void cleanAudio(void)
{
    MIX_DestroyAudio(captureAudio);
    MIX_DestroyAudio(moveAudio);
    MIX_DestroyAudio(castleAudio);
    MIX_DestroyTrack(sfxTrack);
    MIX_DestroyMixer(ChessMixer);
    MIX_Quit();
}