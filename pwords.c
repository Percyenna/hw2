#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;
int *ret;
int k = 0;
typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

dict_t *dic;

char *
make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t *
make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *
insert_word( dict_t *d, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = d;		// following insertion point
  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		// increment count 
      return d;			// return head 
    }
    pd = di;			// advance ptr pair
    di = di->next;
  }
  nd = make_dict(word);		// not found, make entry 
  nd->next = di;		// entry bigger than word or tail 
  if (pd) {
    pd->next = nd;
    return d;			// insert beond head 
  }
  return nd;
}

void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int
get_word( char *buf, int n, FILE *infile) {
pthread_mutex_lock(&locker); 
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
	pthread_mutex_unlock(&locker); 
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
pthread_mutex_unlock(&locker); 
  return 0;			// no more words
}

#define MAXWORD 1024
void *
words( void *file ) {
//pthread_mutex_lock(&locker); 

//k=k+1;
//printf("%d\n",k);

FILE *infile = (FILE *) file;
  char wordbuf[MAXWORD];
print_dict(dic);
  while( get_word( wordbuf, MAXWORD, infile ) ) {
    dic = insert_word(dic, wordbuf); // add to dict
  }
return dic;

 // pthread_exit(dic);
//pthread_mutex_unlock(&locker); 
}

int
main( int argc, char *argv[] ) {

if(pthread_mutex_init(&locker, NULL ) != 0 ){
printf("%d no it didn't work");
}


  dic = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }


pthread_t threads[4];

for(int j=0;j<4;j++){
pthread_create(&threads[j],NULL,words,(void *)infile);
}
for(int j=0;j<4;j++){
pthread_join(threads[j],NULL);
}



/*
pthread_create(&threads[1],NULL,words,(void *)infile);
pthread_join(threads[1],(void**) &ret);

pthread_create(&threads[2],NULL,words,(void *)infile);
pthread_join(threads[2],(void**) &ret);

pthread_create(&threads[3],NULL,words,(void *)infile);
pthread_join(threads[3],(void**) &ret);
*/

  print_dict(dic);
pthread_mutex_destroy(&locker);
  fclose( infile );
}

