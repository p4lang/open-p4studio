%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <dirent.h>


extern void log_file_parsed_cbs(uint64_t committed_burst_size);
extern void log_file_parsed_cir_persec(float committed_rate_bytes_per_second);
extern void log_file_parsed_pbs(uint64_t peak_burst_size);
extern void log_file_parsed_pir_persec(float peak_rate_bytes_per_second);
extern void log_file_parsed_total_pkts(uint64_t pkts);
extern void log_file_parsed_green_pkts(uint64_t pkts);
extern void log_file_parsed_yel_pkts(uint64_t pkts);
extern void log_file_parsed_red_pkts(uint64_t pkts);

extern void setup_bursty_meter();
extern void bursty_meter_inj_pkt(float inj_time, int pktlen, char* pktcolor, int loggingon);
extern void get_bursty_meter_test_result();
extern void print_bursty_meter_total_passed_count();


void metererror (char *s)
{
  printf("Erroring out : %s\n", s);
}

char  meter_config_type[100];
char  pkt_color[20];
float meter_cfg;

float inj_time;
int pktlen;

extern int meterparse();
extern int meterlex();
extern int meterwrap();

int meterdebug = 1;
//#define YYDEBUG 1

%}


%union {
int   int_val;
float float_val;
char  *strg;
};

%token <strg>      _STRING
%token <float_val> _FLOATNUM
%token <int_val>   _INTGR
%token <strg>      _AT
%token <strg>      _DELIMITER
%token <strg>      _DOT
%token <strg>      _TESTEND

%token       _NEWLINE

%%

all_rules:
 all_rules_list
 |  all_rules all_rules_list
 ;

all_rules_list:
   meter_config_rule
 | pkt_color_check_rule
 | test_end_rule
 | any_string_rule
 ;

meter_config_rule:
  _STRING _DELIMITER _FLOATNUM
 {
    strncpy(meter_config_type, $1, 100);
    meter_cfg = $3;

    if (!strncmp($1, "CBS-CIR-times-burstperiod", strlen("CBS-CIR-times-burstperiod"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_cbs((uint64_t)meter_cfg);
    }
    if (!strncmp($1, "PBS-PIR-times-burstperiod", strlen("PBS-PIR-times-burstperiod"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_pbs((uint64_t)meter_cfg);
    }

    if (!strncmp($1, "CIR-persec", strlen("CIR-persec"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_cir_persec((uint64_t)meter_cfg);
    }
    if (!strncmp($1, "PIR-persec", strlen("PIR-persec"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_pir_persec((uint64_t)meter_cfg);
    }

    if (!strncmp($1, "Total-Packets-Offered", strlen("Total-Packets-Offered"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_total_pkts(meter_cfg);
      setup_bursty_meter();
    }
    if (!strncmp($1, "Total-Green-Pkts", strlen("Total-Green-Pkts"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_green_pkts(meter_cfg);
    }
    if (!strncmp($1, "Total-Yel-Pkts", strlen("Total-Yel-Pkts"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_yel_pkts(meter_cfg);
    }
    if (!strncmp($1, "Total-Red-Pkts", strlen("Total-Red-Pkts"))) {
      printf("%s = %.12f\n", $1, meter_cfg);
      log_file_parsed_red_pkts(meter_cfg);
    }

    free($1);
 }
 
 

pkt_color_check_rule:
_AT _FLOATNUM _FLOATNUM _STRING _FLOATNUM _FLOATNUM
 {
    inj_time = $2;
    pktlen = $3;
    strncpy(pkt_color, $4, 20);
    //printf("Time %f,  Pkt-Len = %d, OuputColor = %s\n", inj_time, pktlen, pkt_color);

    // Invoke Model API to inject packet of length pktlen. Move time to inj_time before
    // injecting packet.
    // Expect packet color of pkt_color. Since ASIC applies color of previous packet,
    // in the model code, verify against previously provided color.

    bursty_meter_inj_pkt(inj_time, pktlen, pkt_color, 
                         0 /* pass 1 to enable logging on model end */); 
    free ($4);
 }

test_end_rule:
 _TESTEND
{
    bursty_meter_inj_pkt(0, 0, "DOESNTMATTER", 1);
}

any_string_rule:
 _STRING
 {
 }
  





%%

int meterwrap()
{
  return 1;
}

extern FILE *meterin;

#ifdef PARSE_STDALONE
main(int argc, char **argv)
{
  
  FILE *myfile = NULL;
  int c;
  char filename[200];

  while ((c = getopt(argc, argv, "f:")) != -1) {
    switch (c) {
      case 'f':
        strncpy(filename, optarg,  sizeof(filename));
        myfile = fopen(filename, "r");
        printf("Filename = %s\n", filename);
      break;
      default:
      break;
    }
  }

  if (!myfile) {
    printf("Couldn't open file\n");
  } else {
    meterin = myfile;
    do {
      meterparse();
    } while(!feof(meterin));
  }
}
#else
void harlyn_model_bursty_meter_tc_entry_point()
{
  FILE *myfile = NULL;
  char filename[200];
  char fpath[200];
  DIR  *logdir;
  struct dirent *entry;

  logdir = opendir("./meterlogs/");
  if (logdir == NULL) {
    return;
  }

  // For all files in directory 
  while ((entry = readdir(logdir)) != NULL) {
    strncpy(filename, entry->d_name, sizeof(filename));
    if ((strncmp(filename, ".", 1) == 0) || (strncmp(filename, "..", 2) == 0)) {
      continue;
    }
    strncpy(fpath, "./meterlogs/", sizeof(fpath));
    strncat(fpath, filename, sizeof(fpath)-strlen(fpath)-1);
    myfile = fopen(fpath, "r");
    if (!myfile) {
      printf("Couldn't open file %s\n", fpath);
    } else {
      printf("Processing Meter Log file File %s\n", fpath);
      meterin = myfile;
      do {
        meterparse();
      } while(!feof(meterin));
      fclose(myfile);
    }
    printf("Results after processing Meter Log file %s\n", fpath);
    get_bursty_meter_test_result();
  }
  print_bursty_meter_total_passed_count();

}


#endif


