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

#ifndef		__PARSE_INPUT_H__
#define		__PARSE_INPUT_H__

#define		SAVED_IDX		s_idx
#define		SAVE_CONTEXT(x)		int SAVED_IDX = x
#define		RESTORE_CONTEXT(x)	x = SAVED_IDX

typedef struct	state_s state_t;

typedef enum	StateType {
  OPERATOR,
  IF,
  WHILE,
  FOR,
  FUNCTION,
  ENCLOSED,
  QUOTE
} eStateType;

struct		state_s {
  state_t	*next;

  eStateType	type;

  char		*str;

};

exec_node_t	*parse_input();
token_t		*tokenize(char *s);

// GENERIC OPERATOR BLOCK PARSING FUNC
int		parse_cmd(
    exec_node_t **list,
    int (*parser_downlevel)(exec_node_t**),
    int (*parser_operator)(),
    int (*parser_recursive)(exec_node_t**),
    int (*exec_handler)(exec_node_t*),
    char *operator_s,
    bool end
    );

// OPERATOR PRIORITY
int		parse_cmd_list(exec_node_t **list);
int		parse_cmd_or(exec_node_t **list);
int		parse_cmd_and(exec_node_t **list);
int		parse_cmd_pipe(exec_node_t **list);
int		parse_keyword(exec_node_t **list);

// IF/WHILE/FOR
int		parse_if_cmd(exec_node_t **list);
int		parse_elif_cmd(exec_node_t **list);
int		parse_while_cmd(exec_node_t **list);
int		parse_for_cmd(exec_node_t **list);

// SHELL FUNCTIONS
int		parse_function_keyw(exec_node_t **list);
int		parse_function_parenth(exec_node_t **list);
int		parse_func_decl(exec_node_t **list, bool parenth);

// ENCLOSED (SUBSHELL) {CURSHELL}
int		parse_enclosed_cmd(exec_node_t **list,
    int (*handler)(exec_node_t*), char *opening_pattern,
    char *closing_pattern);
int		parse_subsh_cmd(exec_node_t **list);
int		parse_cursh_cmd(exec_node_t **list);

// REGULAR CMD
int		parse_simple_cmd(exec_node_t **list);

// REDIRS
int		parse_redirections(exec_node_t *node);

// TOKENS
int		parse_tokens(exec_node_t *node);
token_t		*new_token(char *(*handler)(token_t*), eTokenType type,
    int (*expand)(token_t**, token_t*));
int		read_token(token_t **tk_list);
int		read_terminal_token(token_t **tk_list);
int		read_variable(token_t **list);
int		read_quotes(token_t **list, bool allow_expand, char *litteral);
int		read_subst(token_t **tk_list, char *opening, char *closing);

// OPERATORS
int		parse_reg_operator();
int		parse_or_operator();
int		parse_and_operator();
int		parse_pipe_operator();

// LOW LEVEL STREAM CONSUMING FUNCS
int		read_char(char c);
int		read_char_range(char start, char end);
char		peek_char();
void		consume_char();
int		litteral(char *s, bool consume);
int		lookahead_litteral(char *s);
int		read_litteral(char *s);
void		read_single_digit();
void		read_identifier();
void		skip_whitespaces();
char		*capture_substr(void (*reader)());
int		read_to_token(token_t **tk_list, void (*reader)(),
    eTokenType type);
void		capture_whitespaces(token_t **list);

#endif
