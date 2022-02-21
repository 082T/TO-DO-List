#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "protokoltxt.h"

int posaljiPoruku(int sock, int vrstaPoruke, const char *poruka)
{
	int duljinaPoruke = strlen(poruka);
	int poslano, poslanoZadnje;

	int duljinaPoruke_n = htonl(duljinaPoruke);
	poslanoZadnje = send(sock, &duljinaPoruke_n, sizeof(duljinaPoruke_n), 0);
	if(poslanoZadnje != sizeof(duljinaPoruke_n)) return NIJEOK;

	int vrstaPoruke_n = htonl(vrstaPoruke);
	poslanoZadnje = send(sock, &vrstaPoruke_n, sizeof(vrstaPoruke_n), 0);
	if(poslanoZadnje != sizeof(vrstaPoruke_n)) return NIJEOK;

	poslano = 0;
	while(poslano != duljinaPoruke)
	{
		poslanoZadnje = send(sock, poruka+poslano, duljinaPoruke-poslano, 0);

		if(poslanoZadnje == -1 || poslanoZadnje == 0) return NIJEOK;
		poslano += poslanoZadnje;
	}

	return OK;
}


int primiPoruku(int sock, int *vrstaPoruke, char **poruka)
{
	int primljeno, primljenoZadnje;
	int duljinaPoruke_n, duljinaPoruke;
	
	primljenoZadnje = recv(sock, &duljinaPoruke_n, sizeof(duljinaPoruke_n),0);
	if(primljenoZadnje != sizeof(duljinaPoruke_n)) return NIJEOK;
	duljinaPoruke = ntohl(duljinaPoruke_n);

	int vrstaPoruke_n;
	primljenoZadnje = recv(sock, &vrstaPoruke_n, sizeof(vrstaPoruke_n),0);
	if(primljenoZadnje != sizeof(primljenoZadnje)) return NIJEOK;
	*vrstaPoruke = ntohl(vrstaPoruke_n);

	*poruka = (char*)malloc((duljinaPoruke+1)*sizeof(char));

	primljeno = 0;
	while(primljeno != duljinaPoruke)
	{
		primljenoZadnje = recv(sock, *poruka+primljeno, duljinaPoruke-primljeno, 0);

		if(primljenoZadnje == -1 || primljenoZadnje == 0) return NIJEOK;
		primljeno += primljenoZadnje;
	}

	(*poruka)[duljinaPoruke] = '\0';

	return OK;
}
