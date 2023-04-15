#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define PORT 9999
#define MAX 20

// client

/*structures*/

typedef struct
{
  char nom[MAX];
  char prenom[MAX];
  char gsm[MAX];
  char email[50];
  char rue[MAX];
  char ville[MAX];
  char pays[MAX];

} Contact;

/*functions*/
void error(const char *msg);
void emptyBuffer();
void saisir(int sock, Contact *e);
void Afficher(Contact e);

/*main*/
int main(int argc, char *argv[])
{

  char buffer[1024];

  bzero(buffer, sizeof(buffer));

  /*nom et prenom a chercher ou a supprimer*/
  char nom[MAX];
  char prenom[MAX];
  bzero(nom, sizeof(nom));
  bzero(prenom, sizeof(prenom));

  Contact c;

  /*used to send an int value */
  int converted_number, number_to_send;

  /*Create a socket */

  int sock, valread, client_fd;

  struct sockaddr_in serv_addr;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    error("SOCKET NOT CREATED");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  // Convert IPv4 and IPv6 addresses from text to binary form

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    error("ADDRESS NOT SUPPORTED");
  }

  /*connecting*/
  if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
  {
    error("CONNECTION FAILED");
  }

  /*reading the hello msg sent by the server*/
  valread = read(sock, buffer, 1024);
  printf("%s\n", buffer);
  bzero(buffer, sizeof(buffer));

  /*input log in data*/

  char username[20];
  char password[20];
  char profile[20];

  int count = 0;
  int Auth = 1;

  /*Authentification process*/
  do
  {
    printf("Username:\t");
    scanf("%s", username);
    username[strlen(username)] = '\0';
    printf("\n");
    send(sock, username, strlen(username), 0);

    emptyBuffer();

    printf("Password:\t");
    scanf("%s", password);
    password[strlen(password)] = '\0';
    send(sock, password, strlen(password), 0);
    emptyBuffer();

    valread = 0;
    bzero(buffer, sizeof(buffer));

    valread = read(sock, buffer, 1024);
    buffer[strlen(buffer)] = '\0';
    printf("%s\n", buffer);

    if (strcmp(buffer, "No such user!\n\0") == 0)
    {
      count++;
      Auth = 0;
    }
    else
    {
      break;
    }

    if (count > 2)
    {
      printf("Session ended\n");
      close(client_fd);
      exit(1);
    }

  } while (Auth == 0);

  /*The menu*/

  valread = 0;
  valread = read(sock, profile, 20);
  profile[strlen(profile)] = '\0';
  printf("\n\n------ %s menu :-------------\n", profile);

  int choice_admin = 0;
  int choice_user = 0;

  bzero(buffer, sizeof(buffer));
  valread = 0;
  char menu_admin[] = "\n\n1-Ajouter un contact\n2-Rechercher un contact\n3-Supprimer un contact\n4-Modifier un contact\n5-Afficher tous les contacts\n6-Quitter\n\n\0";
  char menu_user[] = "\n\n1-Rechercher un contact\n2-Afficher tous les contacts\n3-Quitter\n\n\0";

  if (strcmp(profile, "admin") == 0)
  {

    while (1)
    {
      // valread = read(sock, buffer, 1024);
      // buffer[strlen(buffer)] = '\0';
      printf("%s\n", menu_admin);

      printf("\nVous voulez faire : ");
      scanf("%d", &choice_admin);
      fflush(stdin);

      printf("\n\nProcessing the operation number %d\n\n", choice_admin);

      converted_number = 0;

      number_to_send = choice_admin;
      converted_number = htonl(number_to_send);

      // Write the number to the opened socket
      write(sock, &converted_number, sizeof(converted_number));

      if (choice_admin == 6)
      {
        break;
      }
      bzero(buffer, sizeof(buffer));
      valread = 0;

      switch (choice_admin)
      {
      case 1:

        bzero(&c, sizeof(c));
        saisir(sock, &c);
        Afficher(c);

        break;
      case 2:
        printf("\nLe nom a chercher: ");
        scanf("%s", nom);
        send(sock, nom, strlen(nom), 0);
        emptyBuffer();
        printf("\nLe prenom a chercher: ");
        scanf("%s", prenom);
        send(sock, prenom, strlen(prenom), 0);
        emptyBuffer();
        bzero(nom, sizeof(nom));
        bzero(prenom, sizeof(prenom));
        bzero(buffer, sizeof(buffer));

        valread = recv(sock, buffer, sizeof(buffer), 0);
        buffer[strlen(buffer)] = '\0';
        printf("%s", buffer);
        bzero(buffer, sizeof(buffer));

        break;
      case 3:
        printf("\nLe nom a supprimer: ");
        scanf("%s", nom);
        send(sock, nom, strlen(nom), 0);
        emptyBuffer();
        printf("\nLe prenom a supprimer: ");
        scanf("%s", prenom);
        send(sock, prenom, strlen(prenom), 0);
        emptyBuffer();
        bzero(nom, sizeof(nom));
        bzero(prenom, sizeof(prenom));
        valread = 0;
        bzero(buffer, sizeof(buffer));

        valread = read(sock, buffer, 1024);
        buffer[strlen(buffer)] = '\0';
        printf("%s\n", buffer);
        break;

      case 4:
        printf("\nLe nom a modifier: ");
        scanf("%s", nom);
        send(sock, nom, strlen(nom), 0);
        emptyBuffer();
        printf("\nLe prenom a modifier: ");
        scanf("%s", prenom);
        send(sock, prenom, strlen(prenom), 0);
        emptyBuffer();
        bzero(nom, sizeof(nom));
        bzero(prenom, sizeof(prenom));
        printf("Entrer les nouvelles informations\n");
        bzero(&c, sizeof(c));
        saisir(sock, &c);
        Afficher(c);
        valread = 0;
        bzero(buffer, sizeof(buffer));

        valread = read(sock, buffer, 1024);
        buffer[strlen(buffer)] = '\0';
        printf("%s\n", buffer);
        bzero(buffer, sizeof(buffer));

        break;
      case 5:
        valread = 0;
        bzero(buffer, sizeof(buffer));

        valread = read(sock, buffer, 1024);
        buffer[strlen(buffer)] = '\0';
        printf("%s\n", buffer);

        break;
      default:
        break;
      }
    }
  }

  else
  {

    while (1)
    {

      printf("%s\n", menu_user);

      printf("\nVous voulez faire : ");
      scanf("%d", &choice_user);
      fflush(stdin);

      printf("\n\nProcessing the operation number %d\n\n", choice_user);

      converted_number = 0;

      number_to_send = choice_user;
      converted_number = htonl(number_to_send);

      // Write the number to the opened socket
      write(sock, &converted_number, sizeof(converted_number));

      if (choice_user == 3)
      {
        break;
      }
      bzero(buffer, sizeof(buffer));
      valread = 0;

      switch (choice_user)
      {
      case 1:
        printf("\nLe nom a chercher: ");
        scanf("%s", nom);
        send(sock, nom, strlen(nom), 0);
        emptyBuffer();
        printf("\nLe prenom a chercher: ");
        scanf("%s", prenom);
        send(sock, prenom, strlen(prenom), 0);
        emptyBuffer();
        bzero(nom, sizeof(nom));
        bzero(prenom, sizeof(prenom));
        bzero(buffer, sizeof(buffer));

        valread = recv(sock, buffer, sizeof(buffer), 0);

        buffer[strlen(buffer)] = '\0';
        printf("%s", buffer);
        bzero(buffer, sizeof(buffer));

        break;
      case 2:
        valread = 0;
        bzero(buffer, sizeof(buffer));

        valread = read(sock, buffer, 1024);
        buffer[strlen(buffer)] = '\0';
        printf("%s\n", buffer);
        bzero(buffer, sizeof(buffer));

        break;
      default:
        break;
      }
    }
  }

  close(client_fd);
  return 0;
}

