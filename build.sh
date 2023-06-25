# if you don't want emcc installed on your machine,
# run this in docker container with emcc, e.g.:
# docker run -it -v $(pwd):/src --name emsdk emscripten/emsdk /bin/bash
# and then:

emcc -std=gnu99 -O2 -Wall -Wextra -fno-builtin -DLUA_COMPAT_5_3 -DLUA_USE_POSIX c_src/*.c -o lua.js \
    -DLUA_PROMPT='""' -DLUA_PROMPT2='""' -lm -sASYNCIFY -sEXPORTED_FUNCTIONS=_shmain,_passInput -sEXPORTED_RUNTIME_METHODS=ccall,cwrap