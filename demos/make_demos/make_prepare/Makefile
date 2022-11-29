all:;@:

prepared_file:
	echo "create prepared_file"
	touch prepared_file

prepare: | prepared_file

foo: prepare
	echo run foo

.PHONY: all foo prepare

# `|`
# stands for order-only prerequisite
#
# all:;@:
# https://stackoverflow.com/questions/58038712/what-is-a-semicolon-in-a-makefile-prerequisites-list
#
