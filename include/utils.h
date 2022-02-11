#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>
#include <termios.h>

/////////////////
//// LOGGING ////
/////////////////

// Opens error log
int openErrorLog() {
  int fd;

  fd = open("logs/errors.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
  if (fd == -1) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h openErrorLog");
    exit(-1);
  }

  return fd;
}

// Opens info log
int openInfoLog() {
  int fd;

  fd = open("logs/info.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
  if (fd == -1) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h openInfoLog");
    exit(-1);
  }

  return fd;
}

// Writes to info log
void writeInfoLog(int fd, char* string) {
  // get current time
  time_t rawtime;
  struct tm * timeinfo;
  char* currentTime = malloc(sizeof(timeinfo));
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  // prettier format...
  sprintf(currentTime, "[%d-%d-%d %d:%d:%d]", timeinfo->tm_mday,
      timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour,
      timeinfo->tm_min, timeinfo->tm_sec);

  // make sure print is atomic
  flock(fd, LOCK_EX);
  if (dprintf(fd, "%s %s\n", currentTime, string) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h writeInfoLog");
    exit(-1);
  }
  flock(fd, LOCK_UN);
}

// Writes to error log
void writeErrorLog(int fd, char* string, int errorCode) {
  // get current time
  time_t rawtime;
  struct tm * timeinfo;
  char* currentTime = malloc(sizeof(timeinfo));
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  // prettier format...
  sprintf(currentTime, "[%d-%d-%d %d:%d:%d]", timeinfo->tm_mday,
      timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour,
      timeinfo->tm_min, timeinfo->tm_sec);

  // make sure print is atomic
  flock(fd, LOCK_EX);
  if (dprintf(fd, "%s (code: %d) %s\n", currentTime, errorCode, string) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h writeErrorLog");
    exit(-1);
  }
  flock(fd, LOCK_UN);
}

// Closes log defined by fd
void closeLog(int fd) {
  if (close(fd) == -1) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h closeLog");
    exit(-1);
  }
}

//////////////
//// MISC ////
//////////////

// Changes terminal color
// colorCode - ANSI color code
void terminalColor(int colorCode, bool isBold) {
  char* specialCode = "";
  if (isBold) {
    specialCode = "1;";
  }

  printf("\033[%s%dm", specialCode, colorCode);
}

// Resets the terminal to initial state with title
void clearTerminal() {
  printf("\033c");
  system("clear");
  terminalColor(31, true);
  // Needs to center a map 80 characters long
  printf("                                  DRONE OVERLORD\n");
  printf("                                       MAP\n");
  terminalColor(37, true);
  fflush(stdout);
}

// Decorative text print, with "typing" effect
// delay - in microseconds
void displayText (char* str, int delay) {
  for (int i = 0; str[i] != '\0'; i++) {
    printf("%c", str[i]);
    fflush(stdout);
    usleep(delay);
  }
}

// Detects key presses, without waiting for newline (ENTER)
int detectKeyPress () {
  int input;
  struct termios orig_term_attr;
  struct termios new_term_attr;

  /* set the terminal to raw mode rather than canonical mode */
  tcgetattr(fileno(stdin), &orig_term_attr);
  memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
  new_term_attr.c_lflag &= ~(ECHO|ICANON);
  new_term_attr.c_cc[VTIME] = 0;
  new_term_attr.c_cc[VMIN] = 1;
  tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

  /* read a character from the stdin stream without blocking */
  /* returns EOF (-1) if no character is available */
  input = getchar();

  /* restore the original terminal attributes */
  tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

  return input;
}

/////////////////
//// SOCKETS ////
/////////////////

// Wrapper for socket()
int socketCreate(int domain, int type, int protocol, int fdlog_err) {
  int fd;

  fd = socket(domain, type, protocol);
  if (fd < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketCreate");
    writeErrorLog(fdlog_err, "common.h: socketCreate failed", errno);
    exit(-1);
  }

  return fd;
}

// Wrapper for gethostbyname()
struct hostent* getHostFromName(const char* name, int fdlog_err) {
  struct hostent* ent;

  ent = gethostbyname(name);
  if (ent == NULL) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h getHostFromName");
    writeErrorLog(fdlog_err, "common.h: getHostFromName failed (no such host)", errno);
    exit(-1);
  }

  return ent;
}

// Wrapper for connect()
void socketConnect(int sockfd, const struct sockaddr* addr, socklen_t addrlen, int fdlog_err) {
  bool isFailed = true;
  int retConnect;

  // Keep trying to connect for a while before giving up
  for (int i = 0; i < 5; i++) {
    retConnect = connect(sockfd, addr, addrlen);
    if (retConnect < 0) {
      sleep(1);
    } else {
      isFailed = false;
      break;
    }
  }

  if (isFailed) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketConnect");
    writeErrorLog(fdlog_err, "common.h: socketConnect failed, connection timed out", errno);
    exit(-1);
  }
}

// Wrapper for write()
void socketWrite(int fd, int message, int messageLength, int fdlog_err) {
  if (write(fd, &message, messageLength) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketWrite");
    writeErrorLog(fdlog_err, "common.h: socketWrite failed", errno);
    exit(-1);
  }
}

// Read from socket and return value
int socketRead(int fd, int messageLength, int fdlog_err) {
  int message;

  if (read(fd, &message, messageLength) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketRead");
    writeErrorLog(fdlog_err, "common.h: socketRead failed", errno);
    exit(-1);
  }

  return message;
}

// Wrapper for bind()
void socketBind (int sockfd, const struct sockaddr* addr, socklen_t addrlen, int fdlog_err) {
  if (bind(sockfd, addr, addrlen) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketBind");
    writeErrorLog(fdlog_err, "common.h: socketBind failed", errno);
    exit(-1);
  }
}

// Wrapper for listen()
void socketListen (int sockfd, int backlog, int fdlog_err) {
  if (listen(sockfd, backlog) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketListen");
    writeErrorLog(fdlog_err, "common.h: socketListen failed", errno);
    exit(-1);
  }
}

// Wrapper for accept()
int socketAccept (int sockfd, struct sockaddr* cliAddr, socklen_t* addrlen, int fdlog_err) {
  if (accept(sockfd, cliAddr, addrlen) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketAccept");
    writeErrorLog(fdlog_err, "common.h: socketAccept failed", errno);
    exit(-1);
  }
}

// Wrapper for setsockopt()
void socketSetOpt (int sockfd, int level, int optname,
    const void* optval, socklen_t optlen, int fdlog_err) {
  if (setsockopt(sockfd, level, optname, optval, optlen) < 0) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketSetOpt");
    writeErrorLog(fdlog_err, "common.h: socketSetOpt failed", errno);
    exit(-1);
  }
}

// Closes the socket
void socketClose(int fd, int fdlog_err) {
  if (close(fd) == -1) {
    printf("Error %d in ", errno);
    fflush(stdout);
    perror("common.h socketClose");
    writeErrorLog(fdlog_err, "common.h: socketClose failed", errno);
    exit(-1);
  }
}
