extern volatile long sndActive;

int sndInitalize(long *stkmem, char *bufrmemarg, long bufrsizearg,
		char *inpmemarg, long inpsizearg, long notifyarg, long sndvectarg,
		void (*snderrorarg)(long code, char *msg));
int sndPlay(char *filespecarg, long  volumearg, long notcntarg,
		long notifyarg);
int sndTone(long freqarg, long timearg, long volumearg, long notcntarg,
		long notifyarg);
int sndStop(void);
