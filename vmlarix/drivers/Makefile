
MAKE_DIRS = char

export

all: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE); cd ..; \
	done

clean: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) clean; cd ..; \
	done

realclean: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) realclean; cd ..; \
	done

depend: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) depend; cd ..; \
	done

