a.out : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o ../pflayer/pflayer.o test1.o amscan.o amprint.o
	@cc -w -g -O0 am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o ../pflayer/pflayer.o test1.o amscan.o amprint.o 

amlayer.o : am.o amfns.o amsearch.o aminsert.o amstack.o amglobals.o amscan.o amprint.o
	ld -r am.o amfns.o amsearch.o aminsert.o  amstack.o amglobals.o amscan.o amprint.o  -o amlayer.o

am.o : am.c am.h pf.h
	@cc -w -g -O0 -c am.c

amfns.o : amfns.c am.h pf.h
	@cc -w -g -O0 -c amfns.c

amsearch.o : amsearch.c am.h pf.h
	@cc -w -g -O0 -c amsearch.c

aminsert.o : aminsert.c am.h pf.h
	@cc -w -g -O0 -c aminsert.c

amscan.o : amscan.c am.h pf.h
	@cc -w -g -O0 -c amscan.c

amstack.o : amstack.c am.h pf.h
	@cc -w -g -O0 -c amstack.c

amglobals.o : amglobals.c am.h
	@cc -w -g -O0 -c amglobals.c

amprint.o : amprint.c am.h pf.h 
	@cc -w -g -O0 -c amprint.c
	
test1.o : test1.c am.h pf.h 
	@cc -w -g -O0 -c test1.c


