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

#ifndef		__SH_EXEC_HANDLERS_H__
#define		__SH_EXEC_HANDLERS_H__

int		SUBSH_handler(exec_node_t *node);
int		CURSH_handler(exec_node_t *node);

int		AND_handler(exec_node_t *node);
int		OR_handler(exec_node_t *node);

int		REG_handler(exec_node_t *node);

int		CONDITION_handler(exec_node_t *node, bool loop);
int		IF_handler(exec_node_t *node);
int		WHILE_handler(exec_node_t *node);

int		FOR_handler(exec_node_t *node);

int		PIPE_handler(exec_node_t *node);

int		VARASSIGN_handler(exec_node_t *node);

int		FUNC_handler(exec_node_t *node);

int		exec_function(func_t *func, char **argv);

int		EXEC_handler(exec_node_t *node);
char		*find_exec(char *name);

#endif
