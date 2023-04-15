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

/* ---sever---- */

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
void saisir(Contact *e);
void Afficher(Contact e);
void Ajouter(char *nom_fich, Contact e);
void Rechercher(char *nom_fich, char nom[], char prenom[], int newsockfd);
void Supprimer(char *nom_fich, char nom[], char prenom[], int newsockfd);
void read_(char nom_fich[], char buffer[]);

/*main*/

int main(int argc, char *argv[])
{
  /*variables*/
  char buffer[1024];
  bzero(buffer, sizeof(buffer));

  char hello[] = "--------Bienvenue----------\n\0";

  /*nom et prenom a chercher ou a supprimer*/
  char nom[MAX];
  char prenom[MAX];
  bzero(nom, sizeof(nom));
  bzero(prenom, sizeof(prenom));

  Contact c;

  /*used to receive an int value*/
  int received_int = 0;
  int return_status = 0;

  /*Create a socket */

  int sockfd, newsockfd, valread;

  struct sockaddr_in address;
  int addrlen = sizeof(address);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
  {
    error("SOCKET NOT CREATED...");
  }

  /*binding a socket*/

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    error("binding failed");
  }

  /*listenning*/

  if (listen(sockfd, 3) < 0)
  {
    error("listening failed");
  }
  else
  {
    printf("Listening...\n");
  }

  /*accepting a connection request*/

  if ((newsockfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
  {
    error("ERROR ACCEPTING");
  }
  else
  {
    printf("Connection established\n");
  }

  /*sending the hello msg*/
  bzero(buffer, sizeof(buffer));

  send(newsockfd, hello, strlen(hello), 0);

  /*used to read  users info from the acoounts.txt file*/
  char u_file[20];
  char pw_file[20];
  char profile[20];

  FILE *file = NULL;
  file = fopen("accounts.txt", "r");
  if (file == NULL)
  {

    error("Cannot open the file\n");
  }
  /*used to receive the log in data of a client using the server*/
  char un_input[20];
  char pw_input[20];

  bzero(un_input, sizeof(un_input));
  bzero(pw_input, sizeof(pw_input));

  /*counting number of trials*/
  int count = 0;
  /*Authentification*/
  int found = 0;

  do
  {
    bzero(u_file, sizeof(u_file));
    bzero(pw_file, sizeof(pw_file));
    bzero(profile, sizeof(profile));

    bzero(un_input, sizeof(un_input));
    bzero(pw_input, sizeof(pw_input));

    valread = recv(newsockfd, un_input, sizeof(un_input), 0);
    un_input[strlen(un_input)] = '\0';

    printf("Username entered: %s", un_input);

    valread = 0;
    printf("\n");

    valread = recv(newsockfd, pw_input, sizeof(pw_input), 0);
    pw_input[strlen(pw_input)] = '\0';

    printf("Password entered: %s\n", pw_input);

    rewind(file);
    while (!feof(file))
    {

      fscanf(file, "%s %s %s", u_file, pw_file, profile);
      u_file[strlen(u_file)] = '\0';
      pw_file[strlen(pw_file)] = '\0';
      profile[strlen(profile)] = '\0';

      /*Authentification process*/
      if (strcmp(u_file, un_input) == 0 && strcmp(pw_file, pw_input) == 0)
      {

        found = 1;
        break;
      }
    }
    if (found == 1)
    {

      send(newsockfd, "You are logged in!\n\0", strlen("You are logged in!\n\0"), 0);

      printf("\nYou are logged in!\n");
      break;
    }
    else if (found == 0)
    {
      send(newsockfd, "No such user!\n\0", strlen("No such user!\n\0"), 0);
      count++;
    }

    if (count > 2)
    {
      break;
    }

  } while (found == 0);

  fclose(file);

  /* Menu showing in case of authentification successfull*/

  if (found == 1)
  {
    send(newsockfd, profile, strlen(profile), 0);

    // char menu_admin[] = "\n\n1-Ajouter un contact\n2-Rechercher un contact\n3-Supprimer un contact\n4-Modifier un contact\n5-Afficher tous les contacts\n6-Quitter\n\n\0";
    // char menu_user[] = "\n\n1-Rechercher un contact\n2-Afficher tous les contacts\n3-Quitter\n\n\0";

    int choice_admin = 0;
    int choice_user = 0;

    if (strcmp(profile, "admin") == 0)
    {

      while (1)
      {
        // send(newsockfd, menu_admin, strlen(menu_admin), 0);

        return_status = read(newsockfd, &received_int, sizeof(received_int));
        choice_admin = ntohl(received_int);
        printf("\nOperation number %d\n", choice_admin);
        if (choice_admin == 6 || choice_admin == 0)
        {
          break;
        }

        return_status = 0;
        bzero(&received_int, sizeof(received_int));

        switch (choice_admin)
        {
        case 1:
          bzero(&c, sizeof(c));
          valread = 0;
          valread = recv(newsockfd, c.nom, sizeof(c.nom), 0);
          c.nom[strlen(c.nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.prenom, sizeof(c.prenom), 0);
          c.prenom[strlen(c.prenom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.email, sizeof(c.email), 0);
          c.email[strlen(c.email)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.gsm, sizeof(c.gsm), 0);
          c.gsm[strlen(c.gsm)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.rue, sizeof(c.rue), 0);
          c.rue[strlen(c.rue)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.ville, sizeof(c.ville), 0);
          c.ville[strlen(c.ville)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.pays, sizeof(c.pays), 0);
          c.pays[strlen(c.pays)] = '\0';
          Ajouter("contacts.txt", c);
          bzero(&c, sizeof(c));

          break;
        case 2:
          valread = 0;
          valread = recv(newsockfd, nom, sizeof(nom), 0);
          nom[strlen(nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, prenom, sizeof(prenom), 0);
          prenom[strlen(prenom)] = '\0';
          Rechercher("contacts.txt", nom, prenom, newsockfd);
          bzero(nom, sizeof(nom));
          bzero(prenom, sizeof(prenom));

          break;
        case 3:
          valread = 0;
          valread = recv(newsockfd, nom, sizeof(nom), 0);
          nom[strlen(nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, prenom, sizeof(prenom), 0);
          prenom[strlen(prenom)] = '\0';
          Supprimer("contacts.txt", nom, prenom, newsockfd);
          bzero(nom, sizeof(nom));
          bzero(prenom, sizeof(prenom));
          break;
        case 4:
          /*modifier un contact*/
          printf("if the contact does not exist it will be added!\n");
          valread = 0;
          valread = recv(newsockfd, nom, sizeof(nom), 0);
          nom[strlen(nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, prenom, sizeof(prenom), 0);
          prenom[strlen(prenom)] = '\0';
          Supprimer("contacts.txt", nom, prenom, newsockfd);
          bzero(nom, sizeof(nom));
          bzero(prenom, sizeof(prenom));

          bzero(&c, sizeof(c));
          valread = 0;
          valread = recv(newsockfd, c.nom, sizeof(c.nom), 0);
          c.nom[strlen(c.nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.prenom, sizeof(c.prenom), 0);
          c.prenom[strlen(c.prenom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.email, sizeof(c.email), 0);
          c.email[strlen(c.email)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.gsm, sizeof(c.gsm), 0);
          c.gsm[strlen(c.gsm)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.rue, sizeof(c.rue), 0);
          c.rue[strlen(c.rue)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.ville, sizeof(c.ville), 0);
          c.ville[strlen(c.ville)] = '\0';
          valread = 0;
          valread = recv(newsockfd, c.pays, sizeof(c.pays), 0);
          c.pays[strlen(c.pays)] = '\0';
          Ajouter("contacts.txt", c);
          bzero(&c, sizeof(c));

          break;
        case 5:
          bzero(buffer, sizeof(buffer));
          read_("contacts.txt", buffer);
          send(newsockfd, buffer, strlen(buffer), 0);
          bzero(buffer, sizeof(buffer));

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

        return_status = read(newsockfd, &received_int, sizeof(received_int));
        choice_user = ntohl(received_int);
        printf("\nOperation number %d\n", choice_user);
        if (choice_user == 3 || choice_user == 0)
        {
          break;
        }
        return_status = 0;
        bzero(&received_int, sizeof(received_int));

        switch (choice_user)
        {
        case 1:
          valread = 0;
          valread = recv(newsockfd, nom, sizeof(nom), 0);
          nom[strlen(nom)] = '\0';
          valread = 0;
          valread = recv(newsockfd, prenom, sizeof(prenom), 0);
          prenom[strlen(prenom)] = '\0';
          Rechercher("contacts.txt", nom, prenom, newsockfd);
          bzero(nom, sizeof(nom));
          bzero(prenom, sizeof(prenom));
          break;
        case 2:
          bzero(buffer, sizeof(buffer));
          read_("contacts.txt", buffer);
          send(newsockfd, buffer, strlen(buffer), 0);
          bzero(buffer, sizeof(buffer));

          break;
        default:
          break;
        }
      }
    }
  }

  // closing the connected socket

  close(newsockfd);

  // closing the listening socket
  shutdown(sockfd, SHUT_RDWR);

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

void saisir(Contact *e)
{
  printf("\n entrer le nom: ");
  scanf("%s", e->nom);
  emptyBuffer();
  printf(" entrer le prenom: ");
  scanf("%s", e->prenom);
  emptyBuffer();
  printf(" entrer l email: ");
  scanf("%s", e->email);
  emptyBuffer();
  printf(" entrer le gsm: ");
  scanf("%s", e->gsm);
  fflush(stdin);
  printf("Saisir l addresse\n");
  printf(" entrer la rue: ");
  scanf("%s", e->rue);
  emptyBuffer();
  printf(" entrer la ville: ");
  scanf("%s", e->ville);
  emptyBuffer();
  printf(" entrer le pays: ");
  scanf("%s", e->pays);
  emptyBuffer();
  printf("\n");
}

void Ajouter(char *nom_fich, Contact e)
{
  FILE *fich;
  fich = fopen(nom_fich, "a");
  if (fich == NULL)
  {
    printf("ERROR");
    exit(1);
  }
  fprintf(fich, "%s %s %s %s %s %s %s\n", e.nom, e.prenom, e.email, e.gsm, e.rue, e.ville, e.pays);

  fclose(fich);
}

void Rechercher(char *nom_fich, char nom[], char prenom[], int newsockfd)
{
  Contact e;
  FILE *fich;
  char buffer[1024];
  int found = 0;

  fich = fopen(nom_fich, "r");
  if (fich == NULL)
  {
    printf("ERROR");
    exit(1);
  }
  printf("Found:\n");
  rewind(fich);

  do
  {

    fscanf(fich, "%s %s %s %s %s %s %s\n", e.nom, e.prenom, e.email, e.gsm, e.rue, e.ville, e.pays);

    if (strcmp(e.nom, nom) == 0 && strcmp(e.prenom, prenom) == 0)
    {
      Afficher(e);
      found = 1;
      bzero(buffer, sizeof(buffer));
      strcat(buffer, e.nom);
      strcat(buffer, " # ");
      strcat(buffer, e.prenom);
      strcat(buffer, " # ");
      strcat(buffer, e.email);
      strcat(buffer, " # ");
      strcat(buffer, e.gsm);
      strcat(buffer, " # ");
      strcat(buffer, e.rue);
      strcat(buffer, " # ");
      strcat(buffer, e.ville);
      strcat(buffer, " # ");
      strcat(buffer, e.pays);
      strcat(buffer, " \n\0");

      send(newsockfd, buffer, strlen(buffer), 0);
    }

  } while (!feof(fich));
  if (found == 0)
  {
    send(newsockfd, "Not found\0", strlen("Not found\0"), 0);
  }

  fclose(fich);
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

void Supprimer(char *nom_fich, char nom[], char prenom[], int newsockfd)
{
  Contact e;
  FILE *fich;
  FILE *fich2;

  int found = 0;

  fich = fopen(nom_fich, "r");
  fich2 = fopen("temp.txt", "w");
  if (fich == NULL)
  {
    printf("ERROR");
    exit(1);
  }
  if (fich2 == NULL)
  {
    printf("ERROR");
    exit(1);
  }

  rewind(fich);

  do
  {
    if (fscanf(fich, "%s %s %s %s %s %s %s\n", e.nom, e.prenom, e.email, e.gsm, e.rue, e.ville, e.pays) != -1)
    {
      if (strcmp(e.nom, nom) != 0 || strcmp(e.prenom, prenom) != 0)
      {
        Ajouter("temp.txt", e);
      }
      else if (strcmp(e.nom, nom) == 0 && strcmp(e.prenom, prenom) == 0)
      {
        found = 1;
      }
    }

  } while (!feof(fich));
  fclose(fich);
  fclose(fich2);
  remove(nom_fich);
  rename("temp.txt", nom_fich);
  if (found == 1)
  {
    printf("Successfuly Done\n");
    send(newsockfd, "Successfuly Done\n\0", strlen("Successfuly Done\n\0"), 0);
  }
  else
  {
    printf("No such elments\n");
    send(newsockfd, "No such elements\n\0", strlen("No such elements\n\0"), 0);
  }
}

void read_(char nom_fich[], char buffer[])
{

  FILE *f;
  f = fopen(nom_fich, "r");
  int i = 0;
  char caractereActuel;
  do
  {
    caractereActuel = fgetc(f); // On lit le caractère
    buffer[i] = caractereActuel;
    i++;
  } while (caractereActuel != EOF);
  buffer[strlen(buffer)] = '\0';
  fclose(f);
}