tex-to-md: tex-to-md.l tex-to-md.y tex-to-md.h tex-to-md-functions.c
				bison -d tex-to-md.y
				flex -o tex-to-md.lex.c tex-to-md.l
				gcc -o $@ tex-to-md.tab.c tex-to-md.lex.c tex-to-md-functions.c -lm
				@echo Success: $@ compiled successfully.