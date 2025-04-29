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

all: $(OUT)/compiler
	./$?

$(OUT)/compiler: $(addprefix $(OUT)/, $(SOURCES))
	gcc -lfl -o $@ $^ $(INCLUDES)

$(OUT):
	mkdir $(OUT)

$(OUT)/$(LEXER).yy.c: $(LEXER).l $(OUT)
	flex -o $@ $(LEXER).l

$(OUT)/$(SYNTAXER).tab.%: $(SYNTAXER).y $(OUT)
	bison -d -o $@ $(SYNTAXER).y
