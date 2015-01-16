/*
 * Name: Dustin Dobranksy
 * ID:   250575030
 *
 * Project: CS3357 - Lab 2
 *
 * Description:
 * This program takes in up to three command line arguments
 * and logs different messages according to the input.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <stdbool.h>

int main(int argc, char** argv) {
  int c;
  static int log_level = -1; // the log level
  char* username = NULL;

  while (true) {
    static struct option long_options[] =
    {
      {"verbose", no_argument, &log_level, LOG_INFO},
      {"debug", no_argument, &log_level, LOG_DEBUG},
      {"username", required_argument, 0, 'u'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    c = getopt_long(argc, argv, "vdu:", long_options, &option_index);

    //printf("log level: %d", log_level);

    if (c == -1)
      break;

    switch (c) {
      case 'd':
        log_level = LOG_DEBUG;
        break;
      case 'v':
        log_level = LOG_INFO;
        break;
      case 'u':
        username = optarg;
        break;
      case '?':
        log_level = LOG_WARNING;
        printf("? arg\n");
        break;
    }
  }

  if (log_level == -1) {
    log_level = LOG_WARNING;
  }

/*
  switch (log_level) {
    case 0:
      printf("#define LOG_EMERG 0\n"); break;
    case 1:
      printf("#define LOG_ALERT 1\n"); break;
    case 2:
      printf("#define LOG_CRIT 2\n"); break;
    case 3:
      printf("#define LOG_ERR 3\n"); break;
    case 4:
      printf("#define LOG_WARNING 4\n"); break;
    case 5:
      printf("#define LOG_NOTICE 5\n"); break;
    case 6:
      printf("#define LOG_INFO 6\n"); break;
    case 7:
      printf("#define LOG_DEBUG 7\n"); break;
    default:
      printf("non\n");
      break;
  }
*/

  openlog("Lab2", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);

  setlogmask(LOG_UPTO(log_level));
  //setlogmask(LOG_UPTO(LOG_DEBUG));

  if (username != NULL) {
   syslog(LOG_INFO, "Your username is %s", username);
  } else {
    syslog(LOG_WARNING, "No username specified");
  }

  syslog(LOG_DEBUG, "Hello world");

  closelog();

  exit(EXIT_SUCCESS);
}
