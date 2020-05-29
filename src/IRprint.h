// Helper functions for printing IR codes

#include <IRutils.h>

#ifndef IRprint
#define IRprint

void fullCode (decode_results *results);
void dumpInfo(decode_results *results);
void dumpRaw(decode_results *results);
void dumpCode(decode_results *results);

#endif