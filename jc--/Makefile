dll_comp = g++ -fPIC -shared -o libs/$(basename $1).so libs/$(basename $1).cpp -g
test:
dll:
	$(call dll_comp,scan)  
	$(call dll_comp, parse)
	$(call dll_comp, gen)
prog:
	g++ main.cpp token_t.cpp -ldl -L./libs -Wl,-rpath,'./libs/' -* -g -o jc-- 
	make dll
dgb:
	gdb ./jqcdg
run:
	./jqc main.jql

