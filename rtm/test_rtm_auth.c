#include "rtmapi.h"

#define MY_API_KEY "123456"
#define MY_API_SECRET "987654"

int main(int argc, char **argv) {
  rtm_api *rtm;
  char *url;

  rtm = rtm_api_init(MY_API_KEY, MY_API_SECRET);

  url = rtm_api_auth_get_url(rtm, "write", NULL);

  printf("Authentication URL: %s\n", url);

  free(url);

  rtm_api_free(rtm);

  return 0;
}
