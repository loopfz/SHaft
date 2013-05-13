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
#include	"sh_list.h"
#include	"env.h"
#include	"sh_local_vars.h"

var_t		*get_local_var(char *name) {
  var_t		*it;

  for (it = sh.local_vars; it != NULL; it = it->next) {
    if (strcmp(name, it->name) == 0)
      return it;
  }
  return NULL;
}

void		set_local_var(char *name, char *value) {
  var_t		*new;

  if ((new = get_local_var(name)) == NULL) {
    new = xmalloc(sizeof(*new));
    memset(new, 0, sizeof(*new));
    new->name = strdup(name);
    push_in_list((list_t**)&sh.local_vars, (list_t*)new);
  }
  new->value = strdup(value);
}

int		unset_local_var(char *name) {
  var_t		*var;

  if ((var = get_local_var(name)) != NULL) {
    if (var->next)
      var->next->prev = var->prev;
    if (var->prev)
      var->prev->next = var->next;
    else
      sh.local_vars = var->next;
    free(var->name);
    if (var->value)
      free(var->value);
    free(var);
    return 1;
  }
  return 0;
}

void		set_variable(char *name, char *value) {
  if (sh_getenv(name) != NULL) {
    sh_setenv(name, value);
  }
  else
    set_local_var(name, value);
}
