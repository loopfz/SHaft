/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Thomas Schaffer <thomas.schaffer@epitech.eu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include	<string.h>
#include	"main.h"
#include	"env.h"

char		*sh_getenv(char *name) {
  char		**env;
  int		i;
  int		len;

  if ((env = sh.environment) == NULL)
    return NULL;
  len = strlen(name);
  for (i = 0; env[i]; i++) {
    if (strncmp(name, env[i], len) == 0 && env[i][len] == '=') {
      return &env[i][len + 1];
    }
  }
  return NULL;
}

void		sh_setenv(char *name, char *value) {
  char		buf[2048];
  int		i;
  char		**env;
  int		len;

  len = strlen(name);
  if (value == NULL)
    value = "";
  i = 0;
  snprintf(buf, sizeof(buf), "%s=%s", name, value);
  if ((env = sh.environment) != NULL) {
    for (i = 0; env[i] != NULL; i++) {
      if (strncmp(name, env[i], len) == 0 && env[i][len] == '=') {
	free(env[i]);
	env[i] = strdup(buf);
	return;
      }
    }
  }
  env = xmalloc((i + 2) * sizeof(*env));
  for (i = 0; sh.environment[i] != NULL; i++) {
    env[i] = strdup(sh.environment[i]);
  }
  env[i++] = strdup(buf);
  env[i] = NULL;
  free_2dtab(sh.environment);
  sh.environment = env;
}

int		sh_unsetenv(char *name) {
  int		i;
  int		len;
  int		name_len;
  char		**env;
  int		j;

  if (sh.environment == NULL)
    return 0;
  i = -1;
  name_len = strlen(name);
  for (len = 0; sh.environment[len] != NULL; len++) {
    if (strncmp(name, sh.environment[len], name_len) == 0)
      i = len;
  }
  if (i == -1)
    return 0;
  env = xmalloc(len * sizeof(*env));
  for (len = 0; sh.environment[len] != NULL; len++) {
    if (len != i) {
      env[j++] = strdup(sh.environment[len]);
    }
  }
  env[j] = NULL;
  free_2dtab(sh.environment);
  sh.environment = env;
  return 1;
}
