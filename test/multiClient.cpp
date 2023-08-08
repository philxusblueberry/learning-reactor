#include "Buffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "ThreadPool.h"
#include <cstring>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

void usage() {

  printf("argument: \n");

  printf("\t --ports,-p \n"

         "\t --msgs, -m\n"

         "\t --wait, -w\n"

         "\t --help,  -h\n"

  );
}

void doOneConnection(int msgs, int wait, int thread) {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress("", 23323);
  sock->connect(addr);
  // printf("xxl check connect done\n");
  delete addr;

  Buffer buf;
  char tmpBuf[1024];

  usleep(wait * 1000);

  while (msgs--) {
    // usleep(wait * 1000);
    sprintf(tmpBuf, "xxl send msg %d in thread %d", msgs, thread);
    // printf("%s", tmpBuf);
    int size = sock->write((const char *)tmpBuf, strlen(tmpBuf));
    if (size < 0) {
      printf("socket already disconnect\n");
      break;
    }
    int cur = 0;
    while (true) {
      int tmp = sock->read(tmpBuf, sizeof(tmpBuf));
      cur += tmp;
      if (tmp > 0) {
        buf.append((const uint8_t *)tmpBuf, tmp);
      } else if (tmp == -1) {
        // printf("read is interupt %ld", tmp);
        continue;
      } else {
        sock->close();
        printf("socket is close by peer\n");
        exit(EXIT_SUCCESS);
      }
      if (cur >= size) {
        break;
      }
    }
    // printf("xxl check read msg %s", buf.getBuf());
    buf.clear();
  }
  delete sock;
}

// portnum 100 msg loop 15 wait 1000ms
int main(int argc, char **argv) {
  int connections = 20, msgLoop = 20, waitT = 1000;

  int c;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {{"ports", required_argument, 0, 'p'},
                                           {"msgs", required_argument, 0, 'm'},
                                           {"wait", required_argument, 0, 'w'},
                                           {"help", no_argument, 0, 'h'},
                                           {0, 0, 0, 0}};

    c = getopt_long(argc, argv, "p:m:w:h", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 'p':
      connections = atoi(optarg);
      break;
    case 'm':
      msgLoop = atoi(optarg);
      break;
    case 'w':
      waitT = atoi(optarg);
      break;
    case 'h':
      usage();
      exit(EXIT_SUCCESS);
    case '?':
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc)
      printf("%s ", argv[optind++]);
    printf("\n");
  }

  if (!connections || !msgLoop) {
    printf("input error");
    exit(EXIT_FAILURE);
  }

  int n = std::thread::hardware_concurrency();
  printf("xxl check input ports %d msgLoop %d wait %d with concurrency %d\n",
         connections, msgLoop, waitT, n);

  ThreadPool *pool = new ThreadPool();
  for (int i = 0; i < connections; ++i) {
    pool->add(doOneConnection, msgLoop, waitT, i);
  }

  usleep(10000000);

  printf("xxl check main loop done\n");

  return 0;
}