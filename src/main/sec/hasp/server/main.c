#include "sec/hasp/lib/server.h"

int main(int argc, char **argv)
{
  sec_hasp_server_init(NULL, 0);
  sec_hasp_server_run();
}