/*functions*/
void error(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void emptyBuffer()
{
  int c = 0;
  while (c != '\n' && c != EOF)
  {
    c = getchar();
  }
}

void saisir(int sock, Contact *e)
{
  printf(" entrer le nom: ");
  scanf("%s", e->nom);
  send(sock, e->nom, strlen(e->nom), 0);
  emptyBuffer();
  printf(" entrer le prenom: ");
  scanf("%s", e->prenom);
  send(sock, e->prenom, strlen(e->prenom), 0);
  emptyBuffer();
  printf(" entrer l email: ");
  scanf("%s", e->email);
  send(sock, e->email, strlen(e->email), 0);
  emptyBuffer();
  printf(" entrer le gsm: ");
  scanf("%s", e->gsm);
  send(sock, e->gsm, strlen(e->gsm), 0);
  emptyBuffer();
  printf("Saisir l addresse\n");
  printf(" entrer la rue: ");
  scanf("%s", e->rue);
  send(sock, e->rue, strlen(e->rue), 0);
  emptyBuffer();
  printf(" entrer la ville: ");
  scanf("%s", e->ville);
  send(sock, e->ville, strlen(e->ville), 0);
  emptyBuffer();
  printf(" entrer le pays: ");
  scanf("%s", e->pays);
  send(sock, e->pays, strlen(e->pays), 0);
  emptyBuffer();
  printf("\n");
}

void Afficher(Contact e)
{
  printf("nom =%s #", e.nom);
  printf("prenom = %s #", e.prenom);
  printf("email = %s #", e.email);
  printf("gsm = %s #", e.gsm);
  printf("addresse = %s %s %s \n", e.rue, e.ville, e.pays);
  printf("\n");
}