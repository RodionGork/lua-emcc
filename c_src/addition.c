#include <stdio.h>
#include <unistd.h>
#include <emscripten.h>

volatile char fgetsReady = 0;
volatile char fgetsBuf[1024];
char getcBuf[1024] = {0};
int getcPtr = 0;

int getcBufNext() {
    int c = getcBuf[getcPtr];
    if (getcPtr < (int)sizeof(getcBuf) - 1) {
        getcPtr++;
    }
    return c;
}

int getc_unlocked(FILE* f) {
    int i;
    if (f != stdin) {
        return -1;
    }
    if (getcBuf[getcPtr]) {
        return getcBufNext();
    }
    getcPtr = 0;
    while (!fgetsReady) {
        emscripten_sleep(50);
    }
    i = 0;
    while (1) {
        char c = fgetsBuf[i];
        getcBuf[i] = c;
        if (c == 0 || i >= (int)sizeof(getcBuf) - 2) {
            getcBuf[i] = '\n';
            getcBuf[i + 1] = 0;
            break;
        }
        i += 1;
    }
    fgetsReady = 0;
    return getcBufNext();
}

char* fgets(char* s, int num, FILE* f) {
    int i = 0;
    if (f != stdin) {
        s[0] = 0;
        return s;
    }
    while (!fgetsReady) {
        emscripten_sleep(50);
    }
    while (1) {
        char c = fgetsBuf[i];
        if (i < num) {
            s[i] = c;
        }
        if (c == 0) {
            break;
        }
        i += 1;
    }
    fgetsReady = 0;
    return s;
}

void passInput(char* s) {
    int i = 0;
    if (s == NULL) {
        getcPtr = 0;
        getcBuf[0] = 0;
        fgetsReady = 0;
        return;
    }
    while (1) {
        char c = s[i];
        fgetsBuf[i] = c;
        if (c == 0) {
            break;
        }
        i += 1;
    }
    fgetsReady = 1;
}

int isatty(int id) {
    return id == 0 ? 1 : 0;
}

int main(int argc, char** argv);

//input buffer is not intentionally cleared on (re)start
//this allows to set it before execution
//if necessary, call passInput(NULL) beforehand
int shmain(void) {
    char* argv[] = {"lua"};
    char* argvExt[] = {"lua", "-i", "/init.lua"};
    char* argvPrg[] = {"lua", "prog.lua"};
    if (access("/init.lua", F_OK) == 0) {
        return main(3, argvExt);
    }
    if (access("/prog.lua", F_OK) == 0) {
        return main(2, argvPrg);
    }
    return main(1, argv);
}
