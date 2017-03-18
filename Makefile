MAKE_FLAGS = --no-print-directory -C


UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	ALL_TARGETS := server driver smarty unittest
else
	ALL_TARGETS := server driver smarty mc
endif

build : $(ALL_TARGETS)

mc :
	-@ echo MC compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/mc/|| echo BUILD FAILED
	
run : build
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ run || exit 1

program :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ program || exit 1
	
tests :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ test || exit 1

smarty :
	-@ echo smarty compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ || echo BUILD FAILED
	
driver :
	-@ echo driver compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/driver/ || echo BUILD FAILED
    
server :
	-@ echo server compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/server/ || echo BUILD FAILED
	
unittest:
	-@ echo unittest compiling ..
	-@$(MAKE) $(MAKE_FLAGS) tests/ || echo BUILD FAILED

test: unittest
	-@./bin/unittest

clean :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/driver/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/server/ clean || exit 1

rebuild : clean build
	-@cp ./cfg/* ./bin/
	

	
.PHONY : test clean
