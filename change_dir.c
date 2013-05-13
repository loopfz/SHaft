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

#include	<unistd.h>
#include	<sys/stat.h>
#include	"main.h"
#include	"sh_init.h"
#include	"env.h"
#include	"change_dir.h"

int		do_chdir(char *path) {
  char		*pwd;

  if (chdir(path) != 0) {
    shell_warn("cd: chdir() failed.\n");
    return -1;
  }
  if ((pwd = sh_getenv("PWD")) != NULL) {
    sh_setenv("OLDPWD", pwd);
  }
  else
    sh_unsetenv("OLDPWD");
  set_pwd();
  return 0;
}

int		change_dir(char *path) {
  struct stat	sb;

  if (access(path, F_OK) == 0) {
    if (stat(path, &sb) == 0) {
      if (S_ISDIR(sb.st_mode)) {
	if (access(path, X_OK) == 0) {
	  return do_chdir(path);
	}
	else
	  shell_warn("cd: %s: Permission denied.\n", path);
      }
      else
	shell_warn("cd: %s: Not a directory.\n", path);
    }
    else
      shell_warn("cd: %s: Permission denied.\n", path);
  }
  else
    shell_warn("cd: %s: No such file or directory.\n", path);
  return -1;
}

int		cd_to_env_var(char *var) {
  char		*path;

  if ((path = sh_getenv(var)) != NULL) {
    return change_dir(path);
  }
  else
    shell_warn("cd: %s not set.\n", var);
  return -1;
}

int		builtin_cd(char **argv) {

  if (argv[1] == NULL) {
    return cd_to_env_var("HOME");
  }
  if (argv[2] != NULL) {
    shell_warn("cd: Too many arguments.\n");
    return -1;
  }
  if (strcmp("-", argv[1]) == 0)
    return cd_to_env_var("OLDPWD");
  return change_dir(argv[1]);
}
