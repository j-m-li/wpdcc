// Devfine SND device parameters

#define IOPAR_SND_VOLUME 0x8001	// Volume
#define IOPAR_SND_RATE   0x8002	// Sample rate
#define IOPAR_SND_WIDTH  0x8003	// Sample width
#define IOPAR_SND_CHNLS  0x8004	// Number of channels
#define IOPAR_SND_FIFOSZ 0x8005	// Get FIFO size
#define IOPAR_SND_MAPREG 0x8006	// Map device registers

// Define values for the bits in the SPECIAL function value

#define SDF_SND_START   0x0001	// Start IO
#define SDF_SND_STOP    0x0002	// Stop IO
#define SDF_SND_SETBUFR 0x0004	// Set up buffer
#define SDF_SND_RELBUFR 0x0008	// Release buffer
#define SDF_SND_OUTSTRM 0x0010	// Set up output stream
#define SDF_SND_INSTRM  0x0020	// Set up input stream
#define SDF_SND_RELSTRM 0x0040	// Release stream
