
ALL_DIRS := $(dir $(wildcard */Makefile))

.PHONY: all $(ALL_DIRS)

all: $(ALL_DIRS)

$(ALL_DIRS):
	$(MAKE) -C $@

clean:
	@for dir in $(ALL_DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

realclean: clean
	@for dir in $(ALL_DIRS); do \
		$(MAKE) -C $$dir realclean; \
	done
	rm -rf $(OBJ_DIR)
