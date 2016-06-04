CC              = cc
SRC             = main.c graph.c new.c user.c
TARGET          = project
INCLUDE       = /home/kostas/Downloads/cudd-2.4.2/include 
WHERE	 = /home/kostas/Downloads/cudd-2.4.2
LIB           = $(WHERE)/dddmp/libdddmp.a $(WHERE)/cudd/libcudd.a \
	$(WHERE)/mtr/libmtr.a $(WHERE)/st/libst.a $(WHERE)/util/libutil.a \
	$(WHERE)/epd/libepd.a
MYLIB		= bdd.a
OBJ             = $(SRC:.c=.o)
DDDEBUG         =
#------------------------------------------------------
$(TARGET): $(OBJ)
	$(CC)  -o $@ $(OBJ)  -L$(LIB) -lm

$(OBJ): $(SRC)
	$(CC) -g  -I$(INCLUDE) -c $(SRC) $(DDDEBUG)

clean:
	rm -f *.o *~ $(TARGET)
