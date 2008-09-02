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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curl/curl.h>
#include <openssl/md5.h>
#include <glib.h>


#include "rtmapi.h"

#define RTM_API_VERSION 1

static gint _rtm_api_param_key_compare(gconstpointer k1, gconstpointer k2,
                                       gpointer data) {
  return strcmp(k1, k2);
}

static void _rtm_api_param_destructor(gpointer data) {
  free(data);
}

static void _rtm_api_init_curl(rtm_api *rtm) {
  curl_global_init(CURL_GLOBAL_ALL);
  rtm->_curl = curl_easy_init();
}

static void _rtm_api_free_curl(rtm_api *rtm) {
  curl_easy_cleanup(rtm->_curl);
  curl_global_cleanup(); 
}

static char *_rtm_api_md5_from_digest(unsigned char *md) {
  char *hash, *ptr;
  int i, j;

  hash = malloc((32+1) * sizeof(char));

  if (hash == NULL) {
    return NULL;
  }

  ptr = hash;

  for (i = 0; i < MD5_DIGEST_LENGTH/4; i++) {
    for (j = 0; j < 4; j++) {
      sprintf(ptr,"%02x", md[i*4+j]);
      ptr += 2;
    }
  }

  *ptr = '\0';
    
  return hash;
}


rtm_api *rtm_api_init(const char *api_key, const char *api_secret) {
  rtm_api *rtm;

  rtm = calloc(1, sizeof(rtm_api));
  if (rtm == NULL) {
    return NULL;
  }

  memcpy(rtm->api_key, api_key, RTM_API_KEY_LEN);
  memcpy(rtm->api_secret, api_secret, RTM_API_SECRET_LEN);

  _rtm_api_init_curl(rtm);
  return rtm;
}

void rtm_api_set_auth_token(rtm_api *rtm, const char *auth_token) {
  memcpy(rtm->auth_token, auth_token, RTM_API_AUTH_TOKEN_LEN);
}

void rtm_api_set_timeline(rtm_api *rtm, unsigned long timeline) {
  rtm->timeline = timeline;
}

void rtm_api_free(rtm_api *rtm) {
  if (rtm == NULL) {
    return;
  }

  _rtm_api_free_curl(rtm);
  rtm_api_param_clear(rtm);

  free(rtm);
  rtm = NULL;
}

void rtm_api_param_init(rtm_api *rtm) {
  rtm->_param_tree = g_tree_new_full(_rtm_api_param_key_compare,
                                     NULL,
                                     _rtm_api_param_destructor,
                                     _rtm_api_param_destructor);
}

void rtm_api_param_try_init(rtm_api *rtm) {
  if (rtm->_param_tree == NULL) {
    rtm_api_param_init(rtm);
  }
}

void rtm_api_param_clear(rtm_api *rtm) {
  if (rtm->_param_tree) {
    g_tree_destroy(rtm->_param_tree);
  }

  rtm->_param_tree = NULL;
}

void rtm_api_param_reset(rtm_api *rtm) {
  rtm_api_param_clear(rtm);
  rtm_api_param_init(rtm);
}

void rtm_api_param_add(rtm_api *rtm, const char *parameter, const char *value) {
  rtm_api_param_try_init(rtm);

  g_tree_insert(rtm->_param_tree, g_strdup(parameter), g_strdup(value));
}

void rtm_api_param_add_long(rtm_api *rtm, const char *parameter, long lvalue) {
  char *value;
    
  rtm_api_param_try_init(rtm);

  value = g_strdup_printf("%ld", lvalue);
  g_tree_insert(rtm->_param_tree, g_strdup(parameter), value);
}

void rtm_api_param_add_date(rtm_api *rtm, const char *parameter, time_t tvalue) {
  char value[100];
  struct tm tmvalue;

  rtm_api_param_try_init(rtm);

  gmtime_r(&tvalue, &tmvalue);

  strftime(value, 100, "%Y-%m-%dT%H:%M:%SZ", &tmvalue);
  g_tree_insert(rtm->_param_tree, g_strdup(parameter), g_strdup(value));
}

static void rtm_api_dump(rtm_api *rtm) {
  printf("rtm_api_version = %d\n", RTM_API_VERSION);
  printf("api_key = %s\n", rtm->api_key);
  printf("api_secret = %s\n", rtm->api_secret);
  printf("auth_token = %s\n", rtm->auth_token);
  printf("timeline = %u\n", rtm->timeline);
  printf("_curl = %p\n", rtm->_curl);
}

static gboolean _rtm_api_param_sign(gpointer key, gpointer value,
                                    gpointer user_data) {
  MD5_CTX *context = user_data;
  char *parameter = key;
  char *val = value;

  MD5_Update(context, parameter, strlen(parameter));
  MD5_Update(context, val, strlen(val));

  return FALSE;
}

