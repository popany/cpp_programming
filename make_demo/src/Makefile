OBJ_NAME = main.o foo.o bar.o
APP_NAME = demo_app

# --------------------------------------------------------------------------
# BEGIN - Auto-Dependency Generation
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
OBJDIR := obj
DEPDIR := $(OBJDIR)/.deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

COMPILE.cpp = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(OBJ_NAME:%.o=$(DEPDIR)/%.d)
$(DEPFILES):
include $(wildcard $(DEPFILES))
# END - Auto-Dependency Generation
# --------------------------------------------------------------------------

STD = -std=c++11
WARN = -Wall
OPT = -O2
DEBUG = -g -ggdb
CPPFLAGS = $(STD) $(WARN) $(OPT) $(DEBUG)

CC = g++
LD = $(CC) $(DEBUG)

all: $(APP_NAME)
	@echo "build " $(APP_NAME)

.PHONY: all

OBJ = $(OBJ_NAME:%=$(OBJDIR)/%)
$(APP_NAME): $(OBJ)
	$(LD) -o $@ $^

