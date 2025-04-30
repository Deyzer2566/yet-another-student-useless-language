LEXER = lexer
SYNTAXER = syntaxer
OUT = out
INCLUDES = \
	. \
	out
INCLUDES := ${addprefix -I,$(INCLUDES)}

SOURCES = \
	$(SYNTAXER).tab.c \
	$(LEXER).yy.c

SOURCES := $(addprefix $(OUT)/, $(SOURCES))

all: $(OUT)/compiler
	./$?

$(OUT)/compiler: $(SOURCES)
	gcc -lfl -o $@ $^ $(INCLUDES)

$(OUT):
	mkdir $(OUT)

$(OUT)/$(LEXER).yy.c: $(LEXER).l $(OUT)
	flex -o $@ $(LEXER).l

$(OUT)/$(SYNTAXER).tab.c: $(SYNTAXER).y $(OUT)
	bison -d -o $@ -Wcounterexamples -Werror $(SYNTAXER).y
