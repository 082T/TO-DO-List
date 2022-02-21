#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <pthread.h>
#include "protokoltxt.h"

/*
	1. port
*/


#define MAXDRETVI 10

typedef struct
{
	int commSocket;
	int indexDretve;
} obradiKlijenta__parametar;


obradiKlijenta__parametar parametarDretve[MAXDRETVI];
pthread_mutex_t lokot_aktivneDretve = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lokot_filesystem = PTHREAD_MUTEX_INITIALIZER;
int aktivneDretve[MAXDRETVI] = { 0 };


char* inttostr(int val, int base){

	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];

}


void krajKomunikacije( void *parametar, const char *ime )
{
	obradiKlijenta__parametar *param = (obradiKlijenta__parametar *) parametar;
	int commSocket  = param->commSocket;
	int indexDretve = param->indexDretve;

	printf( "Kraj komunikacije [dretva=%d, ime=%s]... \n", indexDretve, ime );

	pthread_mutex_lock( &lokot_aktivneDretve );
	aktivneDretve[ indexDretve ] = 2;
	pthread_mutex_unlock( &lokot_aktivneDretve );

	close( commSocket );
}

int isprintaj( int sock, const char *ime, const char *poruka )
{
	if( strlen( poruka ) != 0 ) return NIJEOK;

	FILE *dat;
    FILE *users;
	pthread_mutex_lock( &lokot_filesystem );

    dat=fopen("to_do_list.txt","rt");
    users=fopen("usersave.txt","rt");

        char ch;
        int linije=0;
        while(!feof(dat))
        {
        ch = fgetc(dat);
        if(ch == '\n')
            {
                linije++;
            }
        }
    char *koliko = inttostr (linije, 10);


    posaljiPoruku(sock, PRINT, koliko);

    char line[256];
    rewind(dat);
    char *a;


    int ident;
    char *imestr=malloc(10);
    while (fgets(line, sizeof(line), dat)) {
        fscanf(users,"%d %s\n",&ident,imestr);

        char *identstr = inttostr (ident, 10);

        posaljiPoruku(sock,PRINT,identstr);
        posaljiPoruku(sock,PRINT,line);
    }


    fclose(dat);
    fclose(users);

    free(imestr);

    pthread_mutex_unlock( &lokot_filesystem );

return OK;
}

int addnew (int sock, const char *ime, const char *poruka)
{
    pthread_mutex_lock( &lokot_filesystem );

    char naziv[9];
	char sifra[9];
    char a[9],b[9];

	int ii;
    int j=0;

	for(ii=0;ii<18;ii++)
    {
        if(ime[ii]!=' ')
        {
            naziv[ii]=ime[ii];
        }
        else break;
    }
    naziv[ii]='\0';
    ii++;
    while(ii+j<18)
    {
        sifra[j]=ime[ii+j];
        if(ime[ii+j]=='\0') break;
        j++;
    }


    FILE *dat;
    FILE *users;

    users=fopen("usersave.txt","rt+wt");
    rewind(users);

    int i=1;
    char *us=malloc(9);
    int t;

    while(fscanf(users,"%d %s\n",&t, us) == 2)
    {
        if(t==i) {
            rewind(users);
            i++;
        }
    }

 rewind(users);
 fclose(users);

 dat=fopen("to_do_list.txt","a");
 users=fopen("usersave.txt","a");

 fprintf(users,"%d %s\n",i,naziv);
 fprintf(dat,"%s\n",poruka);

 fclose(dat);
 fclose(users);

 free(us);

    pthread_mutex_unlock( &lokot_filesystem );
    return OK;
}

int deleteelement( int sock, const char *ime , const char *poruka)
{
    pthread_mutex_lock( &lokot_filesystem );

    char naziv[9];
	char sifra[9];
    char a[9],b[9];

	int ii;
    int j=0;

	for(ii=0;ii<18;ii++)
    {
        if(ime[ii]!=' ')
        {
            naziv[ii]=ime[ii];
        }
        else break;
    }
    naziv[ii]='\0';
    ii++;
    while(ii+j<18)
    {
        sifra[j]=ime[ii+j];
        if(ime[ii+j]=='\0') break;
        j++;
    }

    FILE *users, *users2, *dat, *dat2;

    users=fopen("usersave.txt","r");
    rewind(users);

    char prvi[10],drugi[10];
    int nasao=0;

    while( ( fscanf(users, "%s %s\n", prvi, drugi) ) == 2 )
    {
        if (strcmp(prvi,poruka) == 0)
        {
            nasao=1;
            if (strcmp(drugi,naziv)!=0)
            {
                posaljiPoruku(sock,0,"");
                fclose(users);
                pthread_mutex_unlock( &lokot_filesystem );
                return OK;
            }
            else break;
        }
    }
    if (nasao==0)
    {
        posaljiPoruku(sock,0,"");
        fclose(users);
        pthread_mutex_unlock( &lokot_filesystem );
        return OK;
    }
    rewind(users);
    fclose(users);

    users=fopen("usersave.txt","r");
    users2=fopen("usersave2.txt","w");
    dat=fopen("to_do_list.txt","r");
    dat2=fopen("to_do_list2.txt","w");
    rewind(users);
    rewind(users2);
    rewind(dat);
    rewind(dat2);


    char *imestr=malloc(10);
    char line[256];
    int ident;
    int brojident=atoi(prvi);

    while (fgets(line, sizeof(line), dat)) {
        fscanf(users,"%d %s\n",&ident,imestr);

        if(ident==brojident) continue;


        fprintf(dat2,"%s",line);
        fprintf(users2,"%d %s\n",ident,imestr);
    }

    fclose(dat);
    fclose(users);
    remove("usersave.txt");
    remove("to_do_list.txt");
    fclose(users2);
    fclose(dat2);

    rename("usersave2.txt","usersave.txt");
    rename("to_do_list2.txt","to_do_list.txt");


    free(imestr);
    pthread_mutex_unlock( &lokot_filesystem );

    posaljiPoruku(sock,1,"");
    return OK;
}


