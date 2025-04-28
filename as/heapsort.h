typedef int HEAPCMP(void *a, void *b, void *data);

void  *heapsort(void *root, HEAPCMP *comp, void *data);
