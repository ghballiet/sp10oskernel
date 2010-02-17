
MAKE_DIRS = ARM

export

all: 
	for X in $(MAKE_DIRS) ; do \
		cd $$X; $(MAKE) || exit 1; cd ..; \
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
		cd $$X; $(MAKE) depend || exit 1; cd ..; \
	done

