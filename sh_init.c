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

#include	<pwd.h>
#include	<string.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<signal.h>
#include	"main.h"
#include	"sh_init.h"
#include	"env.h"
#include	"read_cmd.h"

void		open_config_file() {
  char		path[4096];
  char		*home_path;

  if ((home_path = sh_getenv("HOME")) != NULL) {
    snprintf(path, sizeof(path), "%s/%s", home_path, SHAFT_CONFIG_FILE);
    if ((sh.read_cmd_fd = open(path, O_RDONLY|O_CREAT, 0644)) != -1) {

      read_cmd_loop();
      close(sh.read_cmd_fd);
      sh.read_cmd_fd = 0;
    }
    else
      shell_warn("Could not open configuraton file: '%s'.\n", path);
  }
  else
    shell_warn("No '$HOME' environment variable, \
could not load configuration file.\n");
}

char		*get_username() {
  struct passwd	*pw;

  if ((pw = getpwuid(geteuid())) != NULL) {
    return pw->pw_name;
  }
  return NULL;
}

void		init_prompt()
{
  char		*user;
  char		*host;
  char		prompt[255];
  char		c;
  uid_t		uid;

  c = ((uid = geteuid()) == 0 ? '#' : '$' );
  if ((user = get_username()) != NULL) {
    if ((host = sh_getenv("HOSTNAME")) != NULL) {
      snprintf(prompt, sizeof(prompt), "%s@%s%c ", user, host, c);
    }
    else
      snprintf(prompt, sizeof(prompt), "%s%c ", user, c);
  }
  else
    snprintf(prompt, sizeof(prompt), "%c ", c);

  sh.prompt = strdup(prompt);
}

char		**make_sh_args_tab() {
  char		**ret;
  size_t	sz;

  sz = 11 * sizeof(*ret);
  ret = xmalloc(sz);
  memset(ret, 0, sz);
  return ret;
}

void		init_signals(bool ignore) {
  __sighandler_t handler;

  if (ignore)
    handler = SIG_IGN;
  else
    handler = SIG_DFL;
  signal(SIGINT, handler);
  signal(SIGKILL, handler);
  signal(SIGTSTP, handler);
}

void		set_pwd() {
  char		buf[4096];

  if (getcwd(buf, sizeof(buf)) != NULL) {
    sh_setenv("PWD", buf);
  }
}

void		init_pwd() {
  sh_unsetenv("OLDPWD");
  set_pwd();
}

void		init_shell(char **initial_env) {
  memset(&sh, 0, sizeof(sh));
  sh.environment = duplicate_env(initial_env);
  sh.args = make_sh_args_tab();
  init_pwd();

  open_config_file();
  init_prompt();
  init_signals(1);
}
