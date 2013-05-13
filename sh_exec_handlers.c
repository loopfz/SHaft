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
#include	<sys/wait.h>
#include	"main.h"
#include	"sh_tree.h"
#include	"sh_init.h"
#include	"sh_exec_handlers.h"
#include	"sh_token_handlers.h"
#include	"env.h"
#include	"sh_local_vars.h"
#include	"parse_input.h"
#include	"sh_list.h"
#include	"built_ins.h"
#include	"alias.h"

int		exec_redirs(exec_node_t *node) {
  redir_t	*it;

  for (it = node->redirs; it != NULL; it = it->next) {
    if (it->redir_handler(it->redir_arg) == 0)
      return 0;
  }
  return 1;
}

int		AND_handler(exec_node_t *node) {
  if (exec_redirs(node) == 0)
    return 0;
  return (node->left->exec_handler(node->left)
      && node->right->exec_handler(node->right));
}

int		OR_handler(exec_node_t *node) {
  if (exec_redirs(node) == 0)
    return 0;
  return (node->left->exec_handler(node->left)
      || node->right->exec_handler(node->right));
}

int		REG_handler(exec_node_t *node) {
  int ret;

  if (exec_redirs(node) == 0)
    return 0;
  ret = node->left->exec_handler(node->left);
  if (node->right != NULL)
    return node->right->exec_handler(node->right);
  return ret;
}

int		CONDITION_handler(exec_node_t *node, bool loop) {
  int ret;

  ret = 1;
  if (exec_redirs(node) == 0)
    return 0;
  if (node->cond->exec_handler(node->cond)) {
    ret = node->right->exec_handler(node->right);
    if (loop)
      return node->exec_handler(node);
  }
  else if (node->left != NULL) {
    node->left->exec_handler(node->left);
  }
  return ret;
}

int		IF_handler(exec_node_t *node) {
  return CONDITION_handler(node, 0);
}

int		WHILE_handler(exec_node_t *node) {
  return CONDITION_handler(node, 1);
}

int		FOR_handler(exec_node_t *node) {
  char		**values;
  int		i;
  int		ret;

  if ((values = token_to_args(node->tokens)) == NULL)
    return 0;

  ret = 1;
  for (i = 1; values[i] != NULL; i++) {
    set_variable(values[0], values[i]);
    ret = node->right->exec_handler(node->right);
    free(values[i]);
  }

  free(values[0]);
  free(values);

  return ret;
}

int		PIPE_handler(exec_node_t *node) {
  int		pid;
  int		ret;
  int		pfd[2];
  int		st;

  if (exec_redirs(node) == 0)
    return 0;
  ret = 0;
  if (pipe(pfd) != -1) {
    if ((pid = fork()) == -1)
      shell_warn("fork() failed, skipping.\n");
    else if (pid == 0) {
      dup2(pfd[1], 1);
      close(pfd[0]);
      close(pfd[1]);
      node->left->exec_handler(node->left);
      exit(0);
    }
    else {
      dup2(pfd[0], 0);
      close(pfd[1]);
      close(pfd[0]);
      ret = node->right->exec_handler(node->right);
      waitpid(pid, &st, 0);
    }
  }
  else
    shell_warn("pipe() failed, skipping.\n");
  return ret;
}

int		VARASSIGN_handler(exec_node_t *node) {
  char		**values;
  char		*name;

  if ((values = token_to_args(node->tokens->next)) == NULL)
    return 0;

  name = node->tokens->to_string(node->tokens);

  set_local_var(name, values[0]);

  free(name);
  free(values[0]);
  free(values);

  return 1;
}

int		FUNC_handler(exec_node_t *node) {
  func_t	*func;
  char		*name;

  if ((name = node->tokens->to_string(node->tokens)) == NULL)
    return 0;
  if ((func = sh_get_func(name)) != NULL) {
    exec_node_deep_free(func->cmd_list);
    func->cmd_list = exec_node_deep_copy(node->right);
    free(name);
  }
  else {
    func = xmalloc(sizeof(*func));
    memset(func, 0, sizeof(*func));
    func->name = name;
    func->cmd_list = exec_node_deep_copy(node->right);
    push_in_list((list_t**)&sh.funcs, (list_t*)func);
  }
  return 1;
}