static char *rtm_api_param_sign(rtm_api *rtm) {
  unsigned char md[MD5_DIGEST_LENGTH];
  MD5_CTX context;

  MD5_Init(&context);
  MD5_Update(&context, rtm->api_secret, strlen(rtm->api_secret));

  g_tree_foreach(rtm->_param_tree, _rtm_api_param_sign, &context);

  MD5_Final(md, &context);

  return _rtm_api_md5_from_digest(md);
}

void rtm_api_param_sign_and_append(rtm_api *rtm) {
  char *api_sig;

  api_sig = rtm_api_param_sign(rtm);

  rtm_api_param_add(rtm, "api_sig", api_sig);

  free(api_sig);
}

typedef struct _rtm_data {
  GString *data;
  gint nodes_expected;
  gint nodes_seen;
} rtm_data;

static gboolean _rtm_api_param_escape(gpointer key, gpointer value,
                                      gpointer user_data) {
  rtm_data *rdata = user_data;
  char *escaped;

  g_string_append(rdata->data, key);
  g_string_append_c(rdata->data, '=');

  escaped = curl_escape(value, strlen(value));
  g_string_append(rdata->data, escaped);
  free(escaped);

  if (++rdata->nodes_seen < rdata->nodes_expected) {
    g_string_append_c(rdata->data, '&');
  }

  return FALSE;
}

char *rtm_api_param_get_data(rtm_api *rtm) {
  rtm_data rdata;
  char *data;

  rdata.data = g_string_sized_new(RTM_API_KEY_LEN * 2);
  rdata.nodes_expected = g_tree_nnodes(rtm->_param_tree);
  rdata.nodes_seen = 0;

  g_tree_foreach(rtm->_param_tree, _rtm_api_param_escape, &rdata);

  data = g_strdup(rdata.data->str);

  g_string_free(rdata.data, TRUE);

  return data;
}

char *rtm_api_auth_get_url(rtm_api *rtm, const char *perms, const char *frob) {
  if (strcmp(perms, "none") == 0 || strcmp(perms, "read") == 0 ||
      strcmp(perms, "write") == 0 || strcmp(perms, "delete") == 0) {

    char *data, *url;
    int auth_len;

    rtm_api_param_init(rtm);
    rtm_api_param_add(rtm, "api_key", rtm->api_key);
    rtm_api_param_add(rtm, "perms", perms);
    if (frob) {
      rtm_api_param_add(rtm, "frob", frob);
    }
    rtm_api_param_sign_and_append(rtm);

    data = rtm_api_param_get_data(rtm);

    rtm_api_param_clear(rtm);

    auth_len = strlen(RTM_API_AUTH_ENDPOINT);
    url = malloc((auth_len + 1 + strlen(data) + 1) * sizeof(char));
    memcpy(url, RTM_API_AUTH_ENDPOINT, auth_len);
    url[auth_len] = '?';
    memcpy(url + auth_len + 1, data, strlen(data));

    free(data);

    return url;
  }

  return NULL;
}

static size_t _rtm_api_curl_write_data(void *buffer, size_t size,
                                       size_t nmemb, void *userp) {
  GString *http_data = userp;

  g_string_append_len(http_data, buffer, size * nmemb);

  return size * nmemb;
}

static char *rtm_api_perform_post(rtm_api *rtm, char *data) {
  GString *http_data;

  http_data = g_string_sized_new(80);

  curl_easy_setopt(rtm->_curl, CURLOPT_POSTFIELDS, data);
  curl_easy_setopt(rtm->_curl, CURLOPT_URL, RTM_API_REST_ENDPOINT);
  curl_easy_setopt(rtm->_curl, CURLOPT_WRITEFUNCTION, _rtm_api_curl_write_data);
  curl_easy_setopt(rtm->_curl, CURLOPT_WRITEDATA, http_data);

  curl_easy_perform(rtm->_curl);

  free(data);

  data = g_strdup(http_data->str);
  
  g_string_free(http_data, TRUE);

  return data;
}

char *rtm_api_call_method(rtm_api *rtm, char *method) {
  char *data;

  rtm_api_param_try_init(rtm);
  rtm_api_param_add(rtm, "api_key", rtm->api_key);
  rtm_api_param_add(rtm, "method", method);
  rtm_api_param_sign_and_append(rtm);

  data = rtm_api_param_get_data(rtm);

  rtm_api_param_clear(rtm);

  return rtm_api_perform_post(rtm, data);
}

char *rtm_api_call_authenticated_method(rtm_api *rtm, char *method) {
  rtm_api_param_try_init(rtm);
  rtm_api_param_add(rtm, "auth_token", rtm->auth_token);
  if (rtm->timeline) {
      rtm_api_param_add_long(rtm, "timeline", rtm->timeline);
  }

  return rtm_api_call_method(rtm, method);
}
