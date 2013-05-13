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

#include	"main.h"
#include	"sh_tree.h"
#include	"sh_input.h"
#include	"parse_input.h"
#include	"sh_exec_handlers.h"
#include	"sh_redirs_handlers.h"
#include	"sh_token_handlers.h"
#include	"sh_list.h"

static char	*input;
static int	gidx;

static bool	global_error;

static bool	inhib_bslash;
static bool	inhib_context;
static bool	expand_values;

static char	*closing_symbol;

static struct {
  char *keyword;
  int (*parser)(exec_node_t**);
} start_keywords[] = {
  { "while", parse_while_cmd },
  { "if", parse_if_cmd },
  { "for", parse_for_cmd },
  { "(", parse_subsh_cmd },
  { "{", parse_cursh_cmd },
  { "function", parse_function_keyw },
  { NULL, NULL }
};

static char *scoping_keywords[] = {
  "do",
  "done",
  "then",
  "elif",
  "else",
  "fi",
  ")",
  "}",
  NULL
};

static char *operators[] = {
  "||",
  "&&",
  ";",
  "|",
  ")",
  "}"
};

static state_t	*state_stack;

exec_node_t	*new_exec_node(int (*handler)(exec_node_t*)) {
  exec_node_t	*new;

  new = xmalloc(sizeof(*new));
  memset(new, 0, sizeof(*new));
  new->exec_handler = handler;
  return new;
}

void		push_state(eStateType type, char *str) {
  state_t	*new;

  new = xmalloc(sizeof(*new));
  new->type = type;
  new->str = str;
  new->next = state_stack;
  state_stack = new;
}

void		pop_state(eStateType type) {
  state_t	*st;

  if (state_stack == NULL || state_stack->type != type)
    return ;

  st = state_stack;
  state_stack = state_stack->next;
  free(st);
}

void		change_state(eStateType type, eStateType new_type, char *str) {
  if (state_stack == NULL || state_stack->type != type)
    return ;

  state_stack->type = new_type;
  state_stack->str = str;
}

void		init_parser(bool expand, bool inhib)
{
  gidx = 0;

  global_error = 0;
  inhib_bslash = 0;
  inhib_context = inhib;
  expand_values = expand;
  sh.interrupt = 0;

  if (input != NULL) {
    free(input);
    input = NULL;
  }

  while (state_stack != NULL) {
    pop_state(state_stack->type);
  }
}

token_t		*tokenize(char *s) {
  token_t	*tk_list;

  init_parser(0, 1);
  tk_list = NULL;
  input = s;

  capture_whitespaces(&tk_list);
  while (read_terminal_token(&tk_list)) {
    capture_whitespaces(&tk_list);
  }

  input = NULL;

  return tk_list;
}

exec_node_t	*parse_input() {
  SAVE_CONTEXT(gidx);
  exec_node_t	*list;

  list = NULL;
  init_parser(1, 0);

  if (parse_cmd_list(&list)) {
    if (input == NULL || input[gidx] != '\0') {
      exec_node_deep_free(list);
      shell_warn("Syntax error.\n", input);
      return NULL;
    }
    return list;
  }

  skip_whitespaces();

  if (list != NULL)
    exec_node_deep_free(list);

  if ((input == NULL || input[gidx] != '\0')
      && !sh.interrupt) {
    shell_warn("Syntax error.\n", input);
  }

  RESTORE_CONTEXT(gidx);
  return NULL;
}

