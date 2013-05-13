--------------------------------------------------------------------------------
|                ______   __    __             ______     __                   |
|               /      \ |  \  |  \           /      \   |  \                  |
|              |  $$$$$$\| $$  | $$  ______  |  $$$$$$\ _| $$_                 |
|              | $$___\$$| $$__| $$ |      \ | $$_  \$$|   $$ \                |
|               \$$    \ | $$    $$  \$$$$$$\| $$ \     \$$$$$$                |
|               _\$$$$$$\| $$$$$$$$ /      $$| $$$$      | $$ __               |
|              |  \__| $$| $$  | $$|  $$$$$$$| $$        | $$|  \              |
|               \$$    $$| $$  | $$ \$$    $$| $$         \$$  $$              |
|                \$$$$$$  \$$   \$$  \$$$$$$$ \$$          \$$$$               |
|                                                                              |
--------------------------------------------------------------------------------
| Written by Thomas Schaffer <thomas.schaffer@epitech.eu> in C.                |
| Licensed under the MIT license.                                              |
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
| Description                                                                  |
--------------------------------------------------------------------------------

SHaft is a simple UNIX shell implementing most common features, such as:
	* Pipelines
	* Redirections
	* Logical operators && and ||
	* Environment and local variables substitution
	* Function declaration
	* Scripting keywords (if elif else, while, for)
	* Command aliases
	* Command subsitution with `` or $()
   and many others (cf. TODO).

It can be run in interactive mode, or given a script file as parameter.

It is a compact codebase (~3000 SLOC) entirely written by myself as a fun
project over a week. It uses a hand written parser and does not depend on any
external library.
