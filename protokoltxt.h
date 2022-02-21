#ifndef __MESSAGEPROTOCOL_H_
#define __MESSAGEPROTOCOL_H_

#define LOGIN       	1
#define BYE         	2
#define PRINT           3
#define DODAJ           4
#define BRISI           5
#define PROMJENI        6

#define OK      1
#define NIJEOK  0

int primiPoruku( int sock, int *vrstaPoruke, char **poruka );
int posaljiPoruku( int sock, int vrstaPoruke, const char *poruka );

#define error1(s) {printf(s); exit(0);}
#define error2(s1, s2) {printf(s1, s2); exit(0);}
#define myperror(s) {perror(s); exit(0);}

#endif
