
namespace Sound {
	void Init();
	void Shutdown();

	void PlaySnd(sound_enum sound_enum, float volume, long freq);
	int LoadSound(const char* soundname);
	void PlaySnd(int soundid, float volume, long freq);
};