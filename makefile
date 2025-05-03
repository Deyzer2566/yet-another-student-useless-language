LEXER = lexer
SYNTAXER = syntaxer
OUT = out
INCLUDES = \
	. \
	out
INCLUDES := ${addprefix -I,$(INCLUDES)}

SOURCES = \
	$(SYNTAXER).tab.c \
	$(LEXER).yy.c \
	ast.c \
	main.c

OBJECTS = $(addprefix $(OUT)/, $(SOURCES:.c=.o))

all: $(OUT)/compiler
	./$?

$(OUT)/compiler: $(OBJECTS)
	gcc -lfl -o $@ $^

%.o: %.c
	mkdir -p $(dir $@)
	gcc -c -o $@ $? $(INCLUDES)

$(OUT)/%.o: %.c
	mkdir -p $(dir $@)
	gcc -c -o $@ $? $(INCLUDES)

$(OUT)/$(LEXER).yy.c: $(LEXER).l
	mkdir -p $(dir $@)
	flex -o $@ $(LEXER).l

$(OUT)/$(SYNTAXER).tab.c: $(SYNTAXER).y
	mkdir -p $(dir $@)
	bison -d -o $@ -Wcounterexamples -Werror $(SYNTAXER).y
