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

token_t		*token_deep_copy(token_t *token, token_t *prev) {
  token_t	*new;

  if (token == NULL)
    return NULL;
  new = xmalloc(sizeof(*new));
  memset(new, 0, sizeof(*new));

  new->subtokens = token_deep_copy(token->subtokens, NULL);
  new->prev = prev;
  new->next = token_deep_copy(token->next, new);
  new->expand = token->expand;
  new->type = token->type;
  new->to_string = token->to_string;

  if (token->content.text != NULL)
    new->content.text = strdup(token->content.text);
  else if (token->content.substitution_tree != NULL)
    new->content.substitution_tree = exec_node_deep_copy(
	token->content.substitution_tree);

  return new;
}

redir_t		*redir_deep_copy(redir_t *redir, redir_t *prev) {
  redir_t	*new;

  if (redir == NULL)
    return NULL;

  new = xmalloc(sizeof(*new));
  memset(new, 0, sizeof(*new));

  new->prev = prev;
  new->next = redir_deep_copy(redir->next, new);

  new->redir_handler = redir->redir_handler;
  new->redir_arg = token_deep_copy(redir->redir_arg, NULL);

  return new;
}

exec_node_t	*exec_node_deep_copy(exec_node_t *node) {
  exec_node_t	*new;

  if (node == NULL)
    return NULL;

  new = xmalloc(sizeof(*new));
  memset(new, 0, sizeof(*new));

  new->cond = exec_node_deep_copy(node->cond);
  new->left = exec_node_deep_copy(node->left);
  new->right = exec_node_deep_copy(node->right);

  new->exec_handler = node->exec_handler;

  new->redirs = redir_deep_copy(node->redirs, NULL);
  new->tokens = token_deep_copy(node->tokens, NULL);

  return new;
}

void		token_deep_free(token_t *token) {
  if (token == NULL)
    return;

  token_deep_free(token->next);
  token_deep_free(token->subtokens);

  if (token->content.text != NULL)
    free(token->content.text);
  else if (token->content.substitution_tree != NULL)
    exec_node_deep_free(token->content.substitution_tree);

  free(token);
}

void		redir_deep_free(redir_t *redir) {
  if (redir == NULL)
    return;

  redir_deep_free(redir->next);
  token_deep_free(redir->redir_arg);

  free(redir);
}

void		exec_node_deep_free(exec_node_t *node) {
  if (node == NULL)
    return;

  exec_node_deep_free(node->cond);
  exec_node_deep_free(node->left);
  exec_node_deep_free(node->right);

  redir_deep_free(node->redirs);
  token_deep_free(node->tokens);

  free(node);
}

void		free_2dtab(char **tab) {
  int		i;

  for (i = 0; tab[i] != NULL; i++) {
    free(tab[i]);
  }
  free(tab);
}
