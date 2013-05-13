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
#include	"main.h"
#include	"sh_tree.h"
#include	"sh_token_handlers.h"
#include	"env.h"
#include	"sh_local_vars.h"
#include	"parse_input.h"

char		*concat_and_free(char *s1, char *s2) {
  int		len;
  char		*ret;

  if (s1 == NULL)
    return s2;
  if (s2 == NULL)
    return s1;

  len = strlen(s1) + strlen(s2) + 1;
  if (len <= 1)
    return NULL;
  ret = xmalloc(len);
  strcpy(ret, s1);
  strcat(ret, s2);
  free(s1);
  free(s2);
  return ret;
}

char		*quotes_token_handler(token_t *token) {
  char		*ret;
  char		*s;
  token_t	*it;

  ret = NULL;
  for (it = token->subtokens; it != NULL; it = it->next) {
    s = it->to_string(it);
    ret = concat_and_free(ret, s);
  }
  return ret;
}

char		*variable_token_handler(token_t *token) {
  char		*var;
  var_t		*localv;

  if ((var = sh_getenv(token->content.text)) != NULL)
    return strdup(var);
  if ((localv = get_local_var(token->content.text)) != NULL) {
    return strdup(localv->value);
  }
  return strdup("");
}

char		*sh_arg_token_handler(token_t *token) {
  int		i;
  int		val;

  if ((val = atoi(token->content.text)) >= 0
      || val <= 9) {
    for (i = 0; sh.args[i] != NULL; i++) {
      if (i == val)
	return strdup(sh.args[i]);
    }
  }
  return strdup("");
}

char		*text_handler(token_t *token) {
  return strdup(token->content.text);
}

char		*substitution_token_handler(token_t *token) {
  int		pfd[2];
  char		buf[512 + 1];
  char		*s;
  int		pid;
  int		ret;
  int		len;

  if (pipe(pfd) != -1) {
    if ((pid = fork()) != -1) {
      if (pid == 0) {
	dup2(pfd[1], 1);
	close(pfd[0]);
	exit(token->content.substitution_tree->
	    exec_handler(token->content.substitution_tree));
      }
      else {
	s = NULL;
	close(pfd[1]);
	len = 0;
	while ((ret = xread(pfd[0], buf, sizeof(buf) - 1)) > 0) {
	  len += ret;
	  buf[ret] = '\0';
	  s = concat_and_free(s, strdup(buf));
	}
	if (len > 0 && s[len - 1] == '\n')
	  s[len - 1] = '\0';
	close(pfd[0]);
	return s;
      }
    }
  }
  return strdup("");
}

int		aggregate_tokens(token_t *list, char **res) {
  int		n;
  bool		reach;
  token_t	*it;
  char		*s;

  n = 0;
  it = list;
  while (it != NULL) {
    while (it != NULL && it->type == WHITESPACE)
      it = it->next;
    reach = 0;
    s = NULL;
    while (it != NULL && it->type != WHITESPACE) {
      if (res != NULL)
	s = concat_and_free(s, it->to_string(it));
      reach = 1;
      it = it->next;
    }
    if (reach) {
      if (res != NULL)
	res[n] = s;
      n++;
    }
  }
  if (res != NULL)
    res[n] = NULL;
  return n;
}

char		**token_to_args(token_t *list) {
  int		n_words;
  char		**ret;

  if ((n_words = aggregate_tokens(list, NULL)) == 0)
    return NULL;
  ret = xmalloc((n_words + 1) * sizeof(*ret));
  aggregate_tokens(list, ret);
  return ret;
}

void		expand_tokens(token_t **list) {
  token_t	*it;
  token_t	*next;

  for (it = *list; it != NULL; it = next) {
    next = it->next;
    if (it->subtokens != NULL) {
      expand_tokens(&it->subtokens);
    }
    if (it->expand != NULL)
      it->expand(list, it);
  }
}

int		tokenize_expand(token_t **list, token_t *token) {
  char		*s;
  token_t	*tk_list;
  token_t	*it;

  if ((s = token->to_string(token)) == NULL)
    return 0;
  if ((tk_list = tokenize(s)) == NULL)
    return 0;
  free(s);
  for (it = tk_list; it->next != NULL; it = it->next)
  {}
  if (token->next)
    token->next->prev = it;
  it->next = token->next;
  if (token->prev)
    token->prev->next = tk_list;
  else
    *list = tk_list;

  free(token);
  //TODO deep free tk

  return 1;
}
