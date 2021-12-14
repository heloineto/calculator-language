calculator: calculator.l calculator.y calculator.h calculator-functions.c
				bison -d calculator.y
				flex -o calculator.lex.c calculator.l
				gcc -o $@ calculator.tab.c calculator.lex.c calculator-functions.c -lm
				@echo Success: $@ compiled successfully.