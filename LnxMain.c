/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>

#include "PsxCommon.h"
#include "Sio.h"
#ifdef IPHONE
extern const char* get_resource_path(char* file);
extern const char* get_documents_path(char* file);
#endif

int __saved = 0;

PcsxConfig Config;
R3000Acpu *psxCpu;
psxRegisters* psxRegs;
u32 *psxMemWLUT;
u32 *psxMemRLUT;
s8 *psxH;
s8 *psxR;
s8 *psxP;
s8 *psxM;
psxCounter psxCounters[5];
unsigned long psxNextCounter, psxNextsCounter;
cdrStruct cdr;
char CdromId[9];
char CdromLabel[11];
FILE *emuLog;
long LoadCdBios;
int StatesC;
int cdOpenCase;
int NetOpened;
s8* packfile = NULL;
s8 gamepath[1024]={ '.', '/', 0, 0 };
s8 save_filename[1024];

static void ChangeWorkingDirectory(char *exe)
{
  s8 exepath[1024];
  s8* s;
  sprintf(exepath, "%s", exe);
	s = strrchr(exepath, '/');
	if (s != NULL) {
		*s = '\0';
		chdir(exepath);
	}
}

#ifdef IPHONE
#define main iphone_main
extern 
#endif
int main(int argc, char *argv[])
{
	char *file = NULL;
	char *lang;
	int runcd = 0;
	char* loadst = NULL;
	int i;

  __saved = 0;
	memset(&Config, 0, sizeof(PcsxConfig));
	Config.HLE = 0;
	sprintf(Config.Bios, "/scph1001.bin");
	
	strcpy(Config.Net, _("Disabled"));
#ifdef IPHONE
  ChangeWorkingDirectory(get_documents_path("psx4iphone"));
#else
	ChangeWorkingDirectory(argv[0]);
#endif
	getcwd(gamepath, 1024);
	sprintf(Config.BiosDir, "%s", gamepath);
	sprintf(Config.Mcd1, "%s/mcd001.mcr", gamepath);
	sprintf(Config.Mcd2, "%s/mcd002.mcr", gamepath);
	Config.PsxAuto = 1;
	Config.Cdda = 0;
	Config.Xa = 0;
#ifdef DYNAREC
	Config.Cpu = 0;
#else
	Config.Cpu = 1;
#endif
	Config.Mdec = 0;
	Config.PsxOut = 0;
	Config.PsxType = 0;
	Config.QKeys = 0;
	Config.RCntFix = 0 ;
	Config.Sio = 0;
	Config.SpuIrq = 0;
	Config.VSyncWA = 0;

	for (i=1; i<argc; i++) {
		if (!strcmp(argv[i], "-runcd")) runcd = 1;
		else if (!strcmp(argv[i], "-runcdbios")) runcd = 2;
		else if (!strcmp(argv[i], "-psxout")) Config.PsxOut = 1;
		else if (!strcmp(argv[i], "-load")) loadst = argv[++i];
		else if (!strcmp(argv[i], "-hle")) 
		{
		  Config.HLE = 1;
    	sprintf(Config.Bios, "HLE");
		}
		else if (!strcmp(argv[i], "-h") ||
			 !strcmp(argv[i], "-help")) {
			 printf("%s %s\n", argv[0], _(
			 				"[options] [file]\n"
							"\toptions:\n"
							"\t-runcd\t\tRuns CdRom\n"
							"\t-runcdbios\tRuns CdRom Through Bios\n"
							"\t-hle\t\tUse HLE\n"
							"\t-psxout\t\tEnable psx output\n"
							"\t-load STATENUM\tLoads savestate STATENUM (1-5)\n"
							"\t-h -help\tThis help\n"
							"\tfile\t\tLoads file\n"));
			 return 0;
		} else file = argv[i];
	}
	packfile = file;

	gp2x_init(1000, 16, 11025, 16, 1, 60, 1);

#ifdef MAEMO_CHANGES
	hildon_init (&argc, &argv);
#endif

	if (SysInit() == -1) return 1;

	if (OpenPlugins() == -1) return 1;

	SysReset();

	CheckCdrom();

	/*if (file != NULL) Load(file);
	else*/
	{
		if (runcd == 1) {
			LoadCdBios = 0;
			if (LoadCdrom() == -1) {
			  
				ClosePlugins();
				
				printf(_("Could not load Cdrom\n"));
				return -1;
			}
		} 
		else if (runcd == 2) 
		{
			LoadCdBios = 1;
		}
	}

	if (loadst)
	{
    __saved = 1;
		sprintf (save_filename, "%s", loadst);
		LoadState(save_filename);
	}

	psxCpu->Execute();
	return 0;
}

DIR *dir;

void InitLanguages() {
}

char *GetLanguageNext() {
	return NULL;
}

void CloseLanguages() {
}

void ChangeLanguage(char *lang) {
}

int SysInit() {

#ifdef GTE_DUMP
	gteLog = fopen("gteLog.txt","wb");
	setvbuf(gteLog, NULL, _IONBF, 0);
#endif

#ifdef EMU_LOG
#ifndef LOG_STDOUT
	emuLog = fopen("emuLog.txt","wb");
#else
	emuLog = stdout;
#endif
	setvbuf(emuLog, NULL, _IONBF, 0);
#endif

	psxInit();
	//LoadPlugins();

	LoadMcds(Config.Mcd1, Config.Mcd2);

	return 0;
}

void SysReset() {
	psxReset();
}

void SysClose() {
	psxShutdown();
	ClosePlugins();

	if (emuLog != NULL) fclose(emuLog);
	
  gp2x_deinit();
#ifdef IPHONE
  pthread_exit(NULL);
#endif
}

void SysPrintf(char *fmt, ...) {
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (Config.PsxOut) printf ("%s\n", msg);
#ifdef EMU_LOG
#ifndef LOG_STDOUT
	fprintf(emuLog, "%s\n", msg);
#endif
#endif
}

void *SysLoadLibrary(char *lib) {
	return NULL;
}

void *SysLoadSym(void *lib, char *sym) {
	return NULL;
}

const char *SysLibError() {
	return NULL;
}

void SysCloseLibrary(void *lib) {
}

void SysUpdate() {
}

void SysRunGui() {
}

void SysMessage(char* fmt, ...) {
#ifdef EMU_LOG

	va_list list;

	va_start(list, fmt);
	vsprintf(__sysmsg, fmt, list);
	va_end(list);

	__sysmsg[127]=0;

	if (Config.PsxOut) printf ("%s\n", __sysmsg);
	//SysPrintf("%s", __sysmsg);
	fprintf(emuLog, "%s\r\n", __sysmsg);
	fflush(emuLog);
#endif
}

