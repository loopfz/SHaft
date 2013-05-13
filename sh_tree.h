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

#ifndef		__SH_TREE_H__
#define		__SH_TREE_H__

typedef struct	token_s token_t;
typedef struct	exec_node_s exec_node_t;
typedef struct	redir_s redir_t;

typedef enum	TokenType {
  WHITESPACE,
  VARIABLE,
  QUOTES,
  SUBSTITUTION,
  GLOBBING,
  TEXT
} eTokenType;

struct		token_s {
  token_t	*prev;
  token_t	*next;

  token_t	*subtokens;

  int		(*expand)(token_t**, token_t*);

  union {
    char	*text;
    exec_node_t	*substitution_tree;
  } content;

  eTokenType	type;

  char		*(*to_string)(token_t*);
};

struct		redir_s {
  redir_t	*prev;
  redir_t	*next;

  int		(*redir_handler)(token_t*);
  token_t	*redir_arg;
};

struct		exec_node_s {
  token_t	*tokens;

  redir_t	*redirs;

  exec_node_t	*cond;
  exec_node_t	*left;
  exec_node_t	*right;

  int		(*exec_handler)(exec_node_t*);
};

#endif
