// Definitions for SNDA class sound devices

// Define IO parameters

#define IOPAR_SNDA_OUTSTATE 0x8001
#define IOPAR_SNDA_OUTRATE  0x8002
#define IOPAR_SNDA_OUTMODE  0x8003
#define IOPAR_SNDA_OUTVOL   0x8004

// Define special device functions

#define SPDF_SNDA_MAPOUTBFR 1

// Define output modes

#define SNDA_MODE_8MONO    0
#define SNDA_MODE_8STEREO  1
#define SNDA_MODE_16MONO   2
#define SNDA_MODE_16STEREO 3

// Define output states

#define SNDA_STATE_STOP  0
#define SNDA_STATE_START 1
#define SNDA_STATE_CONT  2
#define SNDA_STATE_FINAL 3