int changeelement (int sock, const char *ime, const char *poruka)
{

    int vrstaporuke;
    char *tekst;
    primiPoruku(sock, &vrstaporuke, &tekst);

    pthread_mutex_lock( &lokot_filesystem );

    char naziv[9];
	char sifra[9];
    char a[9],b[9];

	int ii;
    int j=0;

	for(ii=0;ii<18;ii++)
    {
        if(ime[ii]!=' ')
        {
            naziv[ii]=ime[ii];
        }
        else break;
    }
    naziv[ii]='\0';
    ii++;
    while(ii+j<18)
    {
        sifra[j]=ime[ii+j];
        if(ime[ii+j]=='\0') break;
        j++;
    }

    FILE *users, *dat, *dat2;

    users=fopen("usersave.txt","r");
    rewind(users);

    char prvi[10],drugi[10];
    int nasao=0;

    while( ( fscanf(users, "%s %s\n", prvi, drugi) ) == 2 )
    {
        if (strcmp(prvi,poruka) == 0)
        {
            nasao=1;
            if (strcmp(drugi,naziv)!=0)
            {
                posaljiPoruku(sock,0,"");
                fclose(users);
                pthread_mutex_unlock( &lokot_filesystem );
                return OK;
            }
            else break;
        }
    }

    if (nasao==0)
    {
        posaljiPoruku(sock,0,"");
        fclose(users);
        pthread_mutex_unlock( &lokot_filesystem );
        return OK;
    }
    rewind(users);
    fclose(users);


    users=fopen("usersave.txt","r");
    dat=fopen("to_do_list.txt","r");
    dat2=fopen("to_do_list2.txt","w");

    rewind(users);
    rewind(dat);
    rewind(dat2);


    char *imestr=malloc(10);
    char line[256];
    int ident;
    int brojident=atoi(prvi);

    while (fgets(line, sizeof(line), dat)) {
        fscanf(users,"%d %s\n",&ident,imestr);

        if(ident==brojident)
        {
            fprintf(dat2,"%s\n",tekst);
        }

        else fprintf(dat2,"%s",line);
    }

    fclose(dat);
    fclose(users);
    remove("to_do_list.txt");
    fclose(dat2);

    rename("to_do_list2.txt","to_do_list.txt");


    free(imestr);

    pthread_mutex_unlock( &lokot_filesystem );

    posaljiPoruku(sock,1,"");
    return OK;


}


int obradiLOGIN( int sock, const char *ime, const char *poruka )
{

	pthread_mutex_lock( &lokot_filesystem );
	FILE *dat;

	char naziv[9];
	char sifra[9];
    char a[9],b[9];

	int i;
    int j=0;

	for(i=0;i<18;i++)
    {
        if(ime[i]!=' ')
        {
            naziv[i]=ime[i];
        }
        else break;
    }
    naziv[i]='\0';
    i++;
    while(i+j<18)
    {
        sifra[j]=ime[i+j];
        if(ime[i+j]=='\0') break;
        j++;
    }

    printf("naziv je %s\n",naziv);
    printf("sifra je %s\n",sifra);


	dat = fopen("loginfile.txt", "rt+wt");
    rewind(dat);

    while (fscanf(dat, "%s %s\n", a, b) == 2)
	{
		if (strcmp(a, naziv) == 0)
		{
		    pthread_mutex_unlock( &lokot_filesystem );
			if (strcmp(b, sifra) != 0)
            {
                posaljiPoruku( sock, NIJEOK, "" );

                return 0;
            }
			if (strcmp(b, sifra) == 0)
			{
			    posaljiPoruku( sock, OK, "" );

			    return 1;
			}
		}
	}
    rewind(dat);

    while(fscanf(dat,"%s %s\n",a,b)==2)
    {}

    fprintf(dat, "%s %s\n", naziv, sifra);
    fclose(dat);

    pthread_mutex_unlock( &lokot_filesystem );
    posaljiPoruku( sock, OK, "" );
    return 1;
}


