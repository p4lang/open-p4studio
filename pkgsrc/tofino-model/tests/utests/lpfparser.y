%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <dirent.h>

extern void lpf_setup_test(char* filename);
extern void lpf_push_event(double, double, double);
extern void lpf_start_test(int);

void lpferror (char *s)
{
  printf("Erroring out : %s\n", s);
}

double inj_time;
double input;
double output;
extern int lpfparse();
extern int lpflex();
extern int lpfwrap();
int lpfdebug = 1;
static int g_setup_done = 0;

static  char g_filename[200];

%}


%union {
int   int_val;
double float_val;
char  *strg;
};

%token <strg>      _STRING
%token <float_val> _FLOATNUM
%token <strg>      _AT
%token <strg>      _TIME
%token <strg>      _IS
%token <strg>      _DATA
%token <strg>      _VAL
%token <strg>      _TSTAMP

%token FILE_END

%%

all_rules:
 all_rules_list
 |  all_rules all_rules_list
 ;

all_rules_list:
   lpf_rule
 | end_rule
 | any_string_rule
 ;

lpf_rule:
  _AT _TIME _FLOATNUM _DATA _IS _FLOATNUM _VAL _IS _FLOATNUM _TSTAMP _IS _FLOATNUM
 {
    if (!g_setup_done) {
      lpf_setup_test(g_filename);
      g_setup_done = 1;
    }
    lpf_push_event($3, $6, $9);
 }
 
end_rule:
 FILE_END
 {
    //printf("Starting LPF test...\n");
    //lpf_start_test(1); /* 0 / 1 for turning off/on logging */
 }

any_string_rule:
 _STRING
 {
 }
  





%%


int lpfwrap()
{
  printf("Starting LPF test on new Log file %s...\n", g_filename);
  printf("___________________________________________\n");

  lpf_start_test(0); /* 0 / 1 for turning off/on logging */
  g_setup_done = 0; /* Reset so that next log file can be processed */
  return 1;
}

extern FILE *lpfin;

//#define PARSELPF_STDALONE

#ifdef PARSELPF_STDALONE
int main(int argc, char **argv)
{
  
  FILE *myfile = NULL;
  int c;

  while ((c = getopt(argc, argv, "f:")) != -1) {
    switch (c) {
      case 'f':
        strncpy(g_filename, optarg, sizeof(g_filename));
        myfile = fopen(g_filename, "r");
        printf("Filename = %s\n", g_filename);
      break;
      default:
      break;
    }
  }

  if (!myfile) {
    printf("Couldn't open file\n");
  } else {
    lpfin = myfile;
    do {
      lpfparse();
    } while(!feof(lpfin));
  }
  return 0;
}
#else
void harlyn_model_lpf_tc_entry_point()
{
  FILE *myfile = NULL;
  char fpath[200];
  DIR  *logdir;
  struct dirent *entry;

  logdir = opendir("./lpflogs/");
  if (logdir == NULL) {
    return;
  }

  // For all files in directory 
  while ((entry = readdir(logdir)) != NULL) {
    strncpy(g_filename, entry->d_name, sizeof(g_filename));
    if ((strncmp(g_filename, ".", 1) == 0) || (strncmp(g_filename, "..", 2) == 0)) {
      continue;
    }
    strncpy(fpath, "./lpflogs/", sizeof(fpath));
    strncat(fpath, g_filename, sizeof(fpath)-strlen(fpath)-1);
    myfile = fopen(fpath, "r");
    if (!myfile) {
      printf("Couldn't open file %s\n", fpath);
    } else {
      printf("Processing Lpf Log file File %s\n", fpath);
      lpfin = myfile;
      do {
        lpfparse();
      } while(!feof(lpfin));
      fclose(myfile);
    }
  }

}


#endif