int		SUBSH_handler(exec_node_t *node) {
  int		pid;
  int		st;

  if (exec_redirs(node) == 0)
    return 0;
  if ((pid = fork()) == -1)
    shell_warn("fork() failed, skipping.\n");
  else if (pid == 0) {
    exit(node->right->exec_handler(node->right) == 0);
  }
  else {
    waitpid(pid, &st, 0);
  }
  return (st == 0);
}

int		CURSH_handler(exec_node_t *node) {
  if (exec_redirs(node) == 0)
    return 0;
  return (node->right->exec_handler(node->right) == 0);
}

int		exec_function(func_t *func, char **argv) {
  int		ret;
  char		**args_backup;
  exec_node_t	*exec_tree;

  args_backup = sh.args;
  sh.args = argv;
  exec_tree = exec_node_deep_copy(func->cmd_list);
  ret = exec_tree->exec_handler(exec_tree);
  sh.args = args_backup;
  exec_node_deep_free(exec_tree);
  free_2dtab(argv);
  return ret;
}

int		exec_alias(var_t *alias, exec_node_t *node) {
  token_t	*tk_list;
  token_t	*end_tk_list;
  token_t	*old_list;
  token_t	*it;
  int		ret;

  if ((tk_list = tokenize(alias->value)) == NULL)
    return 0;

  alias->state = 1;

  for (end_tk_list = tk_list; end_tk_list->next != NULL;
      end_tk_list = end_tk_list->next)
  {}
  for (it = node->tokens; it != NULL && it->type != WHITESPACE;
      it = it->next)
  {}
  old_list = node->tokens;
  end_tk_list->next = it;
  node->tokens = tk_list;

  if (it != NULL) {
    if (it->prev != NULL)
      it->prev->next = NULL;
    it->prev = end_tk_list;
  }

  token_deep_free(old_list);

  ret = node->exec_handler(node);

  alias->state = 0;
  return ret;
}

int		EXEC_handler(exec_node_t *node) {
  int		pid;
  int		ret;
  char		*file;
  char		**argv;
  int		st;
  func_t	*func;
  sh_built_in	builtin;
  var_t		*alias;

  if (exec_redirs(node) == 0)
    return 0;

  expand_tokens(&node->tokens);

  if ((argv = token_to_args(node->tokens)) == NULL)
    return 0;

  if ((alias = get_alias(argv[0])) != NULL && !alias->state) {
    free_2dtab(argv);
    return exec_alias(alias, node);
  }

  if ((func = sh_get_func(argv[0])) != NULL) {
    return exec_function(func, argv);
  }

  if ((builtin = get_built_in(argv[0])) != NULL) {
    ret = (builtin(argv) == 0);
    free_2dtab(argv);
    return ret;
  }

  ret = 0;
  if ((file = find_exec(argv[0])) != NULL) {
    if ((pid = fork()) == -1) {
      shell_warn("fork() failed. Skipping.\n");
    }
    else if (pid == 0) {
      init_signals(0);
      execve(file, argv, sh.environment);
    }
    else {
      waitpid(pid, &st, 0);
      ret = st == 0;
    }
    free(file);
  }
  else
    shell_warn("%s: Command not found.\n", argv[0]);

  free_2dtab(argv);

  return ret;
}

int		is_path(char *p) {
  int		i;

  for (i = 0; p[i] != '\0'; i++) {
    if (p[i] == '/')
      return 1;
  }
  return 0;
}

char		*find_exec(char *name) {
  char		*path;
  int		i;
  int		j;
  char		buf[4096];
  bool		end;

  if (is_path(name)) {
    return (access(name, X_OK) == 0 ? strdup(name) : NULL);
  }

  if ((path = sh_getenv("PATH")) == NULL)
    return NULL;

  if ((path = strdup(path)) == NULL)
    return NULL;

  i = 0;
  j = 0;
  end = 0;
  while (path[i] != '\0') {
    while (path[j] != '\0' && path[j] != ':')
      j++;
    if (path[j] == '\0')
      end = 1;
    path[j] = '\0';
    if (j - i > 0) {
      snprintf(buf, sizeof(buf), "%*s/%s", j - i, &path[i], name);
      if (access(buf, X_OK) == 0) {
	free(path);
	return strdup(buf);
      }
    }
    if (!end)
      j++;
    i = j;
  }
  free(path);
  return NULL;
}