int		parse_cmd(
    exec_node_t **list,
    int (*parser_downlevel)(exec_node_t**),
    int (*parser_operator)(),
    int (*parser_recursive)(exec_node_t**),
    int (*exec_handler)(exec_node_t*),
    char *operator_s,
    bool end
    ) {
  SAVE_CONTEXT(gidx);
  exec_node_t	*cmd_list;

  if (global_error)
    return 0;

  cmd_list = NULL;
  if (parser_downlevel(&cmd_list)) {
    skip_whitespaces();
    if (parser_operator()) {
      if (!end)
	push_state(OPERATOR, operator_s);
      *list = new_exec_node(exec_handler);
      (*list)->left = cmd_list;
      if (parser_recursive(&(*list)->right)) {
	skip_whitespaces();
	return 1;
      }
      if (end) {
	skip_whitespaces();
	return 1;
      }
      exec_node_deep_free(*list);
    }
    else {
      skip_whitespaces();
      *list = cmd_list;
      return 1;
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_cmd_list(exec_node_t **list) {
  return parse_cmd(list, parse_cmd_or, parse_reg_operator,
      parse_cmd_list, REG_handler, "sep", 1);
}

int		parse_cmd_or(exec_node_t **list) {
  return parse_cmd(list, parse_cmd_and, parse_or_operator,
      parse_cmd_or, OR_handler, "or", 0);
}

int		parse_cmd_and(exec_node_t **list) {
  return parse_cmd(list, parse_cmd_pipe, parse_and_operator,
      parse_cmd_and, AND_handler, "and", 0);
}

int		parse_cmd_pipe(exec_node_t **list) {
  return parse_cmd(list, parse_keyword, parse_pipe_operator,
      parse_cmd_pipe, PIPE_handler, "pipe", 0);
}

int		parse_reg_operator() {
  return read_litteral(";");
}

int		parse_or_operator() {
  return read_litteral("||");
}

int		parse_and_operator() {
  return read_litteral("&&");
}

int		parse_pipe_operator() {
  if (!lookahead_litteral("||"))
    return read_litteral("|");
  return 0;
}

int		parse_while_cmd(exec_node_t **list) {
  SAVE_CONTEXT(gidx);

  push_state(WHILE, "while");
  *list = new_exec_node(WHILE_handler);
  if (parse_cmd_list(&(*list)->cond)) {
    if (read_litteral("do")) {
      change_state(WHILE, WHILE, "do");
      if (parse_cmd_list(&(*list)->right)) {
	if (read_litteral("done")) {
	  pop_state(WHILE);
	  if (parse_redirections(*list) != -1)
	    return 1;
	}
      }
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_for_cmd(exec_node_t **list) {
  SAVE_CONTEXT(gidx);
  bool		reach;

  push_state(FOR, "for");
  *list = new_exec_node(FOR_handler);
  skip_whitespaces();
  if (read_to_token(&(*list)->tokens, read_identifier, TEXT)) {
    capture_whitespaces(&(*list)->tokens);
    if (read_litteral("in")) {
      change_state(FOR, FOR, "in");
      skip_whitespaces();
      reach = 0;
      while (parse_tokens(*list) && !global_error) {
	reach = 1;
	capture_whitespaces(&(*list)->tokens);
      }
      if (reach) {
	if (parse_reg_operator()) {
	  skip_whitespaces();
	  if (read_litteral("do")) {
	    change_state(FOR, FOR, "do");
	    if (parse_cmd_list(&(*list)->right)) {
	      if (read_litteral("done")) {
		pop_state(FOR);
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_condition_cmd(exec_node_t **list, bool final) {
  SAVE_CONTEXT(gidx);

  if (final) {
    push_state(IF, "if");
  }
  skip_whitespaces();
  *list = new_exec_node(IF_handler);
  if (parse_cmd_list(&(*list)->cond)) {
    skip_whitespaces();
    if (read_litteral("then")) {
      change_state(IF, IF, "then");
      if (parse_cmd_list(&(*list)->right)) {
	if (read_litteral("elif")) {
	  change_state(IF, IF, "elif");
	  if (!parse_elif_cmd(&(*list)->left)) {
	    RESTORE_CONTEXT(gidx);
	    return 0;
	  }
	}
	else if (read_litteral("else")) {
	  change_state(IF, IF, "else");
	  if (!parse_cmd_list(&(*list)->left)) {
	    RESTORE_CONTEXT(gidx);
	    return 0;
	  }
	}
	if (!final)
	  return 1;
	if (read_litteral("fi")) {
	  pop_state(IF);
	  if (parse_redirections(*list) != -1) {
	    return 1;
	  }
	}
      }
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_if_cmd(exec_node_t **list) {
  return parse_condition_cmd(list, 1);
}

int		parse_elif_cmd(exec_node_t **list) {
  return parse_condition_cmd(list, 0);
}

int		parse_enclosed_cmd(exec_node_t **list,
    int (*handler)(exec_node_t*), char *opening_pattern,
    char *closing_pattern) {
  SAVE_CONTEXT(gidx);

  push_state(ENCLOSED, opening_pattern);
  *list = new_exec_node(handler);
  if (parse_cmd_list(&((*list)->right))) {
    if (read_litteral(closing_pattern)) {
      pop_state(ENCLOSED);
      if (parse_redirections(*list) != -1) {
	return 1;
      }
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_subsh_cmd(exec_node_t **list) {
  return parse_enclosed_cmd(list, SUBSH_handler, "(", ")");
}

int		parse_cursh_cmd(exec_node_t **list) {
  return parse_enclosed_cmd(list, CURSH_handler, "{", "}");
}

int		parse_var_assign(exec_node_t **list) {
  SAVE_CONTEXT(gidx);
  char		*s;
  token_t	*var;

  if ((s = capture_substr(read_identifier)) != NULL) {
    if (read_litteral("=")) {
      *list = new_exec_node(VARASSIGN_handler);
      var = new_token(text_handler, TEXT, NULL);
      var->content.text = s;
      (*list)->tokens = var;
      while (!lookahead_litteral(" ") && !lookahead_litteral("\t")
	  && parse_tokens(*list) && !global_error)
      {}
      return 1;
    }
    free(s);
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_func_decl(exec_node_t **list, bool parenth) {
  SAVE_CONTEXT(gidx);
  char		*s;
  exec_node_t	*new;

  if (!parenth)
    push_state(FUNCTION, "function");
  if ((s = capture_substr(read_identifier)) != NULL) {
    skip_whitespaces();
    if (!parenth || read_litteral("()")) {
      if (parenth) {
	pop_state(OPERATOR);
	push_state(FUNCTION, "function");
      }
      skip_whitespaces();
      new = new_exec_node(FUNC_handler);
      new->tokens = new_token(text_handler, TEXT, NULL);
      new->tokens->content.text = s;
      if (read_litteral("{")) {
	if (parse_cmd_list(&new->right)) {
	  if (read_litteral("}")) {
	    pop_state(FUNCTION);
	    *list = new;
	    return 1;
	  }
	}
      }
    }
    free(s);
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_function_keyw(exec_node_t **list) {
  return parse_func_decl(list, 0);
}

int		parse_function_parenth(exec_node_t **list) {
  return parse_func_decl(list, 1);
}

int		parse_keyword(exec_node_t **list) {
  SAVE_CONTEXT(gidx);
  int		ret;
  int		i;

  if (global_error)
    return 0;

  skip_whitespaces();

  if (closing_symbol != NULL
      && lookahead_litteral(closing_symbol)
      && !inhib_bslash) {
    return 0;
  }

  for (i = 0; scoping_keywords[i] != NULL; i++) {
    if (lookahead_litteral(scoping_keywords[i])) {
      RESTORE_CONTEXT(gidx);
      return 0;
    }
  }

  for (i = 0; start_keywords[i].keyword != NULL; i++) {
    if (read_litteral(start_keywords[i].keyword)) {
      pop_state(OPERATOR);
      ret = start_keywords[i].parser(list);
      if (ret == 0)
	RESTORE_CONTEXT(gidx);
      return ret;
    }
  }

  if (parse_function_parenth(list)
      ||parse_var_assign(list)
      || parse_simple_cmd(list))
    return 1;

  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_simple_cmd(exec_node_t **list) {
  SAVE_CONTEXT(gidx);
  bool		reached;

  if (global_error)
    return 0;

  *list = new_exec_node(EXEC_handler);

  reached = 0;
  while (parse_tokens(*list) && !global_error) {
    if (reached == 0)
      pop_state(OPERATOR);
    reached = 1;
    capture_whitespaces(&(*list)->tokens);
  }

  if (reached)
    return 1;

  RESTORE_CONTEXT(gidx);
  return 0;
}

int		parse_redirections(exec_node_t *node) {
  SAVE_CONTEXT(gidx);
  int		i;
  int		ret;
  redir_t	*new;
  struct {
    char *operator;
    int (*handler)(token_t*);
  } redirs[] = {
    { "<<", dleft_redir_handler },
    { ">>", dright_redir_handler },
    { "<", left_redir_handler },
    { ">", right_redir_handler },
    { NULL, NULL }
  };

  skip_whitespaces();
  ret = 0;
  for (i = 0; redirs[i].operator != NULL; i++) {
    if (read_litteral(redirs[i].operator)) {
      skip_whitespaces();

      new = xmalloc(sizeof(*new));
      new->next = NULL;
      new->prev = NULL;
      new->redir_handler = redirs[i].handler;
      new->redir_arg = NULL;

      push_in_list((list_t**)&node->redirs, (list_t*)new);

      if (read_token(&new->redir_arg)) {
	return 1;
      }

      ret = -1;
      break;
    }
  }
  RESTORE_CONTEXT(gidx);
  return ret;
}

int		parse_tokens(exec_node_t *node) {
  SAVE_CONTEXT(gidx);
  int		ret;

  if (global_error)
    return 0;

  if ((ret = parse_redirections(node)) != 0)
    return (ret == 1);

  if (read_token(&node->tokens))
    return 1;

  RESTORE_CONTEXT(gidx);
  return 0;
}

token_t		*new_token(char *(*handler)(token_t*), eTokenType type,
    int (*expand)(token_t**, token_t*)) {
  token_t	*new;

  new = xmalloc(sizeof(*new));
  memset(new, 0, sizeof(*new));
  new->to_string = handler;
  new->type = type;
  new->expand = expand;
  return new;
}

int		read_token(token_t **tk_list) {
  SAVE_CONTEXT(gidx);
  int		i;

  if (global_error)
    return 0;

  if (!inhib_context) {
    for (i = 0; operators[i] != NULL; i++) {
      if (lookahead_litteral(operators[i]))
	return 0;
    }
    if (closing_symbol != NULL) {
      if (lookahead_litteral(closing_symbol) && !inhib_bslash)
	return 0;
    }
  }

  if (read_terminal_token(tk_list))
    return 1;

  RESTORE_CONTEXT(gidx);
  return 0;
}

int		read_terminal_token(token_t **tk_list) {
  SAVE_CONTEXT(gidx);

  if (read_subst(tk_list, "`", "`")
      || read_subst(tk_list, "$(", ")")
      || read_variable(tk_list)
      || read_quotes(tk_list, 0, "'")
      || read_quotes(tk_list, 1, "\"")
      || read_to_token(tk_list, read_identifier, TEXT)
      || read_to_token(tk_list, consume_char, TEXT)
      || inhib_bslash)
    return 1;

  RESTORE_CONTEXT(gidx);
  return 0;
}

int		read_subst(token_t **tk_list, char *opening, char *closing) {
  SAVE_CONTEXT(gidx);
  token_t	*new;
  bool		context = inhib_context;
  char		*cl_p = closing_symbol;

  if (read_litteral(opening)) {
    closing_symbol = closing;
    new = new_token(substitution_token_handler, SUBSTITUTION,
	tokenize_expand);
    inhib_context = 0;
    if (parse_cmd_list(&new->content.substitution_tree)) {
      if (read_litteral(closing)) {
	push_in_list((list_t**)tk_list, (list_t*)new);
	closing_symbol = cl_p;
	inhib_context = context;
	return 1;
      }
    }
    free(new);
  }

  inhib_context = context;
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		read_sh_args_var(token_t **list) {
  SAVE_CONTEXT(gidx);
  char		*s;
  token_t	*new;

  if ((s = capture_substr(read_single_digit)) != NULL) {
    if (!read_char_range('a', 'z')
	&& !read_char_range('A', 'Z')
	&& !read_char('_')
	&& !read_char_range('0', '9')) {
      new = new_token(sh_arg_token_handler, VARIABLE, tokenize_expand);
      new->content.text = s;
      push_in_list((list_t**)list, (list_t*)new);
      return 1;
    }
    free(s);
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		read_variable(token_t **list) {
  SAVE_CONTEXT(gidx);
  token_t	*new;
  char		*s;

  if (global_error)
    return 0;

  if (expand_values && !inhib_bslash && read_litteral("$")) {
    if (read_sh_args_var(list))
      return 1;
    else if ((s = capture_substr(read_identifier)) != NULL) {
      new = new_token(variable_token_handler, VARIABLE, tokenize_expand);
      new->content.text = s;
      push_in_list((list_t**)list, (list_t*)new);
      return 1;
    }
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

int		read_quotes(token_t **list, bool allow_expand,
    char *litteral) {
  SAVE_CONTEXT(gidx);
  token_t	*new;
  int		ret;
  char		*cl_p = closing_symbol;

  if (inhib_bslash || inhib_context || global_error)
    return 0;

  if (read_litteral(litteral)) {
    push_state(QUOTE, litteral);
    inhib_context = 1;
    expand_values = allow_expand;
    closing_symbol = litteral;

    new = new_token(quotes_token_handler, QUOTES, NULL);
    ret = 1;

    while (ret && !(lookahead_litteral(litteral) && !inhib_bslash) && !global_error) {
      capture_whitespaces(&new->subtokens);
      ret = read_token(&new->subtokens);
      capture_whitespaces(&new->subtokens);
    }

    read_litteral(litteral);
    pop_state(QUOTE);
    push_in_list((list_t**)list, (list_t*)new);
    inhib_context = 0;
    expand_values = 1;
    closing_symbol = cl_p;
    return 1;
  }
  RESTORE_CONTEXT(gidx);
  return 0;
}

void		consume_char() {
  if (global_error || !peek_char())
    return;

  if (input[gidx] == '\\' && !inhib_bslash) {
    inhib_bslash = 1;
  }
  else
    inhib_bslash = 0;
  gidx++;
}

void		req_more_input() {
  SAVE_CONTEXT(gidx);

  skip_whitespaces();
  if (input[gidx] != '\0')
    pop_state(OPERATOR);

  RESTORE_CONTEXT(gidx);
}

char		peek_char() {
  if (global_error)
    return '\0';

  while (!sh.interrupt && !global_error
      && (input == NULL || input[gidx] == '\0')) {
    if (input != NULL && state_stack == NULL
	&& !inhib_bslash)
      break;
    input = get_input(input, state_stack);
    if (sh.interrupt || input == NULL) {
      global_error = 1;
      if (state_stack != NULL)
	sh.interrupt = 0;
      return '\0';
    }
    req_more_input();
  }
  return input[gidx];
}

int		read_char(char c) {
  if (c == peek_char() && !global_error) {
    consume_char();
    return 1;
  }
  return 0;
}

int		read_char_range(char start, char end) {
  if (peek_char() >= start && peek_char() <= end) {
    consume_char();
    return 1;
  }
  return 0;
}

int		litteral(char *s, bool consume) {
  int		i;
  bool		bslash = inhib_bslash;
  SAVE_CONTEXT(gidx);

  for (i = 0; s[i] != '\0'; i++) {
    if (!read_char(s[i])) {
      RESTORE_CONTEXT(gidx);
      inhib_bslash = bslash;
      return 0;
    }
  }
  if (!consume) {
    RESTORE_CONTEXT(gidx);
    inhib_bslash = bslash;
  }
  return 1;
}

int		lookahead_litteral(char *s) {
  return litteral(s, 0);
}

int		read_litteral(char *s) {
  return litteral(s, 1);
}

void		skip_comments() {
  char		c;

  if (!inhib_context && peek_char() == '#') {
    while ((c = peek_char()) && c != '\n') {
      consume_char();
    }
  }
}

void		skip_blanks() {
  while (read_char('\n') || read_char(' ') || read_char('\t'))
  {}
}

void		skip_whitespaces() {

  if (inhib_bslash)
    return ;
  skip_blanks();
  skip_comments();
  skip_blanks();
  skip_comments();
}

void		read_identifier() {
  while (read_char_range('a', 'z')
      || read_char_range('A', 'Z')
      || read_char_range('0', '9')
      || read_char('_'))
  {}
}

void		read_single_digit() {
  read_char_range('0', '9');
}

char		*capture_substr(void (*reader)()) {
  SAVE_CONTEXT(gidx);
  char		*s;

  reader();

  int len = gidx - SAVED_IDX;

  if (len <= 0 || (len == 1 && inhib_bslash))
    return NULL;

  s = xmalloc(len + 1);
  strncpy(s, &input[s_idx], len);
  s[len] = '\0';

  return s;
}

int		read_to_token(token_t **tk_list, void (*reader)(), eTokenType type) {
  token_t	*tk;
  char		*s;

  if ((s = capture_substr(reader)) == NULL)
    return 0;

  tk = new_token(text_handler, type, NULL);
  tk->content.text = s;

  push_in_list((list_t**)tk_list, (list_t*)tk);
  return 1;
}

void		capture_whitespaces(token_t **list) {
  read_to_token(list, skip_whitespaces, WHITESPACE);
}
