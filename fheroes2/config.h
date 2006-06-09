#ifndef _CONFIG_H
#define _CONFIG_H

#include "SDL.h"
#include "gamedefs.h"

void InitConfig(const char *);

char	*GetStrValue(const char *);
Uint8	GetIntValue(const char *);
void	SetIntValue(const char *, Uint8);
void	SetStrValue(const char *, const char *);
#endif
