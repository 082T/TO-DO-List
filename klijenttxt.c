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

/*
    1. korisnicko ime
    2. lozinka
    3. ip
    4. port
*/


void napravi_login(int mojSocket, const char *mojeIme, const char *mojaLozinka) {

    char info[18];

    strcpy(info, mojeIme);
    strcat(info, " ");
    strcat(info, mojaLozinka);

    if (posaljiPoruku(mojSocket, LOGIN, info) == 0)
    {
        printf("Login error");
        exit(5);
    }


    int okej;
    char* bzvz;

    if (primiPoruku(mojSocket, &okej, &bzvz) != OK)
    {
        printf("unexpected error, exiting...\n");
        exit(5);
    }

    if (okej != OK)
    {
        printf("login error, exiting...\n");
        exit(6);
    }
    free(bzvz);
}

void napravi_dodaj(int sock, const char *ime)
{

    char poruka[128];

    printf("Message: ");
    scanf ("%[^\n]%*c", poruka);

    printf("\nAdding element... ");

    if( posaljiPoruku( sock, DODAJ, poruka ) == NIJEOK )
    {
            printf("Adding error, exiting...");
            exit(8);
    }
    printf("Element added!");
    printf("\n\n");
}

void napravi_izbrisi(int sock, const char *ime)
{
    char broj[10];
    printf("Identification number: ");
    scanf("%s",broj);

    printf("Deleting element %s...\n",broj);

    if( posaljiPoruku( sock, BRISI, broj ) == NIJEOK )
    {
            printf("Deleting error, exiting...");
            exit(9);
    }
    printf("\n");

    int vrstaporuke;
    char *bezveze;
    primiPoruku(sock,&vrstaporuke,&bezveze);
    if(vrstaporuke==0) printf("Error, could not delete element %s\n",broj);
    else printf("%s deleted successfully!\n",broj);
    printf("\n");

}

void napravi_promjeni(int sock, const char *ime)
{

    char broj[10];
    printf("Identification number: ");
    scanf("%[^\n]%*c",broj);

    char poruka[128];
    printf("Message: ");
    scanf ("%[^\n]%*c", poruka);

    printf("Changing element %s...\n",broj);

    if( posaljiPoruku( sock, PROMJENI, broj ) == NIJEOK )
    {
            printf("Changing error 1, exiting...");
            exit(10);
    }
    if( posaljiPoruku( sock, PROMJENI, poruka )  == NIJEOK )
    {
            printf("Changing error 2, exiting...");
            exit(11);
    }
    printf("\n");

    int vrstaporuke;
    char *bezveze;
    primiPoruku(sock,&vrstaporuke,&bezveze);
    if(vrstaporuke==0) printf("Error, could not change element %s\n",broj);
    else printf("%s changed successfully!\n",broj);
    printf("\n");
}



void napravi_bye( int sock, const char *ime)
{
	if( posaljiPoruku( sock, BYE, "" ) == NIJEOK )
		error1( "Error in BYE...exiting.\n" );

	close( sock );
}

void napravi_print( int sock, const char *ime)
{

    if( posaljiPoruku( sock, PRINT, "" ) == NIJEOK )
    {
            printf("Print error, exiting...");
            exit(7);
    }

    int vrstaporuke;
    char *popiskoliko;
    char *popislinija;

    if (primiPoruku(sock, &vrstaporuke, &popiskoliko) ==NIJEOK)
    {
            printf("Printing error, exiting...");
            exit(8);
    }

    int koliko=atoi(popiskoliko);

    printf("Number of elements: %d;\n\n",koliko);

    for(int i=0;i<koliko;i++)
    {
     char *brojke=malloc(10);
     primiPoruku(sock,&vrstaporuke,&brojke);
     primiPoruku(sock,&vrstaporuke,&popislinija);

     printf("%s ",brojke);

     printf("%s",popislinija);
     free(brojke);
    }
printf("\n\n");
}


int main(int argc, char** argv) {

    if (argc != 5)
    {
        printf("Argc must be = 5");
        exit(1);
    }
    if (strlen(argv[1]) > 8)
    {
        printf("Max name length = 8\n");
        exit(12);
    }
    if (strlen(argv[2]) > 8) 
    {
        printf("Max password length = 8\n");
        exit(12);
    }
    char mojeIme[9];
    strcpy(mojeIme, argv[1]);
            mojeIme[8] = '\0';
    char mojaLozinka[9];
    strcpy(mojaLozinka, argv[2]);
        mojaLozinka[8] = '\0';


    char* dekadskiIP = argv[3];
    int port;
    sscanf(argv[4], "%d", &port);


    /* socket */
    int mojSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mojSocket == -1)
    {
        perror("socket");
        exit(2);
    }

    /* connect */
    struct sockaddr_in adresaServera;
    adresaServera.sin_family = AF_INET;
    adresaServera.sin_port = htons(port);

    if (inet_aton(dekadskiIP, &adresaServera.sin_addr) == 0)
    {
        printf("Adress not valid");
        exit(3);
    }

    memset(adresaServera.sin_zero, '\0', 8);

    /* connect */
    if (connect(mojSocket, (struct sockaddr*) & adresaServera, sizeof(adresaServera)) == -1)
    {
        perror("connect");
        exit(4);
    }

    napravi_login(mojSocket, mojeIme, mojaLozinka);


    int t=0;
    int gotovo = 0;
	while( !gotovo )
	{
	    if(t==0)
	    {
		printf( "\n\nChoices: \n");
		printf( "\t0. exit\n" );
		printf( "\t1. check list\n" );
		printf( "\t2. add element\n" );
		printf( "\t3. change existing element\n" );
		printf( "\t4. delete an element\n\n" );
        t=1;
	    }

		int izbor;
        char enter;
        scanf("%d", &izbor);
        scanf("%c",&enter);

        switch( izbor )
		{
			case 0:
			    napravi_bye(mojSocket, mojeIme);
                gotovo = 1;
                break;
            case 1:
                napravi_print(mojSocket,mojeIme);
                break;
            case 2:
                napravi_dodaj(mojSocket,mojeIme);
                break;
            case 3:
                napravi_promjeni(mojSocket,mojeIme);
                break;
            case 4:
                napravi_izbrisi(mojSocket,mojeIme);
                break;


			default: printf( "Try again: " );
		}

	}


printf("All done, exiting...\n");
close(mojSocket);
	return 0;
}


