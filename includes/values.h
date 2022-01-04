const int PORTNO = 4000; // example: group 1 has portno = (PORTNO+1)
const char* HOSTNAME = "localhost";

struct CoordinatePair {
  int x;
  int y;
};

/**
* TCP CODES
*/
const int MASTER_OK = 1;
const int MASTER_COL = 0;

const int DRONE_TIMEOUT = 3;
const int TIMESTEP = 200; // milliseconds
// map size = 40x80

/**
* STARTING POSITIONS (TEMPORARY)
*/
const struct CoordinatePair START0 = {.x = 0, .y = 0};
const struct CoordinatePair START1 = {.x = 40, .y = 0};
const struct CoordinatePair START2 = {.x = 0, .y = 80};
const struct CoordinatePair START3 = {.x = 40, .y = 80};
const struct CoordinatePair START4 = {.x = 20, .y = 40};
