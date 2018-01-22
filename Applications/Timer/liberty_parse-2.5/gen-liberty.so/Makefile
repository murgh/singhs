compile:
	gcc -c -fpic liberty_wrap.c `perl -MExtUtils::Embed -e ccopts` -I.. -D_GNU_SOURCE
	gcc -shared liberty_wrap.o ../.libs/PI.o ../.libs/syntax_checks.o ../.libs/liberty_parser.o ../.libs/libhash.o ../.libs/libstrtab.o ../.libs/mymalloc.o ../.libs/token.o ../.libs/syntax_decls.o ../.libs/group_lookup.o ../.libs/attr_lookup.o ../.libs/liberty_front_lex.o -o liberty.so
clean:
	rm -rf liberty.so liberty_wrap.o
