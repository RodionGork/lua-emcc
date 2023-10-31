# lua-emcc

Lua compiled to JavaScript with EmScripten / EMCC - may be easily used for executing some
Lua code in webpages and making interaction to it.

Demo: [Lua 5.4.6 Sanbox in your browser](https://rodiongork.github.io/lua-emcc) - it also allows embedding
code to run into the url, thus serving as simplified Lua "Fiddle".

## Using in Your project

Don't get afraid of the following verbose explanations - actually they are just about ten lines of code, as you
can see in examples included here :)

Begin by **Including javascript file**, make sure wasm is accessible (it is called indirectly):

    <script src="./lua.js"></script>
    <!-- lua.wasm should exist at this place also>

or directly from this page

    <script src="https://rodiongork.github.io/lua-emcc/lua.js"></script>

**Now make some initialization steps**

Forbid auto-starting on just site being loaded, let's make all preparation first:

    Module.noInitialRun = true;

When Lua is ready, put some executable code in its "virtual" filesystem as `prog.lua`:

    var code =`
    x = io.read('*n')
    print(x, 'squared', x * x)
    io.stderr:write('test stderr printing, see console.log\\n')
    `;

    Module.onRuntimeInitialized = function() {
        FS.writeFile('/prog.lua', code);
    }

_In case if your code should come somewhere from `DOM` make sure the page is loaded also,
i.e. execute FS.writeFile only after onRuntimeInitialized and, say, `jQuery(document).ready()`
happened._

We need also to setup how output (e.g. `print`) will work, by default it will write to `console.log`
but we can change this. For example this way writing to `stdout` will add text to some
element on the page, while writing to `stderr` instead will add lines to console.log:

    Module.preRun = function() {
        var errbuf = '';
        function stdout(code) {
            var out = document.getElementById('output');
            out.value += String.fromCharCode(code);
        }
        function stderr(code) {
            if (code == 10 || code == 13) {
                console.log(errbuf);
                errbuf = '';
            } else {
                errbuf += String.fromCharCode(code);
            }
        }
        FS.init(()=>null, stdout, stderr);
    }

**Now to start the enterpreter**, for example by clicking some button, add the call to `shmain`
function in the onclick event:

    <button onclick="runLua()">Run Lua to process Input</button>

    function runLua() {
        var inp = document.getElementById('input').value;
        Module.ccall('passInput', null, ['string'], [inp]);
        Module.ccall('shmain', 'number', [], []);
    }

We have `shmain` and `passInput` functions to manipulate lua. In this first example
input is prepared before the program started, so it could execute in "one-shot".
See **Example #1**.

Alternatively this can be done asynchronously, e.g. one button starts Lua, another
provides input when it is ready (perhaps, multiple times on user's action) - see **Example #2**:

    <button onclick="runLua()">Run Lua</button>
    <button onclick="sendLuaData()">Pass Input</button>

    function runLua() {
        Module.ccall('shmain', 'number', [], []);
    }

    function sendLuaData() {
        var inp = document.getElementById('input').value;
        Module.ccall('passInput', null, ['string'], [inp]);
    }

In this way you may have a kind of "asynchronous" web-page with Lua processing data
for it in a loop. See **Example #3**

_In some cases you may want to repeatedly call `shmain` - remember to clear it's input
buffer (unless you want to process what is prepared here intentionally) on any consecutive
calls with this small magic:_

    Module.ccall('passInput', null, ['string'], [null]);

There are two ways of direct interaction with javascript environment - `os.execute` will
run any javascript statement, while `os.getenv` allows retrieving variables (only
works when first letter of the variable name is in lowercase):

    os.execute('alert("Hello, People!")')
    os.getenv('document.title')

**That's all** - despite it is just 3 actions (setting output function, passing input and
starting the interpreter) - great things could be done. Look for reference at the
[Space Invaders puzzle](https://www.codeabbey.com/index/task_view/space-invaders) with
small interactive game at **CodeAbbey**.