void *obradiKlijenta( void *parametar )
{
	obradiKlijenta__parametar *param = (obradiKlijenta__parametar *) parametar;
	int commSocket  = param->commSocket;

	int vrstaPoruke;
	char *poruka;


	// prvo trazi login
	if( primiPoruku( commSocket, &vrstaPoruke, &poruka ) != OK )
	{
		krajKomunikacije( parametar, "" );
		return NULL;
	}
	if( vrstaPoruke != LOGIN || strlen( poruka ) > 17)
	{
		krajKomunikacije( parametar, "" );
		return NULL;
	}

	char imeKlijenta[18];
	strcpy( imeKlijenta, poruka );

 if( obradiLOGIN( commSocket, imeKlijenta, poruka ) != OK )
	{
		krajKomunikacije( parametar, imeKlijenta );
		return NULL;
	}
	free( poruka );


    int gotovo = 0;

    while( !gotovo )
    {

        if( primiPoruku( commSocket, &vrstaPoruke, &poruka ) == NIJEOK )
            {
                krajKomunikacije( parametar, imeKlijenta );
                gotovo = 1;
                continue;
            }

        switch( vrstaPoruke )
            {
                case BYE:
                    krajKomunikacije( parametar, imeKlijenta );
                    gotovo = 1;
                    break;
                case PRINT:
                    if( isprintaj( commSocket, imeKlijenta, poruka ) != OK )
                    {
                    krajKomunikacije( parametar, imeKlijenta ); gotovo = 1;
                    }
                    break;
                case DODAJ:
                    if( addnew( commSocket, imeKlijenta, poruka ) != OK )
                    {
                    krajKomunikacije( parametar, imeKlijenta ); gotovo = 1;
                    }
                    break;
                case BRISI:
                    if( deleteelement( commSocket, imeKlijenta, poruka) !=OK )
                    {
                    krajKomunikacije( parametar, imeKlijenta ); gotovo=1;
                    }
                    break;
                case PROMJENI:
                    if ( changeelement( commSocket, imeKlijenta, poruka) != OK )
                    {
                    krajKomunikacije( parametar, imeKlijenta ); gotovo=1;
                    }
                    break;

                default: krajKomunikacije( parametar, imeKlijenta );
            }


    free( poruka );
    }

	return NULL;
}


int main(int argc, char** argv) {


if (argc != 2)
	{
		printf("Argc must be = 2");
		exit(1);
	}


int port;
	sscanf(argv[1], "%d", &port);

	// socket...
	int listenerSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (listenerSocket == -1)
		perror("socket");

    // bind...
	struct sockaddr_in mojaAdresa;

	mojaAdresa.sin_family = AF_INET;
	mojaAdresa.sin_port = htons(port);
	mojaAdresa.sin_addr.s_addr = INADDR_ANY;
	memset(mojaAdresa.sin_zero, '\0', 8);

	if (bind(listenerSocket, (struct sockaddr*) & mojaAdresa, sizeof(mojaAdresa)) == -1)
		perror("bind");

	// listen...
	if (listen(listenerSocket, 10) == -1)
		perror("listen");


    pthread_t dretve[10];



while (1) {

		// accept...
		struct sockaddr_in klijentAdresa;
		unsigned int lenAddr = sizeof(klijentAdresa);
		int commSocket = accept(listenerSocket, (struct sockaddr*) & klijentAdresa,	&lenAddr);

		if (commSocket == -1)
			perror("accept");

		char* dekadskiIP = inet_ntoa(klijentAdresa.sin_addr);

		printf("Prihvatio konekciju od %s ", dekadskiIP);

		pthread_mutex_lock(&lokot_aktivneDretve);
		int i, indexNeaktivne = -1;

		for (i = 0; i < MAXDRETVI; ++i)
		{
			if (aktivneDretve[i] == 0)
				indexNeaktivne = i;
			else if (aktivneDretve[i] == 2)
			{
				pthread_join(dretve[i], NULL);
				aktivneDretve[i] = 0;
				indexNeaktivne = i;
			}
		}

		if (indexNeaktivne == -1)
		{
			close(commSocket);
			printf("No more threads\n");
		}
		else
		{
			aktivneDretve[indexNeaktivne] = 1;
			parametarDretve[indexNeaktivne].commSocket = commSocket;
			parametarDretve[indexNeaktivne].indexDretve = indexNeaktivne;
			printf("Using thread number = %d.\n", indexNeaktivne);

			pthread_create(&dretve[indexNeaktivne], NULL, obradiKlijenta, &parametarDretve[indexNeaktivne]);
		}
		pthread_mutex_unlock(&lokot_aktivneDretve);

}

    return 0;
}


