/*

 Copyright (c) 2006, Remember The Milk.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

     * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above
 copyright notice, this list of conditions and the following disclaimer
 in the documentation and/or other materials provided with the
 distribution.
     * Neither the name of Remember The Milk nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ---
 Author: Omar Kilani (omar@rememberthemilk.com)

*/

#ifndef RTMAPI_H
#define RTMAPI_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <curl/curl.h>
#include <openssl/md5.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static const char *RTM_API_AUTH_ENDPOINT = "http://www.rememberthemilk.com/services/auth/";
static const char *RTM_API_REST_ENDPOINT = "http://www.rememberthemilk.com/services/rest/";

enum {
  RTM_API_SECRET_LEN = 16,
  RTM_API_KEY_LEN = 32,
  RTM_API_AUTH_TOKEN_LEN = 40
};

typedef struct _rtm_api {
  char api_key[RTM_API_KEY_LEN+1];
  char api_secret[RTM_API_SECRET_LEN+1];
  char auth_token[RTM_API_AUTH_TOKEN_LEN+1];
  unsigned long timeline;

  CURL *_curl; 
  GTree *_param_tree;
} rtm_api;

rtm_api *rtm_api_init(const char *api_key, const char *api_secret);
void rtm_api_set_auth_token(rtm_api *rtm, const char *auth_token);
void rtm_api_set_timeline(rtm_api *rtm, unsigned long timeline);
void rtm_api_free(rtm_api *rtm);

char *rtm_api_auth_get_url(rtm_api *rtm, const char *perms, const char *frob);
char *rtm_api_call_method(rtm_api *rtm, char *method);
char *rtm_api_call_authenticated_method(rtm_api *rtm, char *method);

void rtm_api_param_init(rtm_api *rtm);
void rtm_api_param_try_init(rtm_api *rtm);
void rtm_api_param_clear(rtm_api *rtm);
void rtm_api_param_reset(rtm_api *rtm);
char *rtm_api_param_get_data(rtm_api *rtm);

void rtm_api_param_add(rtm_api *rtm, const char *parameter, const char *value);
void rtm_api_param_add_long(rtm_api *rtm, const char *parameter, long lvalue);
void rtm_api_param_add_date(rtm_api *rtm, const char *parameter, time_t tvalue);
void rtm_api_param_sign_and_append(rtm_api *rtm);

#ifdef __cplusplus
}
#endif

#endif
