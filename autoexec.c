/* Ce programme ex‚cute les fichiers pr‚sents dans \autoexec.cnf,
 * ‚ventuellement avec leur ligne de commande. C'est fait pour palier
 * au fait qu'il n'est pas possible d'appeler les programmes du dossier
 * AUTO avec des paramŠtres sur la ligne de commande.
 * Vinz/MPS 30 D‚cembre 2023
 * D‚velopp‚ sur Foenix A2560U avec MicroEmacs et Pure C
 */

#include <tos.h>
#include <stdlib.h>
#include <string.h>

#define BOOL  int
#define TRUE  -1
#define FALSE 0
#define UBYTE unsigned char
#define WORD short
#define LONG long
#define NULL 0L
#define CRLF "\r\n"


DTA dta;
DTA *old_dta = NULL;
WORD fh = 0;
char *cfg = NULL;
char *line;
char ptail[125];

/* Config options */
BOOL verbose = FALSE;

#define MSG_CFG_FILE_NOT_FOUND "Fichier autoexec.cnf absent\r\n"
#define MSG_NOT_ENOUGH_MEMORY "Plus assez de m‚moire\r\n"
#define MSG_CFG_READ_ERROR "Erreur de lecture du fichier de config\r\n"
#define MSG_TAIL_TOO_BIG1 "La ligne de commande pour "
#define MSG_TAIL_TOO_BIG2 " d‚passe 124 caractŠres. \033qPRG ignor‚!\033p\r\n"


static char *skip_blank(char *l) {
	while (*l && (*l == ' ' || *l == '\t'))
			l++;
	return l;
}


static char *skip_nonblank(char *l) {
	while (*l && (*l != ' ' && *l != '\t'))
			l++;
	return l;
}


static void cleanup(void) {
	if (old_dta)
		Fsetdta(old_dta);

	if (fh)
		Fclose(fh);

	if (cfg)
		free(cfg);
		
	exit(0);
}


WORD main(void) {
	LONG size;
	char *eof;
	char cfg_filename[] = "x:\\autoexec.cnf";
	int  tail_size;
	
	old_dta = Fgetdta();
	Fsetdta(&dta);

	/* Charge le fichier de config */
	cfg_filename[0] = 'A' + (Dgetdrv() & 0xff);
	fh = Fopen(cfg_filename,O_RDONLY);
	if (fh < 0) {
	  Cconws(cfg_filename);Cconws(CRLF);
		Cconws(MSG_CFG_FILE_NOT_FOUND);
		cleanup();
	}

	size = Fseek(0L, fh, SEEK_END);
	if (size < 0)
		cleanup();
	Fseek(0L, fh, 0);

	cfg = (char*)malloc(size+1/*on ajoute un NULL … la fin pour terminer la chaine*/);
	if (!cfg) {
		Cconws(MSG_NOT_ENOUGH_MEMORY);
		cleanup();
	}

	if (Fread(fh, size, cfg) != size) {
		Cconws(MSG_CFG_READ_ERROR);
		cleanup();
	}
	cfg[size] = '\0'; /* C'est plus s–r ! */

	line = cfg;
	eof = cfg+size;
	while (*line && line < eof) {
		char *eol;
		char *tail;
		BOOL skip_line = FALSE;
		
		/* Ignore les lignes vides */
		while (line < eof && (line[0] == '\r' || line[0] == '\n'))
			line++;
			
		/* Trouve la fin de la ligne */
		for (eol = line; eol < eof && *eol != '\r' && *eol != '\n'; eol++);
		*eol = '\0';

		/* Ignore les blanks initiaux */
		line = skip_blank(line);
		if (*line == '\0')
			cleanup();

		/* Ignore les lignes de commentaire (commencent par '#') */
		if (*line == '#') {
			skip_line = TRUE;
		}
		else if (strcmp("*VERBOSE ON",line) == 0) {
			verbose = TRUE;
			skip_line = TRUE;
		}
		else if (strcmp("*VERBOSE OFF",line) == 0) {
			verbose = FALSE;
			skip_line = TRUE;
		}
		
		if (skip_line) {
			line = eol + 1;
			continue;
		}

		/* Trouve un espace d‚limitant la commande et la ligne de commande */
		tail = skip_nonblank(line);
		if (!tail[0]) {
			/* Pas de ligne de commande */
			tail = NULL;
		}
		else {
			tail[0] = '\0'; /* Termine la commande */
			tail++;
			tail = skip_blank(tail);
		}


		if (*line) {
			int tail_size;

			if (verbose) {
				Cconws("\033p");Cconws(line);Cconws("\033q ");
				Cconws(tail?tail:"");Cconws(CRLF);
			}

			tail_size = (int)strlen(tail);
			if (tail_size > 124) {
				/* TOS limitation */
				Cconws(MSG_TAIL_TOO_BIG1);
				Cconws(*line);
				Cconws(MSG_TAIL_TOO_BIG2);
			}
			else {
				ptail[0] = (UBYTE)tail_size;
				strncpy(&ptail[1],tail,124);
				Pexec(PE_LOADGO,line,ptail,0L);
			}
		}

		line = eol+1;
	}	

	cleanup();

	return 0; /* Unreachable */
}

