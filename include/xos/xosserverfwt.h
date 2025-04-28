

typedef struct
{	char  *name;
	void (*func)(char *cmd, int len);
} SFWCMDTBL;

#define SFWRSP_SEND   0x01
#define SFWRSP_LOG    0x02
#define SFWRSP_PREFIX 0x04
#define SFWRSP_FINAL  0x08

// The following items are defined by the server

extern char      prgname[48];
extern int       srvmajver;
extern int       srvminver;
extern int       srveditnum;
extern SFWCMDTBL srvcmdtbl[];

// The following items are defined by the server framework

extern char      sfwReqName[64];
extern char     *sfwLogFile;
extern CRIT      sfwTrmCrit;

// The following functions are defined by the server

void srvmessage(char *msg, int len);
void srvsetupcmd(void);
int  srvsetup(void);

// The following functions are defined by the server framework and may be
//   called by the server

long sfwThreadInit(HUMA *args);
#pragma aux sfwThreadInit aborts;
void sfwResponse(long code, int bits, char *fmt, ...);
