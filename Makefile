CC		= gcc
CFLAGS	+= -lncurses -O2 -MMD -Wall
INCLUDE	= -I./include
TARGET	= ./$(shell basename `readlink -f .`)
SRCDIR	= ./src
SRC		= $(wildcard $(SRCDIR)/*.c)
OBJDIR	= ./src/obj
OBJS	= $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))
DEPENDS	= $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(LDFLAGS) $(INCLUDE) -c $< -o $@ $(CFLAGS)

-include $(DEPENDS)

.PHONY: all clean distclean
all: distclean $(TARGET)
clean:
	$(RM) $(OBJS) $(DEPENDS)
distclean: clean
	$(RM) $(TARGET) data/.svf.dat
zip: distclean
	zip -r ../$(TARGET).zip ../$(TARGET)

