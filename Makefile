MAKE_FLAGS = --no-print-directory -C

build : server smarty
	-@ echo MC compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/mc/|| echo BUILD FAILED
	
run : build
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ run || exit 1

program :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ program || exit 1
	
test :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ test || exit 1

smarty :
	-@ echo smarty compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ || echo BUILD FAILED
	
driver :
	-@ echo driver compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/driver/ || echo BUILD FAILED
    
server :
	@ echo server compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/server/ || echo BUILD FAILED

client :
	@ echo client compiling..
	-@$(MAKE) $(MAKE_FLAGS) src/libclient/ || echo BUILD FAILED


kclient: client
	@ echo kclient compiling ..
	-@$(MAKE) $(MAKE_FLAGS) src/kclient/ || echo BUILD FAILED

desktop:
	@ echo desktop compiling ..
	@ echo "%VS110COMNTOOLS%\\vsvars32.bat"
	-@msbuild /P:Configuration=Debug projects\\smarty.sln /target:desktop /nologo /verbosity:m /p:WarningLevel=0
	@ echo "  [$@]"
	

clean :
	-@$(MAKE) $(MAKE_FLAGS) src/mc/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/smarty/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/driver/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/server/ clean || exit 1
	-@$(MAKE) $(MAKE_FLAGS) src/libclient/ clean || exit 1

rebuild : clean build
	-@cp ./cfg/* ./bin/
	

	
.PHONY : test clean
