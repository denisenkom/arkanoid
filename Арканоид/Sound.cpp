#include <stdlib.h>
#include <assert.h>
#include <exception>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include "winerrors.h"
#include "dxerrors.h"

#include "a_shared.h"
#include "winarkanoid.h"
#include "Sound.h"
#include "WINException.h"
#include "dsound.h"
#include "dsutil.h"
#include "debug.h"
#include "log.h"

static struct
{
	sound_enum id;
	const char * fname;
} sounds[] = {
	{BALL_HIT, "sounds/bassdrum.WAV"},
	{RICOCHET, "sounds/ricochet.WAV"},
	{BALL_HIT_EDGE, "sounds/swordswi.WAV"},
	{BALL_HIT_BLOCK, "sounds/wowpulse.WAV"},
	{BALL_HIT_BOARD, "sounds/boing.WAV"}
};

static bool initialized = false;
static LPDIRECTSOUND _dsound = NULL;
static LPDIRECTSOUNDBUFFER _soundbuffer = NULL;
static LPDIRECTSOUNDBUFFER _sndbuf[10] = {NULL};

static HINSTANCE hDSound = NULL;

typedef HRESULT (WINAPI * pDirectSoundCreate_t)(const LPGUID, LPDIRECTSOUND *, LPUNKNOWN);
static pDirectSoundCreate_t pDirectSoundCreate = NULL;

void Sound::Init()
{
	assert_winerror(hDSound = LoadLibrary("dsound.dll"));
	assert_winerror(pDirectSoundCreate = (pDirectSoundCreate_t)GetProcAddress(hDSound, "DirectSoundCreate"));
	assert_hres(pDirectSoundCreate(NULL, &_dsound, NULL));
	assert_hres(_dsound->SetCooperativeLevel(global_hWnd, DSSCL_PRIORITY));

	DSBUFFERDESC desc = {0};
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL;
	assert_hres(_dsound->CreateSoundBuffer(&desc, &_soundbuffer, NULL));

	/*char *buf1, *buf2;
	DWORD bufsize1, bufsize2;
	assert_hres(_soundbuffer->Lock(0, 100, (LPVOID*)&buf1, &bufsize1, (LPVOID*)&buf2, &bufsize2, 0));
	for (int i = 0; i < bufsize1; i++)
		buf1[i] = i;
	assert_hres(_soundbuffer->Unlock((LPVOID)buf1, bufsize1, (LPVOID)buf2, bufsize2));
	assert_hres(_soundbuffer->Play(0, 0, DSBPLAY_LOOPING));*/

	for (int i = 0; i < sizeof(sounds)/sizeof(sounds[0]); i++)
	{
		char fname[128];
		memcpy(fname, sounds[i].fname, strlen(sounds[i].fname) + 1 <= 128 ? strlen(sounds[i].fname)+1 : 127);
		fname[127] = 0;
		_sndbuf[sounds[i].id] = DSFLoadSoundBuffer(_dsound, fname);
	}

	initialized = true;
}

void Sound::PlaySnd(sound_enum sound_enum, float volume, long freq)
{
	if (!initialized || !global_hWnd || fMinimized || !fActive)
		return;

	assert(_sndbuf[sound_enum] != 0);

	assert_hres(_sndbuf[sound_enum]->SetVolume((LONG)(volume - 1.0f)*10000));
	assert_hres(_sndbuf[sound_enum]->SetFrequency(freq));

	DWORD status;
	_sndbuf[sound_enum]->GetStatus(&status);

	if (status & DSBSTATUS_PLAYING)
		assert_hres(_sndbuf[sound_enum]->Stop());

	assert_hres(_sndbuf[sound_enum]->Play(0, 0, 0));

	/*if (hres != DS_OK)
	{
		if (hres == DSERR_BUFFERLOST)
		{
			hres = _sndbuf[sound_enum]->Restore();
			if (hres != DS_OK)
				return;
		}
		else
			return;
	}*/
}

void Sound::Shutdown()
{
	initialized = false;

	for (int i = 0; i < sizeof(_sndbuf)/sizeof(_sndbuf[0]); i++)
		if (_sndbuf[i] != NULL)
		{
			_sndbuf[i]->Release();
			_sndbuf[i] = NULL;
		}
	if (_soundbuffer != NULL)
	{
		_soundbuffer->Release();
		_soundbuffer = NULL;
	}
	if (_dsound != NULL)
	{
		_dsound->Release();
		_dsound = NULL;
	}
	if (hDSound != NULL)
	{
		FreeLibrary(hDSound);
		hDSound = NULL;
	}
}