#include "rtmapi.h"

#define MY_API_KEY "123456"
#define MY_API_SECRET "987654"

int main(int argc, char **argv) {
  rtm_api *rtm;
  char *resp;

  rtm = rtm_api_init(MY_API_KEY, MY_API_SECRET);
  rtm_api_set_auth_token(rtm, "ATOKEN");
  rtm_api_set_timeline(rtm, 123456);

  resp = rtm_api_call_method(rtm, "rtm.test.echo");

  printf("Response from non authentcated method (rtm.test.echo):\n");
  puts(resp);
  free(resp);

  resp = rtm_api_call_authenticated_method(rtm, "rtm.lists.getList");
  printf("Response from authentcated method (rtm.lists.getList):\n");
  puts(resp);
  free(resp);

  rtm_api_free(rtm);

  return 0;
}
