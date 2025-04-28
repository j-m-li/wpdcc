XOSCALL svcSysDateTime(long func, void *data);
XOSCALL svcSysGetEnv(cchar *name, cchar **pstr);
XOSCALL svcIoOpen(long bits, const char *name, void *parm);
XOSCALL svcIoClose(long dev, long bits);
XOSCALL svcIoSetPos(long dev, long pos, long mode);
XOSCALL svcIoOutBlock(long dev, char *data, long size);
XOSCALL svcMemChange(void *base, long bits, long size);
