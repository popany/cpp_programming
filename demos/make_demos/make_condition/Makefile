all:;@:

foo:
	@if [ -f foo.tmp ]; then \
		echo "foo.tmp exist"; \
	else \
		touch foo.tmp; \
	fi

.PHONY: all foo

# all:;@:
# https://stackoverflow.com/questions/58038712/what-is-a-semicolon-in-a-makefile-prerequisites-list
#
