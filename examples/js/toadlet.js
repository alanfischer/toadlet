// Note: For maximum-speed code, see "Optimizing Code" on the Emscripten wiki, https://github.com/kripken/emscripten/wiki/Optimizing-Code
// Note: Some Emscripten settings may limit the speed of the generated code.
// The Module object: Our interface to the outside world. We import
// and export values on it, and do the work to get that through
// closure compiler if necessary. There are various ways Module can be used:
// 1. Not defined. We create it here
// 2. A function parameter, function(Module) { ..generated code.. }
// 3. pre-run appended it, var Module = {}; ..generated code..
// 4. External script tag defines var Module.
// We need to do an eval in order to handle the closure compiler
// case, where this code here is minified but Module was defined
// elsewhere (e.g. case 4 above). We also need to check if Module
// already exists (e.g. case 3 above).
// Note that if you want to run closure, and also to use Module
// after the generated code, you will need to define   var Module = {};
// before the code. Then that object will be used in the code, and you
// can continue to use Module afterwards as well.
var Module;
if (!Module) Module = eval('(function() { try { return Module || {} } catch(e) { return {} } })()');
// Sometimes an existing Module object exists with properties
// meant to overwrite the default module functionality. Here
// we collect those properties and reapply _after_ we configure
// the current environment's defaults to avoid having to be so
// defensive during initialization.
var moduleOverrides = {};
for (var key in Module) {
  if (Module.hasOwnProperty(key)) {
    moduleOverrides[key] = Module[key];
  }
}
// The environment setup code below is customized to use Module.
// *** Environment setup code ***
var ENVIRONMENT_IS_NODE = typeof process === 'object' && typeof require === 'function';
var ENVIRONMENT_IS_WEB = typeof window === 'object';
var ENVIRONMENT_IS_WORKER = typeof importScripts === 'function';
var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;
if (ENVIRONMENT_IS_NODE) {
  // Expose functionality in the same simple way that the shells work
  // Note that we pollute the global namespace here, otherwise we break in node
  Module['print'] = function print(x) {
    process['stdout'].write(x + '\n');
  };
  Module['printErr'] = function printErr(x) {
    process['stderr'].write(x + '\n');
  };
  var nodeFS = require('fs');
  var nodePath = require('path');
  Module['read'] = function read(filename, binary) {
    filename = nodePath['normalize'](filename);
    var ret = nodeFS['readFileSync'](filename);
    // The path is absolute if the normalized version is the same as the resolved.
    if (!ret && filename != nodePath['resolve'](filename)) {
      filename = path.join(__dirname, '..', 'src', filename);
      ret = nodeFS['readFileSync'](filename);
    }
    if (ret && !binary) ret = ret.toString();
    return ret;
  };
  Module['readBinary'] = function readBinary(filename) { return Module['read'](filename, true) };
  Module['load'] = function load(f) {
    globalEval(read(f));
  };
  Module['arguments'] = process['argv'].slice(2);
  module['exports'] = Module;
}
else if (ENVIRONMENT_IS_SHELL) {
  Module['print'] = print;
  if (typeof printErr != 'undefined') Module['printErr'] = printErr; // not present in v8 or older sm
  if (typeof read != 'undefined') {
    Module['read'] = read;
  } else {
    Module['read'] = function read() { throw 'no read() available (jsc?)' };
  }
  Module['readBinary'] = function readBinary(f) {
    return read(f, 'binary');
  };
  if (typeof scriptArgs != 'undefined') {
    Module['arguments'] = scriptArgs;
  } else if (typeof arguments != 'undefined') {
    Module['arguments'] = arguments;
  }
  this['Module'] = Module;
  eval("if (typeof gc === 'function' && gc.toString().indexOf('[native code]') > 0) var gc = undefined"); // wipe out the SpiderMonkey shell 'gc' function, which can confuse closure (uses it as a minified name, and it is then initted to a non-falsey value unexpectedly)
}
else if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  Module['read'] = function read(url) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, false);
    xhr.send(null);
    return xhr.responseText;
  };
  if (typeof arguments != 'undefined') {
    Module['arguments'] = arguments;
  }
  if (typeof console !== 'undefined') {
    Module['print'] = function print(x) {
      console.log(x);
    };
    Module['printErr'] = function printErr(x) {
      console.log(x);
    };
  } else {
    // Probably a worker, and without console.log. We can do very little here...
    var TRY_USE_DUMP = false;
    Module['print'] = (TRY_USE_DUMP && (typeof(dump) !== "undefined") ? (function(x) {
      dump(x);
    }) : (function(x) {
      // self.postMessage(x); // enable this if you want stdout to be sent as messages
    }));
  }
  if (ENVIRONMENT_IS_WEB) {
    this['Module'] = Module;
  } else {
    Module['load'] = importScripts;
  }
}
else {
  // Unreachable because SHELL is dependant on the others
  throw 'Unknown runtime environment. Where are we?';
}
function globalEval(x) {
  eval.call(null, x);
}
if (!Module['load'] == 'undefined' && Module['read']) {
  Module['load'] = function load(f) {
    globalEval(Module['read'](f));
  };
}
if (!Module['print']) {
  Module['print'] = function(){};
}
if (!Module['printErr']) {
  Module['printErr'] = Module['print'];
}
if (!Module['arguments']) {
  Module['arguments'] = [];
}
// *** Environment setup code ***
// Closure helpers
Module.print = Module['print'];
Module.printErr = Module['printErr'];
// Callbacks
Module['preRun'] = [];
Module['postRun'] = [];
// Merge back in the overrides
for (var key in moduleOverrides) {
  if (moduleOverrides.hasOwnProperty(key)) {
    Module[key] = moduleOverrides[key];
  }
}
// === Auto-generated preamble library stuff ===
//========================================
// Runtime code shared with compiler
//========================================
var Runtime = {
  stackSave: function () {
    return STACKTOP;
  },
  stackRestore: function (stackTop) {
    STACKTOP = stackTop;
  },
  forceAlign: function (target, quantum) {
    quantum = quantum || 4;
    if (quantum == 1) return target;
    if (isNumber(target) && isNumber(quantum)) {
      return Math.ceil(target/quantum)*quantum;
    } else if (isNumber(quantum) && isPowerOfTwo(quantum)) {
      return '(((' +target + ')+' + (quantum-1) + ')&' + -quantum + ')';
    }
    return 'Math.ceil((' + target + ')/' + quantum + ')*' + quantum;
  },
  isNumberType: function (type) {
    return type in Runtime.INT_TYPES || type in Runtime.FLOAT_TYPES;
  },
  isPointerType: function isPointerType(type) {
  return type[type.length-1] == '*';
},
  isStructType: function isStructType(type) {
  if (isPointerType(type)) return false;
  if (isArrayType(type)) return true;
  if (/<?{ ?[^}]* ?}>?/.test(type)) return true; // { i32, i8 } etc. - anonymous struct types
  // See comment in isStructPointerType()
  return type[0] == '%';
},
  INT_TYPES: {"i1":0,"i8":0,"i16":0,"i32":0,"i64":0},
  FLOAT_TYPES: {"float":0,"double":0},
  or64: function (x, y) {
    var l = (x | 0) | (y | 0);
    var h = (Math.round(x / 4294967296) | Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  and64: function (x, y) {
    var l = (x | 0) & (y | 0);
    var h = (Math.round(x / 4294967296) & Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  xor64: function (x, y) {
    var l = (x | 0) ^ (y | 0);
    var h = (Math.round(x / 4294967296) ^ Math.round(y / 4294967296)) * 4294967296;
    return l + h;
  },
  getNativeTypeSize: function (type) {
    switch (type) {
      case 'i1': case 'i8': return 1;
      case 'i16': return 2;
      case 'i32': return 4;
      case 'i64': return 8;
      case 'float': return 4;
      case 'double': return 8;
      default: {
        if (type[type.length-1] === '*') {
          return Runtime.QUANTUM_SIZE; // A pointer
        } else if (type[0] === 'i') {
          var bits = parseInt(type.substr(1));
          assert(bits % 8 === 0);
          return bits/8;
        } else {
          return 0;
        }
      }
    }
  },
  getNativeFieldSize: function (type) {
    return Math.max(Runtime.getNativeTypeSize(type), Runtime.QUANTUM_SIZE);
  },
  dedup: function dedup(items, ident) {
  var seen = {};
  if (ident) {
    return items.filter(function(item) {
      if (seen[item[ident]]) return false;
      seen[item[ident]] = true;
      return true;
    });
  } else {
    return items.filter(function(item) {
      if (seen[item]) return false;
      seen[item] = true;
      return true;
    });
  }
},
  set: function set() {
  var args = typeof arguments[0] === 'object' ? arguments[0] : arguments;
  var ret = {};
  for (var i = 0; i < args.length; i++) {
    ret[args[i]] = 0;
  }
  return ret;
},
  STACK_ALIGN: 8,
  getAlignSize: function (type, size, vararg) {
    // we align i64s and doubles on 64-bit boundaries, unlike x86
    if (type == 'i64' || type == 'double' || vararg) return 8;
    if (!type) return Math.min(size, 8); // align structures internally to 64 bits
    return Math.min(size || (type ? Runtime.getNativeFieldSize(type) : 0), Runtime.QUANTUM_SIZE);
  },
  calculateStructAlignment: function calculateStructAlignment(type) {
    type.flatSize = 0;
    type.alignSize = 0;
    var diffs = [];
    var prev = -1;
    var index = 0;
    type.flatIndexes = type.fields.map(function(field) {
      index++;
      var size, alignSize;
      if (Runtime.isNumberType(field) || Runtime.isPointerType(field)) {
        size = Runtime.getNativeTypeSize(field); // pack char; char; in structs, also char[X]s.
        alignSize = Runtime.getAlignSize(field, size);
      } else if (Runtime.isStructType(field)) {
        if (field[1] === '0') {
          // this is [0 x something]. When inside another structure like here, it must be at the end,
          // and it adds no size
          // XXX this happens in java-nbody for example... assert(index === type.fields.length, 'zero-length in the middle!');
          size = 0;
          if (Types.types[field]) {
            alignSize = Runtime.getAlignSize(null, Types.types[field].alignSize);
          } else {
            alignSize = type.alignSize || QUANTUM_SIZE;
          }
        } else {
          size = Types.types[field].flatSize;
          alignSize = Runtime.getAlignSize(null, Types.types[field].alignSize);
        }
      } else if (field[0] == 'b') {
        // bN, large number field, like a [N x i8]
        size = field.substr(1)|0;
        alignSize = 1;
      } else if (field[0] === '<') {
        // vector type
        size = alignSize = Types.types[field].flatSize; // fully aligned
      } else if (field[0] === 'i') {
        // illegal integer field, that could not be legalized because it is an internal structure field
        // it is ok to have such fields, if we just use them as markers of field size and nothing more complex
        size = alignSize = parseInt(field.substr(1))/8;
        assert(size % 1 === 0, 'cannot handle non-byte-size field ' + field);
      } else {
        assert(false, 'invalid type for calculateStructAlignment');
      }
      if (type.packed) alignSize = 1;
      type.alignSize = Math.max(type.alignSize, alignSize);
      var curr = Runtime.alignMemory(type.flatSize, alignSize); // if necessary, place this on aligned memory
      type.flatSize = curr + size;
      if (prev >= 0) {
        diffs.push(curr-prev);
      }
      prev = curr;
      return curr;
    });
    if (type.name_[0] === '[') {
      // arrays have 2 elements, so we get the proper difference. then we scale here. that way we avoid
      // allocating a potentially huge array for [999999 x i8] etc.
      type.flatSize = parseInt(type.name_.substr(1))*type.flatSize/2;
    }
    type.flatSize = Runtime.alignMemory(type.flatSize, type.alignSize);
    if (diffs.length == 0) {
      type.flatFactor = type.flatSize;
    } else if (Runtime.dedup(diffs).length == 1) {
      type.flatFactor = diffs[0];
    }
    type.needsFlattening = (type.flatFactor != 1);
    return type.flatIndexes;
  },
  generateStructInfo: function (struct, typeName, offset) {
    var type, alignment;
    if (typeName) {
      offset = offset || 0;
      type = (typeof Types === 'undefined' ? Runtime.typeInfo : Types.types)[typeName];
      if (!type) return null;
      if (type.fields.length != struct.length) {
        printErr('Number of named fields must match the type for ' + typeName + ': possibly duplicate struct names. Cannot return structInfo');
        return null;
      }
      alignment = type.flatIndexes;
    } else {
      var type = { fields: struct.map(function(item) { return item[0] }) };
      alignment = Runtime.calculateStructAlignment(type);
    }
    var ret = {
      __size__: type.flatSize
    };
    if (typeName) {
      struct.forEach(function(item, i) {
        if (typeof item === 'string') {
          ret[item] = alignment[i] + offset;
        } else {
          // embedded struct
          var key;
          for (var k in item) key = k;
          ret[key] = Runtime.generateStructInfo(item[key], type.fields[i], alignment[i]);
        }
      });
    } else {
      struct.forEach(function(item, i) {
        ret[item[1]] = alignment[i];
      });
    }
    return ret;
  },
  dynCall: function (sig, ptr, args) {
    if (args && args.length) {
      return FUNCTION_TABLE[ptr].apply(null, args);
    } else {
      return FUNCTION_TABLE[ptr]();
    }
  },
  addFunction: function (func) {
    var table = FUNCTION_TABLE;
    var ret = table.length;
    assert(ret % 2 === 0);
    table.push(func);
    for (var i = 0; i < 2-1; i++) table.push(0);
    return ret;
  },
  removeFunction: function (index) {
    var table = FUNCTION_TABLE;
    table[index] = null;
  },
  getAsmConst: function (code, numArgs) {
    // code is a constant string on the heap, so we can cache these
    if (!Runtime.asmConstCache) Runtime.asmConstCache = {};
    var func = Runtime.asmConstCache[code];
    if (func) return func;
    var args = [];
    for (var i = 0; i < numArgs; i++) {
      args.push(String.fromCharCode(36) + i); // $0, $1 etc
    }
    return Runtime.asmConstCache[code] = eval('(function(' + args.join(',') + '){ ' + Pointer_stringify(code) + ' })'); // new Function does not allow upvars in node
  },
  warnOnce: function (text) {
    if (!Runtime.warnOnce.shown) Runtime.warnOnce.shown = {};
    if (!Runtime.warnOnce.shown[text]) {
      Runtime.warnOnce.shown[text] = 1;
      Module.printErr(text);
    }
  },
  funcWrappers: {},
  getFuncWrapper: function (func, sig) {
    assert(sig);
    if (!Runtime.funcWrappers[func]) {
      Runtime.funcWrappers[func] = function dynCall_wrapper() {
        return Runtime.dynCall(sig, func, arguments);
      };
    }
    return Runtime.funcWrappers[func];
  },
  UTF8Processor: function () {
    var buffer = [];
    var needed = 0;
    this.processCChar = function (code) {
      code = code & 0xFF;
      if (buffer.length == 0) {
        if ((code & 0x80) == 0x00) {        // 0xxxxxxx
          return String.fromCharCode(code);
        }
        buffer.push(code);
        if ((code & 0xE0) == 0xC0) {        // 110xxxxx
          needed = 1;
        } else if ((code & 0xF0) == 0xE0) { // 1110xxxx
          needed = 2;
        } else {                            // 11110xxx
          needed = 3;
        }
        return '';
      }
      if (needed) {
        buffer.push(code);
        needed--;
        if (needed > 0) return '';
      }
      var c1 = buffer[0];
      var c2 = buffer[1];
      var c3 = buffer[2];
      var c4 = buffer[3];
      var ret;
      if (buffer.length == 2) {
        ret = String.fromCharCode(((c1 & 0x1F) << 6)  | (c2 & 0x3F));
      } else if (buffer.length == 3) {
        ret = String.fromCharCode(((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6)  | (c3 & 0x3F));
      } else {
        // http://mathiasbynens.be/notes/javascript-encoding#surrogate-formulae
        var codePoint = ((c1 & 0x07) << 18) | ((c2 & 0x3F) << 12) |
                        ((c3 & 0x3F) << 6)  | (c4 & 0x3F);
        ret = String.fromCharCode(
          Math.floor((codePoint - 0x10000) / 0x400) + 0xD800,
          (codePoint - 0x10000) % 0x400 + 0xDC00);
      }
      buffer.length = 0;
      return ret;
    }
    this.processJSString = function processJSString(string) {
      string = unescape(encodeURIComponent(string));
      var ret = [];
      for (var i = 0; i < string.length; i++) {
        ret.push(string.charCodeAt(i));
      }
      return ret;
    }
  },
  stackAlloc: function (size) { var ret = STACKTOP;STACKTOP = (STACKTOP + size)|0;STACKTOP = (((STACKTOP)+7)&-8); return ret; },
  staticAlloc: function (size) { var ret = STATICTOP;STATICTOP = (STATICTOP + size)|0;STATICTOP = (((STATICTOP)+7)&-8); return ret; },
  dynamicAlloc: function (size) { var ret = DYNAMICTOP;DYNAMICTOP = (DYNAMICTOP + size)|0;DYNAMICTOP = (((DYNAMICTOP)+7)&-8); if (DYNAMICTOP >= TOTAL_MEMORY) enlargeMemory();; return ret; },
  alignMemory: function (size,quantum) { var ret = size = Math.ceil((size)/(quantum ? quantum : 8))*(quantum ? quantum : 8); return ret; },
  makeBigInt: function (low,high,unsigned) { var ret = (unsigned ? ((low>>>0)+((high>>>0)*4294967296)) : ((low>>>0)+((high|0)*4294967296))); return ret; },
  GLOBAL_BASE: 8,
  QUANTUM_SIZE: 4,
  __dummy__: 0
}
//========================================
// Runtime essentials
//========================================
var __THREW__ = 0; // Used in checking for thrown exceptions.
var setjmpId = 1; // Used in setjmp/longjmp
var setjmpLabels = {};
var ABORT = false; // whether we are quitting the application. no code should run after this. set in exit() and abort()
var EXITSTATUS = 0;
var undef = 0;
// tempInt is used for 32-bit signed values or smaller. tempBigInt is used
// for 32-bit unsigned values or more than 32 bits. TODO: audit all uses of tempInt
var tempValue, tempInt, tempBigInt, tempInt2, tempBigInt2, tempPair, tempBigIntI, tempBigIntR, tempBigIntS, tempBigIntP, tempBigIntD, tempDouble, tempFloat;
var tempI64, tempI64b;
var tempRet0, tempRet1, tempRet2, tempRet3, tempRet4, tempRet5, tempRet6, tempRet7, tempRet8, tempRet9;
function assert(condition, text) {
  if (!condition) {
    abort('Assertion failed: ' + text);
  }
}
var globalScope = this;
// C calling interface. A convenient way to call C functions (in C files, or
// defined with extern "C").
//
// Note: LLVM optimizations can inline and remove functions, after which you will not be
//       able to call them. Closure can also do so. To avoid that, add your function to
//       the exports using something like
//
//         -s EXPORTED_FUNCTIONS='["_main", "_myfunc"]'
//
// @param ident      The name of the C function (note that C++ functions will be name-mangled - use extern "C")
// @param returnType The return type of the function, one of the JS types 'number', 'string' or 'array' (use 'number' for any C pointer, and
//                   'array' for JavaScript arrays and typed arrays; note that arrays are 8-bit).
// @param argTypes   An array of the types of arguments for the function (if there are no arguments, this can be ommitted). Types are as in returnType,
//                   except that 'array' is not possible (there is no way for us to know the length of the array)
// @param args       An array of the arguments to the function, as native JS values (as in returnType)
//                   Note that string arguments will be stored on the stack (the JS string will become a C string on the stack).
// @return           The return value, as a native JS value (as in returnType)
function ccall(ident, returnType, argTypes, args) {
  return ccallFunc(getCFunc(ident), returnType, argTypes, args);
}
Module["ccall"] = ccall;
// Returns the C function with a specified identifier (for C++, you need to do manual name mangling)
function getCFunc(ident) {
  try {
    var func = Module['_' + ident]; // closure exported function
    if (!func) func = eval('_' + ident); // explicit lookup
  } catch(e) {
  }
  assert(func, 'Cannot call unknown function ' + ident + ' (perhaps LLVM optimizations or closure removed it?)');
  return func;
}
// Internal function that does a C call using a function, not an identifier
function ccallFunc(func, returnType, argTypes, args) {
  var stack = 0;
  function toC(value, type) {
    if (type == 'string') {
      if (value === null || value === undefined || value === 0) return 0; // null string
      value = intArrayFromString(value);
      type = 'array';
    }
    if (type == 'array') {
      if (!stack) stack = Runtime.stackSave();
      var ret = Runtime.stackAlloc(value.length);
      writeArrayToMemory(value, ret);
      return ret;
    }
    return value;
  }
  function fromC(value, type) {
    if (type == 'string') {
      return Pointer_stringify(value);
    }
    assert(type != 'array');
    return value;
  }
  var i = 0;
  var cArgs = args ? args.map(function(arg) {
    return toC(arg, argTypes[i++]);
  }) : [];
  var ret = fromC(func.apply(null, cArgs), returnType);
  if (stack) Runtime.stackRestore(stack);
  return ret;
}
// Returns a native JS wrapper for a C function. This is similar to ccall, but
// returns a function you can call repeatedly in a normal way. For example:
//
//   var my_function = cwrap('my_c_function', 'number', ['number', 'number']);
//   alert(my_function(5, 22));
//   alert(my_function(99, 12));
//
function cwrap(ident, returnType, argTypes) {
  var func = getCFunc(ident);
  return function() {
    return ccallFunc(func, returnType, argTypes, Array.prototype.slice.call(arguments));
  }
}
Module["cwrap"] = cwrap;
// Sets a value in memory in a dynamic way at run-time. Uses the
// type data. This is the same as makeSetValue, except that
// makeSetValue is done at compile-time and generates the needed
// code then, whereas this function picks the right code at
// run-time.
// Note that setValue and getValue only do *aligned* writes and reads!
// Note that ccall uses JS types as for defining types, while setValue and
// getValue need LLVM types ('i8', 'i32') - this is a lower-level operation
function setValue(ptr, value, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': HEAP8[(ptr)]=value; break;
      case 'i8': HEAP8[(ptr)]=value; break;
      case 'i16': HEAP16[((ptr)>>1)]=value; break;
      case 'i32': HEAP32[((ptr)>>2)]=value; break;
      case 'i64': (tempI64 = [value>>>0,(tempDouble=value,Math_abs(tempDouble) >= 1 ? (tempDouble > 0 ? Math_min(Math_floor((tempDouble)/4294967296), 4294967295)>>>0 : (~~(Math_ceil((tempDouble - +(((~~(tempDouble)))>>>0))/4294967296)))>>>0) : 0)],HEAP32[((ptr)>>2)]=tempI64[0],HEAP32[(((ptr)+(4))>>2)]=tempI64[1]); break;
      case 'float': HEAPF32[((ptr)>>2)]=value; break;
      case 'double': HEAPF64[((ptr)>>3)]=value; break;
      default: abort('invalid type for setValue: ' + type);
    }
}
Module['setValue'] = setValue;
// Parallel to setValue.
function getValue(ptr, type, noSafe) {
  type = type || 'i8';
  if (type.charAt(type.length-1) === '*') type = 'i32'; // pointers are 32-bit
    switch(type) {
      case 'i1': return HEAP8[(ptr)];
      case 'i8': return HEAP8[(ptr)];
      case 'i16': return HEAP16[((ptr)>>1)];
      case 'i32': return HEAP32[((ptr)>>2)];
      case 'i64': return HEAP32[((ptr)>>2)];
      case 'float': return HEAPF32[((ptr)>>2)];
      case 'double': return HEAPF64[((ptr)>>3)];
      default: abort('invalid type for setValue: ' + type);
    }
  return null;
}
Module['getValue'] = getValue;
var ALLOC_NORMAL = 0; // Tries to use _malloc()
var ALLOC_STACK = 1; // Lives for the duration of the current function call
var ALLOC_STATIC = 2; // Cannot be freed
var ALLOC_DYNAMIC = 3; // Cannot be freed except through sbrk
var ALLOC_NONE = 4; // Do not allocate
Module['ALLOC_NORMAL'] = ALLOC_NORMAL;
Module['ALLOC_STACK'] = ALLOC_STACK;
Module['ALLOC_STATIC'] = ALLOC_STATIC;
Module['ALLOC_DYNAMIC'] = ALLOC_DYNAMIC;
Module['ALLOC_NONE'] = ALLOC_NONE;
// allocate(): This is for internal use. You can use it yourself as well, but the interface
//             is a little tricky (see docs right below). The reason is that it is optimized
//             for multiple syntaxes to save space in generated code. So you should
//             normally not use allocate(), and instead allocate memory using _malloc(),
//             initialize it with setValue(), and so forth.
// @slab: An array of data, or a number. If a number, then the size of the block to allocate,
//        in *bytes* (note that this is sometimes confusing: the next parameter does not
//        affect this!)
// @types: Either an array of types, one for each byte (or 0 if no type at that position),
//         or a single type which is used for the entire block. This only matters if there
//         is initial data - if @slab is a number, then this does not matter at all and is
//         ignored.
// @allocator: How to allocate memory, see ALLOC_*
function allocate(slab, types, allocator, ptr) {
  var zeroinit, size;
  if (typeof slab === 'number') {
    zeroinit = true;
    size = slab;
  } else {
    zeroinit = false;
    size = slab.length;
  }
  var singleType = typeof types === 'string' ? types : null;
  var ret;
  if (allocator == ALLOC_NONE) {
    ret = ptr;
  } else {
    ret = [_malloc, Runtime.stackAlloc, Runtime.staticAlloc, Runtime.dynamicAlloc][allocator === undefined ? ALLOC_STATIC : allocator](Math.max(size, singleType ? 1 : types.length));
  }
  if (zeroinit) {
    var ptr = ret, stop;
    assert((ret & 3) == 0);
    stop = ret + (size & ~3);
    for (; ptr < stop; ptr += 4) {
      HEAP32[((ptr)>>2)]=0;
    }
    stop = ret + size;
    while (ptr < stop) {
      HEAP8[((ptr++)|0)]=0;
    }
    return ret;
  }
  if (singleType === 'i8') {
    if (slab.subarray || slab.slice) {
      HEAPU8.set(slab, ret);
    } else {
      HEAPU8.set(new Uint8Array(slab), ret);
    }
    return ret;
  }
  var i = 0, type, typeSize, previousType;
  while (i < size) {
    var curr = slab[i];
    if (typeof curr === 'function') {
      curr = Runtime.getFunctionIndex(curr);
    }
    type = singleType || types[i];
    if (type === 0) {
      i++;
      continue;
    }
    if (type == 'i64') type = 'i32'; // special case: we have one i32 here, and one i32 later
    setValue(ret+i, curr, type);
    // no need to look up size unless type changes, so cache it
    if (previousType !== type) {
      typeSize = Runtime.getNativeTypeSize(type);
      previousType = type;
    }
    i += typeSize;
  }
  return ret;
}
Module['allocate'] = allocate;
function Pointer_stringify(ptr, /* optional */ length) {
  // TODO: use TextDecoder
  // Find the length, and check for UTF while doing so
  var hasUtf = false;
  var t;
  var i = 0;
  while (1) {
    t = HEAPU8[(((ptr)+(i))|0)];
    if (t >= 128) hasUtf = true;
    else if (t == 0 && !length) break;
    i++;
    if (length && i == length) break;
  }
  if (!length) length = i;
  var ret = '';
  if (!hasUtf) {
    var MAX_CHUNK = 1024; // split up into chunks, because .apply on a huge string can overflow the stack
    var curr;
    while (length > 0) {
      curr = String.fromCharCode.apply(String, HEAPU8.subarray(ptr, ptr + Math.min(length, MAX_CHUNK)));
      ret = ret ? ret + curr : curr;
      ptr += MAX_CHUNK;
      length -= MAX_CHUNK;
    }
    return ret;
  }
  var utf8 = new Runtime.UTF8Processor();
  for (i = 0; i < length; i++) {
    t = HEAPU8[(((ptr)+(i))|0)];
    ret += utf8.processCChar(t);
  }
  return ret;
}
Module['Pointer_stringify'] = Pointer_stringify;
// Given a pointer 'ptr' to a null-terminated UTF16LE-encoded string in the emscripten HEAP, returns
// a copy of that string as a Javascript String object.
function UTF16ToString(ptr) {
  var i = 0;
  var str = '';
  while (1) {
    var codeUnit = HEAP16[(((ptr)+(i*2))>>1)];
    if (codeUnit == 0)
      return str;
    ++i;
    // fromCharCode constructs a character from a UTF-16 code unit, so we can pass the UTF16 string right through.
    str += String.fromCharCode(codeUnit);
  }
}
Module['UTF16ToString'] = UTF16ToString;
// Copies the given Javascript String object 'str' to the emscripten HEAP at address 'outPtr', 
// null-terminated and encoded in UTF16LE form. The copy will require at most (str.length*2+1)*2 bytes of space in the HEAP.
function stringToUTF16(str, outPtr) {
  for(var i = 0; i < str.length; ++i) {
    // charCodeAt returns a UTF-16 encoded code unit, so it can be directly written to the HEAP.
    var codeUnit = str.charCodeAt(i); // possibly a lead surrogate
    HEAP16[(((outPtr)+(i*2))>>1)]=codeUnit
  }
  // Null-terminate the pointer to the HEAP.
  HEAP16[(((outPtr)+(str.length*2))>>1)]=0
}
Module['stringToUTF16'] = stringToUTF16;
// Given a pointer 'ptr' to a null-terminated UTF32LE-encoded string in the emscripten HEAP, returns
// a copy of that string as a Javascript String object.
function UTF32ToString(ptr) {
  var i = 0;
  var str = '';
  while (1) {
    var utf32 = HEAP32[(((ptr)+(i*4))>>2)];
    if (utf32 == 0)
      return str;
    ++i;
    // Gotcha: fromCharCode constructs a character from a UTF-16 encoded code (pair), not from a Unicode code point! So encode the code point to UTF-16 for constructing.
    if (utf32 >= 0x10000) {
      var ch = utf32 - 0x10000;
      str += String.fromCharCode(0xD800 | (ch >> 10), 0xDC00 | (ch & 0x3FF));
    } else {
      str += String.fromCharCode(utf32);
    }
  }
}
Module['UTF32ToString'] = UTF32ToString;
// Copies the given Javascript String object 'str' to the emscripten HEAP at address 'outPtr', 
// null-terminated and encoded in UTF32LE form. The copy will require at most (str.length+1)*4 bytes of space in the HEAP,
// but can use less, since str.length does not return the number of characters in the string, but the number of UTF-16 code units in the string.
function stringToUTF32(str, outPtr) {
  var iChar = 0;
  for(var iCodeUnit = 0; iCodeUnit < str.length; ++iCodeUnit) {
    // Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code unit, not a Unicode code point of the character! We must decode the string to UTF-32 to the heap.
    var codeUnit = str.charCodeAt(iCodeUnit); // possibly a lead surrogate
    if (codeUnit >= 0xD800 && codeUnit <= 0xDFFF) {
      var trailSurrogate = str.charCodeAt(++iCodeUnit);
      codeUnit = 0x10000 + ((codeUnit & 0x3FF) << 10) | (trailSurrogate & 0x3FF);
    }
    HEAP32[(((outPtr)+(iChar*4))>>2)]=codeUnit
    ++iChar;
  }
  // Null-terminate the pointer to the HEAP.
  HEAP32[(((outPtr)+(iChar*4))>>2)]=0
}
Module['stringToUTF32'] = stringToUTF32;
function demangle(func) {
  try {
    if (typeof func === 'number') func = Pointer_stringify(func);
    if (func[0] !== '_') return func;
    if (func[1] !== '_') return func; // C function
    if (func[2] !== 'Z') return func;
    switch (func[3]) {
      case 'n': return 'operator new()';
      case 'd': return 'operator delete()';
    }
    var i = 3;
    // params, etc.
    var basicTypes = {
      'v': 'void',
      'b': 'bool',
      'c': 'char',
      's': 'short',
      'i': 'int',
      'l': 'long',
      'f': 'float',
      'd': 'double',
      'w': 'wchar_t',
      'a': 'signed char',
      'h': 'unsigned char',
      't': 'unsigned short',
      'j': 'unsigned int',
      'm': 'unsigned long',
      'x': 'long long',
      'y': 'unsigned long long',
      'z': '...'
    };
    function dump(x) {
      //return;
      if (x) Module.print(x);
      Module.print(func);
      var pre = '';
      for (var a = 0; a < i; a++) pre += ' ';
      Module.print (pre + '^');
    }
    var subs = [];
    function parseNested() {
      i++;
      if (func[i] === 'K') i++; // ignore const
      var parts = [];
      while (func[i] !== 'E') {
        if (func[i] === 'S') { // substitution
          i++;
          var next = func.indexOf('_', i);
          var num = func.substring(i, next) || 0;
          parts.push(subs[num] || '?');
          i = next+1;
          continue;
        }
        if (func[i] === 'C') { // constructor
          parts.push(parts[parts.length-1]);
          i += 2;
          continue;
        }
        var size = parseInt(func.substr(i));
        var pre = size.toString().length;
        if (!size || !pre) { i--; break; } // counter i++ below us
        var curr = func.substr(i + pre, size);
        parts.push(curr);
        subs.push(curr);
        i += pre + size;
      }
      i++; // skip E
      return parts;
    }
    var first = true;
    function parse(rawList, limit, allowVoid) { // main parser
      limit = limit || Infinity;
      var ret = '', list = [];
      function flushList() {
        return '(' + list.join(', ') + ')';
      }
      var name;
      if (func[i] === 'N') {
        // namespaced N-E
        name = parseNested().join('::');
        limit--;
        if (limit === 0) return rawList ? [name] : name;
      } else {
        // not namespaced
        if (func[i] === 'K' || (first && func[i] === 'L')) i++; // ignore const and first 'L'
        var size = parseInt(func.substr(i));
        if (size) {
          var pre = size.toString().length;
          name = func.substr(i + pre, size);
          i += pre + size;
        }
      }
      first = false;
      if (func[i] === 'I') {
        i++;
        var iList = parse(true);
        var iRet = parse(true, 1, true);
        ret += iRet[0] + ' ' + name + '<' + iList.join(', ') + '>';
      } else {
        ret = name;
      }
      paramLoop: while (i < func.length && limit-- > 0) {
        //dump('paramLoop');
        var c = func[i++];
        if (c in basicTypes) {
          list.push(basicTypes[c]);
        } else {
          switch (c) {
            case 'P': list.push(parse(true, 1, true)[0] + '*'); break; // pointer
            case 'R': list.push(parse(true, 1, true)[0] + '&'); break; // reference
            case 'L': { // literal
              i++; // skip basic type
              var end = func.indexOf('E', i);
              var size = end - i;
              list.push(func.substr(i, size));
              i += size + 2; // size + 'EE'
              break;
            }
            case 'A': { // array
              var size = parseInt(func.substr(i));
              i += size.toString().length;
              if (func[i] !== '_') throw '?';
              i++; // skip _
              list.push(parse(true, 1, true)[0] + ' [' + size + ']');
              break;
            }
            case 'E': break paramLoop;
            default: ret += '?' + c; break paramLoop;
          }
        }
      }
      if (!allowVoid && list.length === 1 && list[0] === 'void') list = []; // avoid (void)
      return rawList ? list : ret + flushList();
    }
    return parse();
  } catch(e) {
    return func;
  }
}
function demangleAll(text) {
  return text.replace(/__Z[\w\d_]+/g, function(x) { var y = demangle(x); return x === y ? x : (x + ' [' + y + ']') });
}
function stackTrace() {
  var stack = new Error().stack;
  return stack ? demangleAll(stack) : '(no stack trace available)'; // Stack trace is not available at least on IE10 and Safari 6.
}
// Memory management
var PAGE_SIZE = 4096;
function alignMemoryPage(x) {
  return (x+4095)&-4096;
}
var HEAP;
var HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;
var STATIC_BASE = 0, STATICTOP = 0, staticSealed = false; // static area
var STACK_BASE = 0, STACKTOP = 0, STACK_MAX = 0; // stack area
var DYNAMIC_BASE = 0, DYNAMICTOP = 0; // dynamic area handled by sbrk
function enlargeMemory() {
  abort('Cannot enlarge memory arrays. Either (1) compile with -s TOTAL_MEMORY=X with X higher than the current value ' + TOTAL_MEMORY + ', (2) compile with ALLOW_MEMORY_GROWTH which adjusts the size at runtime but prevents some optimizations, or (3) set Module.TOTAL_MEMORY before the program runs.');
}
var TOTAL_STACK = Module['TOTAL_STACK'] || 5242880;
var TOTAL_MEMORY = Module['TOTAL_MEMORY'] || 16777216;
var FAST_MEMORY = Module['FAST_MEMORY'] || 2097152;
// Initialize the runtime's memory
// check for full engine support (use string 'subarray' to avoid closure compiler confusion)
assert(typeof Int32Array !== 'undefined' && typeof Float64Array !== 'undefined' && !!(new Int32Array(1)['subarray']) && !!(new Int32Array(1)['set']),
       'Cannot fallback to non-typed array case: Code is too specialized');
var buffer = new ArrayBuffer(TOTAL_MEMORY);
HEAP8 = new Int8Array(buffer);
HEAP16 = new Int16Array(buffer);
HEAP32 = new Int32Array(buffer);
HEAPU8 = new Uint8Array(buffer);
HEAPU16 = new Uint16Array(buffer);
HEAPU32 = new Uint32Array(buffer);
HEAPF32 = new Float32Array(buffer);
HEAPF64 = new Float64Array(buffer);
// Endianness check (note: assumes compiler arch was little-endian)
HEAP32[0] = 255;
assert(HEAPU8[0] === 255 && HEAPU8[3] === 0, 'Typed arrays 2 must be run on a little-endian system');
Module['HEAP'] = HEAP;
Module['HEAP8'] = HEAP8;
Module['HEAP16'] = HEAP16;
Module['HEAP32'] = HEAP32;
Module['HEAPU8'] = HEAPU8;
Module['HEAPU16'] = HEAPU16;
Module['HEAPU32'] = HEAPU32;
Module['HEAPF32'] = HEAPF32;
Module['HEAPF64'] = HEAPF64;
function callRuntimeCallbacks(callbacks) {
  while(callbacks.length > 0) {
    var callback = callbacks.shift();
    if (typeof callback == 'function') {
      callback();
      continue;
    }
    var func = callback.func;
    if (typeof func === 'number') {
      if (callback.arg === undefined) {
        Runtime.dynCall('v', func);
      } else {
        Runtime.dynCall('vi', func, [callback.arg]);
      }
    } else {
      func(callback.arg === undefined ? null : callback.arg);
    }
  }
}
var __ATPRERUN__  = []; // functions called before the runtime is initialized
var __ATINIT__    = []; // functions called during startup
var __ATMAIN__    = []; // functions called when main() is to be run
var __ATEXIT__    = []; // functions called during shutdown
var __ATPOSTRUN__ = []; // functions called after the runtime has exited
var runtimeInitialized = false;
function preRun() {
  // compatibility - merge in anything from Module['preRun'] at this time
  if (Module['preRun']) {
    if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
    while (Module['preRun'].length) {
      addOnPreRun(Module['preRun'].shift());
    }
  }
  callRuntimeCallbacks(__ATPRERUN__);
}
function ensureInitRuntime() {
  if (runtimeInitialized) return;
  runtimeInitialized = true;
  callRuntimeCallbacks(__ATINIT__);
}
function preMain() {
  callRuntimeCallbacks(__ATMAIN__);
}
function exitRuntime() {
  callRuntimeCallbacks(__ATEXIT__);
}
function postRun() {
  // compatibility - merge in anything from Module['postRun'] at this time
  if (Module['postRun']) {
    if (typeof Module['postRun'] == 'function') Module['postRun'] = [Module['postRun']];
    while (Module['postRun'].length) {
      addOnPostRun(Module['postRun'].shift());
    }
  }
  callRuntimeCallbacks(__ATPOSTRUN__);
}
function addOnPreRun(cb) {
  __ATPRERUN__.unshift(cb);
}
Module['addOnPreRun'] = Module.addOnPreRun = addOnPreRun;
function addOnInit(cb) {
  __ATINIT__.unshift(cb);
}
Module['addOnInit'] = Module.addOnInit = addOnInit;
function addOnPreMain(cb) {
  __ATMAIN__.unshift(cb);
}
Module['addOnPreMain'] = Module.addOnPreMain = addOnPreMain;
function addOnExit(cb) {
  __ATEXIT__.unshift(cb);
}
Module['addOnExit'] = Module.addOnExit = addOnExit;
function addOnPostRun(cb) {
  __ATPOSTRUN__.unshift(cb);
}
Module['addOnPostRun'] = Module.addOnPostRun = addOnPostRun;
// Tools
// This processes a JS string into a C-line array of numbers, 0-terminated.
// For LLVM-originating strings, see parser.js:parseLLVMString function
function intArrayFromString(stringy, dontAddNull, length /* optional */) {
  var ret = (new Runtime.UTF8Processor()).processJSString(stringy);
  if (length) {
    ret.length = length;
  }
  if (!dontAddNull) {
    ret.push(0);
  }
  return ret;
}
Module['intArrayFromString'] = intArrayFromString;
function intArrayToString(array) {
  var ret = [];
  for (var i = 0; i < array.length; i++) {
    var chr = array[i];
    if (chr > 0xFF) {
      chr &= 0xFF;
    }
    ret.push(String.fromCharCode(chr));
  }
  return ret.join('');
}
Module['intArrayToString'] = intArrayToString;
// Write a Javascript array to somewhere in the heap
function writeStringToMemory(string, buffer, dontAddNull) {
  var array = intArrayFromString(string, dontAddNull);
  var i = 0;
  while (i < array.length) {
    var chr = array[i];
    HEAP8[(((buffer)+(i))|0)]=chr
    i = i + 1;
  }
}
Module['writeStringToMemory'] = writeStringToMemory;
function writeArrayToMemory(array, buffer) {
  for (var i = 0; i < array.length; i++) {
    HEAP8[(((buffer)+(i))|0)]=array[i];
  }
}
Module['writeArrayToMemory'] = writeArrayToMemory;
function writeAsciiToMemory(str, buffer, dontAddNull) {
  for (var i = 0; i < str.length; i++) {
    HEAP8[(((buffer)+(i))|0)]=str.charCodeAt(i)
  }
  if (!dontAddNull) HEAP8[(((buffer)+(str.length))|0)]=0
}
Module['writeAsciiToMemory'] = writeAsciiToMemory;
function unSign(value, bits, ignore, sig) {
  if (value >= 0) {
    return value;
  }
  return bits <= 32 ? 2*Math.abs(1 << (bits-1)) + value // Need some trickery, since if bits == 32, we are right at the limit of the bits JS uses in bitshifts
                    : Math.pow(2, bits)         + value;
}
function reSign(value, bits, ignore, sig) {
  if (value <= 0) {
    return value;
  }
  var half = bits <= 32 ? Math.abs(1 << (bits-1)) // abs is needed if bits == 32
                        : Math.pow(2, bits-1);
  if (value >= half && (bits <= 32 || value > half)) { // for huge values, we can hit the precision limit and always get true here. so don't do that
                                                       // but, in general there is no perfect solution here. With 64-bit ints, we get rounding and errors
                                                       // TODO: In i64 mode 1, resign the two parts separately and safely
    value = -2*half + value; // Cannot bitshift half, as it may be at the limit of the bits JS uses in bitshifts
  }
  return value;
}
if (!Math['imul']) Math['imul'] = function imul(a, b) {
  var ah  = a >>> 16;
  var al = a & 0xffff;
  var bh  = b >>> 16;
  var bl = b & 0xffff;
  return (al*bl + ((ah*bl + al*bh) << 16))|0;
};
Math.imul = Math['imul'];
var Math_abs = Math.abs;
var Math_cos = Math.cos;
var Math_sin = Math.sin;
var Math_tan = Math.tan;
var Math_acos = Math.acos;
var Math_asin = Math.asin;
var Math_atan = Math.atan;
var Math_atan2 = Math.atan2;
var Math_exp = Math.exp;
var Math_log = Math.log;
var Math_sqrt = Math.sqrt;
var Math_ceil = Math.ceil;
var Math_floor = Math.floor;
var Math_pow = Math.pow;
var Math_imul = Math.imul;
var Math_fround = Math.fround;
var Math_min = Math.min;
// A counter of dependencies for calling run(). If we need to
// do asynchronous work before running, increment this and
// decrement it. Incrementing must happen in a place like
// PRE_RUN_ADDITIONS (used by emcc to add file preloading).
// Note that you can add dependencies in preRun, even though
// it happens right before run - run will be postponed until
// the dependencies are met.
var runDependencies = 0;
var runDependencyWatcher = null;
var dependenciesFulfilled = null; // overridden to take different actions when all run dependencies are fulfilled
function addRunDependency(id) {
  runDependencies++;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
}
Module['addRunDependency'] = addRunDependency;
function removeRunDependency(id) {
  runDependencies--;
  if (Module['monitorRunDependencies']) {
    Module['monitorRunDependencies'](runDependencies);
  }
  if (runDependencies == 0) {
    if (runDependencyWatcher !== null) {
      clearInterval(runDependencyWatcher);
      runDependencyWatcher = null;
    }
    if (dependenciesFulfilled) {
      var callback = dependenciesFulfilled;
      dependenciesFulfilled = null;
      callback(); // can add another dependenciesFulfilled
    }
  }
}
Module['removeRunDependency'] = removeRunDependency;
Module["preloadedImages"] = {}; // maps url to image data
Module["preloadedAudios"] = {}; // maps url to audio data
var memoryInitializer = null;
// === Body ===
STATIC_BASE = 8;
STATICTOP = STATIC_BASE + 73632;
var _stdout;
var _stdout=_stdout=allocate([0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
var _stderr;
var _stderr=_stderr=allocate([0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
/* global initializers */ __ATINIT__.push({ func: function() { runPostSets() } },{ func: function() { __GLOBAL__I_a() } },{ func: function() { __GLOBAL__I_a102() } },{ func: function() { __GLOBAL__I_a263() } },{ func: function() { __GLOBAL__I_a437() } },{ func: function() { __GLOBAL__I_a485() } },{ func: function() { __GLOBAL__I_a531() } },{ func: function() { __GLOBAL__I_a583() } },{ func: function() { __GLOBAL__I_a640() } },{ func: function() { __GLOBAL__I_a696() } },{ func: function() { __GLOBAL__I_a798() } },{ func: function() { __GLOBAL__I_a974() } },{ func: function() { __GLOBAL__I_a1146() } },{ func: function() { __GLOBAL__I_a1310() } },{ func: function() { __GLOBAL__I_a1472() } },{ func: function() { __GLOBAL__I_a1667() } },{ func: function() { __GLOBAL__I_a1827() } },{ func: function() { __GLOBAL__I_a1991() } },{ func: function() { __GLOBAL__I_a2158() } },{ func: function() { __GLOBAL__I_a2324() } },{ func: function() { __GLOBAL__I_a2491() } },{ func: function() { __GLOBAL__I_a2653() } },{ func: function() { __GLOBAL__I_a2815() } },{ func: function() { __GLOBAL__I_a2990() } },{ func: function() { __GLOBAL__I_a3165() } },{ func: function() { __GLOBAL__I_a3342() } },{ func: function() { __GLOBAL__I_a3505() } },{ func: function() { __GLOBAL__I_a3678() } },{ func: function() { __GLOBAL__I_a3840() } },{ func: function() { __GLOBAL__I_a3993() } },{ func: function() { __GLOBAL__I_a4159() } },{ func: function() { __GLOBAL__I_a4326() } },{ func: function() { __GLOBAL__I_a4528() } },{ func: function() { __GLOBAL__I_a4694() } },{ func: function() { __GLOBAL__I_a4932() } },{ func: function() { __GLOBAL__I_a5096() } },{ func: function() { __GLOBAL__I_a5263() } },{ func: function() { __GLOBAL__I_a5428() } },{ func: function() { __GLOBAL__I_a5595() } },{ func: function() { __GLOBAL__I_a5758() } },{ func: function() { __GLOBAL__I_a5974() } },{ func: function() { __GLOBAL__I_a6207() } },{ func: function() { __GLOBAL__I_a6376() } },{ func: function() { __GLOBAL__I_a6609() } },{ func: function() { __GLOBAL__I_a6784() } },{ func: function() { __GLOBAL__I_a6948() } },{ func: function() { __GLOBAL__I_a7117() } },{ func: function() { __GLOBAL__I_a7286() } },{ func: function() { __GLOBAL__I_a7457() } },{ func: function() { __GLOBAL__I_a7628() } },{ func: function() { __GLOBAL__I_a7792() } },{ func: function() { __GLOBAL__I_a7963() } },{ func: function() { __GLOBAL__I_a8131() } },{ func: function() { __GLOBAL__I_a8326() } },{ func: function() { __GLOBAL__I_a8490() } },{ func: function() { __GLOBAL__I_a8650() } },{ func: function() { __GLOBAL__I_a8814() } },{ func: function() { __GLOBAL__I_a8974() } },{ func: function() { __GLOBAL__I_a9153() } },{ func: function() { __GLOBAL__I_a9317() } },{ func: function() { __GLOBAL__I_a9479() } },{ func: function() { __GLOBAL__I_a9641() } },{ func: function() { __GLOBAL__I_a9809() } },{ func: function() { __GLOBAL__I_a9972() } },{ func: function() { __GLOBAL__I_a10136() } },{ func: function() { __GLOBAL__I_a10391() } },{ func: function() { __GLOBAL__I_a10562() } },{ func: function() { __GLOBAL__I_a10724() } },{ func: function() { __GLOBAL__I_a10896() } },{ func: function() { __GLOBAL__I_a11056() } },{ func: function() { __GLOBAL__I_a11224() } },{ func: function() { __GLOBAL__I_a11395() } },{ func: function() { __GLOBAL__I_a11593() } },{ func: function() { __GLOBAL__I_a11755() } },{ func: function() { __GLOBAL__I_a11919() } },{ func: function() { __GLOBAL__I_a12081() } },{ func: function() { __GLOBAL__I_a12250() } },{ func: function() { __GLOBAL__I_a12418() } },{ func: function() { __GLOBAL__I_a12580() } },{ func: function() { __GLOBAL__I_a12742() } },{ func: function() { __GLOBAL__I_a12906() } },{ func: function() { __GLOBAL__I_a13068() } },{ func: function() { __GLOBAL__I_a13235() } },{ func: function() { __GLOBAL__I_a13407() } },{ func: function() { __GLOBAL__I_a13573() } },{ func: function() { __GLOBAL__I_a13737() } },{ func: function() { __GLOBAL__I_a13905() } },{ func: function() { __GLOBAL__I_a14076() } },{ func: function() { __GLOBAL__I_a14240() } },{ func: function() { __GLOBAL__I_a14405() } },{ func: function() { __GLOBAL__I_a14612() } },{ func: function() { __GLOBAL__I_a14664() } },{ func: function() { __GLOBAL__I_a14826() } },{ func: function() { __GLOBAL__I_a14965() } });
var ___dso_handle;
var __ZTVN10__cxxabiv120__si_class_type_infoE;
__ZTVN10__cxxabiv120__si_class_type_infoE=allocate([0,0,0,0,168,200,0,0,94,11,0,0,58,9,0,0,96,1,0,0,210,14,0,0,190,1,0,0,184,0,0,0,118,13,0,0,232,19,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
var __ZTVN10__cxxabiv119__pointer_type_infoE;
__ZTVN10__cxxabiv119__pointer_type_infoE=allocate([0,0,0,0,184,200,0,0,94,11,0,0,100,7,0,0,96,1,0,0,210,14,0,0,160,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
var __ZTVN10__cxxabiv117__class_type_infoE;
__ZTVN10__cxxabiv117__class_type_infoE=allocate([0,0,0,0,216,200,0,0,94,11,0,0,176,5,0,0,96,1,0,0,210,14,0,0,190,1,0,0,116,4,0,0,52,2,0,0,224,8,0,0,0,0,0,0,0,0,0,0], "i8", ALLOC_STATIC);
var __ZTIt;
__ZTIt=allocate([104,138,0,0,192,138,0,0], "i8", ALLOC_STATIC);
var __ZTIs;
__ZTIs=allocate([104,138,0,0,200,138,0,0], "i8", ALLOC_STATIC);
var __ZTIm;
__ZTIm=allocate([104,138,0,0,208,138,0,0], "i8", ALLOC_STATIC);
var __ZTIl;
__ZTIl=allocate([104,138,0,0,216,138,0,0], "i8", ALLOC_STATIC);
var __ZTIj;
__ZTIj=allocate([104,138,0,0,224,138,0,0], "i8", ALLOC_STATIC);
var __ZTIi;
__ZTIi=allocate([104,138,0,0,232,138,0,0], "i8", ALLOC_STATIC);
var __ZTIh;
__ZTIh=allocate([104,138,0,0,240,138,0,0], "i8", ALLOC_STATIC);
var __ZTIf;
__ZTIf=allocate([104,138,0,0,248,138,0,0], "i8", ALLOC_STATIC);
var __ZTId;
__ZTId=allocate([104,138,0,0,0,139,0,0], "i8", ALLOC_STATIC);
var __ZTIc;
__ZTIc=allocate([104,138,0,0,8,139,0,0], "i8", ALLOC_STATIC);
var __ZTIa;
__ZTIa=allocate([104,138,0,0,152,42,0,0], "i8", ALLOC_STATIC);
/* memory initializer */ allocate([218,5,0,0,0,0,0,0,109,97,116,101,114,105,97,108,68,105,102,102,117,115,101,67,111,108,111,114,0,0,0,0,82,101,115,111,117,114,99,101,77,97,110,97,103,101,114,58,58,102,105,110,100,70,114,111,109,70,105,108,101,58,0,0,100,101,115,116,114,111,121,105,110,103,32,69,71,76,32,99,111,110,116,101,120,116,0,0,80,105,120,101,108,66,117,102,102,101,114,82,101,110,100,101,114,84,97,114,103,101,116,115,32,97,114,101,32,117,110,97,118,97,105,108,97,98,108,101,0,0,0,0,0,0,0,0,103,101,116,69,110,103,105,110,101,0,0,0,0,0,0,0,102,105,120,101,100,0,0,0,109,97,116,101,114,105,97,108,76,105,103,104,116,0,0,0,32,110,111,116,32,102,111,117,110,100,0,0,0,0,0,0,83,99,101,110,101,0,0,0,101,114,114,111,114,32,99,114,101,97,116,105,110,103,32,99,111,110,116,101,120,116,0,0,100,101,115,116,114,111,121,101,100,32,71,76,82,101,110,100,101,114,68,101,118,105,99,101,0,0,0,0,0,0,0,0,84,101,120,116,117,114,101,0,115,97,109,112,0,0,0,0,97,109,98,105,101,110,116,67,111,108,111,114,0,0,0,0,115,116,114,101,97,109,101,114,32,102,111,114,32,0,0,0,99,114,101,97,116,101,71,114,105,100,77,101,115,104,0,0,101,114,114,111,114,32,99,114,101,97,116,105,110,103,32,115,117,114,102,97,99,101,0,0,99,114,101,97,116,101,100,32,71,76,82,101,110,100,101,114,68,101,118,105,99,101,0,0,82,101,110,100,101,114,83,116,97,116,101,0,0,0,0,0,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,0,0,0,0,0,0,108,105,103,104,116,67,111,108,111,114,0,0,0,0,0,0,99,114,101,97,116,101,84,111,114,117,115,77,101,115,104,0,99,104,111,111,115,101,69,71,76,67,111,110,102,105,103,32,99,111,110,102,105,103,58,0,71,76,95,69,88,84,69,78,83,73,79,78,83,58,0,0,110,101,119,95,69,71,76,87,105,110,100,111,119,82,101,110,100,101,114,84,97,114,103,101,116,0,0,0,0,0,0,0,116,101,120,0,0,0,0,0,108,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,0,0,0,0,0,0,0,46,92,0,0,0,0,0,0,99,114,101,97,116,101,68,105,102,102,117,115,101,77,97,116,101,114,105,97,108,0,0,0,101,114,114,111,114,32,99,104,111,111,115,105,110,103,32,99,111,110,102,105,103,0,0,0,71,76,95,83,72,65,68,73,78,71,95,76,65,78,71,85,65,71,69,95,86,69,82,83,73,79,78,58,0,0,0,0,110,101,119,95,71,76,69,83,50,82,101,110,100,101,114,68,101,118,105,99,101,0,0,0,116,101,120,116,117,114,101,77,97,116,114,105,120,0,0,0,110,111,114,109,97,108,77,97,116,114,105,120,0,0,0,0,46,47,0,0,0,0,0,0,115,101,116,65,117,100,105,111,68,101,118,105,99,101,0,0,69,71,76,95,69,88,84,69,78,83,73,79,78,83,58,0,71,76,95,86,69,82,83,73,79,78,58,0,0,0,0,0,115,119,97,112,0,0,0,0,116,101,120,116,117,114,101,32,105,115,110,39,116,32,105,110,32,82,71,66,47,82,71,66,65,32,102,111,114,109,97,116,0,0,0,0,0,0,0,0,46,46,0,0,0,0,0,0,85,78,75,78,79,87,78,0,115,101,116,82,101,110,100,101,114,68,101,118,105,99,101,0,69,71,76,95,86,69,82,83,73,79,78,58,0,0,0,0,71,76,95,82,69,78,68,69,82,69,82,58,0,0,0,0,66,117,102,102,101,114,77,97,110,97,103,101,114,58,58,112,111,115,116,67,111,110,116,101,120,116,82,101,115,101,116,0,101,110,100,83,99,101,110,101,0,0,0,0,0,0,0,0,115,104,97,100,101,114,0,0,35,118,101,114,115,105,111,110,32,49,50,48,10,35,105,102,32,100,101,102,105,110,101,100,40,71,76,95,69,83,41,10,112,114,101,99,105,115,105,111,110,32,109,101,100,105,117,109,112,32,102,108,111,97,116,59,10,35,101,110,100,105,102,10,118,97,114,121,105,110,103,32,118,101,99,52,32,99,111,108,111,114,59,10,118,97,114,121,105,110,103,32,102,108,111,97,116,32,102,111,103,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,116,101,120,116,117,114,101,83,101,116,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,102,111,103,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,116,101,120,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,118,101,99,52,32,102,114,97,103,67,111,108,111,114,61,99,111,108,111,114,42,40,116,101,120,116,117,114,101,50,68,40,116,101,120,44,103,108,95,80,111,105,110,116,67,111,111,114,100,41,43,40,49,46,48,45,116,101,120,116,117,114,101,83,101,116,41,41,59,10,103,108,95,70,114,97,103,67,111,108,111,114,61,109,105,120,40,102,111,103,67,111,108,111,114,44,102,114,97,103,67,111,108,111,114,44,102,111,103,41,59,10,125,10,0,0,116,101,120,116,117,114,101,32,105,115,110,39,116,32,50,32,100,105,109,101,110,115,105,111,110,97,108,0,0,0,0,0,71,76,95,70,82,65,77,69,66,85,70,70,69,82,95,73,78,67,79,77,80,76,69,84,69,95,68,73,77,69,78,83,73,79,78,83,0,0,0,0,105,110,115,116,97,108,108,72,97,110,100,108,101,114,115,0,69,71,76,95,86,69,78,68,79,82,58,0,0,0,0,0,119,105,100,116,104,32,111,114,32,104,101,105,103,104,116,32,110,111,116,32,97,115,32,101,120,112,101,99,116,101,100,0,71,76,95,86,69,78,68,79,82,58,0,0,0,0,0,0,66,117,102,102,101,114,77,97,110,97,103,101,114,58,58,112,114,101,67,111,110,116,101,120,116,82,101,115,101,116,0,0,98,101,103,105,110,83,99,101,110,101,0,0,0,0,0,0,83,107,121,66,111,120,77,97,116,101,114,105,97,108,67,114,101,97,116,111,114,58,58,99,114,101,97,116,101,32,117,110,105,109,112,108,101,109,101,110,116,101,100,0,0,0,0,0,115,116,114,117,99,116,32,71,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,32,102,111,103,32,58,32,70,79,71,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,115,116,114,117,99,116,32,71,79,85,84,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,32,102,111,103,32,58,32,70,79,71,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,102,108,111,97,116,32,112,111,105,110,116,83,105,122,101,59,10,102,108,111,97,116,52,32,118,105,101,119,112,111,114,116,59,10,91,109,97,120,118,101,114,116,101,120,99,111,117,110,116,40,52,41,93,10,118,111,105,100,32,109,97,105,110,40,112,111,105,110,116,32,71,73,78,32,103,105,110,91,49,93,44,105,110,111,117,116,32,84,114,105,97,110,103,108,101,83,116,114,101,97,109,60,71,79,85,84,62,32,115,116,114,101,97,109,41,123,10,102,108,111,97,116,32,97,115,112,101,99,116,61,118,105,101,119,112,111,114,116,46,119,47,118,105,101,119,112,111,114,116,46,122,59,10,102,108,111,97,116,32,119,61,97,115,112,101,99,116,42,112,111,105,110,116,83,105,122,101,47,52,46,48,44,104,61,112,111,105,110,116,83,105,122,101,47,52,46,48,59,10,99,111,110,115,116,32,102,108,111,97,116,51,32,112,111,115,105,116,105,111,110,115,91,52,93,61,123,10,102,108,111,97,116,51,40,45,119,44,45,104,44,48,46,48,41,44,10,102,108,111,97,116,51,40,119,44,45,104,44,48,46,48,41,44,10,102,108,111,97,116,51,40,45,119,44,104,44,48,46,48,41,44,10,102,108,111,97,116,51,40,119,44,104,44,48,46,48,41,44,10,125,59,10,99,111,110,115,116,32,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,115,91,52,93,61,123,10,102,108,111,97,116,50,40,48,46,48,44,48,46,48,41,44,10,102,108,111,97,116,50,40,49,46,48,44,48,46,48,41,44,10,102,108,111,97,116,50,40,48,46,48,44,49,46,48,41,44,10,102,108,111,97,116,50,40,49,46,48,44,49,46,48,41,44,10,125,59,10,71,79,85,84,32,103,111,117,116,59,10,102,111,114,40,105,110,116,32,105,61,48,59,105,60,52,59,105,43,43,41,123,10,103,111,117,116,46,112,111,115,105,116,105,111,110,61,103,105,110,91,48,93,46,112,111,115,105,116,105,111,110,43,102,108,111,97,116,52,40,112,111,115,105,116,105,111,110,115,91,105,93,44,48,46,48,41,59,10,103,111,117,116,46,99,111,108,111,114,61,103,105,110,91,48,93,46,99,111,108,111,114,59,10,103,111,117,116,46,102,111,103,61,103,105,110,91,48,93,46,102,111,103,59,10,103,111,117,116,46,116,101,120,67,111,111,114,100,61,116,101,120,67,111,111,114,100,115,91,105,93,59,10,115,116,114,101,97,109,46,65,112,112,101,110,100,40,103,111,117,116,41,59,10,125,10,115,116,114,101,97,109,46,82,101,115,116,97,114,116,83,116,114,105,112,40,41,59,10,125,10,0,0,0,44,32,110,111,116,32,66,73,95,82,71,66,0,0,0,0,46,0,0,0,0,0,0,0,71,76,95,70,82,65,77,69,66,85,70,70,69,82,95,73,78,67,79,77,80,76,69,84,69,95,77,73,83,83,73,78,71,95,65,84,84,65,67,72,77,69,78,84,0,0,0,0,115,101,116,72,97,115,66,97,99,107,97,98,108,101,70,105,120,101,100,0,0,0,0,0,69,114,114,111,114,32,105,110,32,101,103,108,67,114,101,97,116,101,67,111,110,116,101,120,116,0,0,0,0,0,0,0,66,117,102,102,101,114,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,68,101,97,99,116,105,118,97,116,101,0,0,0,0,0,0,0,0,99,114,101,97,116,101,0,0,44,32,110,111,116,32,49,44,32,50,44,32,52,44,32,56,44,32,50,52,44,32,111,114,32,51,50,32,98,105,116,0,115,116,114,117,99,116,32,80,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,58,32,67,79,76,79,82,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,84,101,120,116,117,114,101,50,68,32,116,101,120,59,10,83,97,109,112,108,101,114,83,116,97,116,101,32,115,97,109,112,59,10,102,108,111,97,116,52,32,109,97,105,110,40,80,73,78,32,112,105,110,41,58,32,83,86,95,84,65,82,71,69,84,123,10,114,101,116,117,114,110,32,112,105,110,46,99,111,108,111,114,32,42,32,116,101,120,46,83,97,109,112,108,101,40,115,97,109,112,44,112,105,110,46,116,101,120,67,111,111,114,100,41,59,10,125,0,0,0,0,115,116,114,117,99,116,32,80,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,58,32,67,79,76,79,82,59,10,102,108,111,97,116,32,102,111,103,58,32,70,79,71,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,102,108,111,97,116,32,116,101,120,116,117,114,101,83,101,116,59,10,102,108,111,97,116,52,32,102,111,103,67,111,108,111,114,59,10,84,101,120,116,117,114,101,50,68,32,116,101,120,59,10,83,97,109,112,108,101,114,83,116,97,116,101,32,115,97,109,112,59,10,102,108,111,97,116,52,32,109,97,105,110,40,80,73,78,32,112,105,110,41,58,32,83,86,95,84,65,82,71,69,84,123,10,102,108,111,97,116,52,32,102,114,97,103,67,111,108,111,114,61,112,105,110,46,99,111,108,111,114,42,40,116,101,120,46,83,97,109,112,108,101,40,115,97,109,112,44,112,105,110,46,116,101,120,67,111,111,114,100,41,43,40,49,46,48,45,116,101,120,116,117,114,101,83,101,116,41,41,59,10,114,101,116,117,114,110,32,108,101,114,112,40,102,111,103,67,111,108,111,114,44,102,114,97,103,67,111,108,111,114,44,112,105,110,46,102,111,103,41,59,10,125,10,0,0,0,0,79,116,104,101,114,32,116,121,112,101,0,0,0,0,0,0,66,77,80,83,116,114,101,97,109,101,114,58,32,67,111,109,112,114,101,115,115,105,111,110,32,105,115,32,0,0,0,0,84,101,120,116,117,114,101,32,110,111,116,32,102,111,117,110,100,32,119,105,116,104,32,110,97,109,101,58,0,0,0,0,32,110,97,109,101,58,0,0,71,76,95,70,82,65,77,69,66,85,70,70,69,82,95,73,78,67,79,77,80,76,69,84,69,95,65,84,84,65,67,72,77,69,78,84,0,0,0,0,115,101,116,72,97,115,66,97,99,107,97,98,108,101,83,104,97,100,101,114,0,0,0,0,67,65,76,67,85,76,65,84,69,68,32,69,71,76,32,86,69,82,83,73,79,78,58,0,69,114,114,111,114,32,105,110,32,101,103,108,67,114,101,97,116,101,80,98,117,102,102,101,114,83,117,114,102,97,99,101,0,0,0,0,0,0,0,0,67,65,76,67,85,76,65,84,69,68,32,71,76,32,86,69,82,83,73,79,78,58,0,0,84,69,88,67,79,79,82,68,0,0,0,0,0,0,0,0,66,117,102,102,101,114,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,65,99,116,105,118,97,116,101,0,0,100,97,116,97,0,0,0,0,82,101,110,100,101,114,68,101,118,105,99,101,0,0,0,0,108,111,110,103,0,0,0,0,84,71,65,83,116,114,101,97,109,101,114,58,32,70,111,114,109,97,116,32,105,115,32,0,35,105,102,32,100,101,102,105,110,101,100,40,71,76,95,69,83,41,10,112,114,101,99,105,115,105,111,110,32,109,101,100,105,117,109,112,32,102,108,111,97,116,59,10,35,101,110,100,105,102,10,118,97,114,121,105,110,103,32,118,101,99,52,32,99,111,108,111,114,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,116,101,120,67,111,111,114,100,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,116,101,120,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,103,108,95,70,114,97,103,67,111,108,111,114,32,61,32,99,111,108,111,114,32,42,32,116,101,120,116,117,114,101,50,68,40,116,101,120,44,116,101,120,67,111,111,114,100,41,59,10,125,0,0,0,0,0,102,114,111,109,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,32,116,111,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,32,117,110,105,109,112,108,101,109,101,110,116,101,100,0,35,105,102,32,100,101,102,105,110,101,100,40,71,76,95,69,83,41,10,112,114,101,99,105,115,105,111,110,32,109,101,100,105,117,109,112,32,102,108,111,97,116,59,10,35,101,110,100,105,102,10,118,97,114,121,105,110,103,32,118,101,99,52,32,99,111,108,111,114,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,116,101,120,67,111,111,114,100,59,10,118,97,114,121,105,110,103,32,102,108,111,97,116,32,102,111,103,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,116,101,120,116,117,114,101,83,101,116,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,102,111,103,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,116,101,120,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,118,101,99,52,32,102,114,97,103,67,111,108,111,114,61,99,111,108,111,114,42,40,116,101,120,116,117,114,101,50,68,40,116,101,120,44,116,101,120,67,111,111,114,100,41,43,40,49,46,48,45,116,101,120,116,117,114,101,83,101,116,41,41,59,10,103,108,95,70,114,97,103,67,111,108,111,114,61,109,105,120,40,102,111,103,67,111,108,111,114,44,102,114,97,103,67,111,108,111,114,44,102,111,103,41,59,10,125,10,0,0,0,0,0,79,116,104,101,114,32,112,97,108,101,116,116,101,0,0,0,82,101,110,100,101,114,86,97,114,105,97,98,108,101,32,110,111,116,32,102,111,117,110,100,32,119,105,116,104,32,110,97,109,101,58,0,0,0,0,0,84,101,120,116,117,114,101,77,97,110,97,103,101,114,58,58,112,111,115,116,67,111,110,116,101,120,116,82,101,115,101,116,0,0,0,0,0,0,0,0,82,101,115,111,117,114,99,101,58,0,0,0,0,0,0,0,71,76,95,70,82,65,77,69,66,85,70,70,69,82,95,67,79,77,80,76,69,84,69,0,100,101,115,116,114,111,121,0,101,103,108,73,110,105,116,105,97,108,105,122,101,32,101,114,114,111,114,0,0,0,0,0,110,111,32,99,111,110,102,105,103,115,32,102,114,111,109,32,101,103,108,67,104,111,111,115,101,67,111,110,102,105,103,58,0,0,0,0,0,0,0,0,99,114,101,97,116,105,110,103,32,69,71,76,32,99,111,110,116,101,120,116,32,102,111,114,32,112,98,117,102,102,101,114,0,0,0,0,0,0,0,0,71,76,84,101,120,116,117,114,101,58,58,103,101,116,71,76,84,97,114,103,101,116,58,32,73,110,118,97,108,105,100,32,116,97,114,103,101,116,0,0,49,46,49,0,0,0,0,0,67,79,76,79,82,0,0,0,117,110,97,98,108,101,32,116,111,32,99,114,101,97,116,101,32,86,97,114,105,97,98,108,101,66,117,102,102,101,114,0,102,109,116,32,0,0,0,0,82,101,110,100,101,114,84,97,114,103,101,116,0,0,0,0,73,110,116,101,114,108,101,97,118,101,100,32,110,111,116,32,99,117,114,114,101,110,116,108,121,32,115,117,112,112,111,114,116,101,100,0,0,0,0,0,115,116,114,117,99,116,32,86,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,115,116,114,117,99,116,32,86,79,85,84,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,102,108,111,97,116,52,120,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,102,108,111,97,116,52,120,52,32,116,101,120,116,117,114,101,77,97,116,114,105,120,59,10,102,108,111,97,116,32,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,59,10,86,79,85,84,32,109,97,105,110,40,86,73,78,32,118,105,110,41,123,10,86,79,85,84,32,118,111,117,116,59,10,118,111,117,116,46,112,111,115,105,116,105,111,110,61,109,117,108,40,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,44,118,105,110,46,112,111,115,105,116,105,111,110,41,59,10,118,111,117,116,46,99,111,108,111,114,61,118,105,110,46,99,111,108,111,114,42,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,43,40,49,46,48,45,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,41,59,10,118,111,117,116,46,116,101,120,67,111,111,114,100,61,109,117,108,40,116,101,120,116,117,114,101,77,97,116,114,105,120,44,102,108,111,97,116,52,40,118,105,110,46,116,101,120,67,111,111,114,100,44,48,46,48,44,49,46,48,41,41,59,10,114,101,116,117,114,110,32,118,111,117,116,59,10,125,0,0,0,102,114,111,109,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,32,116,111,32,84,121,112,101,95,67,65,80,83,85,76,69,32,117,110,105,109,112,108,101,109,101,110,116,101,100,0,0,0,0,0,115,116,114,117,99,116,32,86,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,51,32,110,111,114,109,97,108,32,58,32,78,79,82,77,65,76,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,115,116,114,117,99,116,32,86,79,85,84,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,52,32,99,111,108,111,114,32,58,32,67,79,76,79,82,59,10,102,108,111,97,116,32,102,111,103,58,32,70,79,71,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,102,108,111,97,116,52,120,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,102,108,111,97,116,52,120,52,32,110,111,114,109,97,108,77,97,116,114,105,120,59,10,102,108,111,97,116,52,32,109,97,116,101,114,105,97,108,68,105,102,102,117,115,101,67,111,108,111,114,59,10,102,108,111,97,116,52,32,109,97,116,101,114,105,97,108,65,109,98,105,101,110,116,67,111,108,111,114,59,10,102,108,111,97,116,32,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,59,10,102,108,111,97,116,52,32,108,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,59,10,102,108,111,97,116,52,32,108,105,103,104,116,67,111,108,111,114,59,10,102,108,111,97,116,52,32,97,109,98,105,101,110,116,67,111,108,111,114,59,10,102,108,111,97,116,32,109,97,116,101,114,105,97,108,76,105,103,104,116,59,10,102,108,111,97,116,52,120,52,32,116,101,120,116,117,114,101,77,97,116,114,105,120,59,10,102,108,111,97,116,32,102,111,103,68,101,110,115,105,116,121,59,10,102,108,111,97,116,50,32,102,111,103,68,105,115,116,97,110,99,101,59,10,86,79,85,84,32,109,97,105,110,40,86,73,78,32,118,105,110,41,123,10,86,79,85,84,32,118,111,117,116,59,10,118,111,117,116,46,112,111,115,105,116,105,111,110,61,109,117,108,40,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,44,118,105,110,46,112,111,115,105,116,105,111,110,41,59,10,102,108,111,97,116,51,32,118,105,101,119,78,111,114,109,97,108,61,110,111,114,109,97,108,105,122,101,40,109,117,108,40,110,111,114,109,97,108,77,97,116,114,105,120,44,118,105,110,46,110,111,114,109,97,108,41,41,59,10,102,108,111,97,116,32,108,105,103,104,116,73,110,116,101,110,115,105,116,121,61,99,108,97,109,112,40,45,100,111,116,40,108,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,44,118,105,101,119,78,111,114,109,97,108,41,44,48,46,48,44,49,46,48,41,59,10,102,108,111,97,116,52,32,108,111,99,97,108,76,105,103,104,116,67,111,108,111,114,61,108,105,103,104,116,73,110,116,101,110,115,105,116,121,42,108,105,103,104,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,76,105,103,104,116,43,40,49,46,48,45,109,97,116,101,114,105,97,108,76,105,103,104,116,41,59,10,118,111,117,116,46,99,111,108,111,114,61,99,108,97,109,112,40,108,111,99,97,108,76,105,103,104,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,68,105,102,102,117,115,101,67,111,108,111,114,32,43,32,97,109,98,105,101,110,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,65,109,98,105,101,110,116,67,111,108,111,114,44,48,46,48,44,49,46,48,41,59,10,118,111,117,116,46,99,111,108,111,114,61,118,105,110,46,99,111,108,111,114,42,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,43,118,111,117,116,46,99,111,108,111,114,42,40,49,46,48,45,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,41,59,10,118,111,117,116,46,116,101,120,67,111,111,114,100,61,109,117,108,40,116,101,120,116,117,114,101,77,97,116,114,105,120,44,102,108,111,97,116,52,40,118,105,110,46,116,101,120,67,111,111,114,100,44,48,46,48,44,49,46,48,41,41,59,10,32,118,111,117,116,46,102,111,103,61,99,108,97,109,112,40,49,46,48,45,102,111,103,68,101,110,115,105,116,121,42,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,44,48,46,48,44,49,46,48,41,59,10,114,101,116,117,114,110,32,118,111,117,116,59,10,125,0,0,0,0,0,0,0,0,105,110,99,111,114,114,101,99,116,32,115,105,122,101,0,0,66,77,80,83,116,114,101,97,109,101,114,58,32,70,111,114,109,97,116,32,105,115,32,0,33,61,0,0,0,0,0,0,84,101,120,116,117,114,101,77,97,110,97,103,101,114,58,58,112,114,101,67,111,110,116,101,120,116,82,101,115,101,116,0,97,100,100,105,110,103,32,100,101,102,97,117,108,116,32,115,116,114,101,97,109,101,114,0,105,110,118,97,108,105,100,32,70,111,110,116,68,97,116,97,0,0,0,0,0,0,0,0,71,76,70,66,79,82,101,110,100,101,114,84,97,114,103,101,116,58,58,103,101,116,71,76,65,116,116,97,99,104,109,101,110,116,58,32,73,110,118,97,108,105,100,32,97,116,116,97,99,104,109,101,110,116,0,0,69,110,103,105,110,101,0,0,101,114,114,111,114,32,103,101,116,116,105,110,103,32,100,105,115,112,108,97,121,0,0,0,32,115,116,101,110,99,105,108,58,0,0,0,0,0,0,0,70,111,114,109,97,116,95,83,69,77,65,78,84,73,67,95,68,69,80,84,72,32,110,111,116,32,97,118,97,105,108,97,98,108,101,32,102,111,114,32,112,98,117,102,102,101,114,115,0,0,0,0,0,0,0,0,71,76,84,101,120,116,117,114,101,58,58,114,101,97,100,32,105,115,32,110,111,116,32,115,117,112,112,111,114,116,101,100,0,0,0,0,0,0,0,0,50,46,0,0,0,0,0,0,78,79,82,77,65,76,0,0,110,111,116,32,82,73,70,70,32,87,65,86,69,32,102,111,114,109,97,116,0,0,0,0,86,105,101,119,112,111,114,116,0,0,0,0,0,0,0,0,116,111,111,32,109,97,110,121,32,115,117,98,109,101,115,104,101,115,58,0,0,0,0,0,79,110,108,121,32,116,114,117,101,99,111,108,111,114,32,99,117,114,114,101,110,116,108,121,32,115,117,112,112,111,114,116,101,100,0,0,0,0,0,0,97,116,116,114,105,98,117,116,101,32,118,101,99,52,32,80,79,83,73,84,73,79,78,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,52,32,67,79,76,79,82,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,50,32,84,69,88,67,79,79,82,68,48,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,99,111,108,111,114,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,116,101,120,67,111,111,114,100,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,116,101,120,116,117,114,101,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,103,108,95,80,111,115,105,116,105,111,110,61,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,32,42,32,80,79,83,73,84,73,79,78,59,10,99,111,108,111,114,61,67,79,76,79,82,42,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,43,40,49,46,48,45,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,41,59,10,116,101,120,67,111,111,114,100,61,40,116,101,120,116,117,114,101,77,97,116,114,105,120,32,42,32,118,101,99,52,40,84,69,88,67,79,79,82,68,48,44,48,46,48,44,49,46,48,41,41,46,120,121,59,10,125,0,0,0,0,0,0,102,114,111,109,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,32,116,111,32,84,121,112,101,95,65,65,66,79,88,32,117,110,105,109,112,108,101,109,101,110,116,101,100,0,0,0,0,0,0,0,97,116,116,114,105,98,117,116,101,32,118,101,99,52,32,80,79,83,73,84,73,79,78,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,51,32,78,79,82,77,65,76,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,52,32,67,79,76,79,82,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,50,32,84,69,88,67,79,79,82,68,48,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,99,111,108,111,114,59,10,118,97,114,121,105,110,103,32,102,108,111,97,116,32,102,111,103,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,116,101,120,67,111,111,114,100,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,110,111,114,109,97,108,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,109,97,116,101,114,105,97,108,68,105,102,102,117,115,101,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,109,97,116,101,114,105,97,108,65,109,98,105,101,110,116,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,108,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,108,105,103,104,116,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,97,109,98,105,101,110,116,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,109,97,116,101,114,105,97,108,76,105,103,104,116,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,116,101,120,116,117,114,101,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,102,111,103,68,101,110,115,105,116,121,59,10,117,110,105,102,111,114,109,32,118,101,99,50,32,102,111,103,68,105,115,116,97,110,99,101,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,103,108,95,80,111,115,105,116,105,111,110,61,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,32,42,32,80,79,83,73,84,73,79,78,59,10,118,101,99,51,32,118,105,101,119,78,111,114,109,97,108,61,110,111,114,109,97,108,105,122,101,40,110,111,114,109,97,108,77,97,116,114,105,120,32,42,32,118,101,99,52,40,78,79,82,77,65,76,44,48,46,48,41,41,46,120,121,122,59,10,102,108,111,97,116,32,108,105,103,104,116,73,110,116,101,110,115,105,116,121,61,99,108,97,109,112,40,45,100,111,116,40,108,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,46,120,121,122,44,118,105,101,119,78,111,114,109,97,108,41,44,48,46,48,44,49,46,48,41,59,10,118,101,99,52,32,108,111,99,97,108,76,105,103,104,116,67,111,108,111,114,61,40,108,105,103,104,116,73,110,116,101,110,115,105,116,121,42,108,105,103,104,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,76,105,103,104,116,41,43,40,49,46,48,45,109,97,116,101,114,105,97,108,76,105,103,104,116,41,59,10,99,111,108,111,114,61,99,108,97,109,112,40,108,111,99,97,108,76,105,103,104,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,68,105,102,102,117,115,101,67,111,108,111,114,32,43,32,97,109,98,105,101,110,116,67,111,108,111,114,42,109,97,116,101,114,105,97,108,65,109,98,105,101,110,116,67,111,108,111,114,44,48,46,48,44,49,46,48,41,59,10,99,111,108,111,114,61,67,79,76,79,82,42,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,43,99,111,108,111,114,42,40,49,46,48,45,109,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,41,59,10,116,101,120,67,111,111,114,100,61,40,116,101,120,116,117,114,101,77,97,116,114,105,120,32,42,32,118,101,99,52,40,84,69,88,67,79,79,82,68,48,44,48,46,48,44,49,46,48,41,41,46,120,121,59,10,102,111,103,61,99,108,97,109,112,40,49,46,48,45,102,111,103,68,101,110,115,105,116,121,42,40,103,108,95,80,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,44,48,46,48,44,49,46,48,41,59,10,125,0,0,105,110,118,97,108,105,100,32,102,111,114,109,97,116,32,111,114,32,99,97,112,115,0,0,44,32,110,111,116,32,49,0,32,102,111,114,109,97,116,32,100,111,101,115,32,110,111,116,32,109,97,116,99,104,32,102,111,114,109,97,116,58,0,0,84,101,120,116,117,114,101,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,68,101,97,99,116,105,118,97,116,101,0,0,0,0,0,0,0,83,104,97,100,101,114,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,68,101,97,99,116,105,118,97,116,101,0,0,0,0,0,0,0,0,97,100,100,105,110,103,32,115,116,114,101,97,109,101,114,32,102,111,114,32,101,120,116,101,110,115,105,111,110,32,0,0,61,0,0,0,0,0,0,0,119,105,110,100,111,119,0,0,32,100,101,112,116,104,58,0,116,101,120,116,117,114,101,32,105,115,32,110,117,108,108,0,99,97,110,32,110,111,116,32,114,101,97,100,32,115,117,98,32,115,101,99,116,105,111,110,115,0,0,0,0,0,0,0,78,85,76,76,32,82,101,110,100,101,114,84,97,114,103,101,116,0,0,0,0,0,0,0,66,76,69,78,68,95,73,78,68,73,67,69,83,0,0,0,44,0,0,0,0,0,0,0,117,110,97,98,108,101,32,116,111,32,99,114,101,97,116,101,32,86,101,114,116,101,120,66,117,102,102,101,114,0,0,0,87,65,86,69,0,0,0,0,77,97,116,101,114,105,97,108,0,0,0,0,0,0,0,0,78,111,116,32,84,77,83,72,32,118,101,114,115,105,111,110,58,0,0,0,0,0,0,0,73,110,118,97,108,105,100,32,99,111,108,111,114,32,109,97,112,32,116,121,112,101,0,0,102,114,111,109,32,84,121,112,101,95,67,65,80,83,85,76,69,32,116,111,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,32,117,110,105,109,112,108,101,109,101,110,116,101,100,0,0,0,0,0,98,97,100,32,109,97,103,105,99,32,110,117,109,98,101,114,0,0,0,0,0,0,0,0,66,77,80,83,116,114,101,97,109,101,114,58,32,80,108,97,110,101,115,32,97,114,101,32,0,0,0,0,0,0,0,0,82,101,110,100,101,114,86,97,114,105,97,98,108,101,58,0,84,101,120,116,117,114,101,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,65,99,116,105,118,97,116,101,0,83,104,97,100,101,114,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,65,99,116,105,118,97,116,101,0,0,114,101,109,111,118,105,110,103,32,115,116,114,101,97,109,101,114,32,102,111,114,32,101,120,116,101,110,115,105,111,110,32,0,0,0,0,0,0,0,0,58,0,0,0,0,0,0,0,117,110,107,110,111,119,110,32,98,111,117,110,100,32,116,121,112,101,0,0,0,0,0,0,70,66,79,32,87,97,114,110,105,110,103,58,0,0,0,0,77,101,115,104,0,0,0,0,80,105,120,101,108,66,117,102,102,101,114,32,105,115,32,110,111,116,32,97,32,84,101,120,116,117,114,101,77,105,112,80,105,120,101,108,66,117,102,102,101,114,0,0,0,0,0,0,101,114,114,111,114,32,105,110,32,99,114,101,97,116,101,67,111,110,116,101,120,116,0,0,32,102,111,117,110,100,32,108,111,99,97,116,105,111,110,58,0,0,0,0,0,0,0,0,103,108,115,108,32,99,111,109,112,105,108,101,32,101,114,114,111,114,58,10,0,0,0,0,99,97,110,118,97,115,0,0,46,71,76,82,101,110,100,101,114,68,101,118,105,99,101,0,66,76,69,78,68,95,87,69,73,71,72,84,83,0,0,0,66,97,99,107,97,98,108,101,84,101,120,116,117,114,101,58,58,99,111,110,118,101,114,116,67,114,101,97,116,101,32,45,32,99,111,110,118,101,114,116,105,110,103,32,116,101,120,116,117,114,101,58,0,0,0,0,117,110,97,98,108,101,32,116,111,32,99,114,101,97,116,101,32,73,110,100,101,120,66,117,102,102,101,114,0,0,0,0,99,114,101,97,116,101,65,117,100,105,111,66,117,102,102,101,114,32,99,97,108,108,101,100,32,119,105,116,104,32,97,32,110,117,108,108,32,115,116,114,101,97,109,0,0,0,0,0,82,73,70,70,0,0,0,0,83,105,109,117,108,97,116,111,114,0,0,0,0,0,0,0,78,111,116,32,111,102,32,84,77,83,72,32,102,111,114,109,97,116,0,0,0,0,0,0,83,116,114,101,97,109,32,105,115,32,78,85,76,76,0,0,116,114,97,99,101,83,101,103,109,101,110,116,32,110,111,116,32,105,109,112,108,101,109,101,110,116,101,100,32,102,111,114,32,84,121,112,101,95,67,79,78,86,69,88,83,79,76,73,68,0,0,0,0,0,0,0,69,114,114,111,114,32,105,110,32,116,101,120,116,117,114,101,32,99,111,110,118,101,114,116,67,114,101,97,116,101,0,0,69,114,114,111,114,32,105,110,32,115,104,97,100,101,114,32,99,111,110,118,101,114,116,67,114,101,97,116,101,0,0,0,101,114,114,111,114,32,117,110,109,97,110,97,103,105,110,103,32,114,101,115,111,117,114,99,101,44,32,99,104,101,99,107,32,116,104,97,116,32,114,101,115,111,117,114,99,101,32,105,115,32,109,97,110,97,103,101,100,44,32,111,114,32,99,104,101,99,107,32,105,110,104,101,114,105,116,97,110,99,101,32,104,101,105,97,114,99,104,121,0,0,0,0,0,0,0,0,80,97,114,116,105,116,105,111,110,78,111,100,101,0,0,0,78,111,100,101,0,0,0,0,77,101,115,104,67,111,109,112,111,110,101,110,116,0,0,0,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,48,49,50,51,52,53,54,55,56,57,126,33,64,35,36,37,94,38,42,40,41,95,43,124,123,125,58,34,39,60,62,63,96,45,61,92,47,91,93,59,44,46,32,9,0,0,0,0,0,0,0,0,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,0,99,97,110,32,110,111,116,32,98,105,110,100,32,80,105,120,101,108,66,117,102,102,101,114,32,116,111,32,70,66,79,0,80,105,120,101,108,66,117,102,102,101,114,115,32,110,111,116,32,115,117,112,112,111,114,116,101,100,0,0,0,0,0,0,99,114,101,97,116,105,110,103,32,69,71,76,32,99,111,110,116,101,120,116,32,102,111,114,32,0,0,0,0,0,0,0,67,104,111,111,115,105,110,103,32,99,111,110,102,105,103,32,102,111,114,32,99,111,108,111,114,58,0,0,0,0,0,0,110,117,108,108,32,80,105,120,101,108,66,117,102,102,101,114,0,0,0,0,0,0,0,0,71,76,84,101,120,116,117,114,101,58,32,67,97,110,110,111,116,32,108,111,97,100,32,97,32,110,111,110,32,112,111,119,101,114,32,111,102,32,50,32,116,101,120,116,117,114,101,0,83,101,109,97,110,116,105,99,32,110,97,109,101,58,0,0,103,108,115,108,32,108,105,110,107,32,101,114,114,111,114,58,10,0,0,0,0,0,0,0,112,114,111,103,114,97,109,32,98,101,105,110,103,32,99,111,109,112,105,108,101,100,58,10,0,0,0,0,0,0,0,0,99,114,101,97,116,105,110,103,32,0,0,0,0,0,0,0,99,97,110,110,111,116,32,97,100,100,32,109,117,108,116,105,112,108,101,32,98,117,102,102,101,114,115,32,116,111,32,97,32,86,101,114,116,101,120,68,97,116,97,32,119,105,116,104,32,97,110,32,117,110,115,112,101,99,105,102,105,101,100,32,86,101,114,116,101,120,70,111,114,109,97,116,0,0,0,0,110,111,32,102,111,114,109,97,116,32,99,111,110,118,101,114,115,105,111,110,32,97,118,97,105,108,97,98,108,101,0,0,80,79,83,73,84,73,79,78,0,0,0,0,0,0,0,0,65,117,116,111,32,99,97,108,99,117,108,97,116,101,100,32,109,105,112,32,108,101,118,101,108,115,32,115,112,101,99,105,102,105,101,100,32,119,105,116,104,32,110,111,32,97,117,116,111,103,101,110,44,32,110,111,110,32,122,101,114,111,32,108,101,118,101,108,115,32,109,97,121,32,98,101,32,101,109,112,116,121,0,0,0,0,0,0,72,111,112,67,111,109,112,111,110,101,110,116,0,0,0,0,117,110,97,98,108,101,32,116,111,32,99,114,101,97,116,101,32,86,101,114,116,101,120,70,111,114,109,97,116,0,0,0,99,114,101,97,116,101,65,117,100,105,111,66,117,102,102,101,114,32,99,97,108,108,101,100,32,119,105,116,104,111,117,116,32,97,32,118,97,108,105,100,32,65,117,100,105,111,68,101,118,105,99,101,0,0,0,0,117,110,97,98,108,101,32,116,111,32,114,101,97,100,32,104,101,97,100,101,114,0,0,0,77,111,100,117,108,101,0,0,117,110,115,105,103,110,101,100,32,105,110,116,0,0,0,0,37,89,45,37,109,45,37,100,32,37,72,58,37,77,58,37].concat([83,0,0,0,0,0,0,0,117,110,105,109,112,108,101,109,101,110,116,101,100,0,0,0,110,117,108,108,80,111,105,110,116,101,114,0,0,0,0,0,105,110,118,97,108,105,100,80,97,114,97,109,101,116,101,114,115,0,0,0,0,0,0,0,37,100,0,0,0,0,0,0,116,97,115,115,101,114,116,0,105,110,116,0,0,0,0,0,48,46,57,46,48,0,0,0,69,114,114,111,114,0,0,0,101,120,99,101,115,115,0,0,100,101,98,117,103,0,0,0,97,108,101,114,116,0,0,0,119,97,114,110,105,110,103,0,101,114,114,111,114,0,0,0,105,110,105,116,105,97,108,105,122,101,0,0,0,0,0,0,76,111,103,0,0,0,0,0,117,110,115,105,103,110,101,100,32,115,104,111,114,116,0,0,81,117,97,116,101,114,110,105,111,110,0,0,0,0,0,0,86,101,99,116,111,114,52,0,102,111,114,99,105,110,103,32,84,114,97,100,105,116,105,111,110,97,108,32,109,97,116,104,0,0,0,0,0,0,0,0,86,101,99,116,111,114,51,0,68,101,116,101,99,116,101,100,32,78,69,79,78,32,118,101,114,115,105,111,110,58,0,0,86,101,99,116,111,114,50,0,68,101,116,101,99,116,101,100,32,83,83,69,32,118,101,114,115,105,111,110,58,0,0,0,83,116,114,105,110,103,0,0,46,112,97,100,0,0,0,0,46,116,101,114,114,97,105,110,0,0,0,0,0,0,0,0,115,104,111,114,116,0,0,0,46,115,116,117,100,105,111,0,46,110,111,100,101,0,0,0,37,102,0,0,0,0,0,0,46,98,115,112,0,0,0,0,46,116,97,100,112,111,108,101,0,0,0,0,0,0,0,0,46,114,105,98,98,105,116,0,117,110,115,105,103,110,101,100,32,99,104,97,114,0,0,0,46,112,101,101,112,101,114,0,46,107,110,111,116,0,0,0,46,104,111,112,0,0,0,0,46,102,108,105,99,107,0,0,85,78,75,78,79,87,78,58,32,0,0,0,0,0,0,0,69,88,67,69,83,83,58,32,32,0,0,0,0,0,0,0,46,80,114,111,102,105,108,101,0,0,0,0,0,0,0,0,68,69,66,85,71,58,32,32,32,0,0,0,0,0,0,0,43,0,0,0,0,0,0,0,65,76,69,82,84,58,32,32,32,0,0,0,0,0,0,0,87,65,82,78,73,78,71,58,32,0,0,0,0,0,0,0,115,116,100,58,58,98,97,100,95,97,108,108,111,99,0,0,115,105,103,110,101,100,32,99,104,97,114,0,0,0,0,0,46,101,103,103,0,0,0,0,98,0,0,0,0,0,0,0,119,0,0,0,0,0,0,0,69,82,82,79,82,58,32,32,32,0,0,0,0,0,0,0,114,0,0,0,0,0,0,0,76,79,71,71,69,82,58,32,32,0,0,0,0,0,0,0,58,32,0,0,0,0,0,0,97,0,0,0,0,0,0,0,66,97,99,107,116,114,97,99,101,32,101,110,100,101,100,0,101,114,114,111,114,32,105,110,32,112,116,104,114,101,97,100,95,109,117,116,101,120,95,105,110,105,116,0,0,0,0,0,99,114,101,97,116,105,110,103,32,116,111,97,100,108,101,116,46,101,103,103,46,76,111,103,103,101,114,58,0,0,0,0,117,110,107,110,111,119,110,0,79,112,101,110,105,110,103,32,102,105,108,101,58,32,0,0,66,97,99,107,116,114,97,99,101,32,115,116,97,114,116,105,110,103,0,0,0,0,0,0,99,104,97,114,0,0,0,0,101,109,115,99,114,105,112,116,101,110,58,58,109,101,109,111,114,121,95,118,105,101,119,0,101,109,115,99,114,105,112,116,101,110,58,58,118,97,108,0,118,111,105,100,0,0,0,0,116,111,97,100,108,101,116,0,119,97,118,101,77,97,116,114,105,120,0,0,0,0,0,0,69,110,103,105,110,101,58,58,99,111,110,116,101,120,116,68,101,97,99,116,105,118,97,116,101,0,0,0,0,0,0,0,114,101,102,114,97,99,116,77,97,116,114,105,120,0,0,0,69,110,103,105,110,101,58,58,99,111,110,116,101,120,116,65,99,116,105,118,97,116,101,0,115,116,100,58,58,119,115,116,114,105,110,103,0,0,0,0,114,101,102,108,101,99,116,77,97,116,114,105,120,0,0,0,69,110,103,105,110,101,58,58,99,111,110,116,101,120,116,82,101,115,101,116,0,0,0,0,98,117,109,112,83,97,109,112,0,0,0,0,0,0,0,0,99,97,110,32,110,111,116,32,99,104,97,110,103,101,32,65,117,100,105,111,68,101,118,105,99,101,0,0,0,0,0,0,119,97,118,101,84,101,120,0,116,101,120,116,117,114,101,65,117,116,111,103,101,110,77,105,112,77,97,112,115,58,0,0,116,101,120,116,117,114,101,78,111,110,80,111,119,101,114,79,102,50,58,0,0,0,0,0,114,101,102,114,97,99,116,83,97,109,112,0,0,0,0,0,112,114,101,118,0,0,0,0,116,101,120,116,117,114,101,78,111,110,80,111,119,101,114,79,102,50,82,101,115,116,114,105,99,116,101,100,58,0,0,0,114,101,102,114,97,99,116,84,101,120,0,0,0,0,0,0,110,101,120,116,0,0,0,0,116,101,120,116,117,114,101,68,111,116,51,58,0,0,0,0,114,101,102,108,101,99,116,83,97,109,112,0,0,0,0,0,97,116,69,110,100,0,0,0,114,101,110,100,101,114,84,111,84,101,120,116,117,114,101,78,111,110,80,111,119,101,114,79,102,50,82,101,115,116,114,105,99,116,101,100,58,0,0,0,114,101,102,108,101,99,116,84,101,120,0,0,0,0,0,0,97,116,66,101,103,105,110,0,114,101,110,100,101,114,84,111,68,101,112,116,104,84,101,120,116,117,114,101,58,0,0,0,109,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,0,0,0,0,0,0,0,78,111,100,101,82,97,110,103,101,0,0,0,0,0,0,0,114,101,110,100,101,114,84,111,84,101,120,116,117,114,101,58,0,0,0,0,0,0,0,0,109,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,0,0,0,0,0,0,0,0,109,97,120,84,101,120,116,117,114,101,83,105,122,101,58,0,119,97,118,101,72,101,105,103,104,116,0,0,0,0,0,0,103,101,116,78,111,100,101,115,0,0,0,0,0,0,0,0,109,97,120,84,101,120,116,117,114,101,83,116,97,103,101,115,58,0,0,0,0,0,0,0,108,105,103,104,116,83,112,101,99,117,108,97,114,0,0,0,103,101,116,87,111,114,108,100,84,114,97,110,115,102,111,114,109,0,0,0,0,0,0,0,82,101,110,100,101,114,68,101,118,105,99,101,32,67,97,112,97,98,105,108,105,116,105,101,115,58,0,0,0,0,0,0,108,105,103,104,116,80,111,115,105,116,105,111,110,0,0,0,103,101,116,84,114,97,110,115,102,111,114,109,0,0,0,0,115,116,100,58,58,115,116,114,105,110,103,0,0,0,0,0,99,97,110,32,110,111,116,32,99,104,97,110,103,101,32,82,101,110,100,101,114,68,101,118,105,99,101,32,105,110,32,97,110,32,117,110,98,97,99,107,101,100,32,101,110,103,105,110,101,0,0,0,0,0,0,0,99,97,109,101,114,97,80,111,115,105,116,105,111,110,0,0,115,101,116,84,114,97,110,115,102,111,114,109,0,0,0,0,119,97,118,0,0,0,0,0,102,111,103,67,111,108,111,114,0,0,0,0,0,0,0,0,103,101,116,83,99,111,112,101,0,0,0,0,0,0,0,0,116,109,115,104,0,0,0,0,102,111,103,68,105,115,116,97,110,99,101,0,0,0,0,0,115,101,116,83,99,111,112,101,0,0,0,0,0,0,0,0,116,103,97,0,0,0,0,0,109,111,100,101,108,77,97,116,114,105,120,0,0,0,0,0,114,101,109,111,118,101,0,0,98,109,112,0,0,0,0,0,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,0,0,0,0,0,0,0,97,116,116,97,99,104,0,0,100,100,115,0,0,0,0,0,115,116,114,117,99,116,32,80,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,32,102,111,103,58,32,70,79,71,59,10,102,108,111,97,116,52,32,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,48,59,10,102,108,111,97,116,52,32,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,49,59,10,102,108,111,97,116,50,32,98,117,109,112,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,50,59,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,70,117,108,108,58,32,84,69,88,67,79,79,82,68,51,59,10,125,59,10,102,108,111,97,116,52,32,102,111,103,67,111,108,111,114,59,10,102,108,111,97,116,52,32,108,105,103,104,116,80,111,115,105,116,105,111,110,59,10,102,108,111,97,116,52,32,108,105,103,104,116,83,112,101,99,117,108,97,114,59,10,102,108,111,97,116,52,32,99,97,109,101,114,97,80,111,115,105,116,105,111,110,59,10,102,108,111,97,116,32,119,97,118,101,72,101,105,103,104,116,59,10,102,108,111,97,116,52,32,109,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,59,10,102,108,111,97,116,32,109,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,59,10,84,101,120,116,117,114,101,50,68,32,114,101,102,108,101,99,116,84,101,120,59,10,84,101,120,116,117,114,101,50,68,32,114,101,102,114,97,99,116,84,101,120,59,10,84,101,120,116,117,114,101,50,68,32,119,97,118,101,84,101,120,59,10,83,97,109,112,108,101,114,83,116,97,116,101,32,114,101,102,108,101,99,116,83,97,109,112,59,10,83,97,109,112,108,101,114,83,116,97,116,101,32,114,101,102,114,97,99,116,83,97,109,112,59,10,83,97,109,112,108,101,114,83,116,97,116,101,32,98,117,109,112,83,97,109,112,59,10,102,108,111,97,116,52,32,109,97,105,110,40,80,73,78,32,112,105,110,41,58,32,83,86,95,84,65,82,71,69,84,123,10,102,108,111,97,116,51,32,110,111,114,109,97,108,86,101,99,116,111,114,61,102,108,111,97,116,51,40,48,44,48,44,49,46,48,41,59,10,102,108,111,97,116,52,32,98,117,109,112,67,111,108,111,114,61,119,97,118,101,84,101,120,46,83,97,109,112,108,101,40,98,117,109,112,83,97,109,112,44,112,105,110,46,98,117,109,112,80,111,115,105,116,105,111,110,41,59,10,102,108,111,97,116,51,32,98,117,109,112,86,101,99,116,111,114,61,40,98,117,109,112,67,111,108,111,114,46,120,121,122,45,48,46,53,41,42,50,46,48,59,10,102,108,111,97,116,50,32,112,101,114,116,117,114,98,97,116,105,111,110,61,119,97,118,101,72,101,105,103,104,116,42,98,117,109,112,86,101,99,116,111,114,46,120,121,59,10,102,108,111,97,116,51,32,101,121,101,86,101,99,116,111,114,32,61,32,110,111,114,109,97,108,105,122,101,40,99,97,109,101,114,97,80,111,115,105,116,105,111,110,46,120,121,122,32,45,32,112,105,110,46,112,111,115,105,116,105,111,110,70,117,108,108,46,120,121,122,41,59,10,102,108,111,97,116,32,102,114,101,115,110,101,108,84,101,114,109,32,61,32,48,46,48,50,43,48,46,54,55,42,112,111,119,40,40,49,46,48,45,100,111,116,40,101,121,101,86,101,99,116,111,114,44,32,98,117,109,112,86,101,99,116,111,114,41,41,44,53,46,48,41,59,10,102,114,101,115,110,101,108,84,101,114,109,61,99,108,97,109,112,40,102,114,101,115,110,101,108,84,101,114,109,44,48,46,48,44,49,46,48,41,59,10,102,108,111,97,116,52,32,114,101,102,108,101,99,116,67,111,108,111,114,32,61,32,114,101,102,108,101,99,116,84,101,120,46,83,97,109,112,108,101,40,114,101,102,108,101,99,116,83,97,109,112,44,40,112,105,110,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,120,121,32,47,32,112,105,110,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,122,41,32,43,32,112,101,114,116,117,114,98,97,116,105,111,110,41,59,10,102,108,111,97,116,52,32,114,101,102,114,97,99,116,67,111,108,111,114,32,61,32,114,101,102,114,97,99,116,84,101,120,46,83,97,109,112,108,101,40,114,101,102,114,97,99,116,83,97,109,112,44,40,112,105,110,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,120,121,32,47,32,112,105,110,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,122,41,32,43,32,112,101,114,116,117,114,98,97,116,105,111,110,41,59,10,114,101,102,114,97,99,116,67,111,108,111,114,61,108,101,114,112,40,114,101,102,108,101,99,116,67,111,108,111,114,44,114,101,102,114,97,99,116,67,111,108,111,114,44,112,105,110,46,112,111,115,105,116,105,111,110,70,117,108,108,46,119,41,59,10,102,108,111,97,116,52,32,102,114,97,103,67,111,108,111,114,61,108,101,114,112,40,114,101,102,108,101,99,116,67,111,108,111,114,44,114,101,102,114,97,99,116,67,111,108,111,114,44,102,114,101,115,110,101,108,84,101,114,109,41,59,10,102,108,111,97,116,52,32,100,117,108,108,67,111,108,111,114,61,102,108,111,97,116,52,40,48,46,49,44,48,46,49,44,48,46,50,44,49,46,48,41,59,10,102,108,111,97,116,32,100,117,108,108,66,108,101,110,100,70,97,99,116,111,114,61,48,46,50,59,10,102,114,97,103,67,111,108,111,114,61,108,101,114,112,40,102,114,97,103,67,111,108,111,114,44,100,117,108,108,67,111,108,111,114,44,100,117,108,108,66,108,101,110,100,70,97,99,116,111,114,41,59,10,102,108,111,97,116,32,115,112,101,99,80,101,114,116,117,114,98,61,52,46,48,59,10,102,108,111,97,116,51,32,104,97,108,102,118,101,99,61,110,111,114,109,97,108,105,122,101,40,101,121,101,86,101,99,116,111,114,45,108,105,103,104,116,80,111,115,105,116,105,111,110,46,120,121,122,43,102,108,111,97,116,51,40,112,101,114,116,117,114,98,97,116,105,111,110,46,120,42,115,112,101,99,80,101,114,116,117,114,98,44,112,101,114,116,117,114,98,97,116,105,111,110,46,121,42,115,112,101,99,80,101,114,116,117,114,98,44,48,41,41,59,10,102,108,111,97,116,32,115,112,101,99,61,112,111,119,40,100,111,116,40,104,97,108,102,118,101,99,44,110,111,114,109,97,108,86,101,99,116,111,114,41,44,109,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,41,59,10,102,114,97,103,67,111,108,111,114,61,102,114,97,103,67,111,108,111,114,43,108,101,114,112,40,48,46,48,44,108,105,103,104,116,83,112,101,99,117,108,97,114,42,109,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,44,115,112,101,99,41,59,10,102,114,97,103,67,111,108,111,114,46,119,61,49,46,48,59,10,114,101,116,117,114,110,32,108,101,114,112,40,102,111,103,67,111,108,111,114,44,102,114,97,103,67,111,108,111,114,44,112,105,110,46,102,111,103,41,59,10,125,10,0,0,0,0,0,0,0,69,110,103,105,110,101,58,32,105,110,115,116,97,108,108,105,110,103,32,104,97,110,100,108,101,114,115,0,0,0,0,0,35,105,102,32,100,101,102,105,110,101,100,40,71,76,95,69,83,41,10,112,114,101,99,105,115,105,111,110,32,109,101,100,105,117,109,112,32,102,108,111,97,116,59,10,35,101,110,100,105,102,10,118,97,114,121,105,110,103,32,102,108,111,97,116,32,102,111,103,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,98,117,109,112,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,112,111,115,105,116,105,111,110,70,117,108,108,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,102,111,103,67,111,108,111,114,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,108,105,103,104,116,80,111,115,105,116,105,111,110,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,108,105,103,104,116,83,112,101,99,117,108,97,114,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,99,97,109,101,114,97,80,111,115,105,116,105,111,110,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,119,97,118,101,72,101,105,103,104,116,59,10,117,110,105,102,111,114,109,32,118,101,99,52,32,109,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,59,10,117,110,105,102,111,114,109,32,102,108,111,97,116,32,109,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,114,101,102,108,101,99,116,84,101,120,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,114,101,102,114,97,99,116,84,101,120,59,10,117,110,105,102,111,114,109,32,115,97,109,112,108,101,114,50,68,32,119,97,118,101,84,101,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,114,101,102,108,101,99,116,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,114,101,102,114,97,99,116,77,97,116,114,105,120,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,118,101,99,51,32,110,111,114,109,97,108,86,101,99,116,111,114,61,118,101,99,51,40,48,44,48,44,49,46,48,41,59,10,118,101,99,52,32,98,117,109,112,67,111,108,111,114,61,116,101,120,116,117,114,101,50,68,40,119,97,118,101,84,101,120,44,98,117,109,112,80,111,115,105,116,105,111,110,41,59,10,118,101,99,51,32,98,117,109,112,86,101,99,116,111,114,61,40,98,117,109,112,67,111,108,111,114,46,120,121,122,45,48,46,53,41,42,50,46,48,59,10,118,101,99,50,32,112,101,114,116,117,114,98,97,116,105,111,110,61,119,97,118,101,72,101,105,103,104,116,42,98,117,109,112,86,101,99,116,111,114,46,120,121,59,10,118,101,99,51,32,101,121,101,86,101,99,116,111,114,32,61,32,110,111,114,109,97,108,105,122,101,40,99,97,109,101,114,97,80,111,115,105,116,105,111,110,46,120,121,122,32,45,32,112,111,115,105,116,105,111,110,70,117,108,108,46,120,121,122,41,59,10,102,108,111,97,116,32,102,114,101,115,110,101,108,84,101,114,109,32,61,32,48,46,48,50,43,48,46,54,55,42,112,111,119,40,40,49,46,48,45,100,111,116,40,101,121,101,86,101,99,116,111,114,44,32,98,117,109,112,86,101,99,116,111,114,41,41,44,53,46,48,41,59,10,102,114,101,115,110,101,108,84,101,114,109,61,99,108,97,109,112,40,102,114,101,115,110,101,108,84,101,114,109,44,48,46,48,44,49,46,48,41,59,10,118,101,99,52,32,114,101,102,108,101,99,116,67,111,108,111,114,32,61,32,116,101,120,116,117,114,101,50,68,40,114,101,102,108,101,99,116,84,101,120,44,109,111,100,40,40,114,101,102,108,101,99,116,77,97,116,114,105,120,32,42,32,118,101,99,52,40,40,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,120,121,32,47,32,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,122,41,32,43,32,112,101,114,116,117,114,98,97,116,105,111,110,44,48,44,48,41,41,46,120,121,44,49,46,48,41,41,59,10,118,101,99,52,32,114,101,102,114,97,99,116,67,111,108,111,114,32,61,32,116,101,120,116,117,114,101,50,68,40,114,101,102,114,97,99,116,84,101,120,44,109,111,100,40,40,114,101,102,114,97,99,116,77,97,116,114,105,120,32,42,32,118,101,99,52,40,40,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,120,121,32,47,32,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,122,41,32,43,32,112,101,114,116,117,114,98,97,116,105,111,110,44,48,44,48,41,41,46,120,121,44,49,46,48,41,41,59,10,114,101,102,114,97,99,116,67,111,108,111,114,61,109,105,120,40,114,101,102,108,101,99,116,67,111,108,111,114,44,114,101,102,114,97,99,116,67,111,108,111,114,44,112,111,115,105,116,105,111,110,70,117,108,108,46,119,41,59,10,118,101,99,52,32,102,114,97,103,67,111,108,111,114,61,109,105,120,40,114,101,102,108,101,99,116,67,111,108,111,114,44,114,101,102,114,97,99,116,67,111,108,111,114,44,102,114,101,115,110,101,108,84,101,114,109,41,59,10,118,101,99,52,32,100,117,108,108,67,111,108,111,114,61,118,101,99,52,40,48,46,49,44,48,46,49,44,48,46,50,44,49,46,48,41,59,10,102,108,111,97,116,32,100,117,108,108,66,108,101,110,100,70,97,99,116,111,114,61,48,46,50,59,10,102,114,97,103,67,111,108,111,114,61,109,105,120,40,102,114,97,103,67,111,108,111,114,44,100,117,108,108,67,111,108,111,114,44,100,117,108,108,66,108,101,110,100,70,97,99,116,111,114,41,59,10,102,108,111,97,116,32,115,112,101,99,80,101,114,116,117,114,98,61,52,46,48,59,10,118,101,99,51,32,104,97,108,102,118,101,99,61,110,111,114,109,97,108,105,122,101,40,101,121,101,86,101,99,116,111,114,45,108,105,103,104,116,80,111,115,105,116,105,111,110,46,120,121,122,43,118,101,99,51,40,112,101,114,116,117,114,98,97,116,105,111,110,46,120,42,115,112,101,99,80,101,114,116,117,114,98,44,112,101,114,116,117,114,98,97,116,105,111,110,46,121,42,115,112,101,99,80,101,114,116,117,114,98,44,48,41,41,59,10,102,108,111,97,116,32,115,112,101,99,61,112,111,119,40,100,111,116,40,104,97,108,102,118,101,99,44,110,111,114,109,97,108,86,101,99,116,111,114,41,44,109,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,41,59,10,102,114,97,103,67,111,108,111,114,61,102,114,97,103,67,111,108,111,114,43,109,105,120,40,118,101,99,52,40,48,41,44,108,105,103,104,116,83,112,101,99,117,108,97,114,42,109,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,44,115,112,101,99,41,59,10,102,114,97,103,67,111,108,111,114,46,119,61,49,46,48,59,10,103,108,95,70,114,97,103,67,111,108,111,114,61,109,105,120,40,102,111,103,67,111,108,111,114,44,102,114,97,103,67,111,108,111,114,44,102,111,103,41,59,10,125,10,0,0,0,0,0,0,0,0,115,101,116,78,97,109,101,0,69,110,103,105,110,101,58,58,100,101,115,116,114,111,121,0,115,116,114,117,99,116,32,86,73,78,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,51,32,110,111,114,109,97,108,32,58,32,78,79,82,77,65,76,59,10,102,108,111,97,116,50,32,116,101,120,67,111,111,114,100,58,32,84,69,88,67,79,79,82,68,48,59,10,125,59,10,115,116,114,117,99,116,32,86,79,85,84,123,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,32,58,32,83,86,95,80,79,83,73,84,73,79,78,59,10,102,108,111,97,116,32,102,111,103,58,32,70,79,71,59,10,102,108,111,97,116,52,32,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,48,59,10,102,108,111,97,116,52,32,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,49,59,10,102,108,111,97,116,50,32,98,117,109,112,80,111,115,105,116,105,111,110,58,32,84,69,88,67,79,79,82,68,50,59,10,102,108,111,97,116,52,32,112,111,115,105,116,105,111,110,70,117,108,108,58,32,84,69,88,67,79,79,82,68,51,59,10,125,59,10,102,108,111,97,116,52,120,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,102,108,111,97,116,52,120,52,32,109,111,100,101,108,77,97,116,114,105,120,59,10,102,108,111,97,116,52,120,52,32,119,97,118,101,77,97,116,114,105,120,59,10,102,108,111,97,116,50,32,102,111,103,68,105,115,116,97,110,99,101,59,10,86,79,85,84,32,109,97,105,110,40,86,73,78,32,118,105,110,41,123,10,86,79,85,84,32,118,111,117,116,59,10,118,111,117,116,46,112,111,115,105,116,105,111,110,61,109,117,108,40,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,44,118,105,110,46,112,111,115,105,116,105,111,110,41,59,10,118,111,117,116,46,112,111,115,105,116,105,111,110,70,117,108,108,61,109,117,108,40,109,111,100,101,108,77,97,116,114,105,120,44,118,105,110,46,112,111,115,105,116,105,111,110,41,59,10,118,111,117,116,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,120,32,61,32,48,46,53,32,42,32,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,32,45,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,120,41,59,10,118,111,117,116,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,121,32,61,32,48,46,53,32,42,32,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,32,45,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,121,41,59,10,118,111,117,116,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,122,32,61,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,59,10,118,111,117,116,46,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,119,61,49,46,48,59,10,118,111,117,116,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,120,32,61,32,48,46,53,32,42,32,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,32,43,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,120,41,59,10,118,111,117,116,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,121,32,61,32,48,46,53,32,42,32,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,32,45,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,121,41,59,10,118,111,117,116,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,122,32,61,32,118,111,117,116,46,112,111,115,105,116,105,111,110,46,119,59,10,118,111,117,116,46,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,119,61,49,46,48,59,10,118,111,117,116,46,98,117,109,112,80,111,115,105,116,105,111,110,61,109,117,108,40,119,97,118,101,77,97,116,114,105,120,44,102,108,111,97,116,52,40,118,105,110,46,116,101,120,67,111,111,114,100,44,48,46,48,44,49,46,48,41,41,59,10,32,118,111,117,116,46,102,111,103,61,99,108,97,109,112,40,49,46,48,45,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,44,48,46,48,44,49,46,48,41,59,10,118,111,117,116,46,112,111,115,105,116,105,111,110,70,117,108,108,46,119,61,99,108,97,109,112,40,49,46,48,45,40,118,111,117,116,46,112,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,47,52,46,48,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,47,52,46,48,45,102,111,103,68,105,115,116,97,110,99,101,46,120,47,52,46,48,41,44,48,46,48,44,49,46,48,41,59,10,114,101,116,117,114,110,32,118,111,117,116,59,10,125,10,0,0,0,0,0,66,97,115,101,67,111,109,112,111,110,101,110,116,0,0,0,100,111,117,98,108,101,0,0,69,110,103,105,110,101,58,58,126,69,110,103,105,110,101,0,97,116,116,114,105,98,117,116,101,32,118,101,99,52,32,80,79,83,73,84,73,79,78,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,51,32,78,79,82,77,65,76,59,10,97,116,116,114,105,98,117,116,101,32,118,101,99,50,32,84,69,88,67,79,79,82,68,48,59,10,118,97,114,121,105,110,103,32,102,108,111,97,116,32,102,111,103,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,50,32,98,117,109,112,80,111,115,105,116,105,111,110,59,10,118,97,114,121,105,110,103,32,118,101,99,52,32,112,111,115,105,116,105,111,110,70,117,108,108,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,109,111,100,101,108,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,109,97,116,52,32,119,97,118,101,77,97,116,114,105,120,59,10,117,110,105,102,111,114,109,32,118,101,99,50,32,102,111,103,68,105,115,116,97,110,99,101,59,10,118,111,105,100,32,109,97,105,110,40,41,123,10,103,108,95,80,111,115,105,116,105,111,110,61,109,111,100,101,108,86,105,101,119,80,114,111,106,101,99,116,105,111,110,77,97,116,114,105,120,32,42,32,80,79,83,73,84,73,79,78,59,10,112,111,115,105,116,105,111,110,70,117,108,108,61,109,111,100,101,108,77,97,116,114,105,120,32,42,32,80,79,83,73,84,73,79,78,59,10,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,120,32,61,32,48,46,53,32,42,32,40,103,108,95,80,111,115,105,116,105,111,110,46,119,32,45,32,103,108,95,80,111,115,105,116,105,111,110,46,120,41,59,10,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,121,32,61,32,48,46,53,32,42,32,40,103,108,95,80,111,115,105,116,105,111,110,46,119,32,45,32,103,108,95,80,111,115,105,116,105,111,110,46,121,41,59,10,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,122,32,61,32,103,108,95,80,111,115,105,116,105,111,110,46,119,59,10,114,101,102,108,101,99,116,80,111,115,105,116,105,111,110,46,119,32,61,32,49,46,48,59,10,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,120,32,61,32,48,46,53,32,42,32,40,103,108,95,80,111,115,105,116,105,111,110,46,119,32,43,32,103,108,95,80,111,115,105,116,105,111,110,46,120,41,59,10,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,121,32,61,32,48,46,53,32,42,32,40,103,108,95,80,111,115,105,116,105,111,110,46,119,32,45,32,103,108,95,80,111,115,105,116,105,111,110,46,121,41,59,10,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,122,32,61,32,103,108,95,80,111,115,105,116,105,111,110,46,119,59,10,114,101,102,114,97,99,116,80,111,115,105,116,105,111,110,46,119,32,61,32,49,46,48,59,10,98,117,109,112,80,111,115,105,116,105,111,110,61,40,119,97,118,101,77,97,116,114,105,120,32,42,32,118,101,99,52,40,84,69,88,67,79,79,82,68,48,44,48,46,48,44,49,46,48,41,41,46,120,121,59,10,102,111,103,61,99,108,97,109,112,40,49,46,48,45,40,103,108,95,80,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,45,102,111,103,68,105,115,116,97,110,99,101,46,120,41,44,48,46,48,44,49,46,48,41,59,10,112,111,115,105,116,105,111,110,70,117,108,108,46,119,61,99,108,97,109,112,40,49,46,48,45,40,103,108,95,80,111,115,105,116,105,111,110,46,122,45,102,111,103,68,105,115,116,97,110,99,101,46,120,47,52,46,48,41,47,40,102,111,103,68,105,115,116,97,110,99,101,46,121,47,52,46,48,45,102,111,103,68,105,115,116,97,110,99,101,46,120,47,52,46,48,41,44,48,46,48,44,49,46,48,41,59,10,125,10,0,0,0,115,101,116,77,101,115,104,87,105,116,104,77,101,115,104,0,77,97,116,101,114,105,97,108,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,68,101,97,99,116,105,118,97,116,101,0,0,0,0,0,0,46,69,110,103,105,110,101,58,0,0,0,0,0,0,0,0,104,108,115,108,0,0,0,0,115,101,116,77,101,115,104,87,105,116,104,78,97,109,101,0,83,107,101,108,101,116,111,110,67,111,109,112,111,110,101,110,116,0,0,0,0,0,0,0,115,107,105,112,112,105,110,103,32,101,120,99,101,115,115,105,118,101,32,100,116,58,0,0,77,97,116,101,114,105,97,108,77,97,110,97,103,101,114,58,58,99,111,110,116,101,120,116,65,99,116,105,118,97,116,101,0,0,0,0,0,0,0,0,97,108,108,111,99,97,116,105,110,103,32,0,0,0,0,0,103,108,115,108,0,0,0,0,103,101,116,67,97,109,101,114,97,0,0,0,0,0,0,0,103,101,116,71,76,77,97,103,70,105,108,116,101,114,58,32,73,110,118,97,108,105,100,32,102,105,108,116,101,114,0,0,115,101,116,76,111,111,107,65,116,0,0,0,0,0,0,0,103,101,116,71,76,77,105,110,70,105,108,116,101,114,58,32,73,110,118,97,108,105,100,32,102,105,108,116,101,114,0,0,103,101,116,71,76,87,114,97,112,58,32,73,110,118,97,108,105,100,32,97,100,100,114,101,115,115,0,0,0,0,0,0,103,101,116,78,97,109,101,0,98,111,111,108,0,0,0,0,103,101,116,71,76,73,110,100,101,120,84,121,112,101,58,32,73,110,118,97,108,105,100,32,116,121,112,101,0,0,0,0,67,111,109,112,111,110,101,110,116,0,0,0,0,0,0,0,103,101,116,71,76,68,97,116,97,84,121,112,101,58,32,73,110,118,97,108,105,100,32,100,97,116,97,32,116,121,112,101,0,0,0,0,0,0,0,0,103,101,116,86,105,101,119,112,111,114,116,0,0,0,0,0,103,101,116,71,76,67,117,108,108,70,97,99,101,58,32,73,110,118,97,108,105,100,32,99,117,108,108,32,116,121,112,101,0,0,0,0,0,0,0,0,115,101,116,86,105,101,119,112,111,114,116,0,0,0,0,0,103,101,116,71,76,66,108,101,110,100,79,112,101,114,97,116,105,111,110,58,32,73,110,118,97,108,105,100,32,98,108,101,110,100,32,111,112,101,114,97,116,105,111,110,0,0,0,0,115,101,116,67,108,101,97,114,67,111,108,111,114,0,0,0,67,97,109,101,114,97,0,0,102,108,111,97,116,0,0,0,103,101,116,71,76,68,101,112,116,104,70,117,110,99,58,32,73,110,118,97,108,105,100,32,100,101,112,116,104,32,116,101,115,116,0,0,0,0,0,0,115,101,116,82,111,116,97,116,101,0,0,0,0,0,0,0,103,101,116,82,111,116,97,116,101,0,0,0,0,0,0,0,116,101,120,116,117,114,101,83,101,116,0,0,0,0,0,0,99,111,112,121,80,105,120,101,108,66,117,102,102,101,114,32,110,111,116,32,105,109,112,108,101,109,101,110,116,101,100,32,102,111,114,32,116,104,101,115,101,32,112,105,120,101,108,32,98,117,102,102,101,114,32,116,121,112,101,115,0,0,0,0,115,101,116,83,99,97,108,101,0,0,0,0,0,0,0,0,71,76,82,101,110,100,101,114,68,101,118,105,99,101,58,58,99,111,112,121,84,111,83,117,114,102,97,99,101,58,32,117,110,105,109,112,108,101,109,101,110,116,101,100,32,102,111,114,32,112,98,111,0,0,0,0,103,101,116,83,99,97,108,101,0,0,0,0,0,0,0,0,71,76,82,101,110,100,101,114,68,101,118,105,99,101,58,58,99,111,112,121,84,111,83,117,114,102,97,99,101,58,32,117,110,105,109,112,108,101,109,101,110,116,101,100,32,102,111,114,32,102,98,111,0,0,0,0,115,101,116,84,114,97,110,115,108,97,116,101,0,0,0,0,86,101,114,116,101,120,68,97,116,97,32,111,114,32,73,110,100,101,120,68,97,116,97,32,105,115,32,78,85,76,76,0,103,101,116,84,114,97,110,115,108,97,116,101,0,0,0,0,115,101,116,86,105,101,119,112,111,114,116,32,99,97,108,108,101,100,32,119,105,116,104,111,117,116,32,97,32,118,97,108,105,100,32,114,101,110,100,101,114,32,116,97,114,103,101,116,0,0,0,0,0,0,0,0,84,114,97,110,115,102,111,114,109,0,0,0,0,0,0,0,102,111,103,68,101,110,115,105,116,121,0,0,0,0,0,0,101,120,116,101,110,115,105,111,110,32,110,111,116,32,102,111,117,110,100,32,111,110,32,102,105,108,101,0,0,0,0,0,83,104,97,100,101,114,83,116,97,116,101,115,32,97,114,101,32,117,110,97,118,97,105,108,97,98,108,101,0,0,0,0,114,101,110,100,101,114,0,0,102,105,108,101,32,0,0,0,117,110,115,105,103,110,101,100,32,108,111,110,103,0,0,0,81,117,101,114,105,101,115,32,97,114,101,32,117,110,97,118,97,105,108,97,98,108,101,0,117,112,100,97,116,101,0,0,109,97,116,101,114,105,97,108,65,109,98,105,101,110,116,67,111,108,111,114,0,0,0,0,115,116,114,101,97,109,32,108,101,110,103,116,104,32,116,111,111,32,108,97,114,103,101,44,32,105,110,99,114,101,97,115,101,32,82,101,115,111,117,114,99,101,77,97,110,97,103,101,114,32,109,97,120,32,115,116,114,101,97,109,32,108,101,110,103,116,104,32,111,114,32,114,101,100,117,99,101,32,114,101,115,111,117,114,99,101,32,115,105,122,101,0,0,0,0,0,83,104,97,100,101,114,115,32,97,114,101,32,117,110,97,118,97,105,108,97,98,108,101,0,103,101,116,82,111,111,116,0,37,117,0,0,0,0,0,0,7,0,0,0,8,0,0,0,9,0,0,0,10,0,0,0,11,0,0,0,12,0,0,0,13,0,0,0,14,0,0,0,16,0,0,0,17,0,0,0,19,0,0,0,21,0,0,0,23,0,0,0,25,0,0,0,28,0,0,0,31,0,0,0,34,0,0,0,37,0,0,0,41,0,0,0,45,0,0,0,50,0,0,0,55,0,0,0,60,0,0,0,66,0,0,0,73,0,0,0,80,0,0,0,88,0,0,0,97,0,0,0,107,0,0,0,118,0,0,0,130,0,0,0,143,0,0,0,157,0,0,0,173,0,0,0,190,0,0,0,209,0,0,0,230,0,0,0,253,0,0,0,23,1,0,0,51,1,0,0,81,1,0,0,115,1,0,0,152,1,0,0,193,1,0,0,238,1,0,0,32,2,0,0,86,2,0,0,146,2,0,0,212,2,0,0,28,3,0,0,108,3,0,0,195,3,0,0,36,4,0,0,142,4,0,0,2,5,0,0,131,5,0,0,16,6,0,0,171,6,0,0,86,7,0,0,18,8,0,0,224,8,0,0,195,9,0,0,189,10,0,0,208,11,0,0,255,12,0,0,76,14,0,0,186,15,0,0,76,17,0,0,7,19,0,0,238,20,0,0,6,23,0,0,84,25,0,0,220,27,0,0,165,30,0,0,182,33,0,0,21,37,0,0,202,40,0,0,223,44,0,0,91,49,0,0,75,54,0,0,185,59,0,0,178,65,0,0,68,72,0,0,126,79,0,0,113,87,0,0,47,96,0,0,206,105,0,0,98,116,0,0,255,127,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,2,0,0,0,4,0,0,0,6,0,0,0,8,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,2,0,0,0,4,0,0,0,6,0,0,0,8,0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,182,0,0,16,18,0,0,20,14,0,0,254,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,160,185,0,0,78,4,0,0,30,11,0,0,50,4,0,0,142,4,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,185,0,0,134,7,0,0,240,3,0,0,92,6,0,0,242,22,0,0,204,18,0,0,148,0,0,0,118,16,0,0,124,16,0,0,28,11,0,0,90,2,0,0,24,7,0,0,220,19,0,0,244,255,255,255,192,185,0,0,108,2,0,0,164,6,0,0,176,3,0,0,68,2,0,0,58,19,0,0,78,17,0,0,160,2,0,0,86,14,0,0,168,16,0,0,82,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,208,185,0,0,128,20,0,0,66,11,0,0,132,6,0,0,178,7,0,0,78,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,185,0,0,24,17,0,0,106,18,0,0,110,4,0,0,178,7,0,0,172,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,185,0,0,202,3,0,0,2,18,0,0,96,0,0,0,178,7,0,0,74,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,186,0,0,46,19,0,0,10,15,0,0,136,1,0,0,178,7,0,0,170,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,186,0,0,196,3,0,0,44,0,0,0,118,11,0,0,178,7,0,0,226,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,186,0,0,92,9,0,0,88,18,0,0,12,16,0,0,178,7,0,0,210,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,186,0,0,64,18,0,0,48,4,0,0,194,9,0,0,178,7,0,0,120,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,186,0,0,146,4,0,0,158,18,0,0,16,7,0,0,98,14,0,0,206,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,186,0,0,58,14,0,0,104,0,0,0,246,20,0,0,178,7,0,0,164,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,186,0,0,110,1,0,0,144,8,0,0,248,18,0,0,178,7,0,0,184,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,186,0,0,66,14,0,0,70,6,0,0,98,7,0,0,178,7,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,186,0,0,194,19,0,0,108,3,0,0,54,11,0,0,178,7,0,0,196,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,186,0,0,216,6,0,0,120,13,0,0,152,2,0,0,178,7,0,0,172,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,160,186,0,0,8,14,0,0,246,17,0,0,222,12,0,0,178,7,0,0,250,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,176,186,0,0,0,14,0,0,224,7,0,0,228,10,0,0,178,7,0,0,12,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,186,0,0,128,19,0,0,2,2,0,0,154,9,0,0,118,0,0,0,94,17,0,0,0,0,0,0])
.concat([0,0,0,0,0,0,0,0,0,0,0,0,208,186,0,0,114,15,0,0,130,19,0,0,146,3,0,0,178,7,0,0,16,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,186,0,0,38,2,0,0,66,8,0,0,210,8,0,0,178,7,0,0,128,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,186,0,0,176,12,0,0,112,1,0,0,246,13,0,0,178,7,0,0,98,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,187,0,0,44,13,0,0,68,20,0,0,228,6,0,0,60,15,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,187,0,0,38,17,0,0,26,21,0,0,136,10,0,0,178,7,0,0,40,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,187,0,0,94,22,0,0,90,5,0,0,28,3,0,0,178,7,0,0,142,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,187,0,0,92,14,0,0,230,2,0,0,78,8,0,0,146,6,0,0,210,2,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,187,0,0,202,1,0,0,178,2,0,0,8,17,0,0,136,17,0,0,156,18,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,187,0,0,178,18,0,0,102,12,0,0,12,9,0,0,216,13,0,0,210,0,0,0,148,0,0,0,36,13,0,0,184,22,0,0,150,8,0,0,24,19,0,0,12,11,0,0,170,9,0,0,244,255,255,255,88,187,0,0,36,5,0,0,78,9,0,0,52,18,0,0,88,15,0,0,96,3,0,0,24,9,0,0,238,9,0,0,98,3,0,0,16,10,0,0,42,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,187,0,0,188,9,0,0,62,20,0,0,150,3,0,0,236,12,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,187,0,0,84,8,0,0,86,17,0,0,32,19,0,0,8,20,0,0,6,19,0,0,148,0,0,0,242,0,0,0,202,18,0,0,106,22,0,0,198,22,0,0,252,1,0,0,208,10,0,0,244,255,255,255,120,187,0,0,170,1,0,0,10,12,0,0,236,19,0,0,246,15,0,0,74,4,0,0,80,12,0,0,44,8,0,0,88,0,0,0,124,3,0,0,118,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,187,0,0,100,11,0,0,232,1,0,0,144,15,0,0,144,15,0,0,254,9,0,0,180,7,0,0,64,21,0,0,168,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,187,0,0,104,8,0,0,12,23,0,0,10,14,0,0,152,11,0,0,34,13,0,0,148,0,0,0,50,3,0,0,208,13,0,0,244,255,255,255,168,187,0,0,100,6,0,0,86,10,0,0,184,15,0,0,138,2,0,0,90,12,0,0,210,15,0,0,64,21,0,0,168,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,187,0,0,120,17,0,0,66,0,0,0,48,21,0,0,100,9,0,0,34,13,0,0,148,0,0,0,242,21,0,0,24,1,0,0,120,11,0,0,244,255,255,255,200,187,0,0,100,13,0,0,112,4,0,0,96,16,0,0,76,0,0,0,100,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,187,0,0,124,7,0,0,198,3,0,0,106,4,0,0,28,6,0,0,34,13,0,0,148,0,0,0,242,6,0,0,210,18,0,0,190,10,0,0,56,3,0,0,104,10,0,0,208,20,0,0,166,0,0,0,216,16,0,0,42,0,0,0,232,7,0,0,244,255,255,255,248,187,0,0,20,11,0,0,76,7,0,0,52,13,0,0,156,19,0,0,248,15,0,0,104,20,0,0,54,18,0,0,58,12,0,0,186,17,0,0,134,1,0,0,104,6,0,0,232,5,0,0,240,6,0,0,164,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,188,0,0,98,11,0,0,26,3,0,0,38,8,0,0,56,2,0,0,16,6,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,188,0,0,72,8,0,0,4,14,0,0,206,3,0,0,0,1,0,0,34,13,0,0,148,0,0,0,124,8,0,0,212,21,0,0,62,2,0,0,176,13,0,0,230,0,0,0,98,16,0,0,42,1,0,0,38,18,0,0,56,20,0,0,28,12,0,0,72,19,0,0,6,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,188,0,0,154,13,0,0,204,9,0,0,34,11,0,0,168,11,0,0,118,12,0,0,148,0,0,0,6,16,0,0,254,5,0,0,182,19,0,0,228,14,0,0,164,13,0,0,48,17,0,0,66,10,0,0,100,1,0,0,158,2,0,0,216,22,0,0,50,21,0,0,242,16,0,0,238,6,0,0,244,255,255,255,56,188,0,0,4,3,0,0,106,9,0,0,116,11,0,0,220,5,0,0,198,9,0,0,22,15,0,0,108,11,0,0,230,22,0,0,218,2,0,0,66,7,0,0,220,255,255,255,56,188,0,0,56,17,0,0,220,9,0,0,78,14,0,0,210,3,0,0,238,16,0,0,158,15,0,0,136,5,0,0,144,22,0,0,10,16,0,0,188,16,0,0,186,0,0,0,198,12,0,0,18,23,0,0,178,5,0,0,158,13,0,0,228,16,0,0,78,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,188,0,0,160,18,0,0,246,3,0,0,132,20,0,0,90,6,0,0,146,2,0,0,148,0,0,0,144,14,0,0,136,8,0,0,154,14,0,0,128,18,0,0,184,7,0,0,138,4,0,0,110,21,0,0,24,12,0,0,10,17,0,0,252,0,0,0,110,3,0,0,18,7,0,0,20,18,0,0,80,14,0,0,244,11,0,0,162,7,0,0,8,18,0,0,132,12,0,0,58,18,0,0,42,18,0,0,96,12,0,0,200,4,0,0,254,17,0,0,86,12,0,0,54,22,0,0,14,23,0,0,26,9,0,0,154,12,0,0,18,13,0,0,200,7,0,0,80,7,0,0,248,12,0,0,218,17,0,0,92,3,0,0,254,10,0,0,190,16,0,0,66,12,0,0,176,14,0,0,154,2,0,0,152,5,0,0,72,15,0,0,244,255,255,255,88,188,0,0,248,9,0,0,178,17,0,0,142,21,0,0,26,1,0,0,12,14,0,0,152,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,188,0,0,120,18,0,0,50,2,0,0,18,6,0,0,24,10,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,188,0,0,188,5,0,0,62,7,0,0,142,16,0,0,14,21,0,0,192,1,0,0,148,0,0,0,158,11,0,0,220,11,0,0,218,1,0,0,146,8,0,0,80,16,0,0,164,14,0,0,26,11,0,0,232,18,0,0,194,14,0,0,8,9,0,0,36,17,0,0,170,16,0,0,46,7,0,0,170,2,0,0,222,4,0,0,170,3,0,0,192,9,0,0,28,5,0,0,60,0,0,0,90,19,0,0,212,7,0,0,78,2,0,0,198,19,0,0,40,22,0,0,32,13,0,0,38,21,0,0,74,6,0,0,4,10,0,0,216,17,0,0,88,17,0,0,72,3,0,0,190,22,0,0,120,4,0,0,252,10,0,0,132,3,0,0,106,3,0,0,202,21,0,0,80,13,0,0,106,17,0,0,128,13,0,0,20,0,0,0,16,19,0,0,246,19,0,0,54,20,0,0,236,22,0,0,242,1,0,0,4,1,0,0,126,18,0,0,114,10,0,0,56,18,0,0,8,16,0,0,180,3,0,0,154,10,0,0,94,14,0,0,150,2,0,0,196,2,0,0,186,16,0,0,2,4,0,0,78,15,0,0,76,9,0,0,58,22,0,0,218,14,0,0,236,16,0,0,126,12,0,0,120,19,0,0,250,5,0,0,86,7,0,0,186,2,0,0,8,8,0,0,58,8,0,0,154,11,0,0,148,10,0,0,174,13,0,0,244,255,255,255,136,188,0,0,60,22,0,0,76,12,0,0,194,11,0,0,194,10,0,0,28,10,0,0,164,0,0,0,130,3,0,0,52,16,0,0,82,3,0,0,50,7,0,0,58,13,0,0,168,5,0,0,64,9,0,0,220,255,255,255,136,188,0,0,84,0,0,0,192,6,0,0,114,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,188,0,0,126,20,0,0,80,3,0,0,44,6,0,0,190,7,0,0,98,4,0,0,148,0,0,0,158,3,0,0,242,10,0,0,244,1,0,0,40,3,0,0,62,0,0,0,0,23,0,0,90,22,0,0,70,14,0,0,72,22,0,0,200,15,0,0,162,8,0,0,14,18,0,0,162,5,0,0,244,255,255,255,168,188,0,0,86,5,0,0,70,1,0,0,226,0,0,0,122,3,0,0,36,19,0,0,250,11,0,0,172,19,0,0,102,3,0,0,252,5,0,0,252,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,188,0,0,102,4,0,0,16,13,0,0,226,1,0,0,250,15,0,0,44,2,0,0,148,0,0,0,124,17,0,0,88,11,0,0,26,0,0,0,122,11,0,0,204,16,0,0,36,22,0,0,244,255,255,255,184,188,0,0,210,1,0,0,36,9,0,0,164,3,0,0,84,12,0,0,38,10,0,0,250,3,0,0,10,20,0,0,196,4,0,0,88,4,0,0,152,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,188,0,0,150,13,0,0,168,22,0,0,42,17,0,0,196,19,0,0,202,2,0,0,148,0,0,0,146,1,0,0,244,255,255,255,200,188,0,0,168,4,0,0,190,21,0,0,136,11,0,0,26,16,0,0,12,8,0,0,166,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,232,188,0,0,208,18,0,0,204,21,0,0,22,4,0,0,98,15,0,0,248,2,0,0,148,0,0,0,132,15,0,0,244,255,255,255,232,188,0,0,86,15,0,0,114,9,0,0,196,14,0,0,214,22,0,0,76,1,0,0,200,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,189,0,0,128,2,0,0,188,21,0,0,140,10,0,0,250,4,0,0,214,10,0,0,148,0,0,0,142,9,0,0,244,255,255,255,8,189,0,0,254,14,0,0,12,19,0,0,84,3,0,0,36,20,0,0,180,2,0,0,230,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,189,0,0,40,10,0,0,42,16,0,0,112,20,0,0,16,9,0,0,64,23,0,0,148,0,0,0,0,8,0,0,244,255,255,255,48,189,0,0,200,19,0,0,124,13,0,0,16,14,0,0,162,4,0,0,70,17,0,0,16,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,189,0,0,154,21,0,0,12,2,0,0,78,16,0,0,230,3,0,0,34,13,0,0,148,0,0,0,252,12,0,0,30,19,0,0,190,3,0,0,234,6,0,0,62,22,0,0,238,1,0,0,234,12,0,0,16,22,0,0,234,21,0,0,244,255,255,255,80,189,0,0,112,2,0,0,154,6,0,0,228,20,0,0,192,11,0,0,70,9,0,0,96,8,0,0,100,17,0,0,76,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,189,0,0,182,15,0,0,214,19,0,0,212,2,0,0,220,14,0,0,224,20,0,0,148,0,0,0,236,10,0,0,244,255,255,255,112,189,0,0,124,9,0,0,212,20,0,0,44,10,0,0,200,20,0,0,170,7,0,0,222,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,189,0,0,68,5,0,0,74,11,0,0,60,20,0,0,138,13,0,0,168,17,0,0,148,0,0,0,94,6,0,0,244,255,255,255,152,189,0,0,130,4,0,0,226,20,0,0,100,19,0,0,230,14,0,0,236,17,0,0,136,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,189,0,0,90,20,0,0,208,4,0,0,220,3,0,0,76,14,0,0,166,4,0,0,148,0,0,0,186,18,0,0,244,255,255,255,184,189,0,0,212,14,0,0,182,3,0,0,174,15,0,0,246,21,0,0,208,5,0,0,82,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,216,189,0,0,126,22,0,0,124,18,0,0,220,13,0,0,108,20,0,0,46,3,0,0,148,0,0,0,180,13,0,0,220,11,0,0,218,1,0,0,146,8,0,0,112,3,0,0,230,6,0,0,216,1,0,0,220,10,0,0,194,14,0,0,70,0,0,0,34,3,0,0,204,12,0,0,64,19,0,0,178,19,0,0,186,3,0,0,90,0,0,0,148,18,0,0,232,20,0,0,232,13,0,0,56,10,0,0,160,9,0,0,248,8,0,0,28,13,0,0,170,5,0,0,70,11,0,0,122,13,0,0,138,10,0,0,220,18,0,0,72,12,0,0,244,255,255,255,216,189,0,0,14,3,0,0,234,22,0,0,30,13,0,0,0,22,0,0,216,20,0,0,178,0,0,0,130,3,0,0,190,14,0,0,8,7,0,0,230,12,0,0,188,19,0,0,122,4,0,0,204,1,0,0,220,255,255,255,216,189,0,0,28,2,0,0,174,9,0,0,212,16,0,0,216,255,255,255,216,189,0,0,156,20,0,0,194,6,0,0,16,17,0,0,200,12,0,0,188,3,0,0,124,20,0,0,234,10,0,0,212,255,255,255,216,189,0,0,76,21,0,0,36,4,0,0,74,0,0,0,240,14,0,0,48,11,0,0,178,10,0,0,208,255,255,255,216,189,0,0,50,23,0,0,30,7,0,0,206,4,0,0,16,20,0,0,148,8,0,0,86,20,0,0,204,255,255,255,216,189,0,0,146,7,0,0,90,13,0,0,160,21,0,0,242,5,0,0,20,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,190,0,0,182,7,0,0,88,1,0,0,30,5,0,0,242,19,0,0,34,13,0,0,148,0,0,0,74,7,0,0,42,4,0,0,184,3,0,0,124,14,0,0,200,17,0,0,100,16,0,0,124,1,0,0,132,9,0,0,32,8,0,0,232,7,0,0,244,255,255,255,24,190,0,0,162,19,0,0,80,0,0,0,140,4,0,0,12,1,0,0,128,9,0,0,132,11,0,0,158,14,0,0,64,3,0,0,194,2,0,0,166,1,0,0,90,9,0,0,236,4,0,0,24,11,0,0,164,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,190,0,0,222,18,0,0,192,14,0,0,216,11,0,0,166,14,0,0,224,6,0,0,148,0,0,0,94,16,0,0,244,255,255,255,40,190,0,0,90,8,0,0,114,16,0,0,244,9,0,0,220,8,0,0,182,2,0,0,80,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,190,0,0,238,5,0,0,242,3,0,0,164,2,0,0,254,12,0,0,188,6,0,0,148,0,0,0,104,9,0,0,244,255,255,255,120,190,0,0,12,10,0,0,162,16,0,0,232,15,0,0,216,2,0,0,144,4,0,0,118,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,190,0,0,46,6,0,0,46,12,0,0,68,15,0,0,162,6,0,0,188,17,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,58,20,0,0,220,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,186,20,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,152,190,0,0,56,12,0,0,120,8,0,0,122,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,190,0,0,90,17,0,0,8,11,0,0,46,11,0,0,166,6,0,0,214,8,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,58,20,0,0,220,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,64,20,0,0,240,10,0,0,20,21,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,200,190,0,0,152,3,0,0,234,9,0,0,54,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,216,190,0,0,80,8,0,0,30,4,0,0,176,19,0,0,110,10,0,0,248,0,0,0,148,0,0,0,148,15,0,0,244,255,255,255,216,190,0,0,186,10,0,0,106,7,0,0,42,8,0,0,64,6,0,0,180,0,0,0,110,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,191,0,0,62,17,0,0,58,21,0,0,170,20,0,0,100,18,0,0,112,0,0,0,148,0,0,0,122,7,0,0,220,11,0,0,218,1,0,0,146,8,0,0,248,6,0,0,230,6,0,0,216,1,0,0,220,10,0,0,194,14,0,0,70,0,0,0,62,11,0,0,204,12,0,0,162,14,0,0,206,15,0,0,44,3,0,0,4,18,0,0,118,17,0,0,40,23,0,0,244,255,255,255,0,191,0,0,148,2,0,0,176,10,0,0,254,18,0,0,202,7,0,0,50,16,0,0,250,17,0,0,130,3,0,0,162,2,0,0,8,7,0,0,230,12,0,0,66,16,0,0,122,4,0,0,54,17,0,0,220,255,255,255,0,191,0,0,48,8,0,0,252,13,0,0,210,7,0,0,238,10,0,0,24,18,0,0,48,10,0,0,84,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,191,0,0,252,21,0,0,98,2,0,0,148,11,0,0,212,12,0,0,148,14,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,58,20,0,0,220,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,2,1,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,32,191,0,0,30,6,0,0,232,11,0,0,142,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,64,191,0,0,214,7,0,0,178,13,0,0,78,5,0,0,92,15,0,0,188,17,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,122,22,0,0,216,14,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,186,20,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,64,191,0,0,32,14,0,0,148,5,0,0,122,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,191,0,0,250,20,0,0,194,3,0,0,148,1,0,0,104,14,0,0,188,17,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,58,20,0,0,220,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,186,20,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,80,191,0,0,166,11,0,0,24,5,0,0,122,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,191,0,0,168,21,0,0,64,12,0,0,254,6,0,0,6,17,0,0,178,4,0,0,22,14,0,0,238,11,0,0,196,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,191,0,0,20,19,0,0,6,10,0,0,72,5,0,0,242,9,0,0,192,1,0,0,148,0,0,0,24,22,0,0,220,11,0,0,218,1,0,0,146,8,0,0,80,16,0,0,164,14,0,0,26,11,0,0,232,18,0,0,194,14,0,0,8,9,0,0,36,17,0,0,170,16,0,0,46,7,0,0,170,2,0,0,222,4,0,0,170,3,0,0,192,9,0,0,28,5,0,0,60,0,0,0,90,19,0,0,212,7,0,0,78,2,0,0,198,19,0,0,40,22,0,0,32,13,0,0,38,21,0,0,74,6,0,0,4,10,0,0,216,17,0,0,88,17,0,0,72,3,0,0,190,22,0,0,120,4,0,0,252,10,0,0,132,3,0,0,106,3,0,0,202,21,0,0,80,13,0,0,106,17,0,0,128,13,0,0,20,0,0,0,16,19,0,0,246,19,0,0,54,20,0,0,236,22,0,0,242,1,0,0,4,1,0,0,126,18,0,0,114,10,0,0,56,18,0,0,8,16,0,0,180,3,0,0,154,10,0,0,94,14,0,0,150,2,0,0,196,2,0,0,186,16,0,0,2,4,0,0,78,15,0,0,76,9,0,0,58,22,0,0,218,14,0,0,236,16,0,0,126,12,0,0,120,19,0,0,250,5,0,0,86,7,0,0,186,2,0,0,8,8,0,0,58,8,0,0,154,11,0,0,148,10,0,0,174,13,0,0,56,4,0,0,152,15,0,0,174,14,0,0,244,255,255,255,120,191,0,0,182,0,0,0,214,4,0,0,92,12,0,0,172,21,0,0,28,10,0,0,52,23,0,0,130,3,0,0,52,16,0,0,82,3,0,0,50,7,0,0,58,13,0,0,168,5,0,0,64,9,0,0,220,255,255,255,120,191,0,0,198,13,0,0,134,10,0,0,114,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,191,0,0,222,14,0,0,122,20,0,0,14,20,0,0,20,3,0,0,248,14,0,0,148,0,0,0,38,11,0,0,220,11,0,0,218,1,0,0,146,8,0,0,56,23,0,0,230,6,0,0,216,1,0,0,220,10,0,0,194,14,0,0,70,0,0,0,122,14,0,0,204,12,0,0,148,21,0,0,138,15,0,0,28,21,0,0,204,2,0,0,158,17,0,0,168,18,0,0,128,7,0,0,180,21,0,0,244,7,0,0,204,8,0,0,96,2,0,0,180,8,0,0,142,18,0,0,218,21,0,0,0,10,0,0,244,255,255,255,136,191,0,0,186,5,0,0,186,22,0,0,232,22,0,0,28,18,0,0,224,12,0,0,236,8,0,0,130,3,0,0,128,22,0,0,8,7,0,0,230,12,0,0,8,0,0,0,122,4,0,0,218,18,0,0,220,255,255,255,136,191,0,0,72,13,0,0,126,19,0,0,86,2,0,0,216,255,255,255,136,191,0,0,100,20,0,0,250,13,0,0,40,12,0,0,160,3,0,0,174,19,0,0,224,9,0,0,184,2,0,0,212,255,255,255,136,191,0,0,192,7,0,0,132,1,0,0,208,2,0,0,130,2,0,0,82,19,0,0,132,18,0,0,126,8,0,0,208,255,255,255,136,191,0,0,60,17,0,0,6,13,0,0,166,20,0,0,34,19,0,0,106,8,0,0,48,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,191,0,0,26,6,0,0,32,10,0,0,136,4,0,0,218,7,0,0,34,13,0,0,148,0,0,0,148,19,0,0,246,6,0,0,218,6,0,0,168,13,0,0,244,255,255,255,192,191,0,0,142,15,0,0,206,8,0,0,6,21,0,0,146,11,0,0,152,10,0,0,52,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,191,0,0,92,2,0,0,76,18,0,0,172,22,0,0,138,6,0,0,40,16,0,0,148,0,0,0,68,22,0,0,244,255,255,255,224,191,0,0,190,18,0,0,124,12,0,0,140,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,0,0,248,5,0,0,252,9,0,0,216,21,0,0,192,8,0,0,112,0,0,0,148,0,0,0,170,8,0,0,220,11,0,0,218,1,0,0,146,8,0,0,80,16,0,0,230,6,0,0,216,1,0,0,220,10,0,0,194,14,0,0,70,0,0,0,62,11,0,0,204,12,0,0,162,14,0,0,244,255,255,255,0,192,0,0,146,19,0,0,12,12,0,0,214,1,0,0,46,16,0,0,50,16,0,0,100,0,0,0,130,3,0,0,52,16,0,0,8,7,0,0,230,12,0,0,66,16,0,0,122,4,0,0,54,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,192,0,0,56,0,0,0,82,8,0,0,158,0,0,0,38,3,0,0,34,13,0,0,148,0,0,0,142,17,0,0,244,255,255,255,48,192,0,0,164,20,0,0,68,14,0,0,172,12,0,0,140,7,0,0,4,4,0,0,228,13,0,0,122,19,0,0,102,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,192,0,0,26,5,0,0,122,16,0,0,240,15,0,0,2,3,0,0,34,13,0,0,148,0,0,0,224,15,0,0,52,21,0,0,244,255,255,255,80,192,0,0,82,0,0,0,70,19,0,0,202,17,0,0,114,13,0,0,4,4,0,0,38,13,0,0,54,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,192,0,0,196,11,0,0,34,21,0,0,120,9,0,0,236,5,0,0,110,9,0,0,148,0,0,0,114,6,0,0,226,10,0,0,218,1,0,0,220,4,0,0,252,14,0,0,46,18,0,0,216,1,0,0,220,10,0,0,140,21,0,0,228,3,0,0,134,9,0,0,82,12,0,0,84,11,0,0,206,20,0,0,228,0,0,0,200,0,0,0,198,5,0,0,206,5,0,0,136,16,0,0,58,6,0,0,168,0,0,0,80,21,0,0,214,21,0,0,172,7,0,0,26,18,0,0,214,9,0,0,54,6,0,0,82,18,0,0,196,12,0,0,240,11,0,0,118,4,0,0,44,14,0,0,190,8,0,0,8,22,0,0,216,0,0,0,154,22,0,0,24,14,0,0,176,2,0,0,108,9,0,0,8,5,0,0,220,0,0,0,162,9,0,0,244,255,255,255,112,192,0,0,216,7,0,0,204,3,0,0,110,20,0,0,130,20,0,0,96,19,0,0,16,4,0,0,68,16,0,0,242,13,0,0,244,21,0,0,60,3,0,0,66,1,0,0,18,2,0,0,144,1,0,0,220,255,255,255,112,192,0,0,152,17,0,0,150,5,0,0,226,18,0,0,172,17,0,0,202,5,0,0,178,6,0,0,26,12,0,0,240,17,0,0,140,11,0,0,66,19,0,0,42,7,0,0,12,6,0,0,184,5,0,0,142,8,0,0,182,14,0,0,170,19,0,0,10,4,0,0,150,10,0,0,82,5,0,0,254,11,0,0,18,22,0,0,64,13,0,0,116,12,0,0,208,3,0,0,48,23,0,0,18,10,0,0,36,0,0,0,224,19,0,0,220,20,0,0,212,1,0,0,38,1,0,0,0,7,0,0,174,4,0,0,216,255,255,255,112,192,0,0,118,14,0,0,102,22,0,0,148,6,0,0,164,18,0,0,108,22,0,0,238,18,0,0,2,10,0,0,212,255,255,255,112,192,0,0,124,10,0,0,208,6,0,0,92,7,0,0,208,255,255,255,112,192,0,0,222,6,0,0,166,12,0,0,224,3,0,0,204,5,0,0,134,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,160,192,0,0,56,8,0,0,6,2,0,0,242,12,0,0,54,14,0,0,188,17,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,72,9,0,0,58,20,0,0,220,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,186,20,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,160,192,0,0,142,2,0,0,200,2,0,0,122,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,176,192,0,0,138,18,0,0,2,15,0,0,52,7,0,0,88,22,0,0,34,13,0,0,148,0,0,0,146,15,0,0,66,5,0,0,150,18,0,0,226,6,0,0,88,3,0,0,178,21,0,0,212,6,0,0,62,6,0,0,218,0,0,0,236,9,0,0,206,9,0,0,88,13,0,0,244,255,255,255,176,192,0,0,134,21,0,0,184,17,0,0,166,3,0,0,2,22,0,0,22,7,0,0,94,9,0,0,52,4,0,0,228,1,0,0,88,16,0,0,224,10,0,0,144,16,0,0,226,11,0,0,218,9,0,0,152,4,0,0,230,9,0,0,220,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,208,192,0,0,82,17,0,0,34,0,0,0,10,2,0,0,42,20,0,0,34,13,0,0,148,0,0,0,84,1,0,0,162,10,0,0,244,255,255,255,208,192,0,0,160,11,0,0,216,5,0,0,116,9,0,0,64,5,0,0,4,4,0,0,150,9,0,0,116,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,240,192,0,0,8,2,0,0,60,8,0,0,148,22,0,0,154,15,0,0,14,2,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,193,0,0,104,4,0,0,68,0,0,0,72,7,0,0,246,8,0,0,188,17,0,0,148,0,0,0,36,15,0,0,86,11,0,0,108,5,0,0,234,8,0,0,68,6,0,0,52,5,0,0,108,17,0,0,122,6,0,0,36,11,0,0,224,18,0,0,144,11,0,0,12,21,0,0,174,11,0,0,186,20,0,0,240,10,0,0,158,8,0,0,126,9,0,0,208,12,0,0,108,15,0,0,244,255,255,255,0,193,0,0,96,11,0,0,182,17,0,0,122,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,193,0,0,136,19,0,0,228,21,0,0,176,15,0,0,230,4,0,0,34,13,0,0,148,0,0,0,26,20,0,0,36,21,0,0,244,255,255,255,16,193,0,0,198,14,0,0,160,8,0,0,182,5,0,0,54,9,0,0,4,4,0,0,48,22,0,0,32,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,193,0,0,54,16,0,0,146,9,0,0,2,23,0,0,44,12,0,0,34,13,0,0,148,0,0,0,196,0,0,0,190,15,0,0,244,255,255,255,48,193,0,0,172,5,0,0,186,9,0,0,44,9,0,0,232,6,0,0,4,4,0,0,200,1,0,0,250,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,193,0,0,44,15,0,0,84,9,0,0,168,15,0,0,114,7,0,0,34,13,0,0,148,0,0,0,160,12,0,0,152,0,0,0,212,11,0,0,156,14,0,0,238,14,0,0,82,7,0,0,84,17,0,0,154,0,0,0,208,11,0,0,74,1,0,0,122,5,0,0,72,4,0,0,134,5,0,0,86,3,0,0,34,16,0,0,78,22,0,0,172,18,0,0,4,2,0,0,196,18,0,0,142,14,0,0,244,255,255,255,88,193,0,0,88,8,0,0,22,12,0,0,162,22,0,0,20,5,0,0,140,9,0,0,202,14,0,0,96,17,0,0,252,6,0,0,116,14,0,0,134,3,0,0,84,7,0,0,218,4,0,0,24,20,0,0,106,13,0,0,224,13,0,0,240,255,255,255,88,193,0,0,102,10,0,0,68,7,0,0,54,7,0,0,252,3,0,0,18,9,0,0,88,6,0,0,120,7,0,0,18,11,0,0,126,2,0,0,156,10,0,0,62,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,193,0,0,76,10,0,0,6,4,0,0,200,5,0,0,60,19,0,0,130,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,158,10,0,0,188,22,0,0,218,22,0,0,22,18,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,194,0,0,40,5,0,0,234,7,0,0,92,1,0,0,170,4,0,0,232,3,0,0,148,0,0,0,170,18,0,0,80,11,0,0,220,1,0,0,54,13,0,0,88,5,0,0,62,4,0,0,236,7,0,0,242,14,0,0,34,6,0,0,194,0,0,0,92,8,0,0,126,17,0,0,116,3,0,0,126,4,0,0,4,16,0,0,74,9,0,0,194,21,0,0,122,0,0,0,60,6,0,0,118,20,0,0,244,255,255,255,8,194,0,0,94,3,0,0,234,3,0,0,218,11,0,0,72,16,0,0,182,11,0,0,22,19,0,0,76,4,0,0,82,22,0,0,40,0,0,0,126,7,0,0,220,255,255,255,8,194,0,0,212,19,0,0,108,0,0,0,116,5,0,0,28,20,0,0,230,5,0,0,160,16,0,0,46,10,0,0,90,7,0,0,242,20,0,0,106,21,0,0,32,7,0,0,110,2,0,0,30,3,0,0,76,2,0,0,240,7,0,0,60,4,0,0,168,19,0,0,100,5,0,0,180,14,0,0,136,20,0,0,130,10,0,0,246,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,194,0,0,198,10,0,0,234,16,0,0,72,17,0,0,30,17,0,0,208,14,0,0,148,0,0,0,42,11,0,0,28,19,0,0,134,12,0,0,114,0,0,0,236,1,0,0,212,18,0,0,6,20,0,0,92,11,0,0,248,16,0,0,160,10,0,0,222,10,0,0,240,0,0,0,218,8,0,0,174,2,0,0,186,15,0,0,200,9,0,0,164,22,0,0,238,15,0,0,116,13,0,0,94,21,0,0,28,16,0,0,244,255,255,255,40,194,0,0,56,15,0,0,178,15,0,0,138,5,0,0,182,1,0,0,0,2,0,0,70,16,0,0,158,16,0,0,238,19,0,0,154,4,0,0,88,12,0,0,220,255,255,255,40,194,0,0,78,7,0,0,210,19,0,0,158,21,0,0,120,10,0,0,14,13,0,0,156,6,0,0,226,7,0,0,148,12,0,0,232,9,0,0,74,12,0,0,50,1,0,0,240,19,0,0,248,22,0,0,64,17,0,0,160,1,0,0,118,3,0,0,236,14,0,0,72,1,0,0,118,6,0,0,68,10,0,0,220,6,0,0,64,10,0,0,134,8,0,0,178,8,0,0,250,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,194,0,0,18,4,0,0,22,13,0,0,104,1,0,0,250,12,0,0,36,1,0,0,148,0,0,0,10,6,0,0,114,21,0,0,106,15,0,0,160,0,0,0,212,9,0,0,86,1,0,0,166,17,0,0,54,1,0,0,242,11,0,0,126,13,0,0,224,4,0,0,62,10,0,0,166,18,0,0,18,14,0,0,154,7,0,0,116,0,0,0,244,13,0,0,94,2,0,0,142,19,0,0,82,4,0,0,28,23,0,0,92,20,0,0,94,8,0,0,44,19,0,0,244,255,255,255,88,194,0,0,138,20,0,0,120,16,0,0,228,17,0,0,88,21,0,0,184,11,0,0,32,1,0,0,106,10,0,0,206,7,0,0,40,4,0,0,122,1,0,0,220,255,255,255,88,194,0,0,68,12,0,0,60,16,0,0,144,12,0,0,102,14,0,0,246,1,0,0,46,5,0,0,206,17,0,0,212,10,0,0,8,23,0,0,24,2,0,0,130,21,0,0,102,1,0,0,144,6,0,0,84,20,0,0,4,11,0,0,220,22,0,0,196,255,255,255,88,194,0,0,74,19,0,0,54,12,0,0,156,17,0,0,180,10,0,0,14,10,0,0,146,13,0,0,24,8,0,0,216,12,0,0,52,20,0,0,210,16,0,0,114,4,0,0,134,14,0,0,124,19,0,0,44,23,0,0,102,8,0,0,176,22,0,0,64,1,0,0,124,6,0,0,140,16,0,0,164,11,0,0,216,10,0,0,98,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,194,0,0,72,20,0,0,36,3,0,0,254,15,0,0,104,7,0,0,68,19,0,0,148,0,0,0,130,0,0,0,78,21,0,0,112,18,0,0,210,9,0,0,84,2,0,0,84,19,0,0,102,19,0,0,88,7,0,0,236,13,0,0,22,3,0,0,44,1,0,0,142,5,0,0,130,6,0,0,18,14,0,0,234,4,0,0,214,17,0,0,198,17,0,0,0,15,0,0,244,255,255,255,120,194,0,0,68,11,0,0,148,7,0,0,132,4,0,0,190,5,0,0,132,14,0,0,118,15,0,0,254,2,0,0,156,8,0,0,166,13,0,0,70,15,0,0,220,255,255,255,120,194,0,0,30,9,0,0,194,12,0,0,176,21,0,0,140,3,0,0,120,12,0,0,30,8,0,0,48,1,0,0,254,7,0,0,4,0,0,0,192,3,0,0,40,14,0,0,50,13,0,0,8,10,0,0,56,22,0,0,216,8,0,0,236,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,194,0,0,208,16,0,0,174,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,194,0,0,108,4,0,0,174,10,0,0,34,1,0,0,140,20,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,160,194,0,0,98,6,0,0,92,10,0,0,130,16,0,0,98,18,0,0,154,5,0,0,148,0,0,0,16,2,0,0,166,19,0,0,234,1,0,0,188,14,0,0,164,7,0,0,106,16,0,0,32,12,0,0,212,0,0,0,28,4,0,0,150,1,0,0,20,13,0,0,244,2,0,0,142,22,0,0,18,14,0,0,112,14,0,0,68,4,0,0,138,7,0,0,44,18,0,0,236,11,0,0,194,22,0,0,52,22,0,0,242,8,0,0,100,22,0,0,162,13,0,0,244,255,255,255,160,194,0,0,118,5,0,0,98,1,0,0,56,14,0,0,238,0,0,0,144,13,0,0,224,17,0,0,6,8,0,0,98,10,0,0,140,17,0,0,42,9,0,0,220,255,255,255,160,194,0,0,94,1,0,0,128,5,0,0,238,17,0,0,138,21,0,0,28,15,0,0,84,5,0,0,38,14,0,0,208,17,0,0,244,3,0,0,76,13,0,0,58,15,0,0,22,6,0,0,178,9,0,0,84,13,0,0,2,13,0,0,108,13,0,0,212,255,255,255,160,194,0,0,50,5,0,0,20,1,0,0,158,5,0,0,96,18,0,0,100,10,0,0,210,20,0,0,176,4,0,0,112,5,0,0,184,8,0,0,112,9,0,0,162,12,0,0,186,12,0,0,106,19,0,0,18,1,0,0,128,14,0,0,22,2,0,0,202,19,0,0,214,0,0,0,204,7,0,0,252,22,0,0,10,1,0,0,206,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,194,0,0,138,17,0,0,132,13,0,0,88,9,0,0,154,8,0,0,6,9,0,0,148,0,0,0,36,6,0,0,156,15,0,0,156,7,0,0,252,15,0,0,68,1,0,0,40,20,0,0,206,21,0,0,220,16,0,0,172,6,0,0,222,19,0,0,172,2,0,0,140,12,0,0,210,21,0,0,118,8,0,0,48,20,0,0,74,18,0,0,10,9,0,0,58,2,0,0,6,0,0,0,114,8,0,0,58,7,0,0,32,18,0,0,244,255,255,255,192,194,0,0,188,8,0,0,160,14,0,0,96,22,0,0,110,14,0,0,138,11,0,0,104,18,0,0,172,4,0,0,100,2,0,0,146,21,0,0,196,10,0,0,220,255,255,255,192,194,0,0,76,20,0,0,226,13,0,0,244,16,0,0,46,13,0,0,156,21,0,0,216,19,0,0,244,6,0,0,70,12,0,0,108,10,0,0,166,7,0,0,130,11,0,0,2,8,0,0,232,17,0,0,170,0,0,0,42,5,0,0,32,22,0,0,244,19,0,0,54,4,0,0,226,3,0,0,222,3,0,0,68,13,0,0,80,9,0,0,30,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,194,0,0,138,8,0,0,206,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,232,194,0,0,198,18,0,0,184,14,0,0,22,21,0,0,238,22,0,0,236,20,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,248,194,0,0,118,22,0,0,86,8,0,0,244,0,0,0,2,21,0,0,246,2,0,0,148,0,0,0,108,19,0,0,248,3,0,0,48,18,0,0,30,23,0,0,84,15,0,0,140,2,0,0,158,1,0,0,66,20,0,0,66,9,0,0,252,4,0,0,182,20,0,0,8,15,0,0,184,18,0,0,118,7,0,0,72,2,0,0,216,9,0,0,186,14,0,0,126,5,0,0,222,11,0,0,124,5,0,0,242,17,0,0,244,255,255,255,248,194,0,0,156,16,0,0,236,21,0,0,202,6,0,0,46,21,0,0,44,11,0,0,14,7,0,0,204,14,0,0,120,20,0,0,228,12,0,0,68,21,0,0,220,255,255,255,248,194,0,0,114,2,0,0,152,6,0,0,132,8,0,0,190,11,0,0,116,1,0,0,182,21,0,0,154,16,0,0,26,8,0,0,8,12,0,0,104,19,0,0,142,11,0,0,192,13,0,0,148,20,0,0,172,14,0,0,58,23,0,0,78,11,0,0,180,4,0,0,208,21,0,0,194,16,0,0,66,4,0,0,50,10,0,0,164,15,0,0,222,20,0,0,30,22,0,0,56,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,195,0,0,54,0,0,0,190,6,0,0,24,13,0,0,210,10,0,0,250,9,0,0,148,0,0,0,156,2,0,0,32,4,0,0,94,0,0,0,106,12,0,0,6,11,0,0,74,21,0,0,4,6,0,0,172,10,0,0,42,22,0,0,178,16,0,0,244,18,0,0,62,3,0,0,176,20,0,0,238,4,0,0,244,255,255,255,24,195,0,0,192,21,0,0,210,17,0,0,4,23,0,0,180,20,0,0,144,20,0,0,116,15,0,0,174,20,0,0,192,15,0,0,20,23,0,0,176,7,0,0,220,255,255,255,24,195,0,0,126,15,0,0,0,5,0,0,242,4,0,0,26,10,0,0,238,21,0,0,252,2,0,0,14,16,0,0,62,19,0,0,232,8,0,0,102,13,0,0,184,6,0,0,70,8,0,0,34,22,0,0,102,5,0,0,22,9,0,0,214,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,195,0,0,244,22,0,0,76,15,0,0,68,8,0,0,192,4,0,0,74,17,0,0,148,0,0,0,38,15,0,0,162,1,0,0,58,1,0,0,96,10,0,0,192,5,0,0,28,7,0,0,60,14,0,0,144,0,0,0,110,15,0,0,198,2,0,0,94,4,0,0,6,1,0,0,22,16,0,0,184,4,0,0,202,16,0,0,2,9,0,0,58,17,0,0,54,10,0,0,182,10,0,0,24,0,0,0,96,4,0,0,132,21,0,0,198,6,0,0,44,22,0,0,146,16,0,0,112,19,0,0,102,2,0,0,170,11,0,0,106,1,0,0,244,255,255,255,56,195,0,0,140,13,0,0,192,17,0,0,182,13,0,0,52,14,0,0,238,3,0,0,4,21,0,0,210,4,0,0,132,0,0,0,186,11,0,0,36,18,0,0,220,255,255,255,56,195,0,0,154,3,0,0,204,20,0,0,106,14,0,0,148,9,0,0,110,12,0,0,222,8,0,0,40,8,0,0,206,1,0,0,168,14,0,0,12,15,0,0,108,18,0,0,184,10,0,0,28,1,0,0,208,15,0,0,116,18,0,0,52,12,0,0,190,2,0,0,76,17,0,0,46,20,0,0,104,16,0,0,10,7,0,0,234,15,0,0,234,0,0,0,254,19,0,0,46,2,0,0,94,20,0,0,98,20,0,0,140,19,0,0,114,19,0,0,54,19,0,0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,195,0,0,132,10,0,0,206,16,0,0,2,16,0,0,0,16,0,0,240,9,0,0,148,0,0,0,250,10,0,0,0,12,0,0,148,3,0,0,218,19,0,0,126,1,0,0,246,7,0,0,108,7,0,0,42,10,0,0,152,18,0,0,238,20,0,0,164,5,0,0,70,20,0,0,244,255,255,255,88,195,0,0,150,20,0,0,64,16,0,0,34,15,0,0,114,18,0,0,60,13,0,0,86,16,0,0])
.concat([12,4,0,0,34,8,0,0,28,17,0,0,140,1,0,0,220,255,255,255,88,195,0,0,224,14,0,0,242,18,0,0,116,2,0,0,14,15,0,0,66,23,0,0,200,14,0,0,250,0,0,0,182,12,0,0,50,12,0,0,170,10,0,0,168,9,0,0,242,2,0,0,14,1,0,0,126,3,0,0,54,21,0,0,56,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,195,0,0,20,2,0,0,128,10,0,0,70,18,0,0,86,21,0,0,110,16,0,0,148,0,0,0,42,13,0,0,60,11,0,0,24,4,0,0,92,5,0,0,150,4,0,0,100,21,0,0,6,14,0,0,192,10,0,0,78,6,0,0,60,9,0,0,238,7,0,0,44,5,0,0,150,14,0,0,222,13,0,0,52,8,0,0,186,6,0,0,120,22,0,0,46,17,0,0,96,14,0,0,244,255,255,255,120,195,0,0,172,20,0,0,232,0,0,0,228,7,0,0,244,10,0,0,166,16,0,0,34,14,0,0,0,17,0,0,174,3,0,0,14,8,0,0,174,12,0,0,220,255,255,255,120,195,0,0,16,8,0,0,218,3,0,0,190,9,0,0,192,12,0,0,38,4,0,0,110,8,0,0,200,16,0,0,208,1,0,0,94,19,0,0,148,17,0,0,78,10,0,0,2,14,0,0,46,9,0,0,46,22,0,0,152,16,0,0,90,11,0,0,238,8,0,0,8,4,0,0,180,11,0,0,26,14,0,0,232,10,0,0,192,18,0,0,18,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,195,0,0,254,1,0,0,92,21,0,0,254,15,0,0,104,7,0,0,112,15,0,0,148,0,0,0,130,0,0,0,78,21,0,0,112,18,0,0,210,9,0,0,84,2,0,0,84,19,0,0,222,5,0,0,46,0,0,0,214,14,0,0,130,13,0,0,26,23,0,0,142,5,0,0,12,7,0,0,18,14,0,0,180,22,0,0,204,22,0,0,136,13,0,0,48,14,0,0,244,255,255,255,152,195,0,0,222,7,0,0,2,12,0,0,132,4,0,0,190,5,0,0,58,4,0,0,118,15,0,0,254,2,0,0,156,8,0,0,166,13,0,0,70,15,0,0,220,255,255,255,152,195,0,0,88,2,0,0,166,21,0,0,176,21,0,0,140,3,0,0,234,17,0,0,30,8,0,0,48,1,0,0,254,7,0,0,4,0,0,0,192,3,0,0,50,22,0,0,122,12,0,0,80,1,0,0,20,6,0,0,104,3,0,0,236,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,184,195,0,0,12,5,0,0,32,11,0,0,164,9,0,0,222,1,0,0,12,18,0,0,148,0,0,0,10,13,0,0,28,9,0,0,4,17,0,0,126,6,0,0,98,19,0,0,172,11,0,0,172,13,0,0,164,4,0,0,54,23,0,0,110,5,0,0,174,17,0,0,102,21,0,0,140,5,0,0,18,18,0,0,220,7,0,0,148,13,0,0,122,17,0,0,64,2,0,0,168,20,0,0,160,22,0,0,198,20,0,0,178,22,0,0,22,8,0,0,116,22,0,0,198,0,0,0,200,21,0,0,112,13,0,0,254,16,0,0,178,20,0,0,60,10,0,0,54,8,0,0,224,0,0,0,4,15,0,0,192,0,0,0,38,6,0,0,154,18,0,0,24,16,0,0,222,22,0,0,144,9,0,0,244,255,255,255,184,195,0,0,168,8,0,0,82,9,0,0,22,10,0,0,154,17,0,0,34,9,0,0,14,6,0,0,12,20,0,0,124,21,0,0,152,7,0,0,14,14,0,0,166,5,0,0,92,16,0,0,60,7,0,0,36,12,0,0,196,9,0,0,32,3,0,0,48,5,0,0,64,22,0,0,250,1,0,0,60,18,0,0,66,13,0,0,10,19,0,0,176,16,0,0,246,22,0,0,48,6,0,0,16,16,0,0,222,21,0,0,78,0,0,0,230,15,0,0,90,18,0,0,88,10,0,0,130,5,0,0,220,21,0,0,124,15,0,0,230,1,0,0,230,17,0,0,240,1,0,0,170,14,0,0,206,6,0,0,144,21,0,0,218,16,0,0,106,0,0,0,146,17,0,0,116,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,216,195,0,0,174,16,0,0,192,20,0,0,30,10,0,0,62,23,0,0,16,0,0,0,148,0,0,0,238,12,0,0,66,21,0,0,254,13,0,0,150,22,0,0,234,5,0,0,240,8,0,0,32,16,0,0,142,7,0,0,118,21,0,0,238,2,0,0,140,6,0,0,48,15,0,0,206,12,0,0,108,14,0,0,176,18,0,0,184,20,0,0,150,6,0,0,100,3,0,0,0,11,0,0,64,11,0,0,214,16,0,0,166,8,0,0,0,13,0,0,80,4,0,0,134,0,0,0,228,18,0,0,50,20,0,0,244,255,255,255,216,195,0,0,66,18,0,0,80,22,0,0,88,20,0,0,136,7,0,0,212,17,0,0,206,0,0,0,84,18,0,0,218,12,0,0,240,5,0,0,238,13,0,0,220,255,255,255,216,195,0,0,104,15,0,0,30,2,0,0,236,3,0,0,152,20,0,0,218,13,0,0,250,18,0,0,52,3,0,0,234,2,0,0,58,16,0,0,226,12,0,0,120,15,0,0,198,11,0,0,150,15,0,0,248,11,0,0,24,6,0,0,112,11,0,0,194,5,0,0,156,3,0,0,134,22,0,0,36,23,0,0,222,17,0,0,202,12,0,0,82,2,0,0,240,2,0,0,146,0,0,0,204,10,0,0,156,1,0,0,196,17,0,0,74,2,0,0,218,15,0,0,220,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,196,0,0,92,18,0,0,200,22,0,0,16,11,0,0,128,3,0,0,156,0,0,0,148,0,0,0,110,22,0,0,16,21,0,0,98,8,0,0,206,18,0,0,98,21,0,0,64,0,0,0,6,12,0,0,12,3,0,0,10,11,0,0,26,13,0,0,226,8,0,0,182,8,0,0,196,13,0,0,110,17,0,0,144,7,0,0,232,4,0,0,160,17,0,0,244,255,255,255,24,196,0,0,90,10,0,0,26,4,0,0,144,10,0,0,74,14,0,0,228,5,0,0,98,13,0,0,138,16,0,0,0,6,0,0,232,12,0,0,136,2,0,0,220,255,255,255,24,196,0,0,246,0,0,0,56,16,0,0,32,0,0,0,172,15,0,0,40,1,0,0,136,9,0,0,8,13,0,0,12,13,0,0,164,10,0,0,32,23,0,0,60,2,0,0,220,12,0,0,126,10,0,0,18,15,0,0,58,10,0,0,60,12,0,0,36,2,0,0,142,20,0,0,76,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,196,0,0,96,21,0,0,180,1,0,0,82,15,0,0,40,15,0,0,26,7,0,0,148,0,0,0,240,22,0,0,184,9,0,0,54,2,0,0,32,6,0,0,70,21,0,0,32,17,0,0,250,21,0,0,14,17,0,0,38,16,0,0,174,6,0,0,2,19,0,0,204,19,0,0,184,12,0,0,244,255,255,255,72,196,0,0,102,16,0,0,118,10,0,0,188,20,0,0,202,13,0,0,214,12,0,0,34,4,0,0,88,19,0,0,210,6,0,0,140,15,0,0,104,11,0,0,220,255,255,255,72,196,0,0,208,7,0,0,72,11,0,0,170,6,0,0,34,12,0,0,164,17,0,0,112,7,0,0,58,0,0,0,214,2,0,0,244,4,0,0,78,3,0,0,40,21,0,0,248,19,0,0,32,20,0,0,210,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,196,0,0,224,22,0,0,38,7,0,0,18,20,0,0,240,12,0,0,98,22,0,0,148,0,0,0,224,2,0,0,40,11,0,0,190,0,0,0,4,12,0,0,132,22,0,0,138,1,0,0,120,2,0,0,18,21,0,0,162,11,0,0,58,11,0,0,14,11,0,0,226,22,0,0,188,15,0,0,160,5,0,0,50,0,0,0,178,11,0,0,74,20,0,0,32,9,0,0,30,14,0,0,50,8,0,0,78,13,0,0,226,2,0,0,216,18,0,0,248,20,0,0,160,6,0,0,170,22,0,0,28,22,0,0,44,16,0,0,22,0,0,0,70,22,0,0,202,0,0,0,182,22,0,0,244,255,255,255,104,196,0,0,204,4,0,0,250,8,0,0,198,21,0,0,182,9,0,0,116,16,0,0,68,3,0,0,222,16,0,0,14,4,0,0,186,8,0,0,14,5,0,0,220,255,255,255,104,196,0,0,46,14,0,0,132,19,0,0,194,15,0,0,36,10,0,0,94,18,0,0,10,10,0,0,114,3,0,0,120,3,0,0,44,4,0,0,0,20,0,0,46,8,0,0,58,3,0,0,36,7,0,0,94,7,0,0,176,17,0,0,48,0,0,0,132,2,0,0,176,6,0,0,30,0,0,0,156,13,0,0,62,8,0,0,228,22,0,0,216,255,255,255,104,196,0,0,116,10,0,0,134,16,0,0,86,22,0,0,10,18,0,0,168,10,0,0,6,5,0,0,18,16,0,0,144,18,0,0,102,9,0,0,196,20,0,0,224,21,0,0,18,0,0,0,254,3,0,0,104,17,0,0,6,23,0,0,124,11,0,0,84,10,0,0,102,18,0,0,118,18,0,0,74,15,0,0,198,1,0,0,246,16,0,0,212,255,255,255,104,196,0,0,22,20,0,0,206,2,0,0,8,6,0,0,210,12,0,0,64,4,0,0,184,13,0,0,14,9,0,0,146,22,0,0,252,20,0,0,228,19,0,0,16,12,0,0,80,6,0,0,96,6,0,0,104,13,0,0,248,4,0,0,106,11,0,0,212,3,0,0,132,16,0,0,52,9,0,0,204,0,0,0,46,1,0,0,182,16,0,0,208,255,255,255,104,196,0,0,42,21,0,0,224,1,0,0,104,2,0,0,112,8,0,0,214,13,0,0,114,5,0,0,232,21,0,0,176,0,0,0,134,20,0,0,98,17,0,0,148,4,0,0,244,8,0,0,210,11,0,0,38,9,0,0,138,14,0,0,40,6,0,0,28,8,0,0,198,7,0,0,10,5,0,0,108,12,0,0,130,1,0,0,198,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,160,196,0,0,134,18,0,0,130,22,0,0,40,2,0,0,76,6,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,176,196,0,0,138,0,0,0,4,7,0,0,108,21,0,0,186,21,0,0,46,15,0,0,148,0,0,0,76,22,0,0,178,12,0,0,136,12,0,0,234,14,0,0,48,7,0,0,184,19,0,0,38,12,0,0,18,17,0,0,44,7,0,0,46,23,0,0,244,255,255,255,176,196,0,0,96,7,0,0,234,20,0,0,132,5,0,0,124,2,0,0,28,14,0,0,228,2,0,0,152,8,0,0,112,6,0,0,10,22,0,0,216,15,0,0,220,255,255,255,176,196,0,0,240,20,0,0,200,3,0,0,98,9,0,0,80,2,0,0,128,11,0,0,136,0,0,0,250,7,0,0,6,7,0,0,116,17,0,0,70,10,0,0,62,21,0,0,244,5,0,0,128,21,0,0,146,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,197,0,0,34,2,0,0,224,16,0,0,164,1,0,0,4,20,0,0,186,4,0,0,148,0,0,0,94,12,0,0,124,22,0,0,214,15,0,0,236,2,0,0,170,15,0,0,188,4,0,0,66,17,0,0,2,6,0,0,180,6,0,0,18,8,0,0,80,10,0,0,22,23,0,0,230,20,0,0,104,5,0,0,114,20,0,0,244,255,255,255,16,197,0,0,16,3,0,0,0,19,0,0,120,5,0,0,166,22,0,0,74,5,0,0,250,2,0,0,20,10,0,0,10,0,0,0,156,12,0,0,172,3,0,0,220,255,255,255,16,197,0,0,202,20,0,0,26,17,0,0,168,1,0,0,114,22,0,0,124,0,0,0,128,8,0,0,66,6,0,0,14,12,0,0,114,17,0,0,160,19,0,0,142,13,0,0,102,17,0,0,138,9,0,0,222,15,0,0,230,21,0,0,134,13,0,0,252,18,0,0,96,9,0,0,110,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,197,0,0,246,4,0,0,18,19,0,0,52,6,0,0,236,18,0,0,142,1,0,0,148,0,0,0,194,1,0,0,96,13,0,0,172,16,0,0,188,11,0,0,246,11,0,0,192,16,0,0,160,20,0,0,48,12,0,0,102,0,0,0,10,3,0,0,20,9,0,0,254,8,0,0,80,20,0,0,62,5,0,0,116,6,0,0,22,22,0,0,44,20,0,0,190,4,0,0,168,7,0,0,182,6,0,0,200,10,0,0,180,12,0,0,138,22,0,0,150,19,0,0,240,16,0,0,190,20,0,0,112,21,0,0,198,16,0,0,128,0,0,0,230,13,0,0,244,255,255,255,112,197,0,0,180,9,0,0,92,13,0,0,154,20,0,0,56,19,0,0,130,17,0,0,10,8,0,0,186,13,0,0,34,20,0,0,38,20,0,0,16,23,0,0,220,255,255,255,112,197,0,0,126,21,0,0,30,15,0,0,52,15,0,0,92,17,0,0,182,18,0,0,74,16,0,0,8,1,0,0,162,20,0,0,40,18,0,0,36,8,0,0,48,2,0,0,38,5,0,0,0,18,0,0,42,12,0,0,96,5,0,0,120,1,0,0,14,0,0,0,0,3,0,0,126,14,0,0,212,5,0,0,22,5,0,0,84,16,0,0,216,255,255,255,112,197,0,0,234,18,0,0,202,8,0,0,146,18,0,0,112,22,0,0,150,17,0,0,120,14,0,0,70,5,0,0,186,7,0,0,64,14,0,0,6,3,0,0,24,15,0,0,48,3,0,0,90,1,0,0,144,2,0,0,222,9,0,0,90,15,0,0,12,22,0,0,138,19,0,0,72,18,0,0,174,22,0,0,140,8,0,0,4,5,0,0,212,255,255,255,112,197,0,0,112,16,0,0,230,11,0,0,172,9,0,0,248,17,0,0,106,6,0,0,226,21,0,0,154,19,0,0,230,16,0,0,136,18,0,0,168,12,0,0,152,19,0,0,140,14,0,0,254,20,0,0,208,9,0,0,82,10,0,0,94,5,0,0,48,9,0,0,128,12,0,0,108,16,0,0,188,13,0,0,232,16,0,0,184,16,0,0,130,8,0,0,150,21,0,0,62,15,0,0,208,255,255,255,112,197,0,0,130,9,0,0,106,5,0,0,172,1,0,0,56,13,0,0,180,16,0,0,100,14,0,0,2,11,0,0,82,16,0,0,140,18,0,0,176,8,0,0,188,1,0,0,72,21,0,0,150,16,0,0,214,6,0,0,192,22,0,0,208,0,0,0,86,9,0,0,86,18,0,0,72,0,0,0,50,11,0,0,134,19,0,0,78,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,197,0,0,50,15,0,0,120,21,0,0,158,9,0,0,62,16,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,197,0,0,136,14,0,0,72,6,0,0,58,5,0,0,10,21,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,216,197,0,0,142,0,0,0,34,17,0,0,94,13,0,0,124,4,0,0,152,22,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,232,197,0,0,138,12,0,0,230,18,0,0,18,5,0,0,164,21,0,0,34,13,0,0,148,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,198,0,0,144,17,0,0,90,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,198,0,0,138,3,0,0,22,17,0,0,196,21,0,0,112,12,0,0,226,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,198,0,0,40,7,0,0,86,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,198,0,0,226,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,168,198,0,0,14,22,0,0,102,15,0,0,10,23,0,0,170,13,0,0,34,13,0,0,148,0,0,0,230,10,0,0,44,21,0,0,92,19,0,0,168,6,0,0,68,18,0,0,178,1,0,0,156,5,0,0,252,19,0,0,118,19,0,0,146,20,0,0,48,16,0,0,244,255,255,255,168,198,0,0,200,8,0,0,24,23,0,0,174,7,0,0,174,0,0,0,200,6,0,0,16,5,0,0,136,21,0,0,104,21,0,0,202,22,0,0,236,15,0,0,56,6,0,0,96,20,0,0,180,17,0,0,34,7,0,0,42,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,200,198,0,0,4,8,0,0,126,11,0,0,32,21,0,0,66,22,0,0,34,13,0,0,148,0,0,0,38,22,0,0,196,7,0,0,188,0,0,0,170,21,0,0,30,12,0,0,252,8,0,0,192,2,0,0,166,2,0,0,194,13,0,0,84,22,0,0,26,22,0,0,244,255,255,255,200,198,0,0,150,0,0,0,20,8,0,0,64,7,0,0,42,19,0,0,246,10,0,0,164,19,0,0,254,22,0,0,110,7,0,0,102,20,0,0,78,1,0,0,144,19,0,0,20,15,0,0,236,6,0,0,70,2,0,0,104,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,232,198,0,0,56,5,0,0,230,19,0,0,136,6,0,0,228,8,0,0,34,13,0,0,148,0,0,0,192,19,0,0,52,17,0,0,94,10,0,0,162,18,0,0,36,14,0,0,40,13,0,0,34,5,0,0,190,12,0,0,128,1,0,0,188,18,0,0,166,10,0,0,244,255,255,255,232,198,0,0,146,12,0,0,196,6,0,0,134,2,0,0,132,7,0,0,122,18,0,0,22,1,0,0,28,0,0,0,204,6,0,0,60,23,0,0,92,0,0,0,156,11,0,0,142,3,0,0,202,11,0,0,158,12,0,0,254,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,199,0,0,2,20,0,0,240,13,0,0,252,11,0,0,90,14,0,0,0,4,0,0,122,8,0,0,108,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,199,0,0,92,22,0,0,36,16,0,0,188,12,0,0,86,6,0,0,30,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,199,0,0,158,20,0,0,150,7,0,0,196,8,0,0,130,15,0,0,242,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,199,0,0,212,15,0,0,222,0,0,0,56,9,0,0,122,15,0,0,130,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,199,0,0,82,14,0,0,98,0,0,0,180,5,0,0,102,6,0,0,82,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,104,199,0,0,242,7,0,0,208,22,0,0,156,9,0,0,190,13,0,0,110,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,199,0,0,76,5,0,0,214,20,0,0,8,21,0,0,4,13,0,0,152,1,0,0,122,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,199,0,0,248,7,0,0,42,15,0,0,54,5,0,0,62,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,200,0,0,100,4,0,0,20,20,0,0,78,18,0,0,14,19,0,0,228,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,200,0,0,110,19,0,0,60,5,0,0,162,15,0,0,204,17,0,0,74,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,72,200,0,0,250,6,0,0,72,10,0,0,128,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,200,0,0,0,21,0,0,184,1,0,0,194,7,0,0,226,14,0,0,44,2,0,0,148,0,0,0,88,14,0,0,52,0,0,0,248,13,0,0,116,19,0,0,178,14,0,0,212,13,0,0,244,255,255,255,88,200,0,0,48,19,0,0,6,15,0,0,144,15,0,0,144,15,0,0,38,10,0,0,42,3,0,0,116,20,0,0,230,7,0,0,154,1,0,0,152,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,136,200,0,0,94,11,0,0,74,13,0,0,96,1,0,0,210,14,0,0,4,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,200,0,0,94,11,0,0,228,15,0,0,96,1,0,0,210,14,0,0,190,1,0,0,20,12,0,0,148,16,0,0,64,8,0,0,0,0,0,0,0,0,0,0,118,0,0,0,0,0,0,0,116,0,0,0,0,0,0,0,115,0,0,0,0,0,0,0,109,0,0,0,0,0,0,0,108,0,0,0,0,0,0,0,106,0,0,0,0,0,0,0,105,0,0,0,0,0,0,0,104,0,0,0,0,0,0,0,102,0,0,0,0,0,0,0,100,0,0,0,0,0,0,0,99,0,0,0,0,0,0,0,83,116,57,116,121,112,101,95,105,110,102,111,0,0,0,0,83,116,57,101,120,99,101,112,116,105,111,110,0,0,0,0,83,116,57,98,97,100,95,97,108,108,111,99,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,84,114,97,110,115,102,111,114,109,69,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,67,111,109,112,111,110,101,110,116,69,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,56,77,97,116,101,114,105,97,108,69,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,69,110,103,105,110,101,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,67,97,109,101,114,97,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,53,83,99,101,110,101,69,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,78,111,100,101,69,0,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,77,101,115,104,69,0,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,69,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,80,97,114,116,105,116,105,111,110,78,111,100,101,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,66,97,115,101,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,54,114,105,98,98,105,116,49,49,65,117,100,105,111,68,101,118,105,99,101,69,0,80,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,55,84,101,120,116,117,114,101,69,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,68,101,118,105,99,101,69,0,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,82,101,110,100,101,114,83,116,97,116,101,69,0,80,78,55,116,111,97,100,108,101,116,51,104,111,112,57,83,105,109,117,108,97,116,111,114,69,0,0,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,51,101,103,103,53,69,114,114,111,114,69,0,0,0,80,78,55,116,111,97,100,108,101,116,51,101,103,103,51,76,111,103,69,0,0,0,0,0,80,78,55,116,111,97,100,108,101,116,51,101,103,103,50,48,80,111,105,110,116,101,114,73,116,101,114,97,116,111,114,82,97,110,103,101,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,69,69,0,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,84,114,97,110,115,102,111,114,109,69,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,67,111,109,112,111,110,101,110,116,69,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,56,77,97,116,101,114,105,97,108,69,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,69,110,103,105,110,101,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,67,97,109,101,114,97,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,53,83,99,101,110,101,69,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,78,111,100,101,69,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,77,101,115,104,69,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,69,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,80,97,114,116,105,116,105,111,110,78,111,100,101,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,66,97,115,101,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,55,84,101,120,116,117,114,101,69,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,68,101,118,105,99,101,69,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,82,101,110,100,101,114,83,116,97,116,101,69,0,0,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,51,104,111,112,57,83,105,109,117,108,97,116,111,114,69,0,0,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,51,101,103,103,53,69,114,114,111,114,69,0,0,80,75,78,55,116,111,97,100,108,101,116,51,101,103,103,51,76,111,103,69,0,0,0,0,80,75,78,55,116,111,97,100,108,101,116,51,101,103,103,50,48,80,111,105,110,116,101,114,73,116,101,114,97,116,111,114,82,97,110,103,101,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,69,69,0,0,0,0,0,0,0,78,83,116,51,95,95,49,50,49,95,95,98,97,115,105,99,95,115,116,114,105,110,103,95,99,111,109,109,111,110,73,76,98,49,69,69,69,0,0,0,78,83,116,51,95,95,49,49,50,98,97,115,105,99,95,115,116,114,105,110,103,73,119,78,83,95,49,49,99,104,97,114,95,116,114,97,105,116,115,73,119,69,69,78,83,95,57,97,108,108,111,99,97,116,111,114,73,119,69,69,69,69,0,0,78,83,116,51,95,95,49,49,50,98,97,115,105,99,95,115,116,114,105,110,103,73,99,78,83,95,49,49,99,104,97,114,95,116,114,97,105,116,115,73,99,69,69,78,83,95,57,97,108,108,111,99,97,116,111,114,73,99,69,69,69,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,84,114,97,110,115,102,111,114,109,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,57,67,111,109,112,111,110,101,110,116,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,56,77,97,116,101,114,105,97,108,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,54,77,97,116,101,114,105,97,108,84,114,97,99,107,67,111,108,111,114,86,97,114,105,97,98,108,101,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,53,77,97,116,101,114,105,97,108,83,104,105,110,105,110,101,115,115,86,97,114,105,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,53,76,105,103,104,116,86,105,101,119,80,111,115,105,116,105,111,110,86,97,114,105,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,52,77,97,116,101,114,105,97,108,83,112,101,99,117,108,97,114,86,97,114,105,97,98,108,101,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,51,77,97,116,101,114,105,97,108,68,105,102,102,117,115,101,86,97,114,105,97,98,108,101,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,51,77,97,116,101,114,105,97,108,65,109,98,105,101,110,116,86,97,114,105,97,98,108,101,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,50,67,97,109,101,114,97,80,111,115,105,116,105,111,110,86,97,114,105,97,98,108,101,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,49,84,101,120,116,117,114,101,77,97,116,114,105,120,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,49,77,97,116,101,114,105,97,108,76,105,103,104,116,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,49,76,105,103,104,116,83,112,101,99,117,108,97,114,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,49,76,105,103,104,116,80,111,115,105,116,105,111,110,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,48,78,111,114,109,97,108,77,97,116,114,105,120,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,50,48,76,105,103,104,116,68,105,102,102,117,115,101,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,57,77,111,100,101,108,77,97,116,114,105,120,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,57,70,111,103,68,105,115,116,97,110,99,101,86,97,114,105,97,98,108,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,56,84,101,120,116,117,114,101,83,101,116,86,97,114,105,97,98,108,101,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,56,70,111,103,68,101,110,115,105,116,121,86,97,114,105,97,98,108,101,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,55,77,86,80,77,97,116,114,105,120,86,97,114,105,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,54,70,111,103,67,111,108,111,114,86,97,114,105,97,98,108,101,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,53,83,99,101,110,101,80,97,114,97,109,101,116,101,114,115,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,53,65,109,98,105,101,110,116,86,97,114,105,97,98,108,101,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,52,83,99,97,108,97,114,86,97,114,105,97,98,108,101,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,52,82,101,110,100,101,114,86,97,114,105,97,98,108,101,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,48,82,101,110,100,101,114,80,97,116,104,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,49,48,82,101,110,100,101,114,80,97,115,115,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,83,107,101,108,101,116,111,110,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,83,107,101,108,101,116,111,110,52,66,111,110,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,56,83,101,113,117,101,110,99,101,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,55,86,105,115,105,98,108,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,55,83,112,97,99,105,97,108,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,115,101,110,115,111,114,54,83,101,110,115,111,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,115,101,110,115,111,114,50,48,66,111,117,110,100,105,110,103,86,111,108,117,109,101,83,101,110,115,111,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,115,101,110,115,111,114,49,51,83,101,110,115,111,114,82,101,115,117,108,116,115,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,97,99,116,105,111,110,57,65,110,105,109,97,116,105,111,110,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,97,99,116,105,111,110,49,51,66,97,115,101,65,110,105,109,97,116,105,111,110,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,69,110,103,105,110,101,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,54,67,97,109,101,114,97,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,53,84,114,97,99,107,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,53,83,99,101,110,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,53,66,111,117,110,100,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,78,111,100,101,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,77,101,115,104,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,52,77,101,115,104,55,83,117,98,77,101,115,104,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,50,55,78,111,114,109,97,108,105,122,97,116,105,111,110,84,101,120,116,117,114,101,67,114,101,97,116,111,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,50,50,68,105,102,102,117,115,101,77,97,116,101,114,105,97,108,67,114,101,97,116,111,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,50,49,83,107,121,66,111,120,77,97,116,101,114,105,97,108,67,114,101,97,116,111,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,50,49,83,101,110,115,111,114,82,101,115,117,108,116,115,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,50,48,87,97,116,101,114,77,97,116,101,114,105,97,108,67,114,101,97,116,111,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,57,83,105,109,112,108,101,82,101,110,100,101,114,77,97,110,97,103,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,56,83,107,121,68,111,109,101,77,101,115,104,67,114,101,97,116,111,114,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,55,84,114,97,110,115,102,111,114,109,76,105,115,116,101,110,101,114,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,55,83,112,104,101,114,101,77,101,115,104,67,114,101,97,116,111,114,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,55,83,107,121,66,111,120,77,101,115,104,67,114,101,97,116,111,114,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,55,83,107,101,108,101,116,111,110,67,111,109,112,111,110,101,110,116,69,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,55,83,107,101,108,101,116,111,110,67,111,109,112,111,110,101,110,116,49,55,83,101,113,117,101,110,99,101,65,110,105,109,97,116,105,111,110,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,54,84,111,114,117,115,77,101,115,104,67,114,101,97,116,111,114,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,54,82,101,115,111,117,114,99,101,83,116,114,101,97,109,101,114,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,54,80,104,121,115,105,99,115,67,111,109,112,111,110,101,110,116,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,54,65,65,66,111,120,77,101,115,104,67,114,101,97,116,111,114,69,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,82,101,115,111,117,114,99,101,77,97,110,97,103,101,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,82,101,115,111,117,114,99,101,67,114,101,97,116,111,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,77,97,116,101,114,105,97,108,77,97,110,97,103,101,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,71,114,105,100,77,101,115,104,67,114,101,97,116,111,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,67,111,110,116,101,120,116,76,105,115,116,101,110,101,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,53,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,52,84,101,120,116,117,114,101,77,97,110,97,103,101,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,52,80,104,121,115,105,99,115,77,97,110,97,103,101,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,52,65,114,99,104,105,118,101,77,97,110,97,103,101,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,83,104,97,100,101,114,77,97,110,97,103,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,82,101,110,100,101,114,97,98,108,101,83,101,116,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,82,101,110,100,101,114,77,97,110,97,103,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,80,97,114,116,105,116,105,111,110,78,111,100,101,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,55,83,117,98,77,101,115,104,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,66,117,102,102,101,114,77,97,110,97,103,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,66,97,115,101,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,51,65,117,100,105,111,83,116,114,101,97,109,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,50,87,97,118,101,83,116,114,101,97,109,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,50,84,77,83,72,83,116,114,101,97,109,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,50,72,111,112,67,111,109,112,111,110,101,110,116,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,50,65,117,100,105,111,77,97,110,97,103,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,87,97,118,101,68,101,99,111,100,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,84,71,65,83,116])
.concat([114,101,97,109,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,78,111,100,101,77,97,110,97,103,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,70,111,110,116,77,97,110,97,103,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,68,68,83,83,116,114,101,97,109,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,49,66,77,80,83,116,114,101,97,109,101,114,69,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,48,82,101,110,100,101,114,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,48,72,111,112,77,97,110,97,103,101,114,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,48,72,111,112,77,97,110,97,103,101,114,49,56,83,111,108,105,100,83,101,110,115,111,114,82,101,115,117,108,116,115,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,48,65,116,116,97,99,104,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,55,116,97,100,112,111,108,101,49,48,65,110,105,109,97,116,97,98,108,101,69,0,0,78,55,116,111,97,100,108,101,116,54,114,105,98,98,105,116,49,49,65,117,100,105,111,83,116,114,101,97,109,69,0,0,78,55,116,111,97,100,108,101,116,54,114,105,98,98,105,116,49,49,65,117,100,105,111,68,101,118,105,99,101,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,57,73,110,100,101,120,68,97,116,97,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,56,86,105,101,119,112,111,114,116,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,55,84,101,120,116,117,114,101,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,54,83,104,97,100,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,54,66,117,102,102,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,57,66,97,99,107,97,98,108,101,84,101,120,116,117,114,101,77,105,112,80,105,120,101,108,66,117,102,102,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,54,71,76,69,83,50,84,101,120,116,117,114,101,77,105,112,80,105,120,101,108,66,117,102,102,101,114,69,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,51,80,105,120,101,108,66,117,102,102,101,114,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,51,69,71,76,50,80,66,117,102,102,101,114,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,50,69,71,76,50,87,105,110,100,111,119,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,48,86,101,114,116,101,120,66,117,102,102,101,114,65,99,99,101,115,115,111,114,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,48,86,97,114,105,97,98,108,101,66,117,102,102,101,114,70,111,114,109,97,116,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,48,71,76,69,83,50,70,66,79,82,101,110,100,101,114,84,97,114,103,101,116,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,50,48,66,97,99,107,97,98,108,101,86,101,114,116,101,120,70,111,114,109,97,116,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,57,73,110,100,101,120,66,117,102,102,101,114,65,99,99,101,115,115,111,114,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,57,71,76,69,83,50,83,76,86,101,114,116,101,120,76,97,121,111,117,116,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,57,71,76,69,83,50,70,66,79,80,105,120,101,108,66,117,102,102,101,114,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,57,66,97,99,107,97,98,108,101,83,104,97,100,101,114,83,116,97,116,101,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,57,66,97,99,107,97,98,108,101,82,101,110,100,101,114,83,116,97,116,101,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,56,71,76,69,83,50,83,76,83,104,97,100,101,114,83,116,97,116,101,69,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,55,71,76,69,83,50,86,101,114,116,101,120,70,111,114,109,97,116,69,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,55,71,76,69,83,50,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,55,71,76,69,83,50,82,101,110,100,101,114,68,101,118,105,99,101,69,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,54,71,76,69,83,50,82,101,110,100,101,114,83,116,97,116,101,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,54,71,76,69,83,50,80,105,120,101,108,66,117,102,102,101,114,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,54,69,71,76,50,82,101,110,100,101,114,84,97,114,103,101,116,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,53,66,97,99,107,97,98,108,101,84,101,120,116,117,114,101,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,52,86,97,114,105,97,98,108,101,66,117,102,102,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,52,66,97,99,107,97,98,108,101,83,104,97,100,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,52,66,97,99,107,97,98,108,101,66,117,102,102,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,51,84,101,120,116,117,114,101,70,111,114,109,97,116,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,51,71,76,69,83,50,83,76,83,104,97,100,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,86,101,114,116,101,120,70,111,114,109,97,116,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,86,101,114,116,101,120,66,117,102,102,101,114,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,84,97,114,103,101,116,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,68,101,118,105,99,101,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,50,71,76,69,83,50,84,101,120,116,117,114,101,69,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,83,104,97,100,101,114,83,116,97,116,101,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,82,101,110,100,101,114,83,116,97,116,101,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,80,105,120,101,108,66,117,102,102,101,114,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,73,110,100,101,120,66,117,102,102,101,114,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,49,71,76,69,83,50,66,117,102,102,101,114,69,0,0,78,55,116,111,97,100,108,101,116,54,112,101,101,112,101,114,49,48,86,101,114,116,101,120,68,97,116,97,69,0,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,57,84,114,97,99,101,97,98,108,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,57,83,105,109,117,108,97,116,111,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,55,77,97,110,97,103,101,114,69,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,53,83,111,108,105,100,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,53,83,104,97,112,101,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,104,111,112,49,55,67,111,108,108,105,115,105,111,110,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,57,73,110,116,101,114,102,97,99,101,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,57,69,120,99,101,112,116,105,111,110,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,56,82,101,115,111,117,114,99,101,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,56,73,116,101,114,97,116,111,114,73,78,83,48,95,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,55,69,114,114,111,114,101,114,69,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,54,83,116,114,105,110,103,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,54,79,98,106,101,99,116,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,54,76,111,103,103,101,114,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,53,69,114,114,111,114,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,109,97,116,104,55,86,101,99,116,111,114,52,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,109,97,116,104,55,86,101,99,116,111,114,51,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,109,97,116,104,55,86,101,99,116,111,114,50,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,109,97,116,104,49,53,77,97,116,104,73,110,105,116,105,97,108,105,122,101,114,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,109,97,116,104,49,48,81,117,97,116,101,114,110,105,111,110,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,52,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,51,76,111,103,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,105,111,54,83,116,114,101,97,109,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,105,111,49,50,77,101,109,111,114,121,83,116,114,101,97,109,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,105,111,49,48,70,105,108,101,83,116,114,101,97,109,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,105,111,49,48,68,97,116,97,83,116,114,101,97,109,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,53,82,101,115,111,117,114,99,101,68,101,115,116,114,111,121,101,100,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,50,48,80,111,105,110,116,101,114,73,116,101,114,97,116,111,114,82,97,110,103,101,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,69,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,87,114,97,112,80,111,105,110,116,101,114,73,116,101,114,97,116,111,114,73,78,83,48,95,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,78,83,48,95,49,55,80,111,105,110,116,101,114,67,111,108,108,101,99,116,105,111,110,73,83,52,95,69,56,105,116,101,114,97,116,111,114,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,78,111,110,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,55,83,107,101,108,101,116,111,110,67,111,109,112,111,110,101,110,116,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,78,111,110,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,53,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,78,111,110,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,78,111,110,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,51,66,97,115,101,67,111,109,112,111,110,101,110,116,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,57,78,111,110,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,50,72,111,112,67,111,109,112,111,110,101,110,116,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,56,83,116,97,99,107,84,114,97,99,101,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,56,66,97,115,101,76,111,103,103,101,114,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,55,80,111,115,105,120,69,114,114,111,114,72,97,110,100,108,101,114,69,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,83,116,97,110,100,97,114,100,76,105,115,116,101,110,101,114,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,57,84,114,97,110,115,102,111,114,109,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,57,67,111,109,112,111,110,101,110,116,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,56,109,97,116,101,114,105,97,108,56,77,97,116,101,114,105,97,108,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,54,69,110,103,105,110,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,54,67,97,109,101,114,97,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,53,83,99,101,110,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,52,77,101,115,104,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,49,53,67,97,109,101,114,97,67,111,109,112,111,110,101,110,116,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,49,51,80,97,114,116,105,116,105,111,110,78,111,100,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,55,116,97,100,112,111,108,101,49,51,77,101,115,104,67,111,109,112,111,110,101,110,116,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,54,112,101,101,112,101,114,55,84,101,120,116,117,114,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,84,97,114,103,101,116,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,54,112,101,101,112,101,114,49,50,82,101,110,100,101,114,68,101,118,105,99,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,54,112,101,101,112,101,114,49,49,82,101,110,100,101,114,83,116,97,116,101,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,116,114,117,115,105,118,101,80,111,105,110,116,101,114,73,78,83,95,51,104,111,112,57,83,105,109,117,108,97,116,111,114,69,78,83,48,95,50,53,68,101,102,97,117,108,116,73,110,116,114,117,115,105,118,101,83,101,109,97,110,116,105,99,115,69,69,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,52,78,111,100,101,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,54,73,110,115,116,97,110,116,105,97,98,108,101,84,121,112,101,73,78,83,95,55,116,97,100,112,111,108,101,49,51,80,97,114,116,105,116,105,111,110,78,111,100,101,69,78,83,50,95,57,67,111,109,112,111,110,101,110,116,69,69,69,0,0,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,52,80,111,105,110,116,101,114,67,111,117,110,116,101,114,69,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,52,76,111,103,103,101,114,76,105,115,116,101,110,101,114,69,0,0,78,55,116,111,97,100,108,101,116,51,101,103,103,49,50,66,97,115,101,82,101,115,111,117,114,99,101,69,0,0,0,0,78,49,48,101,109,115,99,114,105,112,116,101,110,51,118,97,108,69,0,0,0,0,0,0,78,49,48,101,109,115,99,114,105,112,116,101,110,49,49,109,101,109,111,114,121,95,118,105,101,119,69,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,50,51,95,95,102,117,110,100,97,109,101,110,116,97,108,95,116,121,112,101,95,105,110,102,111,69,0,78,49,48,95,95,99,120,120,97,98,105,118,49,50,49,95,95,118,109,105,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,50,48,95,95,115,105,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,57,95,95,112,111,105,110,116,101,114,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,55,95,95,112,98,97,115,101,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,55,95,95,99,108,97,115,115,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,78,49,48,95,95,99,120,120,97,98,105,118,49,49,54,95,95,115,104,105,109,95,116,121,112,101,95,105,110,102,111,69,0,0,0,0,0,0,0,0,68,110,0,0,0,0,0,0,104,138,0,0,184,138,0,0,104,138,0,0,88,42,0,0,0,0,0,0,16,139,0,0,0,0,0,0,32,139,0,0,0,0,0,0,48,139,0,0,192,182,0,0,0,0,0,0,0,0,0,0,64,139,0,0,0,0,0,0,160,185,0,0,0,0,0,0,96,139,0,0,0,0,0,0,176,185,0,0,0,0,0,0,128,139,0,0,0,0,0,0,192,185,0,0,0,0,0,0,168,139,0,0,0,0,0,0,24,188,0,0,0,0,0,0,200,139,0,0,0,0,0,0,40,188,0,0,0,0,0,0,232,139,0,0,0,0,0,0,88,188,0,0,0,0,0,0,8,140,0,0,0,0,0,0,136,188,0,0,0,0,0,0,40,140,0,0,0,0,0,0,168,188,0,0,0,0,0,0,72,140,0,0,0,0,0,0,0,191,0,0,0,0,0,0,112,140,0,0,0,0,0,0,120,191,0,0,0,0,0,0,152,140,0,0,0,0,0,0,136,191,0,0,0,0,0,0,192,140,0,0,0,0,0,0,0,192,0,0,0,0,0,0,232,140,0,0,0,0,0,0,176,193,0,0,0,0,0,0,8,141,0,0,0,0,0,0,216,193,0,0,0,0,0,0,40,141,0,0,0,0,0,0,240,196,0,0,0,0,0,0,80,141,0,0,0,0,0,0,0,197,0,0,0,0,0,0,120,141,0,0,0,0,0,0,64,197,0,0,0,0,0,0,152,141,0,0,0,0,0,0,192,197,0,0,0,0,0,0,184,141,0,0,0,0,0,0,88,198,0,0,0,0,0,0,208,141,0,0,0,0,0,0,144,198,0,0,0,0,0,0,232,141,0,0,0,0,0,0,16,199,0,0,0,0,0,0,40,142,0,0,1,0,0,0,160,185,0,0,0,0,0,0,72,142,0,0,1,0,0,0,176,185,0,0,0,0,0,0,104,142,0,0,1,0,0,0,192,185,0,0,0,0,0,0,144,142,0,0,1,0,0,0,24,188,0,0,0,0,0,0,176,142,0,0,1,0,0,0,40,188,0,0,0,0,0,0,208,142,0,0,1,0,0,0,88,188,0,0,0,0,0,0,240,142,0,0,1,0,0,0,136,188,0,0,0,0,0,0,16,143,0,0,1,0,0,0,168,188,0,0,0,0,0,0,48,143,0,0,1,0,0,0,0,191,0,0,0,0,0,0,88,143,0,0,1,0,0,0,120,191,0,0,0,0,0,0,128,143,0,0,1,0,0,0,136,191,0,0,0,0,0,0,168,143,0,0,1,0,0,0,0,192,0,0,0,0,0,0,208,143,0,0,1,0,0,0,216,193,0,0,0,0,0,0,240,143,0,0,1,0,0,0,240,196,0,0,0,0,0,0,24,144,0,0,1,0,0,0,0,197,0,0,0,0,0,0,64,144,0,0,1,0,0,0,64,197,0,0,0,0,0,0,104,144,0,0,1,0,0,0,192,197,0,0,0,0,0,0,136,144,0,0,1,0,0,0,88,198,0,0,0,0,0,0,160,144,0,0,1,0,0,0,144,198,0,0,0,0,0,0,184,144,0,0,1,0,0,0,16,199,0,0,0,0,0,0,248,144,0,0,144,138,0,0,32,145,0,0,0,0,0,0,1,0,0,0,104,185,0,0,0,0,0,0,144,138,0,0,96,145,0,0,0,0,0,0,1,0,0,0,104,185,0,0,0,0,0,0,0,0,0,0,160,145,0,0,64,198,0,0,0,0,0,0,0,0,0,0,192,145,0,0,0,198,0,0,0,0,0,0,0,0,0,0,224,145,0,0,88,200,0,0,0,0,0,0,0,0,0,0,8,146,0,0,48,187,0,0,0,0,0,0,0,0,0,0,64,146,0,0,48,187,0,0,0,0,0,0,0,0,0,0,120,146,0,0,48,187,0,0,0,0,0,0,0,0,0,0,176,146,0,0,48,187,0,0,0,0,0,0,0,0,0,0,232,146,0,0,48,187,0,0,0,0,0,0,0,0,0,0,32,147,0,0,48,187,0,0,0,0,0,0,0,0,0,0,88,147,0,0,48,187,0,0,0,0,0,0,0,0,0,0,144,147,0,0,48,187,0,0,0,0,0,0,0,0,0,0,200,147,0,0,48,187,0,0,0,0,0,0,0,0,0,0,0,148,0,0,48,187,0,0,0,0,0,0,0,0,0,0,56,148,0,0,48,187,0,0,0,0,0,0,0,0,0,0,112,148,0,0,48,187,0,0,0,0,0,0,0,0,0,0,168,148,0,0,48,187,0,0,0,0,0,0,0,0,0,0,224,148,0,0,48,187,0,0,0,0,0,0,0,0,0,0,24,149,0,0,48,187,0,0,0,0,0,0,0,0,0,0,80,149,0,0,48,187,0,0,0,0,0,0,0,0,0,0,128,149,0,0,48,187,0,0,0,0,0,0,0,0,0,0,176,149,0,0,48,187,0,0,0,0,0,0,0,0,0,0,224,149,0,0,48,187,0,0,0,0,0,0,0,0,0,0,16,150,0,0,64,198,0,0,0,0,0,0,0,0,0,0,64,150,0,0,48,187,0,0,0,0,0,0,0,0,0,0,112,150,0,0,48,187,0,0,0,0,0,0,0,0,0,0,160,150,0,0,0,0,0,0,208,150,0,0,64,198,0,0,0,0,0,0,0,0,0,0,248,150,0,0,64,198,0,0,0,0,0,0,0,0,0,0,32,151,0,0,88,200,0,0,0,0,0,0,0,0,0,0,64,151,0,0,64,198,0,0,0,0,0,0,0,0,0,0,104,151,0,0,88,200,0,0,0,0,0,0,0,0,0,0,136,151,0,0,0,0,0,0,168,151,0,0,0,0,0,0,200,151,0,0,0,198,0,0,0,0,0,0,144,138,0,0,240,151,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,152,187,0,0,2,12,0,0,144,138,0,0,32,152,0,0,0,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,40,189,0,0,2,12,0,0,0,0,0,0,80,152,0,0,0,198,0,0,0,0,0,0,144,138,0,0,120,152,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,232,187,0,0,2,12,0,0,0,0,0,0,168,152,0,0,64,198,0,0,0,0,0,0,0,0,0,0,200,152,0,0,64,198,0,0,0,0,0,0,144,138,0,0,232,152,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,248,193,0,0,2,36,0,0,144,138,0,0,8,153,0,0,0,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,248,190,0,0,2,12,0,0,0,0,0,0,40,153,0,0,64,198,0,0,0,0,0,0,144,138,0,0,72,153,0,0,1,0,0,0,2,0,0,0,0,192,0,0,2,0,0,0,144,189,0,0,2,36,0,0,0,0,0,0,96,153,0,0,88,200,0,0,0,0,0,0,0,0,0,0,120,153,0,0,88,200,0,0,0,0,0,0,144,138,0,0,152,153,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,200,153,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,248,153,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,0,0,0,0,40,154,0,0,144,138,0,0,88,154,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,136,154,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,104,191,0,0,2,12,0,0,144,138,0,0,176,154,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,0,0,0,0,216,154,0,0,144,138,0,0,0,155,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,40,155,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,80,155,0,0,1,0,0,0,6,0,0,0,0,192,0,0,2,0,0,0,144,189,0,0,2,36,0,0,144,187,0,0,2,40,0,0,80,193,0,0,2,44,0,0,144,193,0,0,2,48,0,0,152,193,0,0,2,52,0,0,0,0,0,0,120,155,0,0,248,187,0,0,0,0,0,0,144,138,0,0,184,155,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,0,0,0,0,224,155,0,0,0,198,0,0,0,0,0,0,144,138,0,0,8,156,0,0,0,0,0,0,2,0,0,0,176,185,0,0,2,0,0,0,144,187,0,0,2,4,0,0,144,138,0,0,48,156,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,144,138,0,0,88,156,0,0,0,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,8,199,0,0,2,12,0,0,0,0,0,0,128,156,0,0,0,198,0,0,0,0,0,0,0,0,0,0,168,156,0,0,152,190,0,0,0,0,0,0,144,138,0,0,208,156,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,184,190,0,0,2,12,0,0,0,0,0,0,248,156,0,0,144,138,0,0,32,157,0,0,1,0,0,0,2,0,0,0,0,192,0,0,2,0,0,0,144,187,0,0,2,36,0,0,0,0,0,0,72,157,0,0,152,190,0,0,0,0,0,0,0,0,0,0,112,157,0,0,0,198,0,0,0,0,0,0,0,0,0,0,152,157,0,0,152,190,0,0,0,0,0,0,0,0,0,0,192,157,0,0,152,190,0,0,0,0,0,0,0,0,0,0,232,157,0,0,0,0,0,0,16,158,0,0,0,198,0,0,0,0,0,0,0,0,0,0,56,158,0,0,136,188,0,0,0,0,0,0,144,138,0,0,96,158,0,0,1,0,0,0,5,0,0,0,0,192,0,0,2,0,0,0,144,189,0,0,2,36,0,0,144,187,0,0,2,40,0,0,136,187,0,0,2,44,0,0,144,193,0,0,2,48,0,0,144,138,0,0,136,158,0,0,0,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,80,193,0,0,2,12,0,0,144,138,0,0,184,158,0,0,0,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,8,199,0,0,2,12,0,0,144,138,0,0,224,158,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,176,185,0,0,2,12,0,0,0,0,0,0,8,159,0,0,72,190,0,0,0,0,0,0,144,138,0,0,48,159,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,32,192,0,0,2,12,0,0,144,138,0,0,88,159,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,72,190,0,0,2,12,0,0,144,138,0,0,128,159,0,0,1,0,0,0,4,0,0,0,0,192,0,0,2,0,0,0,88,190,0,0,2,36,0,0,248,197,0,0,2,44,0,0,184,197,0,0,2,48,0,0,0,0,0,0,168,159,0,0,152,190,0,0,0,0,0,0,144,138,0,0,208,159,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,160,193,0,0,2,12,0,0,144,138,0,0,240,159,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,72,190,0,0,2,12,0,0,0,0,0,0,16,160,0,0,64,198,0,0,0,0,0,0,0,0,0,0,48,160,0,0,152,190,0,0,0,0,0,0,144,138,0,0,80,160,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,72,190,0,0,2,12,0,0,144,138,0,0,112,160,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,72,190,0,0,2,12,0,0,0,0,0,0,144,160,0,0,144,138,0,0,176,160,0,0,1,0,0,0,3,0,0,0,64,198,0,0,2,0,0,0,48,191,0,0,2,12,0,0,208,197,0,0,2,16,0,0,0,0,0,0,208,160,0,0,40,189,0,0,0,0,0,0,0,0,0,0,8,161,0,0,0,0,0,0,40,161,0,0,0,0,0,0,72,161,0,0,152,198,0,0,0,0,0,0,0,0,0,0,104,161,0,0,0,198,0,0,0,0,0,0,0,0,0,0,136,161,0,0,64,198,0,0,0,0,0,0,0,0,0,0,168,161,0,0,0,0,0,0,200,161,0,0,16,198,0,0,0,0,0,0,0,0,0,0,232,161,0,0,16,198,0,0,0,0,0,0,0,0,0,0,8,162,0,0,16,198,0,0,0,0,0,0,144,138,0,0,40,162,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,80,197,0,0,2,36,0,0,144,138,0,0,96,162,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,248,195,0,0,2,36,0,0,0,0,0,0,144,162,0,0,240,196,0,0,0,0,0,0,144,138,0,0,192,162,0,0,1,0,0,0,2,0,0,0,8,196,0,0,2,0,0,0,72,194,0,0,2,60,0,0,0,0,0,0,240,162,0,0,8,196,0,0,0,0,0,0,0,0,0,0,32,163,0,0,0,0,0,0,72,163,0,0,64,198,0,0,0,0,0,0,144,138,0,0,112,163,0,0,1,0,0,0,2,0,0,0,152,195,0,0,2,0,0,0,72,194,0,0,2,44,0,0,144,138,0,0,152,163,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,208,196,0,0,2,36,0,0,0,0,0,0,192,163,0,0,0,0,0,0,232,163,0,0,64,198,0,0,0,0,0,0,144,138,0,0,16,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,248,195,0,0,2,36,0,0,144,138,0,0,56,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,48,197,0,0,2,36,0,0,144,138,0,0,96,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,64,197,0,0,2,36,0,0,144,138,0,0,136,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,48,197,0,0,2,36,0,0,144,138,0,0,176,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,208,196,0,0,2,36,0,0,144,138,0,0,216,164,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,240,196,0,0,2,36,0,0,144,138,0,0,0,165,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,0,197,0,0,2,12,0,0,144,138,0,0,40,165,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,64,197,0,0,2,36,0,0,0,0,0,0,80,165,0,0,80,197,0,0,0,0,0,0,0,0,0,0,120,165,0,0,152,195,0,0,0,0,0,0,144,138,0,0,160,165,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,216,193,0,0,2,36,0,0,0,0,0,0,200,165,0,0,248,193,0,0,0,0,0,0,144,138,0,0,240,165,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,232,193,0,0,2,36,0,0,144,138,0,0,24,166,0,0,1,0,0,0,5,0,0,0,88,200,0,0,2,0,0,0,96,197,0,0,2,36,0,0,224,196,0,0,2,40,0,0,80,197,0,0,2,44,0,0,56,196,0,0,2,48,0,0,0,0,0,0,64,166,0,0,64,198,0,0,0,0,0,0,144,138,0,0,104,166,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,232,193,0,0,2,36,0,0,0,0,0,0,144,166,0,0,16,198,0,0,0,0,0,0,0,0,0,0,176,166,0,0,248,193,0,0,0,0,0,0,0,0,0,0,208,166,0,0,16,198,0,0,0,0,0,0,0,0,0,0,240,166,0,0,0,198,0,0,0,0,0,0,144,138,0,0,16,167,0,0,1,0,0,0,2,0,0,0,88,200,0,0,2,0,0,0,216,193,0,0,2,36,0,0,0,0,0,0,48,167,0,0,16,198,0,0,0,0,0,0,0,0,0,0,80,167,0,0,16,198,0,0,0,0,0,0,0,0,0,0,112,167,0,0,248,193,0,0,0,0,0,0,0,0,0,0,144,167,0,0,248,193,0,0,0,0,0,0,144,138,0,0,176,167,0,0,1,0,0,0,5,0,0,0,88,200,0,0,2,0,0,0,96,197,0,0,2,36,0,0,224,196,0,0,2,40,0,0,248,195,0,0,2,44,0,0,56,196,0,0,2,48,0,0,0,0,0,0,208,167,0,0,64,198,0,0,0,0,0,0,0,0,0,0,240,167,0,0,0,0,0,0,16,168,0,0,64,198,0,0,0,0,0,0,0,0,0,0,48,168,0,0,0,0,0,0,72,168,0,0,64,198,0,0,0,0,0,0,0,0,0,0,96,168,0,0,64,198,0,0,0,0,0,0,0,0,0,0,120,168,0,0,0,0,0,0,160,168,0,0,0,0,0,0,192,168,0,0,0,0,0,0,224,168,0,0,0,198,0,0,0,0,0,0,0,0,0,0,248,168,0,0,0,0,0,0,96,169,0,0,120,199,0,0,0,0,0,0,0,0,0,0,120,169,0,0,0,0,0,0,144,169,0,0,0,198,0,0,0,0,0,0,0,0,0,0,168,169,0,0,0,0,0,0,192,169,0,0,0,0,0,0,216,169,0,0,0,0,0,0,248,169,0,0,0,0,0,0,24,170,0,0,0,0,0,0,56,170,0,0,0,0,0,0,96,170,0,0,0,0,0,0,128,170,0,0,0,0,0,0,176,170,0,0,0,0,0,0,200,170,0,0,0,198,0,0,0,0,0,0,144,138,0,0,232,170,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,152,198,0,0,2,12,0,0,144,138,0,0,8,171,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,152,198,0,0,2,12,0,0,144,138,0,0,40,171,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,152,198,0,0,2,12,0,0,0,0,0,0,72,171,0,0,0,0,0,0,120,171,0,0,0,0,0,0,176,171,0,0,32,198,0,0,0,0,0,0,0,0,0,0,72,172,0,0,136,198,0,0,0,0,0,0,0,0,0,0,160,172,0,0,136,198,0,0,0,0,0,0,0,0,0,0,248,172,0,0,136,198,0,0,0,0,0,0,0,0,0,0,72,173,0,0,136,198,0,0,0,0,0,0,0,0,0,0,152,173,0,0,136,198,0,0,0,0,0,0,0,0,0,0,232,173,0,0,0,0,0,0,16,174,0,0,80,200,0,0,0,0,0,0,0,0,0,0,56,174,0,0,0,0,0,0,96,174,0,0,128,199,0,0,0,0,0,0,0,0,0,0,136,174,0,0,0,0,0,0,232,174,0,0,0,0,0,0,72,175,0,0,0,0,0,0,176,175,0,0,0,0,0,0,8,176,0,0,0,0,0,0,96,176,0,0,0,0,0,0,184,176,0,0,0,0,0,0,16,177,0,0,0,0,0,0,104,177,0,0,0,0,0,0,200,177,0,0,0,0,0,0,40,178,0,0,0,0,0,0,136,178,0,0])
.concat([0,0,0,0,224,178,0,0,0,0,0,0,64,179,0,0,0,0,0,0,160,179,0,0,0,0,0,0,0,180,0,0,0,0,0,0,88,180,0,0,136,198,0,0,0,0,0,0,0,0,0,0,160,180,0,0,136,198,0,0,0,0,0,0,0,0,0,0,240,180,0,0,0,0,0,0,16,181,0,0,144,138,0,0,48,181,0,0,1,0,0,0,2,0,0,0,64,198,0,0,2,0,0,0,16,198,0,0,2,12,0,0,0,0,0,0,80,181,0,0,0,0,0,0,104,181,0,0,0,0,0,0,136,181,0,0,232,200,0,0,0,0,0,0,0,0,0,0,176,181,0,0,216,200,0,0,0,0,0,0,0,0,0,0,216,181,0,0,216,200,0,0,0,0,0,0,0,0,0,0,0,182,0,0,200,200,0,0,0,0,0,0,0,0,0,0,40,182,0,0,232,200,0,0,0,0,0,0,0,0,0,0,80,182,0,0,232,200,0,0,0,0,0,0,0,0,0,0,120,182,0,0,184,182,0,0,0,0,0,0,104,138,0,0,160,182,0,0,21,133,0,0,22,133,0,0,23,133,0,0,24,133,0,0,25,133,0,0,26,133,0,0,3,0,0,0,4,0,0,0,5,0,0,0,8,0,0,0,7,0,0,0,11,0,0,0,31,0,0,0,13,0,0,0,14,0,0,0,24,0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])
, "i8", ALLOC_NONE, Runtime.GLOBAL_BASE)
function runPostSets() {
HEAP32[((46776 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((46784 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((46792 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((46808 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46824 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46840 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46856 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46872 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46888 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46904 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46920 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46936 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46952 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46968 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((46984 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47000 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47016 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47032 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47048 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47064 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47080 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47096 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47112 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47128 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47144 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47160 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47176 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47192 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47208 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47224 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47240 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47256 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47272 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47288 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47304 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47320 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47336 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47352 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47368 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47384 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47400 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47416 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47432 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47448 )>>2)]=(((__ZTVN10__cxxabiv119__pointer_type_infoE+8)|0));
HEAP32[((47464 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((47520 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47536 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47552 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47568 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47584 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47600 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47616 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47632 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47648 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47664 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47680 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47696 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47712 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47728 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47744 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47760 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47776 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47792 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47808 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47824 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47840 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47856 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47872 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47888 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47904 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47920 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((47928 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47944 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47960 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47976 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((47992 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48008 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((48016 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((48024 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48104 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48152 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48168 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48248 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48296 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48312 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48424 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((48528 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((48664 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48712 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48824 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48840 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48888 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((48928 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48944 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48960 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48976 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((48992 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49000 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49016 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49184 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49312 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49392 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49408 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49488 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49536 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49552 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49560 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49568 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49584 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49600 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49616 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49624 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49640 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49656 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49736 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49784 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49800 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49808 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((49888 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((49896 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50168 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50184 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50232 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50336 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50384 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50400 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50416 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50432 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50480 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50496 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50512 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50528 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50600 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50616 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50624 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50640 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50648 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50664 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50680 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50688 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50696 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50704 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50720 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50728 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50744 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50752 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50768 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50776 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50784 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50792 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50800 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50808 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50816 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50824 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50832 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50840 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50952 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50960 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((50968 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((50984 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51000 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51016 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51032 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51048 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51064 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51072 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51088 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51096 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51112 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51120 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51128 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51136 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51144 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51152 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51160 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51168 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51176 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51184 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51192 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51200 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51208 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51216 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51224 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51232 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51240 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51256 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51272 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51280 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51320 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51328 )>>2)]=(((__ZTVN10__cxxabiv117__class_type_infoE+8)|0));
HEAP32[((51336 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51352 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51368 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51384 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51400 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51416 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
HEAP32[((51432 )>>2)]=(((__ZTVN10__cxxabiv120__si_class_type_infoE+8)|0));
}
var tempDoublePtr = Runtime.alignMemory(allocate(12, "i8", ALLOC_STATIC), 8);
assert(tempDoublePtr % 8 == 0);
function copyTempFloat(ptr) { // functions, because inlining this code increases code size too much
  HEAP8[tempDoublePtr] = HEAP8[ptr];
  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];
  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];
  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];
}
function copyTempDouble(ptr) {
  HEAP8[tempDoublePtr] = HEAP8[ptr];
  HEAP8[tempDoublePtr+1] = HEAP8[ptr+1];
  HEAP8[tempDoublePtr+2] = HEAP8[ptr+2];
  HEAP8[tempDoublePtr+3] = HEAP8[ptr+3];
  HEAP8[tempDoublePtr+4] = HEAP8[ptr+4];
  HEAP8[tempDoublePtr+5] = HEAP8[ptr+5];
  HEAP8[tempDoublePtr+6] = HEAP8[ptr+6];
  HEAP8[tempDoublePtr+7] = HEAP8[ptr+7];
}
  function _atexit(func, arg) {
      __ATEXIT__.unshift({ func: func, arg: arg });
    }var ___cxa_atexit=_atexit;
;
  function ___gxx_personality_v0() {
    }
;
;
;
;
  function __exit(status) {
      // void _exit(int status);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/exit.html
      Module['exit'](status);
    }function _exit(status) {
      __exit(status);
    }function __ZSt9terminatev() {
      _exit(-1234);
    }
;
;
;
;
;
;
;
;
;
;
;
;
  function _memcpy(dest, src, num) {
      dest = dest|0; src = src|0; num = num|0;
      var ret = 0;
      ret = dest|0;
      if ((dest&3) == (src&3)) {
        while (dest & 3) {
          if ((num|0) == 0) return ret|0;
          HEAP8[(dest)]=HEAP8[(src)];
          dest = (dest+1)|0;
          src = (src+1)|0;
          num = (num-1)|0;
        }
        while ((num|0) >= 4) {
          HEAP32[((dest)>>2)]=HEAP32[((src)>>2)];
          dest = (dest+4)|0;
          src = (src+4)|0;
          num = (num-4)|0;
        }
      }
      while ((num|0) > 0) {
        HEAP8[(dest)]=HEAP8[(src)];
        dest = (dest+1)|0;
        src = (src+1)|0;
        num = (num-1)|0;
      }
      return ret|0;
    }var _llvm_memcpy_p0i8_p0i8_i32=_memcpy;
  function _memset(ptr, value, num) {
      ptr = ptr|0; value = value|0; num = num|0;
      var stop = 0, value4 = 0, stop4 = 0, unaligned = 0;
      stop = (ptr + num)|0;
      if ((num|0) >= 20) {
        // This is unaligned, but quite large, so work hard to get to aligned settings
        value = value & 0xff;
        unaligned = ptr & 3;
        value4 = value | (value << 8) | (value << 16) | (value << 24);
        stop4 = stop & ~3;
        if (unaligned) {
          unaligned = (ptr + 4 - unaligned)|0;
          while ((ptr|0) < (unaligned|0)) { // no need to check for stop, since we have large num
            HEAP8[(ptr)]=value;
            ptr = (ptr+1)|0;
          }
        }
        while ((ptr|0) < (stop4|0)) {
          HEAP32[((ptr)>>2)]=value4;
          ptr = (ptr+4)|0;
        }
      }
      while ((ptr|0) < (stop|0)) {
        HEAP8[(ptr)]=value;
        ptr = (ptr+1)|0;
      }
    }var _llvm_memset_p0i8_i32=_memset;
  function ___cxa_pure_virtual() {
      ABORT = true;
      throw 'Pure virtual function called!';
    }
  function ___cxa_call_unexpected(exception) {
      Module.printErr('Unexpected exception thrown, this is not properly supported - aborting');
      ABORT = true;
      throw exception;
    }
  function _llvm_umul_with_overflow_i32(x, y) {
      x = x>>>0;
      y = y>>>0;
      return tempRet0 = x*y > 4294967295,(x*y)>>>0;
    }
  var ERRNO_CODES={EPERM:1,ENOENT:2,ESRCH:3,EINTR:4,EIO:5,ENXIO:6,E2BIG:7,ENOEXEC:8,EBADF:9,ECHILD:10,EAGAIN:11,EWOULDBLOCK:11,ENOMEM:12,EACCES:13,EFAULT:14,ENOTBLK:15,EBUSY:16,EEXIST:17,EXDEV:18,ENODEV:19,ENOTDIR:20,EISDIR:21,EINVAL:22,ENFILE:23,EMFILE:24,ENOTTY:25,ETXTBSY:26,EFBIG:27,ENOSPC:28,ESPIPE:29,EROFS:30,EMLINK:31,EPIPE:32,EDOM:33,ERANGE:34,ENOMSG:42,EIDRM:43,ECHRNG:44,EL2NSYNC:45,EL3HLT:46,EL3RST:47,ELNRNG:48,EUNATCH:49,ENOCSI:50,EL2HLT:51,EDEADLK:35,ENOLCK:37,EBADE:52,EBADR:53,EXFULL:54,ENOANO:55,EBADRQC:56,EBADSLT:57,EDEADLOCK:35,EBFONT:59,ENOSTR:60,ENODATA:61,ETIME:62,ENOSR:63,ENONET:64,ENOPKG:65,EREMOTE:66,ENOLINK:67,EADV:68,ESRMNT:69,ECOMM:70,EPROTO:71,EMULTIHOP:72,EDOTDOT:73,EBADMSG:74,ENOTUNIQ:76,EBADFD:77,EREMCHG:78,ELIBACC:79,ELIBBAD:80,ELIBSCN:81,ELIBMAX:82,ELIBEXEC:83,ENOSYS:38,ENOTEMPTY:39,ENAMETOOLONG:36,ELOOP:40,EOPNOTSUPP:95,EPFNOSUPPORT:96,ECONNRESET:104,ENOBUFS:105,EAFNOSUPPORT:97,EPROTOTYPE:91,ENOTSOCK:88,ENOPROTOOPT:92,ESHUTDOWN:108,ECONNREFUSED:111,EADDRINUSE:98,ECONNABORTED:103,ENETUNREACH:101,ENETDOWN:100,ETIMEDOUT:110,EHOSTDOWN:112,EHOSTUNREACH:113,EINPROGRESS:115,EALREADY:114,EDESTADDRREQ:89,EMSGSIZE:90,EPROTONOSUPPORT:93,ESOCKTNOSUPPORT:94,EADDRNOTAVAIL:99,ENETRESET:102,EISCONN:106,ENOTCONN:107,ETOOMANYREFS:109,EUSERS:87,EDQUOT:122,ESTALE:116,ENOTSUP:95,ENOMEDIUM:123,EILSEQ:84,EOVERFLOW:75,ECANCELED:125,ENOTRECOVERABLE:131,EOWNERDEAD:130,ESTRPIPE:86};
  var ERRNO_MESSAGES={0:"Success",1:"Not super-user",2:"No such file or directory",3:"No such process",4:"Interrupted system call",5:"I/O error",6:"No such device or address",7:"Arg list too long",8:"Exec format error",9:"Bad file number",10:"No children",11:"No more processes",12:"Not enough core",13:"Permission denied",14:"Bad address",15:"Block device required",16:"Mount device busy",17:"File exists",18:"Cross-device link",19:"No such device",20:"Not a directory",21:"Is a directory",22:"Invalid argument",23:"Too many open files in system",24:"Too many open files",25:"Not a typewriter",26:"Text file busy",27:"File too large",28:"No space left on device",29:"Illegal seek",30:"Read only file system",31:"Too many links",32:"Broken pipe",33:"Math arg out of domain of func",34:"Math result not representable",35:"File locking deadlock error",36:"File or path name too long",37:"No record locks available",38:"Function not implemented",39:"Directory not empty",40:"Too many symbolic links",42:"No message of desired type",43:"Identifier removed",44:"Channel number out of range",45:"Level 2 not synchronized",46:"Level 3 halted",47:"Level 3 reset",48:"Link number out of range",49:"Protocol driver not attached",50:"No CSI structure available",51:"Level 2 halted",52:"Invalid exchange",53:"Invalid request descriptor",54:"Exchange full",55:"No anode",56:"Invalid request code",57:"Invalid slot",59:"Bad font file fmt",60:"Device not a stream",61:"No data (for no delay io)",62:"Timer expired",63:"Out of streams resources",64:"Machine is not on the network",65:"Package not installed",66:"The object is remote",67:"The link has been severed",68:"Advertise error",69:"Srmount error",70:"Communication error on send",71:"Protocol error",72:"Multihop attempted",73:"Cross mount point (not really error)",74:"Trying to read unreadable message",75:"Value too large for defined data type",76:"Given log. name not unique",77:"f.d. invalid for this operation",78:"Remote address changed",79:"Can   access a needed shared lib",80:"Accessing a corrupted shared lib",81:".lib section in a.out corrupted",82:"Attempting to link in too many libs",83:"Attempting to exec a shared library",84:"Illegal byte sequence",86:"Streams pipe error",87:"Too many users",88:"Socket operation on non-socket",89:"Destination address required",90:"Message too long",91:"Protocol wrong type for socket",92:"Protocol not available",93:"Unknown protocol",94:"Socket type not supported",95:"Not supported",96:"Protocol family not supported",97:"Address family not supported by protocol family",98:"Address already in use",99:"Address not available",100:"Network interface is not configured",101:"Network is unreachable",102:"Connection reset by network",103:"Connection aborted",104:"Connection reset by peer",105:"No buffer space available",106:"Socket is already connected",107:"Socket is not connected",108:"Can't send after socket shutdown",109:"Too many references",110:"Connection timed out",111:"Connection refused",112:"Host is down",113:"Host is unreachable",114:"Socket already connected",115:"Connection already in progress",116:"Stale file handle",122:"Quota exceeded",123:"No medium (in tape drive)",125:"Operation canceled",130:"Previous owner died",131:"State not recoverable"};
  var ___errno_state=0;function ___setErrNo(value) {
      // For convenient setting and returning of errno.
      HEAP32[((___errno_state)>>2)]=value
      return value;
    }
  var PATH={splitPath:function (filename) {
        var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
        return splitPathRe.exec(filename).slice(1);
      },normalizeArray:function (parts, allowAboveRoot) {
        // if the path tries to go above the root, `up` ends up > 0
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
          var last = parts[i];
          if (last === '.') {
            parts.splice(i, 1);
          } else if (last === '..') {
            parts.splice(i, 1);
            up++;
          } else if (up) {
            parts.splice(i, 1);
            up--;
          }
        }
        // if the path is allowed to go above the root, restore leading ..s
        if (allowAboveRoot) {
          for (; up--; up) {
            parts.unshift('..');
          }
        }
        return parts;
      },normalize:function (path) {
        var isAbsolute = path.charAt(0) === '/',
            trailingSlash = path.substr(-1) === '/';
        // Normalize the path
        path = PATH.normalizeArray(path.split('/').filter(function(p) {
          return !!p;
        }), !isAbsolute).join('/');
        if (!path && !isAbsolute) {
          path = '.';
        }
        if (path && trailingSlash) {
          path += '/';
        }
        return (isAbsolute ? '/' : '') + path;
      },dirname:function (path) {
        var result = PATH.splitPath(path),
            root = result[0],
            dir = result[1];
        if (!root && !dir) {
          // No dirname whatsoever
          return '.';
        }
        if (dir) {
          // It has a dirname, strip trailing slash
          dir = dir.substr(0, dir.length - 1);
        }
        return root + dir;
      },basename:function (path) {
        // EMSCRIPTEN return '/'' for '/', not an empty string
        if (path === '/') return '/';
        var lastSlash = path.lastIndexOf('/');
        if (lastSlash === -1) return path;
        return path.substr(lastSlash+1);
      },extname:function (path) {
        return PATH.splitPath(path)[3];
      },join:function () {
        var paths = Array.prototype.slice.call(arguments, 0);
        return PATH.normalize(paths.join('/'));
      },join2:function (l, r) {
        return PATH.normalize(l + '/' + r);
      },resolve:function () {
        var resolvedPath = '',
          resolvedAbsolute = false;
        for (var i = arguments.length - 1; i >= -1 && !resolvedAbsolute; i--) {
          var path = (i >= 0) ? arguments[i] : FS.cwd();
          // Skip empty and invalid entries
          if (typeof path !== 'string') {
            throw new TypeError('Arguments to path.resolve must be strings');
          } else if (!path) {
            continue;
          }
          resolvedPath = path + '/' + resolvedPath;
          resolvedAbsolute = path.charAt(0) === '/';
        }
        // At this point the path should be resolved to a full absolute path, but
        // handle relative paths to be safe (might happen when process.cwd() fails)
        resolvedPath = PATH.normalizeArray(resolvedPath.split('/').filter(function(p) {
          return !!p;
        }), !resolvedAbsolute).join('/');
        return ((resolvedAbsolute ? '/' : '') + resolvedPath) || '.';
      },relative:function (from, to) {
        from = PATH.resolve(from).substr(1);
        to = PATH.resolve(to).substr(1);
        function trim(arr) {
          var start = 0;
          for (; start < arr.length; start++) {
            if (arr[start] !== '') break;
          }
          var end = arr.length - 1;
          for (; end >= 0; end--) {
            if (arr[end] !== '') break;
          }
          if (start > end) return [];
          return arr.slice(start, end - start + 1);
        }
        var fromParts = trim(from.split('/'));
        var toParts = trim(to.split('/'));
        var length = Math.min(fromParts.length, toParts.length);
        var samePartsLength = length;
        for (var i = 0; i < length; i++) {
          if (fromParts[i] !== toParts[i]) {
            samePartsLength = i;
            break;
          }
        }
        var outputParts = [];
        for (var i = samePartsLength; i < fromParts.length; i++) {
          outputParts.push('..');
        }
        outputParts = outputParts.concat(toParts.slice(samePartsLength));
        return outputParts.join('/');
      }};
  var TTY={ttys:[],init:function () {
        // https://github.com/kripken/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // currently, FS.init does not distinguish if process.stdin is a file or TTY
        //   // device, it always assumes it's a TTY device. because of this, we're forcing
        //   // process.stdin to UTF8 encoding to at least make stdin reading compatible
        //   // with text files until FS.init can be refactored.
        //   process['stdin']['setEncoding']('utf8');
        // }
      },shutdown:function () {
        // https://github.com/kripken/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // inolen: any idea as to why node -e 'process.stdin.read()' wouldn't exit immediately (with process.stdin being a tty)?
        //   // isaacs: because now it's reading from the stream, you've expressed interest in it, so that read() kicks off a _read() which creates a ReadReq operation
        //   // inolen: I thought read() in that case was a synchronous operation that just grabbed some amount of buffered data if it exists?
        //   // isaacs: it is. but it also triggers a _read() call, which calls readStart() on the handle
        //   // isaacs: do process.stdin.pause() and i'd think it'd probably close the pending call
        //   process['stdin']['pause']();
        // }
      },register:function (dev, ops) {
        TTY.ttys[dev] = { input: [], output: [], ops: ops };
        FS.registerDevice(dev, TTY.stream_ops);
      },stream_ops:{open:function (stream) {
          var tty = TTY.ttys[stream.node.rdev];
          if (!tty) {
            throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
          }
          stream.tty = tty;
          stream.seekable = false;
        },close:function (stream) {
          // flush any pending line data
          if (stream.tty.output.length) {
            stream.tty.ops.put_char(stream.tty, 10);
          }
        },read:function (stream, buffer, offset, length, pos /* ignored */) {
          if (!stream.tty || !stream.tty.ops.get_char) {
            throw new FS.ErrnoError(ERRNO_CODES.ENXIO);
          }
          var bytesRead = 0;
          for (var i = 0; i < length; i++) {
            var result;
            try {
              result = stream.tty.ops.get_char(stream.tty);
            } catch (e) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
            if (result === undefined && bytesRead === 0) {
              throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
            }
            if (result === null || result === undefined) break;
            bytesRead++;
            buffer[offset+i] = result;
          }
          if (bytesRead) {
            stream.node.timestamp = Date.now();
          }
          return bytesRead;
        },write:function (stream, buffer, offset, length, pos) {
          if (!stream.tty || !stream.tty.ops.put_char) {
            throw new FS.ErrnoError(ERRNO_CODES.ENXIO);
          }
          for (var i = 0; i < length; i++) {
            try {
              stream.tty.ops.put_char(stream.tty, buffer[offset+i]);
            } catch (e) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
          }
          if (length) {
            stream.node.timestamp = Date.now();
          }
          return i;
        }},default_tty_ops:{get_char:function (tty) {
          if (!tty.input.length) {
            var result = null;
            if (ENVIRONMENT_IS_NODE) {
              result = process['stdin']['read']();
              if (!result) {
                if (process['stdin']['_readableState'] && process['stdin']['_readableState']['ended']) {
                  return null;  // EOF
                }
                return undefined;  // no data available
              }
            } else if (typeof window != 'undefined' &&
              typeof window.prompt == 'function') {
              // Browser.
              result = window.prompt('Input: ');  // returns null on cancel
              if (result !== null) {
                result += '\n';
              }
            } else if (typeof readline == 'function') {
              // Command line.
              result = readline();
              if (result !== null) {
                result += '\n';
              }
            }
            if (!result) {
              return null;
            }
            tty.input = intArrayFromString(result, true);
          }
          return tty.input.shift();
        },put_char:function (tty, val) {
          if (val === null || val === 10) {
            Module['print'](tty.output.join(''));
            tty.output = [];
          } else {
            tty.output.push(TTY.utf8.processCChar(val));
          }
        }},default_tty1_ops:{put_char:function (tty, val) {
          if (val === null || val === 10) {
            Module['printErr'](tty.output.join(''));
            tty.output = [];
          } else {
            tty.output.push(TTY.utf8.processCChar(val));
          }
        }}};
  var MEMFS={ops_table:null,CONTENT_OWNING:1,CONTENT_FLEXIBLE:2,CONTENT_FIXED:3,mount:function (mount) {
        return MEMFS.createNode(null, '/', 16384 | 0777, 0);
      },createNode:function (parent, name, mode, dev) {
        if (FS.isBlkdev(mode) || FS.isFIFO(mode)) {
          // no supported
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (!MEMFS.ops_table) {
          MEMFS.ops_table = {
            dir: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr,
                lookup: MEMFS.node_ops.lookup,
                mknod: MEMFS.node_ops.mknod,
                mknod: MEMFS.node_ops.mknod,
                rename: MEMFS.node_ops.rename,
                unlink: MEMFS.node_ops.unlink,
                rmdir: MEMFS.node_ops.rmdir,
                readdir: MEMFS.node_ops.readdir,
                symlink: MEMFS.node_ops.symlink
              },
              stream: {
                llseek: MEMFS.stream_ops.llseek
              }
            },
            file: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr
              },
              stream: {
                llseek: MEMFS.stream_ops.llseek,
                read: MEMFS.stream_ops.read,
                write: MEMFS.stream_ops.write,
                allocate: MEMFS.stream_ops.allocate,
                mmap: MEMFS.stream_ops.mmap
              }
            },
            link: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr,
                readlink: MEMFS.node_ops.readlink
              },
              stream: {}
            },
            chrdev: {
              node: {
                getattr: MEMFS.node_ops.getattr,
                setattr: MEMFS.node_ops.setattr
              },
              stream: FS.chrdev_stream_ops
            },
          };
        }
        var node = FS.createNode(parent, name, mode, dev);
        if (FS.isDir(node.mode)) {
          node.node_ops = MEMFS.ops_table.dir.node;
          node.stream_ops = MEMFS.ops_table.dir.stream;
          node.contents = {};
        } else if (FS.isFile(node.mode)) {
          node.node_ops = MEMFS.ops_table.file.node;
          node.stream_ops = MEMFS.ops_table.file.stream;
          node.contents = [];
          node.contentMode = MEMFS.CONTENT_FLEXIBLE;
        } else if (FS.isLink(node.mode)) {
          node.node_ops = MEMFS.ops_table.link.node;
          node.stream_ops = MEMFS.ops_table.link.stream;
        } else if (FS.isChrdev(node.mode)) {
          node.node_ops = MEMFS.ops_table.chrdev.node;
          node.stream_ops = MEMFS.ops_table.chrdev.stream;
        }
        node.timestamp = Date.now();
        // add the new node to the parent
        if (parent) {
          parent.contents[name] = node;
        }
        return node;
      },ensureFlexible:function (node) {
        if (node.contentMode !== MEMFS.CONTENT_FLEXIBLE) {
          var contents = node.contents;
          node.contents = Array.prototype.slice.call(contents);
          node.contentMode = MEMFS.CONTENT_FLEXIBLE;
        }
      },node_ops:{getattr:function (node) {
          var attr = {};
          // device numbers reuse inode numbers.
          attr.dev = FS.isChrdev(node.mode) ? node.id : 1;
          attr.ino = node.id;
          attr.mode = node.mode;
          attr.nlink = 1;
          attr.uid = 0;
          attr.gid = 0;
          attr.rdev = node.rdev;
          if (FS.isDir(node.mode)) {
            attr.size = 4096;
          } else if (FS.isFile(node.mode)) {
            attr.size = node.contents.length;
          } else if (FS.isLink(node.mode)) {
            attr.size = node.link.length;
          } else {
            attr.size = 0;
          }
          attr.atime = new Date(node.timestamp);
          attr.mtime = new Date(node.timestamp);
          attr.ctime = new Date(node.timestamp);
          // NOTE: In our implementation, st_blocks = Math.ceil(st_size/st_blksize),
          //       but this is not required by the standard.
          attr.blksize = 4096;
          attr.blocks = Math.ceil(attr.size / attr.blksize);
          return attr;
        },setattr:function (node, attr) {
          if (attr.mode !== undefined) {
            node.mode = attr.mode;
          }
          if (attr.timestamp !== undefined) {
            node.timestamp = attr.timestamp;
          }
          if (attr.size !== undefined) {
            MEMFS.ensureFlexible(node);
            var contents = node.contents;
            if (attr.size < contents.length) contents.length = attr.size;
            else while (attr.size > contents.length) contents.push(0);
          }
        },lookup:function (parent, name) {
          throw FS.genericErrors[ERRNO_CODES.ENOENT];
        },mknod:function (parent, name, mode, dev) {
          return MEMFS.createNode(parent, name, mode, dev);
        },rename:function (old_node, new_dir, new_name) {
          // if we're overwriting a directory at new_name, make sure it's empty.
          if (FS.isDir(old_node.mode)) {
            var new_node;
            try {
              new_node = FS.lookupNode(new_dir, new_name);
            } catch (e) {
            }
            if (new_node) {
              for (var i in new_node.contents) {
                throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
              }
            }
          }
          // do the internal rewiring
          delete old_node.parent.contents[old_node.name];
          old_node.name = new_name;
          new_dir.contents[new_name] = old_node;
          old_node.parent = new_dir;
        },unlink:function (parent, name) {
          delete parent.contents[name];
        },rmdir:function (parent, name) {
          var node = FS.lookupNode(parent, name);
          for (var i in node.contents) {
            throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
          }
          delete parent.contents[name];
        },readdir:function (node) {
          var entries = ['.', '..']
          for (var key in node.contents) {
            if (!node.contents.hasOwnProperty(key)) {
              continue;
            }
            entries.push(key);
          }
          return entries;
        },symlink:function (parent, newname, oldpath) {
          var node = MEMFS.createNode(parent, newname, 0777 | 40960, 0);
          node.link = oldpath;
          return node;
        },readlink:function (node) {
          if (!FS.isLink(node.mode)) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          return node.link;
        }},stream_ops:{read:function (stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          assert(size >= 0);
          if (size > 8 && contents.subarray) { // non-trivial, and typed array
            buffer.set(contents.subarray(position, position + size), offset);
          } else
          {
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          }
          return size;
        },write:function (stream, buffer, offset, length, position, canOwn) {
          var node = stream.node;
          node.timestamp = Date.now();
          var contents = node.contents;
          if (length && contents.length === 0 && position === 0 && buffer.subarray) {
            // just replace it with the new data
            if (canOwn && offset === 0) {
              node.contents = buffer; // this could be a subarray of Emscripten HEAP, or allocated from some other source.
              node.contentMode = (buffer.buffer === HEAP8.buffer) ? MEMFS.CONTENT_OWNING : MEMFS.CONTENT_FIXED;
            } else {
              node.contents = new Uint8Array(buffer.subarray(offset, offset+length));
              node.contentMode = MEMFS.CONTENT_FIXED;
            }
            return length;
          }
          MEMFS.ensureFlexible(node);
          var contents = node.contents;
          while (contents.length < position) contents.push(0);
          for (var i = 0; i < length; i++) {
            contents[position + i] = buffer[offset + i];
          }
          return length;
        },llseek:function (stream, offset, whence) {
          var position = offset;
          if (whence === 1) {  // SEEK_CUR.
            position += stream.position;
          } else if (whence === 2) {  // SEEK_END.
            if (FS.isFile(stream.node.mode)) {
              position += stream.node.contents.length;
            }
          }
          if (position < 0) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          stream.ungotten = [];
          stream.position = position;
          return position;
        },allocate:function (stream, offset, length) {
          MEMFS.ensureFlexible(stream.node);
          var contents = stream.node.contents;
          var limit = offset + length;
          while (limit > contents.length) contents.push(0);
        },mmap:function (stream, buffer, offset, length, position, prot, flags) {
          if (!FS.isFile(stream.node.mode)) {
            throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
          }
          var ptr;
          var allocated;
          var contents = stream.node.contents;
          // Only make a new copy when MAP_PRIVATE is specified.
          if ( !(flags & 2) &&
                (contents.buffer === buffer || contents.buffer === buffer.buffer) ) {
            // We can't emulate MAP_SHARED when the file is not backed by the buffer
            // we're mapping to (e.g. the HEAP buffer).
            allocated = false;
            ptr = contents.byteOffset;
          } else {
            // Try to avoid unnecessary slices.
            if (position > 0 || position + length < contents.length) {
              if (contents.subarray) {
                contents = contents.subarray(position, position + length);
              } else {
                contents = Array.prototype.slice.call(contents, position, position + length);
              }
            }
            allocated = true;
            ptr = _malloc(length);
            if (!ptr) {
              throw new FS.ErrnoError(ERRNO_CODES.ENOMEM);
            }
            buffer.set(contents, ptr);
          }
          return { ptr: ptr, allocated: allocated };
        }}};
  var IDBFS={dbs:{},indexedDB:function () {
        return window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
      },DB_VERSION:20,DB_STORE_NAME:"FILE_DATA",mount:function (mount) {
        return MEMFS.mount.apply(null, arguments);
      },syncfs:function (mount, populate, callback) {
        IDBFS.getLocalSet(mount, function(err, local) {
          if (err) return callback(err);
          IDBFS.getRemoteSet(mount, function(err, remote) {
            if (err) return callback(err);
            var src = populate ? remote : local;
            var dst = populate ? local : remote;
            IDBFS.reconcile(src, dst, callback);
          });
        });
      },reconcile:function (src, dst, callback) {
        var total = 0;
        var create = {};
        for (var key in src.files) {
          if (!src.files.hasOwnProperty(key)) continue;
          var e = src.files[key];
          var e2 = dst.files[key];
          if (!e2 || e.timestamp > e2.timestamp) {
            create[key] = e;
            total++;
          }
        }
        var remove = {};
        for (var key in dst.files) {
          if (!dst.files.hasOwnProperty(key)) continue;
          var e = dst.files[key];
          var e2 = src.files[key];
          if (!e2) {
            remove[key] = e;
            total++;
          }
        }
        if (!total) {
          // early out
          return callback(null);
        }
        var completed = 0;
        function done(err) {
          if (err) return callback(err);
          if (++completed >= total) {
            return callback(null);
          }
        };
        // create a single transaction to handle and IDB reads / writes we'll need to do
        var db = src.type === 'remote' ? src.db : dst.db;
        var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readwrite');
        transaction.onerror = function transaction_onerror() { callback(this.error); };
        var store = transaction.objectStore(IDBFS.DB_STORE_NAME);
        for (var path in create) {
          if (!create.hasOwnProperty(path)) continue;
          var entry = create[path];
          if (dst.type === 'local') {
            // save file to local
            try {
              if (FS.isDir(entry.mode)) {
                FS.mkdir(path, entry.mode);
              } else if (FS.isFile(entry.mode)) {
                var stream = FS.open(path, 'w+', 0666);
                FS.write(stream, entry.contents, 0, entry.contents.length, 0, true /* canOwn */);
                FS.close(stream);
              }
              done(null);
            } catch (e) {
              return done(e);
            }
          } else {
            // save file to IDB
            var req = store.put(entry, path);
            req.onsuccess = function req_onsuccess() { done(null); };
            req.onerror = function req_onerror() { done(this.error); };
          }
        }
        for (var path in remove) {
          if (!remove.hasOwnProperty(path)) continue;
          var entry = remove[path];
          if (dst.type === 'local') {
            // delete file from local
            try {
              if (FS.isDir(entry.mode)) {
                // TODO recursive delete?
                FS.rmdir(path);
              } else if (FS.isFile(entry.mode)) {
                FS.unlink(path);
              }
              done(null);
            } catch (e) {
              return done(e);
            }
          } else {
            // delete file from IDB
            var req = store.delete(path);
            req.onsuccess = function req_onsuccess() { done(null); };
            req.onerror = function req_onerror() { done(this.error); };
          }
        }
      },getLocalSet:function (mount, callback) {
        var files = {};
        function isRealDir(p) {
          return p !== '.' && p !== '..';
        };
        function toAbsolute(root) {
          return function(p) {
            return PATH.join2(root, p);
          }
        };
        var check = FS.readdir(mount.mountpoint)
          .filter(isRealDir)
          .map(toAbsolute(mount.mountpoint));
        while (check.length) {
          var path = check.pop();
          var stat, node;
          try {
            var lookup = FS.lookupPath(path);
            node = lookup.node;
            stat = FS.stat(path);
          } catch (e) {
            return callback(e);
          }
          if (FS.isDir(stat.mode)) {
            check.push.apply(check, FS.readdir(path)
              .filter(isRealDir)
              .map(toAbsolute(path)));
            files[path] = { mode: stat.mode, timestamp: stat.mtime };
          } else if (FS.isFile(stat.mode)) {
            files[path] = { contents: node.contents, mode: stat.mode, timestamp: stat.mtime };
          } else {
            return callback(new Error('node type not supported'));
          }
        }
        return callback(null, { type: 'local', files: files });
      },getDB:function (name, callback) {
        // look it up in the cache
        var db = IDBFS.dbs[name];
        if (db) {
          return callback(null, db);
        }
        var req;
        try {
          req = IDBFS.indexedDB().open(name, IDBFS.DB_VERSION);
        } catch (e) {
          return onerror(e);
        }
        req.onupgradeneeded = function req_onupgradeneeded() {
          db = req.result;
          db.createObjectStore(IDBFS.DB_STORE_NAME);
        };
        req.onsuccess = function req_onsuccess() {
          db = req.result;
          // add to the cache
          IDBFS.dbs[name] = db;
          callback(null, db);
        };
        req.onerror = function req_onerror() {
          callback(this.error);
        };
      },getRemoteSet:function (mount, callback) {
        var files = {};
        IDBFS.getDB(mount.mountpoint, function(err, db) {
          if (err) return callback(err);
          var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readonly');
          transaction.onerror = function transaction_onerror() { callback(this.error); };
          var store = transaction.objectStore(IDBFS.DB_STORE_NAME);
          store.openCursor().onsuccess = function store_openCursor_onsuccess(event) {
            var cursor = event.target.result;
            if (!cursor) {
              return callback(null, { type: 'remote', db: db, files: files });
            }
            files[cursor.key] = cursor.value;
            cursor.continue();
          };
        });
      }};
  var NODEFS={isWindows:false,staticInit:function () {
        NODEFS.isWindows = !!process.platform.match(/^win/);
      },mount:function (mount) {
        assert(ENVIRONMENT_IS_NODE);
        return NODEFS.createNode(null, '/', NODEFS.getMode(mount.opts.root), 0);
      },createNode:function (parent, name, mode, dev) {
        if (!FS.isDir(mode) && !FS.isFile(mode) && !FS.isLink(mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var node = FS.createNode(parent, name, mode);
        node.node_ops = NODEFS.node_ops;
        node.stream_ops = NODEFS.stream_ops;
        return node;
      },getMode:function (path) {
        var stat;
        try {
          stat = fs.lstatSync(path);
          if (NODEFS.isWindows) {
            // On Windows, directories return permission bits 'rw-rw-rw-', even though they have 'rwxrwxrwx', so 
            // propagate write bits to execute bits.
            stat.mode = stat.mode | ((stat.mode & 146) >> 1);
          }
        } catch (e) {
          if (!e.code) throw e;
          throw new FS.ErrnoError(ERRNO_CODES[e.code]);
        }
        return stat.mode;
      },realPath:function (node) {
        var parts = [];
        while (node.parent !== node) {
          parts.push(node.name);
          node = node.parent;
        }
        parts.push(node.mount.opts.root);
        parts.reverse();
        return PATH.join.apply(null, parts);
      },flagsToPermissionStringMap:{0:"r",1:"r+",2:"r+",64:"r",65:"r+",66:"r+",129:"rx+",193:"rx+",514:"w+",577:"w",578:"w+",705:"wx",706:"wx+",1024:"a",1025:"a",1026:"a+",1089:"a",1090:"a+",1153:"ax",1154:"ax+",1217:"ax",1218:"ax+",4096:"rs",4098:"rs+"},flagsToPermissionString:function (flags) {
        if (flags in NODEFS.flagsToPermissionStringMap) {
          return NODEFS.flagsToPermissionStringMap[flags];
        } else {
          return flags;
        }
      },node_ops:{getattr:function (node) {
          var path = NODEFS.realPath(node);
          var stat;
          try {
            stat = fs.lstatSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          // node.js v0.10.20 doesn't report blksize and blocks on Windows. Fake them with default blksize of 4096.
          // See http://support.microsoft.com/kb/140365
          if (NODEFS.isWindows && !stat.blksize) {
            stat.blksize = 4096;
          }
          if (NODEFS.isWindows && !stat.blocks) {
            stat.blocks = (stat.size+stat.blksize-1)/stat.blksize|0;
          }
          return {
            dev: stat.dev,
            ino: stat.ino,
            mode: stat.mode,
            nlink: stat.nlink,
            uid: stat.uid,
            gid: stat.gid,
            rdev: stat.rdev,
            size: stat.size,
            atime: stat.atime,
            mtime: stat.mtime,
            ctime: stat.ctime,
            blksize: stat.blksize,
            blocks: stat.blocks
          };
        },setattr:function (node, attr) {
          var path = NODEFS.realPath(node);
          try {
            if (attr.mode !== undefined) {
              fs.chmodSync(path, attr.mode);
              // update the common node structure mode as well
              node.mode = attr.mode;
            }
            if (attr.timestamp !== undefined) {
              var date = new Date(attr.timestamp);
              fs.utimesSync(path, date, date);
            }
            if (attr.size !== undefined) {
              fs.truncateSync(path, attr.size);
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },lookup:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          var mode = NODEFS.getMode(path);
          return NODEFS.createNode(parent, name, mode);
        },mknod:function (parent, name, mode, dev) {
          var node = NODEFS.createNode(parent, name, mode, dev);
          // create the backing node for this in the fs root as well
          var path = NODEFS.realPath(node);
          try {
            if (FS.isDir(node.mode)) {
              fs.mkdirSync(path, node.mode);
            } else {
              fs.writeFileSync(path, '', { mode: node.mode });
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          return node;
        },rename:function (oldNode, newDir, newName) {
          var oldPath = NODEFS.realPath(oldNode);
          var newPath = PATH.join2(NODEFS.realPath(newDir), newName);
          try {
            fs.renameSync(oldPath, newPath);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },unlink:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          try {
            fs.unlinkSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },rmdir:function (parent, name) {
          var path = PATH.join2(NODEFS.realPath(parent), name);
          try {
            fs.rmdirSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },readdir:function (node) {
          var path = NODEFS.realPath(node);
          try {
            return fs.readdirSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },symlink:function (parent, newName, oldPath) {
          var newPath = PATH.join2(NODEFS.realPath(parent), newName);
          try {
            fs.symlinkSync(oldPath, newPath);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },readlink:function (node) {
          var path = NODEFS.realPath(node);
          try {
            return fs.readlinkSync(path);
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        }},stream_ops:{open:function (stream) {
          var path = NODEFS.realPath(stream.node);
          try {
            if (FS.isFile(stream.node.mode)) {
              stream.nfd = fs.openSync(path, NODEFS.flagsToPermissionString(stream.flags));
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },close:function (stream) {
          try {
            if (FS.isFile(stream.node.mode) && stream.nfd) {
              fs.closeSync(stream.nfd);
            }
          } catch (e) {
            if (!e.code) throw e;
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
        },read:function (stream, buffer, offset, length, position) {
          // FIXME this is terrible.
          var nbuffer = new Buffer(length);
          var res;
          try {
            res = fs.readSync(stream.nfd, nbuffer, 0, length, position);
          } catch (e) {
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          if (res > 0) {
            for (var i = 0; i < res; i++) {
              buffer[offset + i] = nbuffer[i];
            }
          }
          return res;
        },write:function (stream, buffer, offset, length, position) {
          // FIXME this is terrible.
          var nbuffer = new Buffer(buffer.subarray(offset, offset + length));
          var res;
          try {
            res = fs.writeSync(stream.nfd, nbuffer, 0, length, position);
          } catch (e) {
            throw new FS.ErrnoError(ERRNO_CODES[e.code]);
          }
          return res;
        },llseek:function (stream, offset, whence) {
          var position = offset;
          if (whence === 1) {  // SEEK_CUR.
            position += stream.position;
          } else if (whence === 2) {  // SEEK_END.
            if (FS.isFile(stream.node.mode)) {
              try {
                var stat = fs.fstatSync(stream.nfd);
                position += stat.size;
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES[e.code]);
              }
            }
          }
          if (position < 0) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          stream.position = position;
          return position;
        }}};
  var _stdin=allocate(1, "i32*", ALLOC_STATIC);
  var _stdout=allocate(1, "i32*", ALLOC_STATIC);
  var _stderr=allocate(1, "i32*", ALLOC_STATIC);
  function _fflush(stream) {
      // int fflush(FILE *stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fflush.html
      // we don't currently perform any user-space buffering of data
    }var FS={root:null,mounts:[],devices:[null],streams:[null],nextInode:1,nameTable:null,currentPath:"/",initialized:false,ignorePermissions:true,ErrnoError:null,genericErrors:{},handleFSError:function (e) {
        if (!(e instanceof FS.ErrnoError)) throw e + ' : ' + stackTrace();
        return ___setErrNo(e.errno);
      },lookupPath:function (path, opts) {
        path = PATH.resolve(FS.cwd(), path);
        opts = opts || { recurse_count: 0 };
        if (opts.recurse_count > 8) {  // max recursive lookup of 8
          throw new FS.ErrnoError(ERRNO_CODES.ELOOP);
        }
        // split the path
        var parts = PATH.normalizeArray(path.split('/').filter(function(p) {
          return !!p;
        }), false);
        // start at the root
        var current = FS.root;
        var current_path = '/';
        for (var i = 0; i < parts.length; i++) {
          var islast = (i === parts.length-1);
          if (islast && opts.parent) {
            // stop resolving
            break;
          }
          current = FS.lookupNode(current, parts[i]);
          current_path = PATH.join2(current_path, parts[i]);
          // jump to the mount's root node if this is a mountpoint
          if (FS.isMountpoint(current)) {
            current = current.mount.root;
          }
          // follow symlinks
          // by default, lookupPath will not follow a symlink if it is the final path component.
          // setting opts.follow = true will override this behavior.
          if (!islast || opts.follow) {
            var count = 0;
            while (FS.isLink(current.mode)) {
              var link = FS.readlink(current_path);
              current_path = PATH.resolve(PATH.dirname(current_path), link);
              var lookup = FS.lookupPath(current_path, { recurse_count: opts.recurse_count });
              current = lookup.node;
              if (count++ > 40) {  // limit max consecutive symlinks to 40 (SYMLOOP_MAX).
                throw new FS.ErrnoError(ERRNO_CODES.ELOOP);
              }
            }
          }
        }
        return { path: current_path, node: current };
      },getPath:function (node) {
        var path;
        while (true) {
          if (FS.isRoot(node)) {
            var mount = node.mount.mountpoint;
            if (!path) return mount;
            return mount[mount.length-1] !== '/' ? mount + '/' + path : mount + path;
          }
          path = path ? node.name + '/' + path : node.name;
          node = node.parent;
        }
      },hashName:function (parentid, name) {
        var hash = 0;
        for (var i = 0; i < name.length; i++) {
          hash = ((hash << 5) - hash + name.charCodeAt(i)) | 0;
        }
        return ((parentid + hash) >>> 0) % FS.nameTable.length;
      },hashAddNode:function (node) {
        var hash = FS.hashName(node.parent.id, node.name);
        node.name_next = FS.nameTable[hash];
        FS.nameTable[hash] = node;
      },hashRemoveNode:function (node) {
        var hash = FS.hashName(node.parent.id, node.name);
        if (FS.nameTable[hash] === node) {
          FS.nameTable[hash] = node.name_next;
        } else {
          var current = FS.nameTable[hash];
          while (current) {
            if (current.name_next === node) {
              current.name_next = node.name_next;
              break;
            }
            current = current.name_next;
          }
        }
      },lookupNode:function (parent, name) {
        var err = FS.mayLookup(parent);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        var hash = FS.hashName(parent.id, name);
        for (var node = FS.nameTable[hash]; node; node = node.name_next) {
          var nodeName = node.name;
          if (node.parent.id === parent.id && nodeName === name) {
            return node;
          }
        }
        // if we failed to find it in the cache, call into the VFS
        return FS.lookup(parent, name);
      },createNode:function (parent, name, mode, rdev) {
        if (!FS.FSNode) {
          FS.FSNode = function(parent, name, mode, rdev) {
            this.id = FS.nextInode++;
            this.name = name;
            this.mode = mode;
            this.node_ops = {};
            this.stream_ops = {};
            this.rdev = rdev;
            this.parent = null;
            this.mount = null;
            if (!parent) {
              parent = this;  // root node sets parent to itself
            }
            this.parent = parent;
            this.mount = parent.mount;
            FS.hashAddNode(this);
          };
          // compatibility
          var readMode = 292 | 73;
          var writeMode = 146;
          FS.FSNode.prototype = {};
          // NOTE we must use Object.defineProperties instead of individual calls to
          // Object.defineProperty in order to make closure compiler happy
          Object.defineProperties(FS.FSNode.prototype, {
            read: {
              get: function() { return (this.mode & readMode) === readMode; },
              set: function(val) { val ? this.mode |= readMode : this.mode &= ~readMode; }
            },
            write: {
              get: function() { return (this.mode & writeMode) === writeMode; },
              set: function(val) { val ? this.mode |= writeMode : this.mode &= ~writeMode; }
            },
            isFolder: {
              get: function() { return FS.isDir(this.mode); },
            },
            isDevice: {
              get: function() { return FS.isChrdev(this.mode); },
            },
          });
        }
        return new FS.FSNode(parent, name, mode, rdev);
      },destroyNode:function (node) {
        FS.hashRemoveNode(node);
      },isRoot:function (node) {
        return node === node.parent;
      },isMountpoint:function (node) {
        return node.mounted;
      },isFile:function (mode) {
        return (mode & 61440) === 32768;
      },isDir:function (mode) {
        return (mode & 61440) === 16384;
      },isLink:function (mode) {
        return (mode & 61440) === 40960;
      },isChrdev:function (mode) {
        return (mode & 61440) === 8192;
      },isBlkdev:function (mode) {
        return (mode & 61440) === 24576;
      },isFIFO:function (mode) {
        return (mode & 61440) === 4096;
      },isSocket:function (mode) {
        return (mode & 49152) === 49152;
      },flagModes:{"r":0,"rs":1052672,"r+":2,"w":577,"wx":705,"xw":705,"w+":578,"wx+":706,"xw+":706,"a":1089,"ax":1217,"xa":1217,"a+":1090,"ax+":1218,"xa+":1218},modeStringToFlags:function (str) {
        var flags = FS.flagModes[str];
        if (typeof flags === 'undefined') {
          throw new Error('Unknown file open mode: ' + str);
        }
        return flags;
      },flagsToPermissionString:function (flag) {
        var accmode = flag & 2097155;
        var perms = ['r', 'w', 'rw'][accmode];
        if ((flag & 512)) {
          perms += 'w';
        }
        return perms;
      },nodePermissions:function (node, perms) {
        if (FS.ignorePermissions) {
          return 0;
        }
        // return 0 if any user, group or owner bits are set.
        if (perms.indexOf('r') !== -1 && !(node.mode & 292)) {
          return ERRNO_CODES.EACCES;
        } else if (perms.indexOf('w') !== -1 && !(node.mode & 146)) {
          return ERRNO_CODES.EACCES;
        } else if (perms.indexOf('x') !== -1 && !(node.mode & 73)) {
          return ERRNO_CODES.EACCES;
        }
        return 0;
      },mayLookup:function (dir) {
        return FS.nodePermissions(dir, 'x');
      },mayCreate:function (dir, name) {
        try {
          var node = FS.lookupNode(dir, name);
          return ERRNO_CODES.EEXIST;
        } catch (e) {
        }
        return FS.nodePermissions(dir, 'wx');
      },mayDelete:function (dir, name, isdir) {
        var node;
        try {
          node = FS.lookupNode(dir, name);
        } catch (e) {
          return e.errno;
        }
        var err = FS.nodePermissions(dir, 'wx');
        if (err) {
          return err;
        }
        if (isdir) {
          if (!FS.isDir(node.mode)) {
            return ERRNO_CODES.ENOTDIR;
          }
          if (FS.isRoot(node) || FS.getPath(node) === FS.cwd()) {
            return ERRNO_CODES.EBUSY;
          }
        } else {
          if (FS.isDir(node.mode)) {
            return ERRNO_CODES.EISDIR;
          }
        }
        return 0;
      },mayOpen:function (node, flags) {
        if (!node) {
          return ERRNO_CODES.ENOENT;
        }
        if (FS.isLink(node.mode)) {
          return ERRNO_CODES.ELOOP;
        } else if (FS.isDir(node.mode)) {
          if ((flags & 2097155) !== 0 ||  // opening for write
              (flags & 512)) {
            return ERRNO_CODES.EISDIR;
          }
        }
        return FS.nodePermissions(node, FS.flagsToPermissionString(flags));
      },MAX_OPEN_FDS:4096,nextfd:function (fd_start, fd_end) {
        fd_start = fd_start || 1;
        fd_end = fd_end || FS.MAX_OPEN_FDS;
        for (var fd = fd_start; fd <= fd_end; fd++) {
          if (!FS.streams[fd]) {
            return fd;
          }
        }
        throw new FS.ErrnoError(ERRNO_CODES.EMFILE);
      },getStream:function (fd) {
        return FS.streams[fd];
      },createStream:function (stream, fd_start, fd_end) {
        if (!FS.FSStream) {
          FS.FSStream = function(){};
          FS.FSStream.prototype = {};
          // compatibility
          Object.defineProperties(FS.FSStream.prototype, {
            object: {
              get: function() { return this.node; },
              set: function(val) { this.node = val; }
            },
            isRead: {
              get: function() { return (this.flags & 2097155) !== 1; }
            },
            isWrite: {
              get: function() { return (this.flags & 2097155) !== 0; }
            },
            isAppend: {
              get: function() { return (this.flags & 1024); }
            }
          });
        }
        if (stream.__proto__) {
          // reuse the object
          stream.__proto__ = FS.FSStream.prototype;
        } else {
          var newStream = new FS.FSStream();
          for (var p in stream) {
            newStream[p] = stream[p];
          }
          stream = newStream;
        }
        var fd = FS.nextfd(fd_start, fd_end);
        stream.fd = fd;
        FS.streams[fd] = stream;
        return stream;
      },closeStream:function (fd) {
        FS.streams[fd] = null;
      },chrdev_stream_ops:{open:function (stream) {
          var device = FS.getDevice(stream.node.rdev);
          // override node's stream ops with the device's
          stream.stream_ops = device.stream_ops;
          // forward the open call
          if (stream.stream_ops.open) {
            stream.stream_ops.open(stream);
          }
        },llseek:function () {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }},major:function (dev) {
        return ((dev) >> 8);
      },minor:function (dev) {
        return ((dev) & 0xff);
      },makedev:function (ma, mi) {
        return ((ma) << 8 | (mi));
      },registerDevice:function (dev, ops) {
        FS.devices[dev] = { stream_ops: ops };
      },getDevice:function (dev) {
        return FS.devices[dev];
      },syncfs:function (populate, callback) {
        if (typeof(populate) === 'function') {
          callback = populate;
          populate = false;
        }
        var completed = 0;
        var total = FS.mounts.length;
        function done(err) {
          if (err) {
            return callback(err);
          }
          if (++completed >= total) {
            callback(null);
          }
        };
        // sync all mounts
        for (var i = 0; i < FS.mounts.length; i++) {
          var mount = FS.mounts[i];
          if (!mount.type.syncfs) {
            done(null);
            continue;
          }
          mount.type.syncfs(mount, populate, done);
        }
      },mount:function (type, opts, mountpoint) {
        var lookup;
        if (mountpoint) {
          lookup = FS.lookupPath(mountpoint, { follow: false });
          mountpoint = lookup.path;  // use the absolute path
        }
        var mount = {
          type: type,
          opts: opts,
          mountpoint: mountpoint,
          root: null
        };
        // create a root node for the fs
        var root = type.mount(mount);
        root.mount = mount;
        mount.root = root;
        // assign the mount info to the mountpoint's node
        if (lookup) {
          lookup.node.mount = mount;
          lookup.node.mounted = true;
          // compatibility update FS.root if we mount to /
          if (mountpoint === '/') {
            FS.root = mount.root;
          }
        }
        // add to our cached list of mounts
        FS.mounts.push(mount);
        return root;
      },lookup:function (parent, name) {
        return parent.node_ops.lookup(parent, name);
      },mknod:function (path, mode, dev) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var err = FS.mayCreate(parent, name);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.mknod) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return parent.node_ops.mknod(parent, name, mode, dev);
      },create:function (path, mode) {
        mode = mode !== undefined ? mode : 0666;
        mode &= 4095;
        mode |= 32768;
        return FS.mknod(path, mode, 0);
      },mkdir:function (path, mode) {
        mode = mode !== undefined ? mode : 0777;
        mode &= 511 | 512;
        mode |= 16384;
        return FS.mknod(path, mode, 0);
      },mkdev:function (path, mode, dev) {
        if (typeof(dev) === 'undefined') {
          dev = mode;
          mode = 0666;
        }
        mode |= 8192;
        return FS.mknod(path, mode, dev);
      },symlink:function (oldpath, newpath) {
        var lookup = FS.lookupPath(newpath, { parent: true });
        var parent = lookup.node;
        var newname = PATH.basename(newpath);
        var err = FS.mayCreate(parent, newname);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.symlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return parent.node_ops.symlink(parent, newname, oldpath);
      },rename:function (old_path, new_path) {
        var old_dirname = PATH.dirname(old_path);
        var new_dirname = PATH.dirname(new_path);
        var old_name = PATH.basename(old_path);
        var new_name = PATH.basename(new_path);
        // parents must exist
        var lookup, old_dir, new_dir;
        try {
          lookup = FS.lookupPath(old_path, { parent: true });
          old_dir = lookup.node;
          lookup = FS.lookupPath(new_path, { parent: true });
          new_dir = lookup.node;
        } catch (e) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        // need to be part of the same mount
        if (old_dir.mount !== new_dir.mount) {
          throw new FS.ErrnoError(ERRNO_CODES.EXDEV);
        }
        // source must exist
        var old_node = FS.lookupNode(old_dir, old_name);
        // old path should not be an ancestor of the new path
        var relative = PATH.relative(old_path, new_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        // new path should not be an ancestor of the old path
        relative = PATH.relative(new_path, old_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTEMPTY);
        }
        // see if the new path already exists
        var new_node;
        try {
          new_node = FS.lookupNode(new_dir, new_name);
        } catch (e) {
          // not fatal
        }
        // early out if nothing needs to change
        if (old_node === new_node) {
          return;
        }
        // we'll need to delete the old entry
        var isdir = FS.isDir(old_node.mode);
        var err = FS.mayDelete(old_dir, old_name, isdir);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        // need delete permissions if we'll be overwriting.
        // need create permissions if new doesn't already exist.
        err = new_node ?
          FS.mayDelete(new_dir, new_name, isdir) :
          FS.mayCreate(new_dir, new_name);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!old_dir.node_ops.rename) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(old_node) || (new_node && FS.isMountpoint(new_node))) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        // if we are going to change the parent, check write permissions
        if (new_dir !== old_dir) {
          err = FS.nodePermissions(old_dir, 'w');
          if (err) {
            throw new FS.ErrnoError(err);
          }
        }
        // remove the node from the lookup hash
        FS.hashRemoveNode(old_node);
        // do the underlying fs rename
        try {
          old_dir.node_ops.rename(old_node, new_dir, new_name);
        } catch (e) {
          throw e;
        } finally {
          // add the node back to the hash (in case node_ops.rename
          // changed its name)
          FS.hashAddNode(old_node);
        }
      },rmdir:function (path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var err = FS.mayDelete(parent, name, true);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.rmdir) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        parent.node_ops.rmdir(parent, name);
        FS.destroyNode(node);
      },readdir:function (path) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        if (!node.node_ops.readdir) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTDIR);
        }
        return node.node_ops.readdir(node);
      },unlink:function (path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var err = FS.mayDelete(parent, name, false);
        if (err) {
          // POSIX says unlink should set EPERM, not EISDIR
          if (err === ERRNO_CODES.EISDIR) err = ERRNO_CODES.EPERM;
          throw new FS.ErrnoError(err);
        }
        if (!parent.node_ops.unlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(ERRNO_CODES.EBUSY);
        }
        parent.node_ops.unlink(parent, name);
        FS.destroyNode(node);
      },readlink:function (path) {
        var lookup = FS.lookupPath(path, { follow: false });
        var link = lookup.node;
        if (!link.node_ops.readlink) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        return link.node_ops.readlink(link);
      },stat:function (path, dontFollow) {
        var lookup = FS.lookupPath(path, { follow: !dontFollow });
        var node = lookup.node;
        if (!node.node_ops.getattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        return node.node_ops.getattr(node);
      },lstat:function (path) {
        return FS.stat(path, true);
      },chmod:function (path, mode, dontFollow) {
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        node.node_ops.setattr(node, {
          mode: (mode & 4095) | (node.mode & ~4095),
          timestamp: Date.now()
        });
      },lchmod:function (path, mode) {
        FS.chmod(path, mode, true);
      },fchmod:function (fd, mode) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        FS.chmod(stream.node, mode);
      },chown:function (path, uid, gid, dontFollow) {
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        node.node_ops.setattr(node, {
          timestamp: Date.now()
          // we ignore the uid / gid for now
        });
      },lchown:function (path, uid, gid) {
        FS.chown(path, uid, gid, true);
      },fchown:function (fd, uid, gid) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        FS.chown(stream.node, uid, gid);
      },truncate:function (path, len) {
        if (len < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var node;
        if (typeof path === 'string') {
          var lookup = FS.lookupPath(path, { follow: true });
          node = lookup.node;
        } else {
          node = path;
        }
        if (!node.node_ops.setattr) {
          throw new FS.ErrnoError(ERRNO_CODES.EPERM);
        }
        if (FS.isDir(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!FS.isFile(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var err = FS.nodePermissions(node, 'w');
        if (err) {
          throw new FS.ErrnoError(err);
        }
        node.node_ops.setattr(node, {
          size: len,
          timestamp: Date.now()
        });
      },ftruncate:function (fd, len) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        FS.truncate(stream.node, len);
      },utime:function (path, atime, mtime) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        node.node_ops.setattr(node, {
          timestamp: Math.max(atime, mtime)
        });
      },open:function (path, flags, mode, fd_start, fd_end) {
        flags = typeof flags === 'string' ? FS.modeStringToFlags(flags) : flags;
        mode = typeof mode === 'undefined' ? 0666 : mode;
        if ((flags & 64)) {
          mode = (mode & 4095) | 32768;
        } else {
          mode = 0;
        }
        var node;
        if (typeof path === 'object') {
          node = path;
        } else {
          path = PATH.normalize(path);
          try {
            var lookup = FS.lookupPath(path, {
              follow: !(flags & 131072)
            });
            node = lookup.node;
          } catch (e) {
            // ignore
          }
        }
        // perhaps we need to create the node
        if ((flags & 64)) {
          if (node) {
            // if O_CREAT and O_EXCL are set, error out if the node already exists
            if ((flags & 128)) {
              throw new FS.ErrnoError(ERRNO_CODES.EEXIST);
            }
          } else {
            // node doesn't exist, try to create it
            node = FS.mknod(path, mode, 0);
          }
        }
        if (!node) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOENT);
        }
        // can't truncate a device
        if (FS.isChrdev(node.mode)) {
          flags &= ~512;
        }
        // check permissions
        var err = FS.mayOpen(node, flags);
        if (err) {
          throw new FS.ErrnoError(err);
        }
        // do truncation if necessary
        if ((flags & 512)) {
          FS.truncate(node, 0);
        }
        // we've already handled these, don't pass down to the underlying vfs
        flags &= ~(128 | 512);
        // register the stream with the filesystem
        var stream = FS.createStream({
          node: node,
          path: FS.getPath(node),  // we want the absolute path to the node
          flags: flags,
          seekable: true,
          position: 0,
          stream_ops: node.stream_ops,
          // used by the file family libc calls (fopen, fwrite, ferror, etc.)
          ungotten: [],
          error: false
        }, fd_start, fd_end);
        // call the new stream's open function
        if (stream.stream_ops.open) {
          stream.stream_ops.open(stream);
        }
        if (Module['logReadFiles'] && !(flags & 1)) {
          if (!FS.readFiles) FS.readFiles = {};
          if (!(path in FS.readFiles)) {
            FS.readFiles[path] = 1;
            Module['printErr']('read file: ' + path);
          }
        }
        return stream;
      },close:function (stream) {
        try {
          if (stream.stream_ops.close) {
            stream.stream_ops.close(stream);
          }
        } catch (e) {
          throw e;
        } finally {
          FS.closeStream(stream.fd);
        }
      },llseek:function (stream, offset, whence) {
        if (!stream.seekable || !stream.stream_ops.llseek) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        return stream.stream_ops.llseek(stream, offset, whence);
      },read:function (stream, buffer, offset, length, position) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!stream.stream_ops.read) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var seeking = true;
        if (typeof position === 'undefined') {
          position = stream.position;
          seeking = false;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        var bytesRead = stream.stream_ops.read(stream, buffer, offset, length, position);
        if (!seeking) stream.position += bytesRead;
        return bytesRead;
      },write:function (stream, buffer, offset, length, position, canOwn) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.EISDIR);
        }
        if (!stream.stream_ops.write) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        var seeking = true;
        if (typeof position === 'undefined') {
          position = stream.position;
          seeking = false;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(ERRNO_CODES.ESPIPE);
        }
        if (stream.flags & 1024) {
          // seek to the end before writing in append mode
          FS.llseek(stream, 0, 2);
        }
        var bytesWritten = stream.stream_ops.write(stream, buffer, offset, length, position, canOwn);
        if (!seeking) stream.position += bytesWritten;
        return bytesWritten;
      },allocate:function (stream, offset, length) {
        if (offset < 0 || length <= 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(ERRNO_CODES.EBADF);
        }
        if (!FS.isFile(stream.node.mode) && !FS.isDir(node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.ENODEV);
        }
        if (!stream.stream_ops.allocate) {
          throw new FS.ErrnoError(ERRNO_CODES.EOPNOTSUPP);
        }
        stream.stream_ops.allocate(stream, offset, length);
      },mmap:function (stream, buffer, offset, length, position, prot, flags) {
        // TODO if PROT is PROT_WRITE, make sure we have write access
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(ERRNO_CODES.EACCES);
        }
        if (!stream.stream_ops.mmap) {
          throw new FS.errnoError(ERRNO_CODES.ENODEV);
        }
        return stream.stream_ops.mmap(stream, buffer, offset, length, position, prot, flags);
      },ioctl:function (stream, cmd, arg) {
        if (!stream.stream_ops.ioctl) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTTY);
        }
        return stream.stream_ops.ioctl(stream, cmd, arg);
      },readFile:function (path, opts) {
        opts = opts || {};
        opts.flags = opts.flags || 'r';
        opts.encoding = opts.encoding || 'binary';
        var ret;
        var stream = FS.open(path, opts.flags);
        var stat = FS.stat(path);
        var length = stat.size;
        var buf = new Uint8Array(length);
        FS.read(stream, buf, 0, length, 0);
        if (opts.encoding === 'utf8') {
          ret = '';
          var utf8 = new Runtime.UTF8Processor();
          for (var i = 0; i < length; i++) {
            ret += utf8.processCChar(buf[i]);
          }
        } else if (opts.encoding === 'binary') {
          ret = buf;
        } else {
          throw new Error('Invalid encoding type "' + opts.encoding + '"');
        }
        FS.close(stream);
        return ret;
      },writeFile:function (path, data, opts) {
        opts = opts || {};
        opts.flags = opts.flags || 'w';
        opts.encoding = opts.encoding || 'utf8';
        var stream = FS.open(path, opts.flags, opts.mode);
        if (opts.encoding === 'utf8') {
          var utf8 = new Runtime.UTF8Processor();
          var buf = new Uint8Array(utf8.processJSString(data));
          FS.write(stream, buf, 0, buf.length, 0);
        } else if (opts.encoding === 'binary') {
          FS.write(stream, data, 0, data.length, 0);
        } else {
          throw new Error('Invalid encoding type "' + opts.encoding + '"');
        }
        FS.close(stream);
      },cwd:function () {
        return FS.currentPath;
      },chdir:function (path) {
        var lookup = FS.lookupPath(path, { follow: true });
        if (!FS.isDir(lookup.node.mode)) {
          throw new FS.ErrnoError(ERRNO_CODES.ENOTDIR);
        }
        var err = FS.nodePermissions(lookup.node, 'x');
        if (err) {
          throw new FS.ErrnoError(err);
        }
        FS.currentPath = lookup.path;
      },createDefaultDirectories:function () {
        FS.mkdir('/tmp');
      },createDefaultDevices:function () {
        // create /dev
        FS.mkdir('/dev');
        // setup /dev/null
        FS.registerDevice(FS.makedev(1, 3), {
          read: function() { return 0; },
          write: function() { return 0; }
        });
        FS.mkdev('/dev/null', FS.makedev(1, 3));
        // setup /dev/tty and /dev/tty1
        // stderr needs to print output using Module['printErr']
        // so we register a second tty just for it.
        TTY.register(FS.makedev(5, 0), TTY.default_tty_ops);
        TTY.register(FS.makedev(6, 0), TTY.default_tty1_ops);
        FS.mkdev('/dev/tty', FS.makedev(5, 0));
        FS.mkdev('/dev/tty1', FS.makedev(6, 0));
        // we're not going to emulate the actual shm device,
        // just create the tmp dirs that reside in it commonly
        FS.mkdir('/dev/shm');
        FS.mkdir('/dev/shm/tmp');
      },createStandardStreams:function () {
        // TODO deprecate the old functionality of a single
        // input / output callback and that utilizes FS.createDevice
        // and instead require a unique set of stream ops
        // by default, we symlink the standard streams to the
        // default tty devices. however, if the standard streams
        // have been overwritten we create a unique device for
        // them instead.
        if (Module['stdin']) {
          FS.createDevice('/dev', 'stdin', Module['stdin']);
        } else {
          FS.symlink('/dev/tty', '/dev/stdin');
        }
        if (Module['stdout']) {
          FS.createDevice('/dev', 'stdout', null, Module['stdout']);
        } else {
          FS.symlink('/dev/tty', '/dev/stdout');
        }
        if (Module['stderr']) {
          FS.createDevice('/dev', 'stderr', null, Module['stderr']);
        } else {
          FS.symlink('/dev/tty1', '/dev/stderr');
        }
        // open default streams for the stdin, stdout and stderr devices
        var stdin = FS.open('/dev/stdin', 'r');
        HEAP32[((_stdin)>>2)]=stdin.fd;
        assert(stdin.fd === 1, 'invalid handle for stdin (' + stdin.fd + ')');
        var stdout = FS.open('/dev/stdout', 'w');
        HEAP32[((_stdout)>>2)]=stdout.fd;
        assert(stdout.fd === 2, 'invalid handle for stdout (' + stdout.fd + ')');
        var stderr = FS.open('/dev/stderr', 'w');
        HEAP32[((_stderr)>>2)]=stderr.fd;
        assert(stderr.fd === 3, 'invalid handle for stderr (' + stderr.fd + ')');
      },ensureErrnoError:function () {
        if (FS.ErrnoError) return;
        FS.ErrnoError = function ErrnoError(errno) {
          this.errno = errno;
          for (var key in ERRNO_CODES) {
            if (ERRNO_CODES[key] === errno) {
              this.code = key;
              break;
            }
          }
          this.message = ERRNO_MESSAGES[errno];
          this.stack = stackTrace();
        };
        FS.ErrnoError.prototype = new Error();
        FS.ErrnoError.prototype.constructor = FS.ErrnoError;
        // Some errors may happen quite a bit, to avoid overhead we reuse them (and suffer a lack of stack info)
        [ERRNO_CODES.ENOENT].forEach(function(code) {
          FS.genericErrors[code] = new FS.ErrnoError(code);
          FS.genericErrors[code].stack = '<generic error, no stack>';
        });
      },staticInit:function () {
        FS.ensureErrnoError();
        FS.nameTable = new Array(4096);
        FS.root = FS.createNode(null, '/', 16384 | 0777, 0);
        FS.mount(MEMFS, {}, '/');
        FS.createDefaultDirectories();
        FS.createDefaultDevices();
      },init:function (input, output, error) {
        assert(!FS.init.initialized, 'FS.init was previously called. If you want to initialize later with custom parameters, remove any earlier calls (note that one is automatically added to the generated code)');
        FS.init.initialized = true;
        FS.ensureErrnoError();
        // Allow Module.stdin etc. to provide defaults, if none explicitly passed to us here
        Module['stdin'] = input || Module['stdin'];
        Module['stdout'] = output || Module['stdout'];
        Module['stderr'] = error || Module['stderr'];
        FS.createStandardStreams();
      },quit:function () {
        FS.init.initialized = false;
        for (var i = 0; i < FS.streams.length; i++) {
          var stream = FS.streams[i];
          if (!stream) {
            continue;
          }
          FS.close(stream);
        }
      },getMode:function (canRead, canWrite) {
        var mode = 0;
        if (canRead) mode |= 292 | 73;
        if (canWrite) mode |= 146;
        return mode;
      },joinPath:function (parts, forceRelative) {
        var path = PATH.join.apply(null, parts);
        if (forceRelative && path[0] == '/') path = path.substr(1);
        return path;
      },absolutePath:function (relative, base) {
        return PATH.resolve(base, relative);
      },standardizePath:function (path) {
        return PATH.normalize(path);
      },findObject:function (path, dontResolveLastLink) {
        var ret = FS.analyzePath(path, dontResolveLastLink);
        if (ret.exists) {
          return ret.object;
        } else {
          ___setErrNo(ret.error);
          return null;
        }
      },analyzePath:function (path, dontResolveLastLink) {
        // operate from within the context of the symlink's target
        try {
          var lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          path = lookup.path;
        } catch (e) {
        }
        var ret = {
          isRoot: false, exists: false, error: 0, name: null, path: null, object: null,
          parentExists: false, parentPath: null, parentObject: null
        };
        try {
          var lookup = FS.lookupPath(path, { parent: true });
          ret.parentExists = true;
          ret.parentPath = lookup.path;
          ret.parentObject = lookup.node;
          ret.name = PATH.basename(path);
          lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          ret.exists = true;
          ret.path = lookup.path;
          ret.object = lookup.node;
          ret.name = lookup.node.name;
          ret.isRoot = lookup.path === '/';
        } catch (e) {
          ret.error = e.errno;
        };
        return ret;
      },createFolder:function (parent, name, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(canRead, canWrite);
        return FS.mkdir(path, mode);
      },createPath:function (parent, path, canRead, canWrite) {
        parent = typeof parent === 'string' ? parent : FS.getPath(parent);
        var parts = path.split('/').reverse();
        while (parts.length) {
          var part = parts.pop();
          if (!part) continue;
          var current = PATH.join2(parent, part);
          try {
            FS.mkdir(current);
          } catch (e) {
            // ignore EEXIST
          }
          parent = current;
        }
        return current;
      },createFile:function (parent, name, properties, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(canRead, canWrite);
        return FS.create(path, mode);
      },createDataFile:function (parent, name, data, canRead, canWrite, canOwn) {
        var path = name ? PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name) : parent;
        var mode = FS.getMode(canRead, canWrite);
        var node = FS.create(path, mode);
        if (data) {
          if (typeof data === 'string') {
            var arr = new Array(data.length);
            for (var i = 0, len = data.length; i < len; ++i) arr[i] = data.charCodeAt(i);
            data = arr;
          }
          // make sure we can write to the file
          FS.chmod(node, mode | 146);
          var stream = FS.open(node, 'w');
          FS.write(stream, data, 0, data.length, 0, canOwn);
          FS.close(stream);
          FS.chmod(node, mode);
        }
        return node;
      },createDevice:function (parent, name, input, output) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        var mode = FS.getMode(!!input, !!output);
        if (!FS.createDevice.major) FS.createDevice.major = 64;
        var dev = FS.makedev(FS.createDevice.major++, 0);
        // Create a fake device that a set of stream ops to emulate
        // the old behavior.
        FS.registerDevice(dev, {
          open: function(stream) {
            stream.seekable = false;
          },
          close: function(stream) {
            // flush any pending line data
            if (output && output.buffer && output.buffer.length) {
              output(10);
            }
          },
          read: function(stream, buffer, offset, length, pos /* ignored */) {
            var bytesRead = 0;
            for (var i = 0; i < length; i++) {
              var result;
              try {
                result = input();
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES.EIO);
              }
              if (result === undefined && bytesRead === 0) {
                throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
              }
              if (result === null || result === undefined) break;
              bytesRead++;
              buffer[offset+i] = result;
            }
            if (bytesRead) {
              stream.node.timestamp = Date.now();
            }
            return bytesRead;
          },
          write: function(stream, buffer, offset, length, pos) {
            for (var i = 0; i < length; i++) {
              try {
                output(buffer[offset+i]);
              } catch (e) {
                throw new FS.ErrnoError(ERRNO_CODES.EIO);
              }
            }
            if (length) {
              stream.node.timestamp = Date.now();
            }
            return i;
          }
        });
        return FS.mkdev(path, mode, dev);
      },createLink:function (parent, name, target, canRead, canWrite) {
        var path = PATH.join2(typeof parent === 'string' ? parent : FS.getPath(parent), name);
        return FS.symlink(target, path);
      },forceLoadFile:function (obj) {
        if (obj.isDevice || obj.isFolder || obj.link || obj.contents) return true;
        var success = true;
        if (typeof XMLHttpRequest !== 'undefined') {
          throw new Error("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
        } else if (Module['read']) {
          // Command-line.
          try {
            // WARNING: Can't read binary files in V8's d8 or tracemonkey's js, as
            //          read() will try to parse UTF8.
            obj.contents = intArrayFromString(Module['read'](obj.url), true);
          } catch (e) {
            success = false;
          }
        } else {
          throw new Error('Cannot load without read() or XMLHttpRequest.');
        }
        if (!success) ___setErrNo(ERRNO_CODES.EIO);
        return success;
      },createLazyFile:function (parent, name, url, canRead, canWrite) {
        if (typeof XMLHttpRequest !== 'undefined') {
          if (!ENVIRONMENT_IS_WORKER) throw 'Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc';
          // Lazy chunked Uint8Array (implements get and length from Uint8Array). Actual getting is abstracted away for eventual reuse.
          function LazyUint8Array() {
            this.lengthKnown = false;
            this.chunks = []; // Loaded chunks. Index is the chunk number
          }
          LazyUint8Array.prototype.get = function LazyUint8Array_get(idx) {
            if (idx > this.length-1 || idx < 0) {
              return undefined;
            }
            var chunkOffset = idx % this.chunkSize;
            var chunkNum = Math.floor(idx / this.chunkSize);
            return this.getter(chunkNum)[chunkOffset];
          }
          LazyUint8Array.prototype.setDataGetter = function LazyUint8Array_setDataGetter(getter) {
            this.getter = getter;
          }
          LazyUint8Array.prototype.cacheLength = function LazyUint8Array_cacheLength() {
              // Find length
              var xhr = new XMLHttpRequest();
              xhr.open('HEAD', url, false);
              xhr.send(null);
              if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
              var datalength = Number(xhr.getResponseHeader("Content-length"));
              var header;
              var hasByteServing = (header = xhr.getResponseHeader("Accept-Ranges")) && header === "bytes";
              var chunkSize = 1024*1024; // Chunk size in bytes
              if (!hasByteServing) chunkSize = datalength;
              // Function to get a range from the remote URL.
              var doXHR = (function(from, to) {
                if (from > to) throw new Error("invalid range (" + from + ", " + to + ") or no bytes requested!");
                if (to > datalength-1) throw new Error("only " + datalength + " bytes available! programmer error!");
                // TODO: Use mozResponseArrayBuffer, responseStream, etc. if available.
                var xhr = new XMLHttpRequest();
                xhr.open('GET', url, false);
                if (datalength !== chunkSize) xhr.setRequestHeader("Range", "bytes=" + from + "-" + to);
                // Some hints to the browser that we want binary data.
                if (typeof Uint8Array != 'undefined') xhr.responseType = 'arraybuffer';
                if (xhr.overrideMimeType) {
                  xhr.overrideMimeType('text/plain; charset=x-user-defined');
                }
                xhr.send(null);
                if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
                if (xhr.response !== undefined) {
                  return new Uint8Array(xhr.response || []);
                } else {
                  return intArrayFromString(xhr.responseText || '', true);
                }
              });
              var lazyArray = this;
              lazyArray.setDataGetter(function(chunkNum) {
                var start = chunkNum * chunkSize;
                var end = (chunkNum+1) * chunkSize - 1; // including this byte
                end = Math.min(end, datalength-1); // if datalength-1 is selected, this is the last block
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") {
                  lazyArray.chunks[chunkNum] = doXHR(start, end);
                }
                if (typeof(lazyArray.chunks[chunkNum]) === "undefined") throw new Error("doXHR failed!");
                return lazyArray.chunks[chunkNum];
              });
              this._length = datalength;
              this._chunkSize = chunkSize;
              this.lengthKnown = true;
          }
          var lazyArray = new LazyUint8Array();
          Object.defineProperty(lazyArray, "length", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._length;
              }
          });
          Object.defineProperty(lazyArray, "chunkSize", {
              get: function() {
                  if(!this.lengthKnown) {
                      this.cacheLength();
                  }
                  return this._chunkSize;
              }
          });
          var properties = { isDevice: false, contents: lazyArray };
        } else {
          var properties = { isDevice: false, url: url };
        }
        var node = FS.createFile(parent, name, properties, canRead, canWrite);
        // This is a total hack, but I want to get this lazy file code out of the
        // core of MEMFS. If we want to keep this lazy file concept I feel it should
        // be its own thin LAZYFS proxying calls to MEMFS.
        if (properties.contents) {
          node.contents = properties.contents;
        } else if (properties.url) {
          node.contents = null;
          node.url = properties.url;
        }
        // override each stream op with one that tries to force load the lazy file first
        var stream_ops = {};
        var keys = Object.keys(node.stream_ops);
        keys.forEach(function(key) {
          var fn = node.stream_ops[key];
          stream_ops[key] = function forceLoadLazyFile() {
            if (!FS.forceLoadFile(node)) {
              throw new FS.ErrnoError(ERRNO_CODES.EIO);
            }
            return fn.apply(null, arguments);
          };
        });
        // use a custom read function
        stream_ops.read = function stream_ops_read(stream, buffer, offset, length, position) {
          if (!FS.forceLoadFile(node)) {
            throw new FS.ErrnoError(ERRNO_CODES.EIO);
          }
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          assert(size >= 0);
          if (contents.slice) { // normal array
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          } else {
            for (var i = 0; i < size; i++) { // LazyUint8Array from sync binary XHR
              buffer[offset + i] = contents.get(position + i);
            }
          }
          return size;
        };
        node.stream_ops = stream_ops;
        return node;
      },createPreloadedFile:function (parent, name, url, canRead, canWrite, onload, onerror, dontCreateFile, canOwn) {
        Browser.init();
        // TODO we should allow people to just pass in a complete filename instead
        // of parent and name being that we just join them anyways
        var fullname = name ? PATH.resolve(PATH.join2(parent, name)) : parent;
        function processData(byteArray) {
          function finish(byteArray) {
            if (!dontCreateFile) {
              FS.createDataFile(parent, name, byteArray, canRead, canWrite, canOwn);
            }
            if (onload) onload();
            removeRunDependency('cp ' + fullname);
          }
          var handled = false;
          Module['preloadPlugins'].forEach(function(plugin) {
            if (handled) return;
            if (plugin['canHandle'](fullname)) {
              plugin['handle'](byteArray, fullname, finish, function() {
                if (onerror) onerror();
                removeRunDependency('cp ' + fullname);
              });
              handled = true;
            }
          });
          if (!handled) finish(byteArray);
        }
        addRunDependency('cp ' + fullname);
        if (typeof url == 'string') {
          Browser.asyncLoad(url, function(byteArray) {
            processData(byteArray);
          }, onerror);
        } else {
          processData(url);
        }
      },indexedDB:function () {
        return window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
      },DB_NAME:function () {
        return 'EM_FS_' + window.location.pathname;
      },DB_VERSION:20,DB_STORE_NAME:"FILE_DATA",saveFilesToDB:function (paths, onload, onerror) {
        onload = onload || function(){};
        onerror = onerror || function(){};
        var indexedDB = FS.indexedDB();
        try {
          var openRequest = indexedDB.open(FS.DB_NAME(), FS.DB_VERSION);
        } catch (e) {
          return onerror(e);
        }
        openRequest.onupgradeneeded = function openRequest_onupgradeneeded() {
          console.log('creating db');
          var db = openRequest.result;
          db.createObjectStore(FS.DB_STORE_NAME);
        };
        openRequest.onsuccess = function openRequest_onsuccess() {
          var db = openRequest.result;
          var transaction = db.transaction([FS.DB_STORE_NAME], 'readwrite');
          var files = transaction.objectStore(FS.DB_STORE_NAME);
          var ok = 0, fail = 0, total = paths.length;
          function finish() {
            if (fail == 0) onload(); else onerror();
          }
          paths.forEach(function(path) {
            var putRequest = files.put(FS.analyzePath(path).object.contents, path);
            putRequest.onsuccess = function putRequest_onsuccess() { ok++; if (ok + fail == total) finish() };
            putRequest.onerror = function putRequest_onerror() { fail++; if (ok + fail == total) finish() };
          });
          transaction.onerror = onerror;
        };
        openRequest.onerror = onerror;
      },loadFilesFromDB:function (paths, onload, onerror) {
        onload = onload || function(){};
        onerror = onerror || function(){};
        var indexedDB = FS.indexedDB();
        try {
          var openRequest = indexedDB.open(FS.DB_NAME(), FS.DB_VERSION);
        } catch (e) {
          return onerror(e);
        }
        openRequest.onupgradeneeded = onerror; // no database to load from
        openRequest.onsuccess = function openRequest_onsuccess() {
          var db = openRequest.result;
          try {
            var transaction = db.transaction([FS.DB_STORE_NAME], 'readonly');
          } catch(e) {
            onerror(e);
            return;
          }
          var files = transaction.objectStore(FS.DB_STORE_NAME);
          var ok = 0, fail = 0, total = paths.length;
          function finish() {
            if (fail == 0) onload(); else onerror();
          }
          paths.forEach(function(path) {
            var getRequest = files.get(path);
            getRequest.onsuccess = function getRequest_onsuccess() {
              if (FS.analyzePath(path).exists) {
                FS.unlink(path);
              }
              FS.createDataFile(PATH.dirname(path), PATH.basename(path), getRequest.result, true, true, true);
              ok++;
              if (ok + fail == total) finish();
            };
            getRequest.onerror = function getRequest_onerror() { fail++; if (ok + fail == total) finish() };
          });
          transaction.onerror = onerror;
        };
        openRequest.onerror = onerror;
      }};
  var SOCKFS={mount:function (mount) {
        return FS.createNode(null, '/', 16384 | 0777, 0);
      },createSocket:function (family, type, protocol) {
        var streaming = type == 1;
        if (protocol) {
          assert(streaming == (protocol == 6)); // if SOCK_STREAM, must be tcp
        }
        // create our internal socket structure
        var sock = {
          family: family,
          type: type,
          protocol: protocol,
          server: null,
          peers: {},
          pending: [],
          recv_queue: [],
          sock_ops: SOCKFS.websocket_sock_ops
        };
        // create the filesystem node to store the socket structure
        var name = SOCKFS.nextname();
        var node = FS.createNode(SOCKFS.root, name, 49152, 0);
        node.sock = sock;
        // and the wrapping stream that enables library functions such
        // as read and write to indirectly interact with the socket
        var stream = FS.createStream({
          path: name,
          node: node,
          flags: FS.modeStringToFlags('r+'),
          seekable: false,
          stream_ops: SOCKFS.stream_ops
        });
        // map the new stream to the socket structure (sockets have a 1:1
        // relationship with a stream)
        sock.stream = stream;
        return sock;
      },getSocket:function (fd) {
        var stream = FS.getStream(fd);
        if (!stream || !FS.isSocket(stream.node.mode)) {
          return null;
        }
        return stream.node.sock;
      },stream_ops:{poll:function (stream) {
          var sock = stream.node.sock;
          return sock.sock_ops.poll(sock);
        },ioctl:function (stream, request, varargs) {
          var sock = stream.node.sock;
          return sock.sock_ops.ioctl(sock, request, varargs);
        },read:function (stream, buffer, offset, length, position /* ignored */) {
          var sock = stream.node.sock;
          var msg = sock.sock_ops.recvmsg(sock, length);
          if (!msg) {
            // socket is closed
            return 0;
          }
          buffer.set(msg.buffer, offset);
          return msg.buffer.length;
        },write:function (stream, buffer, offset, length, position /* ignored */) {
          var sock = stream.node.sock;
          return sock.sock_ops.sendmsg(sock, buffer, offset, length);
        },close:function (stream) {
          var sock = stream.node.sock;
          sock.sock_ops.close(sock);
        }},nextname:function () {
        if (!SOCKFS.nextname.current) {
          SOCKFS.nextname.current = 0;
        }
        return 'socket[' + (SOCKFS.nextname.current++) + ']';
      },websocket_sock_ops:{createPeer:function (sock, addr, port) {
          var ws;
          if (typeof addr === 'object') {
            ws = addr;
            addr = null;
            port = null;
          }
          if (ws) {
            // for sockets that've already connected (e.g. we're the server)
            // we can inspect the _socket property for the address
            if (ws._socket) {
              addr = ws._socket.remoteAddress;
              port = ws._socket.remotePort;
            }
            // if we're just now initializing a connection to the remote,
            // inspect the url property
            else {
              var result = /ws[s]?:\/\/([^:]+):(\d+)/.exec(ws.url);
              if (!result) {
                throw new Error('WebSocket URL must be in the format ws(s)://address:port');
              }
              addr = result[1];
              port = parseInt(result[2], 10);
            }
          } else {
            // create the actual websocket object and connect
            try {
              var url = 'ws://' + addr + ':' + port;
              // the node ws library API is slightly different than the browser's
              var opts = ENVIRONMENT_IS_NODE ? {headers: {'websocket-protocol': ['binary']}} : ['binary'];
              // If node we use the ws library.
              var WebSocket = ENVIRONMENT_IS_NODE ? require('ws') : window['WebSocket'];
              ws = new WebSocket(url, opts);
              ws.binaryType = 'arraybuffer';
            } catch (e) {
              throw new FS.ErrnoError(ERRNO_CODES.EHOSTUNREACH);
            }
          }
          var peer = {
            addr: addr,
            port: port,
            socket: ws,
            dgram_send_queue: []
          };
          SOCKFS.websocket_sock_ops.addPeer(sock, peer);
          SOCKFS.websocket_sock_ops.handlePeerEvents(sock, peer);
          // if this is a bound dgram socket, send the port number first to allow
          // us to override the ephemeral port reported to us by remotePort on the
          // remote end.
          if (sock.type === 2 && typeof sock.sport !== 'undefined') {
            peer.dgram_send_queue.push(new Uint8Array([
                255, 255, 255, 255,
                'p'.charCodeAt(0), 'o'.charCodeAt(0), 'r'.charCodeAt(0), 't'.charCodeAt(0),
                ((sock.sport & 0xff00) >> 8) , (sock.sport & 0xff)
            ]));
          }
          return peer;
        },getPeer:function (sock, addr, port) {
          return sock.peers[addr + ':' + port];
        },addPeer:function (sock, peer) {
          sock.peers[peer.addr + ':' + peer.port] = peer;
        },removePeer:function (sock, peer) {
          delete sock.peers[peer.addr + ':' + peer.port];
        },handlePeerEvents:function (sock, peer) {
          var first = true;
          var handleOpen = function () {
            try {
              var queued = peer.dgram_send_queue.shift();
              while (queued) {
                peer.socket.send(queued);
                queued = peer.dgram_send_queue.shift();
              }
            } catch (e) {
              // not much we can do here in the way of proper error handling as we've already
              // lied and said this data was sent. shut it down.
              peer.socket.close();
            }
          };
          function handleMessage(data) {
            assert(typeof data !== 'string' && data.byteLength !== undefined);  // must receive an ArrayBuffer
            data = new Uint8Array(data);  // make a typed array view on the array buffer
            // if this is the port message, override the peer's port with it
            var wasfirst = first;
            first = false;
            if (wasfirst &&
                data.length === 10 &&
                data[0] === 255 && data[1] === 255 && data[2] === 255 && data[3] === 255 &&
                data[4] === 'p'.charCodeAt(0) && data[5] === 'o'.charCodeAt(0) && data[6] === 'r'.charCodeAt(0) && data[7] === 't'.charCodeAt(0)) {
              // update the peer's port and it's key in the peer map
              var newport = ((data[8] << 8) | data[9]);
              SOCKFS.websocket_sock_ops.removePeer(sock, peer);
              peer.port = newport;
              SOCKFS.websocket_sock_ops.addPeer(sock, peer);
              return;
            }
            sock.recv_queue.push({ addr: peer.addr, port: peer.port, data: data });
          };
          if (ENVIRONMENT_IS_NODE) {
            peer.socket.on('open', handleOpen);
            peer.socket.on('message', function(data, flags) {
              if (!flags.binary) {
                return;
              }
              handleMessage((new Uint8Array(data)).buffer);  // copy from node Buffer -> ArrayBuffer
            });
            peer.socket.on('error', function() {
              // don't throw
            });
          } else {
            peer.socket.onopen = handleOpen;
            peer.socket.onmessage = function peer_socket_onmessage(event) {
              handleMessage(event.data);
            };
          }
        },poll:function (sock) {
          if (sock.type === 1 && sock.server) {
            // listen sockets should only say they're available for reading
            // if there are pending clients.
            return sock.pending.length ? (64 | 1) : 0;
          }
          var mask = 0;
          var dest = sock.type === 1 ?  // we only care about the socket state for connection-based sockets
            SOCKFS.websocket_sock_ops.getPeer(sock, sock.daddr, sock.dport) :
            null;
          if (sock.recv_queue.length ||
              !dest ||  // connection-less sockets are always ready to read
              (dest && dest.socket.readyState === dest.socket.CLOSING) ||
              (dest && dest.socket.readyState === dest.socket.CLOSED)) {  // let recv return 0 once closed
            mask |= (64 | 1);
          }
          if (!dest ||  // connection-less sockets are always ready to write
              (dest && dest.socket.readyState === dest.socket.OPEN)) {
            mask |= 4;
          }
          if ((dest && dest.socket.readyState === dest.socket.CLOSING) ||
              (dest && dest.socket.readyState === dest.socket.CLOSED)) {
            mask |= 16;
          }
          return mask;
        },ioctl:function (sock, request, arg) {
          switch (request) {
            case 21531:
              var bytes = 0;
              if (sock.recv_queue.length) {
                bytes = sock.recv_queue[0].data.length;
              }
              HEAP32[((arg)>>2)]=bytes;
              return 0;
            default:
              return ERRNO_CODES.EINVAL;
          }
        },close:function (sock) {
          // if we've spawned a listen server, close it
          if (sock.server) {
            try {
              sock.server.close();
            } catch (e) {
            }
            sock.server = null;
          }
          // close any peer connections
          var peers = Object.keys(sock.peers);
          for (var i = 0; i < peers.length; i++) {
            var peer = sock.peers[peers[i]];
            try {
              peer.socket.close();
            } catch (e) {
            }
            SOCKFS.websocket_sock_ops.removePeer(sock, peer);
          }
          return 0;
        },bind:function (sock, addr, port) {
          if (typeof sock.saddr !== 'undefined' || typeof sock.sport !== 'undefined') {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);  // already bound
          }
          sock.saddr = addr;
          sock.sport = port || _mkport();
          // in order to emulate dgram sockets, we need to launch a listen server when
          // binding on a connection-less socket
          // note: this is only required on the server side
          if (sock.type === 2) {
            // close the existing server if it exists
            if (sock.server) {
              sock.server.close();
              sock.server = null;
            }
            // swallow error operation not supported error that occurs when binding in the
            // browser where this isn't supported
            try {
              sock.sock_ops.listen(sock, 0);
            } catch (e) {
              if (!(e instanceof FS.ErrnoError)) throw e;
              if (e.errno !== ERRNO_CODES.EOPNOTSUPP) throw e;
            }
          }
        },connect:function (sock, addr, port) {
          if (sock.server) {
            throw new FS.ErrnoError(ERRNO_CODS.EOPNOTSUPP);
          }
          // TODO autobind
          // if (!sock.addr && sock.type == 2) {
          // }
          // early out if we're already connected / in the middle of connecting
          if (typeof sock.daddr !== 'undefined' && typeof sock.dport !== 'undefined') {
            var dest = SOCKFS.websocket_sock_ops.getPeer(sock, sock.daddr, sock.dport);
            if (dest) {
              if (dest.socket.readyState === dest.socket.CONNECTING) {
                throw new FS.ErrnoError(ERRNO_CODES.EALREADY);
              } else {
                throw new FS.ErrnoError(ERRNO_CODES.EISCONN);
              }
            }
          }
          // add the socket to our peer list and set our
          // destination address / port to match
          var peer = SOCKFS.websocket_sock_ops.createPeer(sock, addr, port);
          sock.daddr = peer.addr;
          sock.dport = peer.port;
          // always "fail" in non-blocking mode
          throw new FS.ErrnoError(ERRNO_CODES.EINPROGRESS);
        },listen:function (sock, backlog) {
          if (!ENVIRONMENT_IS_NODE) {
            throw new FS.ErrnoError(ERRNO_CODES.EOPNOTSUPP);
          }
          if (sock.server) {
             throw new FS.ErrnoError(ERRNO_CODES.EINVAL);  // already listening
          }
          var WebSocketServer = require('ws').Server;
          var host = sock.saddr;
          sock.server = new WebSocketServer({
            host: host,
            port: sock.sport
            // TODO support backlog
          });
          sock.server.on('connection', function(ws) {
            if (sock.type === 1) {
              var newsock = SOCKFS.createSocket(sock.family, sock.type, sock.protocol);
              // create a peer on the new socket
              var peer = SOCKFS.websocket_sock_ops.createPeer(newsock, ws);
              newsock.daddr = peer.addr;
              newsock.dport = peer.port;
              // push to queue for accept to pick up
              sock.pending.push(newsock);
            } else {
              // create a peer on the listen socket so calling sendto
              // with the listen socket and an address will resolve
              // to the correct client
              SOCKFS.websocket_sock_ops.createPeer(sock, ws);
            }
          });
          sock.server.on('closed', function() {
            sock.server = null;
          });
          sock.server.on('error', function() {
            // don't throw
          });
        },accept:function (listensock) {
          if (!listensock.server) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
          var newsock = listensock.pending.shift();
          newsock.stream.flags = listensock.stream.flags;
          return newsock;
        },getname:function (sock, peer) {
          var addr, port;
          if (peer) {
            if (sock.daddr === undefined || sock.dport === undefined) {
              throw new FS.ErrnoError(ERRNO_CODES.ENOTCONN);
            }
            addr = sock.daddr;
            port = sock.dport;
          } else {
            // TODO saddr and sport will be set for bind()'d UDP sockets, but what
            // should we be returning for TCP sockets that've been connect()'d?
            addr = sock.saddr || 0;
            port = sock.sport || 0;
          }
          return { addr: addr, port: port };
        },sendmsg:function (sock, buffer, offset, length, addr, port) {
          if (sock.type === 2) {
            // connection-less sockets will honor the message address,
            // and otherwise fall back to the bound destination address
            if (addr === undefined || port === undefined) {
              addr = sock.daddr;
              port = sock.dport;
            }
            // if there was no address to fall back to, error out
            if (addr === undefined || port === undefined) {
              throw new FS.ErrnoError(ERRNO_CODES.EDESTADDRREQ);
            }
          } else {
            // connection-based sockets will only use the bound
            addr = sock.daddr;
            port = sock.dport;
          }
          // find the peer for the destination address
          var dest = SOCKFS.websocket_sock_ops.getPeer(sock, addr, port);
          // early out if not connected with a connection-based socket
          if (sock.type === 1) {
            if (!dest || dest.socket.readyState === dest.socket.CLOSING || dest.socket.readyState === dest.socket.CLOSED) {
              throw new FS.ErrnoError(ERRNO_CODES.ENOTCONN);
            } else if (dest.socket.readyState === dest.socket.CONNECTING) {
              throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
            }
          }
          // create a copy of the incoming data to send, as the WebSocket API
          // doesn't work entirely with an ArrayBufferView, it'll just send
          // the entire underlying buffer
          var data;
          if (buffer instanceof Array || buffer instanceof ArrayBuffer) {
            data = buffer.slice(offset, offset + length);
          } else {  // ArrayBufferView
            data = buffer.buffer.slice(buffer.byteOffset + offset, buffer.byteOffset + offset + length);
          }
          // if we're emulating a connection-less dgram socket and don't have
          // a cached connection, queue the buffer to send upon connect and
          // lie, saying the data was sent now.
          if (sock.type === 2) {
            if (!dest || dest.socket.readyState !== dest.socket.OPEN) {
              // if we're not connected, open a new connection
              if (!dest || dest.socket.readyState === dest.socket.CLOSING || dest.socket.readyState === dest.socket.CLOSED) {
                dest = SOCKFS.websocket_sock_ops.createPeer(sock, addr, port);
              }
              dest.dgram_send_queue.push(data);
              return length;
            }
          }
          try {
            // send the actual data
            dest.socket.send(data);
            return length;
          } catch (e) {
            throw new FS.ErrnoError(ERRNO_CODES.EINVAL);
          }
        },recvmsg:function (sock, length) {
          // http://pubs.opengroup.org/onlinepubs/7908799/xns/recvmsg.html
          if (sock.type === 1 && sock.server) {
            // tcp servers should not be recv()'ing on the listen socket
            throw new FS.ErrnoError(ERRNO_CODES.ENOTCONN);
          }
          var queued = sock.recv_queue.shift();
          if (!queued) {
            if (sock.type === 1) {
              var dest = SOCKFS.websocket_sock_ops.getPeer(sock, sock.daddr, sock.dport);
              if (!dest) {
                // if we have a destination address but are not connected, error out
                throw new FS.ErrnoError(ERRNO_CODES.ENOTCONN);
              }
              else if (dest.socket.readyState === dest.socket.CLOSING || dest.socket.readyState === dest.socket.CLOSED) {
                // return null if the socket has closed
                return null;
              }
              else {
                // else, our socket is in a valid state but truly has nothing available
                throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
              }
            } else {
              throw new FS.ErrnoError(ERRNO_CODES.EAGAIN);
            }
          }
          // queued.data will be an ArrayBuffer if it's unadulterated, but if it's
          // requeued TCP data it'll be an ArrayBufferView
          var queuedLength = queued.data.byteLength || queued.data.length;
          var queuedOffset = queued.data.byteOffset || 0;
          var queuedBuffer = queued.data.buffer || queued.data;
          var bytesRead = Math.min(length, queuedLength);
          var res = {
            buffer: new Uint8Array(queuedBuffer, queuedOffset, bytesRead),
            addr: queued.addr,
            port: queued.port
          };
          // push back any unread data for TCP connections
          if (sock.type === 1 && bytesRead < queuedLength) {
            var bytesRemaining = queuedLength - bytesRead;
            queued.data = new Uint8Array(queuedBuffer, queuedOffset + bytesRead, bytesRemaining);
            sock.recv_queue.unshift(queued);
          }
          return res;
        }}};function _send(fd, buf, len, flags) {
      var sock = SOCKFS.getSocket(fd);
      if (!sock) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      // TODO honor flags
      return _write(fd, buf, len);
    }
  function _pwrite(fildes, buf, nbyte, offset) {
      // ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/write.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        var slab = HEAP8;
        return FS.write(stream, slab, buf, nbyte, offset);
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }function _write(fildes, buf, nbyte) {
      // ssize_t write(int fildes, const void *buf, size_t nbyte);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/write.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        var slab = HEAP8;
        return FS.write(stream, slab, buf, nbyte);
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }
  function _strlen(ptr) {
      ptr = ptr|0;
      var curr = 0;
      curr = ptr;
      while (HEAP8[(curr)]) {
        curr = (curr + 1)|0;
      }
      return (curr - ptr)|0;
    }function _fputs(s, stream) {
      // int fputs(const char *restrict s, FILE *restrict stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fputs.html
      return _write(stream, s, _strlen(s));
    }
  function _llvm_uadd_with_overflow_i32(x, y) {
      x = x>>>0;
      y = y>>>0;
      return tempRet0 = x+y > 4294967295,(x+y)>>>0;
    }
  function _strstr(ptr1, ptr2) {
      var check = 0, start;
      do {
        if (!check) {
          start = ptr1;
          check = ptr2;
        }
        var curr1 = HEAP8[((ptr1++)|0)];
        var curr2 = HEAP8[((check++)|0)];
        if (curr2 == 0) return start;
        if (curr2 != curr1) {
          // rewind to one character after start, to find ez in eeez
          ptr1 = start + 1;
          check = 0;
        }
      } while (curr1);
      return 0;
    }
  function _tolower(chr) {
      chr = chr|0;
      if ((chr|0) < 65) return chr|0;
      if ((chr|0) > 90) return chr|0;
      return (chr - 65 + 97)|0;
    }
  function __reallyNegative(x) {
      return x < 0 || (x === 0 && (1/x) === -Infinity);
    }function __formatString(format, varargs) {
      var textIndex = format;
      var argIndex = 0;
      function getNextArg(type) {
        // NOTE: Explicitly ignoring type safety. Otherwise this fails:
        //       int x = 4; printf("%c\n", (char)x);
        var ret;
        if (type === 'double') {
          ret = HEAPF64[(((varargs)+(argIndex))>>3)];
        } else if (type == 'i64') {
          ret = [HEAP32[(((varargs)+(argIndex))>>2)],
                 HEAP32[(((varargs)+(argIndex+8))>>2)]];
          argIndex += 8; // each 32-bit chunk is in a 64-bit block
        } else {
          type = 'i32'; // varargs are always i32, i64, or double
          ret = HEAP32[(((varargs)+(argIndex))>>2)];
        }
        argIndex += Math.max(Runtime.getNativeFieldSize(type), Runtime.getAlignSize(type, null, true));
        return ret;
      }
      var ret = [];
      var curr, next, currArg;
      while(1) {
        var startTextIndex = textIndex;
        curr = HEAP8[(textIndex)];
        if (curr === 0) break;
        next = HEAP8[((textIndex+1)|0)];
        if (curr == 37) {
          // Handle flags.
          var flagAlwaysSigned = false;
          var flagLeftAlign = false;
          var flagAlternative = false;
          var flagZeroPad = false;
          var flagPadSign = false;
          flagsLoop: while (1) {
            switch (next) {
              case 43:
                flagAlwaysSigned = true;
                break;
              case 45:
                flagLeftAlign = true;
                break;
              case 35:
                flagAlternative = true;
                break;
              case 48:
                if (flagZeroPad) {
                  break flagsLoop;
                } else {
                  flagZeroPad = true;
                  break;
                }
              case 32:
                flagPadSign = true;
                break;
              default:
                break flagsLoop;
            }
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
          }
          // Handle width.
          var width = 0;
          if (next == 42) {
            width = getNextArg('i32');
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
          } else {
            while (next >= 48 && next <= 57) {
              width = width * 10 + (next - 48);
              textIndex++;
              next = HEAP8[((textIndex+1)|0)];
            }
          }
          // Handle precision.
          var precisionSet = false;
          if (next == 46) {
            var precision = 0;
            precisionSet = true;
            textIndex++;
            next = HEAP8[((textIndex+1)|0)];
            if (next == 42) {
              precision = getNextArg('i32');
              textIndex++;
            } else {
              while(1) {
                var precisionChr = HEAP8[((textIndex+1)|0)];
                if (precisionChr < 48 ||
                    precisionChr > 57) break;
                precision = precision * 10 + (precisionChr - 48);
                textIndex++;
              }
            }
            next = HEAP8[((textIndex+1)|0)];
          } else {
            var precision = 6; // Standard default.
          }
          // Handle integer sizes. WARNING: These assume a 32-bit architecture!
          var argSize;
          switch (String.fromCharCode(next)) {
            case 'h':
              var nextNext = HEAP8[((textIndex+2)|0)];
              if (nextNext == 104) {
                textIndex++;
                argSize = 1; // char (actually i32 in varargs)
              } else {
                argSize = 2; // short (actually i32 in varargs)
              }
              break;
            case 'l':
              var nextNext = HEAP8[((textIndex+2)|0)];
              if (nextNext == 108) {
                textIndex++;
                argSize = 8; // long long
              } else {
                argSize = 4; // long
              }
              break;
            case 'L': // long long
            case 'q': // int64_t
            case 'j': // intmax_t
              argSize = 8;
              break;
            case 'z': // size_t
            case 't': // ptrdiff_t
            case 'I': // signed ptrdiff_t or unsigned size_t
              argSize = 4;
              break;
            default:
              argSize = null;
          }
          if (argSize) textIndex++;
          next = HEAP8[((textIndex+1)|0)];
          // Handle type specifier.
          switch (String.fromCharCode(next)) {
            case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': case 'p': {
              // Integer.
              var signed = next == 100 || next == 105;
              argSize = argSize || 4;
              var currArg = getNextArg('i' + (argSize * 8));
              var origArg = currArg;
              var argText;
              // Flatten i64-1 [low, high] into a (slightly rounded) double
              if (argSize == 8) {
                currArg = Runtime.makeBigInt(currArg[0], currArg[1], next == 117);
              }
              // Truncate to requested size.
              if (argSize <= 4) {
                var limit = Math.pow(256, argSize) - 1;
                currArg = (signed ? reSign : unSign)(currArg & limit, argSize * 8);
              }
              // Format the number.
              var currAbsArg = Math.abs(currArg);
              var prefix = '';
              if (next == 100 || next == 105) {
                if (argSize == 8 && i64Math) argText = i64Math.stringify(origArg[0], origArg[1], null); else
                argText = reSign(currArg, 8 * argSize, 1).toString(10);
              } else if (next == 117) {
                if (argSize == 8 && i64Math) argText = i64Math.stringify(origArg[0], origArg[1], true); else
                argText = unSign(currArg, 8 * argSize, 1).toString(10);
                currArg = Math.abs(currArg);
              } else if (next == 111) {
                argText = (flagAlternative ? '0' : '') + currAbsArg.toString(8);
              } else if (next == 120 || next == 88) {
                prefix = (flagAlternative && currArg != 0) ? '0x' : '';
                if (argSize == 8 && i64Math) {
                  if (origArg[1]) {
                    argText = (origArg[1]>>>0).toString(16);
                    var lower = (origArg[0]>>>0).toString(16);
                    while (lower.length < 8) lower = '0' + lower;
                    argText += lower;
                  } else {
                    argText = (origArg[0]>>>0).toString(16);
                  }
                } else
                if (currArg < 0) {
                  // Represent negative numbers in hex as 2's complement.
                  currArg = -currArg;
                  argText = (currAbsArg - 1).toString(16);
                  var buffer = [];
                  for (var i = 0; i < argText.length; i++) {
                    buffer.push((0xF - parseInt(argText[i], 16)).toString(16));
                  }
                  argText = buffer.join('');
                  while (argText.length < argSize * 2) argText = 'f' + argText;
                } else {
                  argText = currAbsArg.toString(16);
                }
                if (next == 88) {
                  prefix = prefix.toUpperCase();
                  argText = argText.toUpperCase();
                }
              } else if (next == 112) {
                if (currAbsArg === 0) {
                  argText = '(nil)';
                } else {
                  prefix = '0x';
                  argText = currAbsArg.toString(16);
                }
              }
              if (precisionSet) {
                while (argText.length < precision) {
                  argText = '0' + argText;
                }
              }
              // Add sign if needed
              if (currArg >= 0) {
                if (flagAlwaysSigned) {
                  prefix = '+' + prefix;
                } else if (flagPadSign) {
                  prefix = ' ' + prefix;
                }
              }
              // Move sign to prefix so we zero-pad after the sign
              if (argText.charAt(0) == '-') {
                prefix = '-' + prefix;
                argText = argText.substr(1);
              }
              // Add padding.
              while (prefix.length + argText.length < width) {
                if (flagLeftAlign) {
                  argText += ' ';
                } else {
                  if (flagZeroPad) {
                    argText = '0' + argText;
                  } else {
                    prefix = ' ' + prefix;
                  }
                }
              }
              // Insert the result into the buffer.
              argText = prefix + argText;
              argText.split('').forEach(function(chr) {
                ret.push(chr.charCodeAt(0));
              });
              break;
            }
            case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': {
              // Float.
              var currArg = getNextArg('double');
              var argText;
              if (isNaN(currArg)) {
                argText = 'nan';
                flagZeroPad = false;
              } else if (!isFinite(currArg)) {
                argText = (currArg < 0 ? '-' : '') + 'inf';
                flagZeroPad = false;
              } else {
                var isGeneral = false;
                var effectivePrecision = Math.min(precision, 20);
                // Convert g/G to f/F or e/E, as per:
                // http://pubs.opengroup.org/onlinepubs/9699919799/functions/printf.html
                if (next == 103 || next == 71) {
                  isGeneral = true;
                  precision = precision || 1;
                  var exponent = parseInt(currArg.toExponential(effectivePrecision).split('e')[1], 10);
                  if (precision > exponent && exponent >= -4) {
                    next = ((next == 103) ? 'f' : 'F').charCodeAt(0);
                    precision -= exponent + 1;
                  } else {
                    next = ((next == 103) ? 'e' : 'E').charCodeAt(0);
                    precision--;
                  }
                  effectivePrecision = Math.min(precision, 20);
                }
                if (next == 101 || next == 69) {
                  argText = currArg.toExponential(effectivePrecision);
                  // Make sure the exponent has at least 2 digits.
                  if (/[eE][-+]\d$/.test(argText)) {
                    argText = argText.slice(0, -1) + '0' + argText.slice(-1);
                  }
                } else if (next == 102 || next == 70) {
                  argText = currArg.toFixed(effectivePrecision);
                  if (currArg === 0 && __reallyNegative(currArg)) {
                    argText = '-' + argText;
                  }
                }
                var parts = argText.split('e');
                if (isGeneral && !flagAlternative) {
                  // Discard trailing zeros and periods.
                  while (parts[0].length > 1 && parts[0].indexOf('.') != -1 &&
                         (parts[0].slice(-1) == '0' || parts[0].slice(-1) == '.')) {
                    parts[0] = parts[0].slice(0, -1);
                  }
                } else {
                  // Make sure we have a period in alternative mode.
                  if (flagAlternative && argText.indexOf('.') == -1) parts[0] += '.';
                  // Zero pad until required precision.
                  while (precision > effectivePrecision++) parts[0] += '0';
                }
                argText = parts[0] + (parts.length > 1 ? 'e' + parts[1] : '');
                // Capitalize 'E' if needed.
                if (next == 69) argText = argText.toUpperCase();
                // Add sign.
                if (currArg >= 0) {
                  if (flagAlwaysSigned) {
                    argText = '+' + argText;
                  } else if (flagPadSign) {
                    argText = ' ' + argText;
                  }
                }
              }
              // Add padding.
              while (argText.length < width) {
                if (flagLeftAlign) {
                  argText += ' ';
                } else {
                  if (flagZeroPad && (argText[0] == '-' || argText[0] == '+')) {
                    argText = argText[0] + '0' + argText.slice(1);
                  } else {
                    argText = (flagZeroPad ? '0' : ' ') + argText;
                  }
                }
              }
              // Adjust case.
              if (next < 97) argText = argText.toUpperCase();
              // Insert the result into the buffer.
              argText.split('').forEach(function(chr) {
                ret.push(chr.charCodeAt(0));
              });
              break;
            }
            case 's': {
              // String.
              var arg = getNextArg('i8*');
              var argLength = arg ? _strlen(arg) : '(null)'.length;
              if (precisionSet) argLength = Math.min(argLength, precision);
              if (!flagLeftAlign) {
                while (argLength < width--) {
                  ret.push(32);
                }
              }
              if (arg) {
                for (var i = 0; i < argLength; i++) {
                  ret.push(HEAPU8[((arg++)|0)]);
                }
              } else {
                ret = ret.concat(intArrayFromString('(null)'.substr(0, argLength), true));
              }
              if (flagLeftAlign) {
                while (argLength < width--) {
                  ret.push(32);
                }
              }
              break;
            }
            case 'c': {
              // Character.
              if (flagLeftAlign) ret.push(getNextArg('i8'));
              while (--width > 0) {
                ret.push(32);
              }
              if (!flagLeftAlign) ret.push(getNextArg('i8'));
              break;
            }
            case 'n': {
              // Write the length written so far to the next parameter.
              var ptr = getNextArg('i32*');
              HEAP32[((ptr)>>2)]=ret.length
              break;
            }
            case '%': {
              // Literal percent sign.
              ret.push(curr);
              break;
            }
            default: {
              // Unknown specifiers remain untouched.
              for (var i = startTextIndex; i < textIndex + 2; i++) {
                ret.push(HEAP8[(i)]);
              }
            }
          }
          textIndex += 2;
          // TODO: Support a/A (hex float) and m (last error) specifiers.
          // TODO: Support %1${specifier} for arg selection.
        } else {
          ret.push(curr);
          textIndex += 1;
        }
      }
      return ret;
    }function _snprintf(s, n, format, varargs) {
      // int snprintf(char *restrict s, size_t n, const char *restrict format, ...);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/printf.html
      var result = __formatString(format, varargs);
      var limit = (n === undefined) ? result.length
                                    : Math.min(result.length, Math.max(n - 1, 0));
      if (s < 0) {
        s = -s;
        var buf = _malloc(limit+1);
        HEAP32[((s)>>2)]=buf;
        s = buf;
      }
      for (var i = 0; i < limit; i++) {
        HEAP8[(((s)+(i))|0)]=result[i];
      }
      if (limit < n || (n === undefined)) HEAP8[(((s)+(i))|0)]=0;
      return result.length;
    }function _sprintf(s, format, varargs) {
      // int sprintf(char *restrict s, const char *restrict format, ...);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/printf.html
      return _snprintf(s, undefined, format, varargs);
    }
  function _sigemptyset(set) {
      // int sigemptyset(sigset_t *set);
      HEAP32[((set)>>2)]=0;
      return 0;
    }
  function _sigaction(set) {
      // TODO:
      return 0;
    }
  function _abort() {
      Module['abort']();
    }
  function _pthread_mutexattr_init() {}
  function _pthread_mutexattr_settype() {}
  function _pthread_mutex_init() {}
  function _pthread_mutexattr_destroy() {}
  function _pthread_mutex_destroy() {}
  function _pthread_mutex_lock() {}
  function _pthread_mutex_unlock() {}
  function _gettimeofday(ptr) {
      var now = Date.now();
      HEAP32[((ptr)>>2)]=Math.floor(now/1000); // seconds
      HEAP32[(((ptr)+(4))>>2)]=Math.floor((now-1000*Math.floor(now/1000))*1000); // microseconds
      return 0;
    }
  var ___tm_current=allocate(44, "i8", ALLOC_STATIC);
  var ___tm_timezone=allocate(intArrayFromString("GMT"), "i8", ALLOC_STATIC);function _gmtime_r(time, tmPtr) {
      var date = new Date(HEAP32[((time)>>2)]*1000);
      HEAP32[((tmPtr)>>2)]=date.getUTCSeconds()
      HEAP32[(((tmPtr)+(4))>>2)]=date.getUTCMinutes()
      HEAP32[(((tmPtr)+(8))>>2)]=date.getUTCHours()
      HEAP32[(((tmPtr)+(12))>>2)]=date.getUTCDate()
      HEAP32[(((tmPtr)+(16))>>2)]=date.getUTCMonth()
      HEAP32[(((tmPtr)+(20))>>2)]=date.getUTCFullYear()-1900
      HEAP32[(((tmPtr)+(24))>>2)]=date.getUTCDay()
      HEAP32[(((tmPtr)+(36))>>2)]=0
      HEAP32[(((tmPtr)+(32))>>2)]=0
      var start = new Date(date); // define date using UTC, start from Jan 01 00:00:00 UTC
      start.setUTCDate(1);
      start.setUTCMonth(0);
      start.setUTCHours(0);
      start.setUTCMinutes(0);
      start.setUTCSeconds(0);
      start.setUTCMilliseconds(0);
      var yday = Math.floor((date.getTime() - start.getTime()) / (1000 * 60 * 60 * 24));
      HEAP32[(((tmPtr)+(28))>>2)]=yday
      HEAP32[(((tmPtr)+(40))>>2)]=___tm_timezone
      return tmPtr;
    }function _gmtime(time) {
      return _gmtime_r(time, ___tm_current);
    }
  function __isLeapYear(year) {
        return year%4 === 0 && (year%100 !== 0 || year%400 === 0);
    }
  function __arraySum(array, index) {
      var sum = 0;
      for (var i = 0; i <= index; sum += array[i++]);
      return sum;
    }
  var __MONTH_DAYS_LEAP=[31,29,31,30,31,30,31,31,30,31,30,31];
  var __MONTH_DAYS_REGULAR=[31,28,31,30,31,30,31,31,30,31,30,31];function __addDays(date, days) {
      var newDate = new Date(date.getTime());
      while(days > 0) {
        var leap = __isLeapYear(newDate.getFullYear());
        var currentMonth = newDate.getMonth();
        var daysInCurrentMonth = (leap ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR)[currentMonth];
        if (days > daysInCurrentMonth-newDate.getDate()) {
          // we spill over to next month
          days -= (daysInCurrentMonth-newDate.getDate()+1);
          newDate.setDate(1);
          if (currentMonth < 11) {
            newDate.setMonth(currentMonth+1)
          } else {
            newDate.setMonth(0);
            newDate.setFullYear(newDate.getFullYear()+1);
          }
        } else {
          // we stay in current month 
          newDate.setDate(newDate.getDate()+days);
          return newDate;
        }
      }
      return newDate;
    }function _strftime(s, maxsize, format, tm) {
      // size_t strftime(char *restrict s, size_t maxsize, const char *restrict format, const struct tm *restrict timeptr);
      // http://pubs.opengroup.org/onlinepubs/009695399/functions/strftime.html
      var date = {
        tm_sec: HEAP32[((tm)>>2)],
        tm_min: HEAP32[(((tm)+(4))>>2)],
        tm_hour: HEAP32[(((tm)+(8))>>2)],
        tm_mday: HEAP32[(((tm)+(12))>>2)],
        tm_mon: HEAP32[(((tm)+(16))>>2)],
        tm_year: HEAP32[(((tm)+(20))>>2)],
        tm_wday: HEAP32[(((tm)+(24))>>2)],
        tm_yday: HEAP32[(((tm)+(28))>>2)],
        tm_isdst: HEAP32[(((tm)+(32))>>2)]
      };
      var pattern = Pointer_stringify(format);
      // expand format
      var EXPANSION_RULES_1 = {
        '%c': '%a %b %d %H:%M:%S %Y',     // Replaced by the locale's appropriate date and time representation - e.g., Mon Aug  3 14:02:01 2013
        '%D': '%m/%d/%y',                 // Equivalent to %m / %d / %y
        '%F': '%Y-%m-%d',                 // Equivalent to %Y - %m - %d
        '%h': '%b',                       // Equivalent to %b
        '%r': '%I:%M:%S %p',              // Replaced by the time in a.m. and p.m. notation
        '%R': '%H:%M',                    // Replaced by the time in 24-hour notation
        '%T': '%H:%M:%S',                 // Replaced by the time
        '%x': '%m/%d/%y',                 // Replaced by the locale's appropriate date representation
        '%X': '%H:%M:%S',                 // Replaced by the locale's appropriate date representation
      };
      for (var rule in EXPANSION_RULES_1) {
        pattern = pattern.replace(new RegExp(rule, 'g'), EXPANSION_RULES_1[rule]);
      }
      var WEEKDAYS = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
      var MONTHS = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
      function leadingSomething(value, digits, character) {
        var str = typeof value === 'number' ? value.toString() : (value || '');
        while (str.length < digits) {
          str = character[0]+str;
        }
        return str;
      };
      function leadingNulls(value, digits) {
        return leadingSomething(value, digits, '0');
      };
      function compareByDay(date1, date2) {
        function sgn(value) {
          return value < 0 ? -1 : (value > 0 ? 1 : 0);
        };
        var compare;
        if ((compare = sgn(date1.getFullYear()-date2.getFullYear())) === 0) {
          if ((compare = sgn(date1.getMonth()-date2.getMonth())) === 0) {
            compare = sgn(date1.getDate()-date2.getDate());
          }
        }
        return compare;
      };
      function getFirstWeekStartDate(janFourth) {
          switch (janFourth.getDay()) {
            case 0: // Sunday
              return new Date(janFourth.getFullYear()-1, 11, 29);
            case 1: // Monday
              return janFourth;
            case 2: // Tuesday
              return new Date(janFourth.getFullYear(), 0, 3);
            case 3: // Wednesday
              return new Date(janFourth.getFullYear(), 0, 2);
            case 4: // Thursday
              return new Date(janFourth.getFullYear(), 0, 1);
            case 5: // Friday
              return new Date(janFourth.getFullYear()-1, 11, 31);
            case 6: // Saturday
              return new Date(janFourth.getFullYear()-1, 11, 30);
          }
      };
      function getWeekBasedYear(date) {
          var thisDate = __addDays(new Date(date.tm_year+1900, 0, 1), date.tm_yday);
          var janFourthThisYear = new Date(thisDate.getFullYear(), 0, 4);
          var janFourthNextYear = new Date(thisDate.getFullYear()+1, 0, 4);
          var firstWeekStartThisYear = getFirstWeekStartDate(janFourthThisYear);
          var firstWeekStartNextYear = getFirstWeekStartDate(janFourthNextYear);
          if (compareByDay(firstWeekStartThisYear, thisDate) <= 0) {
            // this date is after the start of the first week of this year
            if (compareByDay(firstWeekStartNextYear, thisDate) <= 0) {
              return thisDate.getFullYear()+1;
            } else {
              return thisDate.getFullYear();
            }
          } else { 
            return thisDate.getFullYear()-1;
          }
      };
      var EXPANSION_RULES_2 = {
        '%a': function(date) {
          return WEEKDAYS[date.tm_wday].substring(0,3);
        },
        '%A': function(date) {
          return WEEKDAYS[date.tm_wday];
        },
        '%b': function(date) {
          return MONTHS[date.tm_mon].substring(0,3);
        },
        '%B': function(date) {
          return MONTHS[date.tm_mon];
        },
        '%C': function(date) {
          var year = date.tm_year+1900;
          return leadingNulls(Math.floor(year/100),2);
        },
        '%d': function(date) {
          return leadingNulls(date.tm_mday, 2);
        },
        '%e': function(date) {
          return leadingSomething(date.tm_mday, 2, ' ');
        },
        '%g': function(date) {
          // %g, %G, and %V give values according to the ISO 8601:2000 standard week-based year. 
          // In this system, weeks begin on a Monday and week 1 of the year is the week that includes 
          // January 4th, which is also the week that includes the first Thursday of the year, and 
          // is also the first week that contains at least four days in the year. 
          // If the first Monday of January is the 2nd, 3rd, or 4th, the preceding days are part of 
          // the last week of the preceding year; thus, for Saturday 2nd January 1999, 
          // %G is replaced by 1998 and %V is replaced by 53. If December 29th, 30th, 
          // or 31st is a Monday, it and any following days are part of week 1 of the following year. 
          // Thus, for Tuesday 30th December 1997, %G is replaced by 1998 and %V is replaced by 01.
          return getWeekBasedYear(date).toString().substring(2);
        },
        '%G': function(date) {
          return getWeekBasedYear(date);
        },
        '%H': function(date) {
          return leadingNulls(date.tm_hour, 2);
        },
        '%I': function(date) {
          return leadingNulls(date.tm_hour < 13 ? date.tm_hour : date.tm_hour-12, 2);
        },
        '%j': function(date) {
          // Day of the year (001-366)
          return leadingNulls(date.tm_mday+__arraySum(__isLeapYear(date.tm_year+1900) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, date.tm_mon-1), 3);
        },
        '%m': function(date) {
          return leadingNulls(date.tm_mon+1, 2);
        },
        '%M': function(date) {
          return leadingNulls(date.tm_min, 2);
        },
        '%n': function() {
          return '\n';
        },
        '%p': function(date) {
          if (date.tm_hour > 0 && date.tm_hour < 13) {
            return 'AM';
          } else {
            return 'PM';
          }
        },
        '%S': function(date) {
          return leadingNulls(date.tm_sec, 2);
        },
        '%t': function() {
          return '\t';
        },
        '%u': function(date) {
          var day = new Date(date.tm_year+1900, date.tm_mon+1, date.tm_mday, 0, 0, 0, 0);
          return day.getDay() || 7;
        },
        '%U': function(date) {
          // Replaced by the week number of the year as a decimal number [00,53]. 
          // The first Sunday of January is the first day of week 1; 
          // days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
          var janFirst = new Date(date.tm_year+1900, 0, 1);
          var firstSunday = janFirst.getDay() === 0 ? janFirst : __addDays(janFirst, 7-janFirst.getDay());
          var endDate = new Date(date.tm_year+1900, date.tm_mon, date.tm_mday);
          // is target date after the first Sunday?
          if (compareByDay(firstSunday, endDate) < 0) {
            // calculate difference in days between first Sunday and endDate
            var februaryFirstUntilEndMonth = __arraySum(__isLeapYear(endDate.getFullYear()) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, endDate.getMonth()-1)-31;
            var firstSundayUntilEndJanuary = 31-firstSunday.getDate();
            var days = firstSundayUntilEndJanuary+februaryFirstUntilEndMonth+endDate.getDate();
            return leadingNulls(Math.ceil(days/7), 2);
          }
          return compareByDay(firstSunday, janFirst) === 0 ? '01': '00';
        },
        '%V': function(date) {
          // Replaced by the week number of the year (Monday as the first day of the week) 
          // as a decimal number [01,53]. If the week containing 1 January has four 
          // or more days in the new year, then it is considered week 1. 
          // Otherwise, it is the last week of the previous year, and the next week is week 1. 
          // Both January 4th and the first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]
          var janFourthThisYear = new Date(date.tm_year+1900, 0, 4);
          var janFourthNextYear = new Date(date.tm_year+1901, 0, 4);
          var firstWeekStartThisYear = getFirstWeekStartDate(janFourthThisYear);
          var firstWeekStartNextYear = getFirstWeekStartDate(janFourthNextYear);
          var endDate = __addDays(new Date(date.tm_year+1900, 0, 1), date.tm_yday);
          if (compareByDay(endDate, firstWeekStartThisYear) < 0) {
            // if given date is before this years first week, then it belongs to the 53rd week of last year
            return '53';
          } 
          if (compareByDay(firstWeekStartNextYear, endDate) <= 0) {
            // if given date is after next years first week, then it belongs to the 01th week of next year
            return '01';
          }
          // given date is in between CW 01..53 of this calendar year
          var daysDifference;
          if (firstWeekStartThisYear.getFullYear() < date.tm_year+1900) {
            // first CW of this year starts last year
            daysDifference = date.tm_yday+32-firstWeekStartThisYear.getDate()
          } else {
            // first CW of this year starts this year
            daysDifference = date.tm_yday+1-firstWeekStartThisYear.getDate();
          }
          return leadingNulls(Math.ceil(daysDifference/7), 2);
        },
        '%w': function(date) {
          var day = new Date(date.tm_year+1900, date.tm_mon+1, date.tm_mday, 0, 0, 0, 0);
          return day.getDay();
        },
        '%W': function(date) {
          // Replaced by the week number of the year as a decimal number [00,53]. 
          // The first Monday of January is the first day of week 1; 
          // days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
          var janFirst = new Date(date.tm_year, 0, 1);
          var firstMonday = janFirst.getDay() === 1 ? janFirst : __addDays(janFirst, janFirst.getDay() === 0 ? 1 : 7-janFirst.getDay()+1);
          var endDate = new Date(date.tm_year+1900, date.tm_mon, date.tm_mday);
          // is target date after the first Monday?
          if (compareByDay(firstMonday, endDate) < 0) {
            var februaryFirstUntilEndMonth = __arraySum(__isLeapYear(endDate.getFullYear()) ? __MONTH_DAYS_LEAP : __MONTH_DAYS_REGULAR, endDate.getMonth()-1)-31;
            var firstMondayUntilEndJanuary = 31-firstMonday.getDate();
            var days = firstMondayUntilEndJanuary+februaryFirstUntilEndMonth+endDate.getDate();
            return leadingNulls(Math.ceil(days/7), 2);
          }
          return compareByDay(firstMonday, janFirst) === 0 ? '01': '00';
        },
        '%y': function(date) {
          // Replaced by the last two digits of the year as a decimal number [00,99]. [ tm_year]
          return (date.tm_year+1900).toString().substring(2);
        },
        '%Y': function(date) {
          // Replaced by the year as a decimal number (for example, 1997). [ tm_year]
          return date.tm_year+1900;
        },
        '%z': function(date) {
          // Replaced by the offset from UTC in the ISO 8601:2000 standard format ( +hhmm or -hhmm ),
          // or by no characters if no timezone is determinable. 
          // For example, "-0430" means 4 hours 30 minutes behind UTC (west of Greenwich). 
          // If tm_isdst is zero, the standard time offset is used. 
          // If tm_isdst is greater than zero, the daylight savings time offset is used. 
          // If tm_isdst is negative, no characters are returned. 
          // FIXME: we cannot determine time zone (or can we?)
          return '';
        },
        '%Z': function(date) {
          // Replaced by the timezone name or abbreviation, or by no bytes if no timezone information exists. [ tm_isdst]
          // FIXME: we cannot determine time zone (or can we?)
          return '';
        },
        '%%': function() {
          return '%';
        }
      };
      for (var rule in EXPANSION_RULES_2) {
        if (pattern.indexOf(rule) >= 0) {
          pattern = pattern.replace(new RegExp(rule, 'g'), EXPANSION_RULES_2[rule](date));
        }
      }
      var bytes = intArrayFromString(pattern, false);
      if (bytes.length > maxsize) {
        return 0;
      } 
      writeArrayToMemory(bytes, s);
      return bytes.length-1;
    }
  var _fabs=Math_abs;
  var GL={counter:1,lastError:0,buffers:[],programs:[],framebuffers:[],renderbuffers:[],textures:[],uniforms:[],shaders:[],currArrayBuffer:0,currElementArrayBuffer:0,byteSizeByTypeRoot:5120,byteSizeByType:[1,1,2,2,4,4,4,2,3,4,8],programInfos:{},stringCache:{},packAlignment:4,unpackAlignment:4,init:function () {
        Browser.moduleContextCreatedCallbacks.push(GL.initExtensions);
      },recordError:function recordError(errorCode) {
        if (!GL.lastError) {
          GL.lastError = errorCode;
        }
      },getNewId:function (table) {
        var ret = GL.counter++;
        for (var i = table.length; i < ret; i++) {
          table[i] = null;
        }
        return ret;
      },MINI_TEMP_BUFFER_SIZE:16,miniTempBuffer:null,miniTempBufferViews:[0],MAX_TEMP_BUFFER_SIZE:2097152,tempBufferIndexLookup:null,tempVertexBuffers:null,tempIndexBuffers:null,tempQuadIndexBuffer:null,generateTempBuffers:function (quads) {
        GL.tempBufferIndexLookup = new Uint8Array(GL.MAX_TEMP_BUFFER_SIZE+1);
        GL.tempVertexBuffers = [];
        GL.tempIndexBuffers = [];
        var last = -1, curr = -1;
        var size = 1;
        for (var i = 0; i <= GL.MAX_TEMP_BUFFER_SIZE; i++) {
          if (i > size) {
            size <<= 1;
          }
          if (size != last) {
            curr++;
            GL.tempVertexBuffers[curr] = Module.ctx.createBuffer();
            Module.ctx.bindBuffer(Module.ctx.ARRAY_BUFFER, GL.tempVertexBuffers[curr]);
            Module.ctx.bufferData(Module.ctx.ARRAY_BUFFER, size, Module.ctx.DYNAMIC_DRAW);
            Module.ctx.bindBuffer(Module.ctx.ARRAY_BUFFER, null);
            GL.tempIndexBuffers[curr] = Module.ctx.createBuffer();
            Module.ctx.bindBuffer(Module.ctx.ELEMENT_ARRAY_BUFFER, GL.tempIndexBuffers[curr]);
            Module.ctx.bufferData(Module.ctx.ELEMENT_ARRAY_BUFFER, size, Module.ctx.DYNAMIC_DRAW);
            Module.ctx.bindBuffer(Module.ctx.ELEMENT_ARRAY_BUFFER, null);
            last = size;
          }
          GL.tempBufferIndexLookup[i] = curr;
        }
        if (quads) {
          // GL_QUAD indexes can be precalculated
          GL.tempQuadIndexBuffer = Module.ctx.createBuffer();
          Module.ctx.bindBuffer(Module.ctx.ELEMENT_ARRAY_BUFFER, GL.tempQuadIndexBuffer);
          var numIndexes = GL.MAX_TEMP_BUFFER_SIZE >> 1;
          var quadIndexes = new Uint16Array(numIndexes);
          var i = 0, v = 0;
          while (1) {
            quadIndexes[i++] = v;
            if (i >= numIndexes) break;
            quadIndexes[i++] = v+1;
            if (i >= numIndexes) break;
            quadIndexes[i++] = v+2;
            if (i >= numIndexes) break;
            quadIndexes[i++] = v;
            if (i >= numIndexes) break;
            quadIndexes[i++] = v+2;
            if (i >= numIndexes) break;
            quadIndexes[i++] = v+3;
            if (i >= numIndexes) break;
            v += 4;
          }
          Module.ctx.bufferData(Module.ctx.ELEMENT_ARRAY_BUFFER, quadIndexes, Module.ctx.STATIC_DRAW);
          Module.ctx.bindBuffer(Module.ctx.ELEMENT_ARRAY_BUFFER, null);
        }
      },findToken:function (source, token) {
        function isIdentChar(ch) {
          if (ch >= 48 && ch <= 57) // 0-9
            return true;
          if (ch >= 65 && ch <= 90) // A-Z
            return true;
          if (ch >= 97 && ch <= 122) // a-z
            return true;
          return false;
        }
        var i = -1;
        do {
          i = source.indexOf(token, i + 1);
          if (i < 0) {
            break;
          }
          if (i > 0 && isIdentChar(source[i - 1])) {
            continue;
          }
          i += token.length;
          if (i < source.length - 1 && isIdentChar(source[i + 1])) {
            continue;
          }
          return true;
        } while (true);
        return false;
      },getSource:function (shader, count, string, length) {
        var source = '';
        for (var i = 0; i < count; ++i) {
          var frag;
          if (length) {
            var len = HEAP32[(((length)+(i*4))>>2)];
            if (len < 0) {
              frag = Pointer_stringify(HEAP32[(((string)+(i*4))>>2)]);
            } else {
              frag = Pointer_stringify(HEAP32[(((string)+(i*4))>>2)], len);
            }
          } else {
            frag = Pointer_stringify(HEAP32[(((string)+(i*4))>>2)]);
          }
          source += frag;
        }
        // Let's see if we need to enable the standard derivatives extension
        type = Module.ctx.getShaderParameter(GL.shaders[shader], 0x8B4F /* GL_SHADER_TYPE */);
        if (type == 0x8B30 /* GL_FRAGMENT_SHADER */) {
          if (GL.findToken(source, "dFdx") ||
              GL.findToken(source, "dFdy") ||
              GL.findToken(source, "fwidth")) {
            source = "#extension GL_OES_standard_derivatives : enable\n" + source;
            var extension = Module.ctx.getExtension("OES_standard_derivatives");
          }
        }
        return source;
      },computeImageSize:function (width, height, sizePerPixel, alignment) {
        function roundedToNextMultipleOf(x, y) {
          return Math.floor((x + y - 1) / y) * y
        }
        var plainRowSize = width * sizePerPixel;
        var alignedRowSize = roundedToNextMultipleOf(plainRowSize, alignment);
        return (height <= 0) ? 0 :
                 ((height - 1) * alignedRowSize + plainRowSize);
      },getTexPixelData:function (type, format, width, height, pixels, internalFormat) {
        var sizePerPixel;
        switch (type) {
          case 0x1401 /* GL_UNSIGNED_BYTE */:
            switch (format) {
              case 0x1906 /* GL_ALPHA */:
              case 0x1909 /* GL_LUMINANCE */:
                sizePerPixel = 1;
                break;
              case 0x1907 /* GL_RGB */:
                sizePerPixel = 3;
                break;
              case 0x1908 /* GL_RGBA */:
                sizePerPixel = 4;
                break;
              case 0x190A /* GL_LUMINANCE_ALPHA */:
                sizePerPixel = 2;
                break;
              default:
                throw 'Invalid format (' + format + ')';
            }
            break;
          case 0x1403 /* GL_UNSIGNED_SHORT */:
            if (format == 0x1902 /* GL_DEPTH_COMPONENT */) {
              sizePerPixel = 2;
            } else {
              throw 'Invalid format (' + format + ')';
            }
            break;
          case 0x1405 /* GL_UNSIGNED_INT */:
            if (format == 0x1902 /* GL_DEPTH_COMPONENT */) {
              sizePerPixel = 4;
            } else {
              throw 'Invalid format (' + format + ')';
            }
            break;
          case 0x84FA /* UNSIGNED_INT_24_8_WEBGL */:
            sizePerPixel = 4;
            break;
          case 0x8363 /* GL_UNSIGNED_SHORT_5_6_5 */:
          case 0x8033 /* GL_UNSIGNED_SHORT_4_4_4_4 */:
          case 0x8034 /* GL_UNSIGNED_SHORT_5_5_5_1 */:
            sizePerPixel = 2;
            break;
          case 0x1406 /* GL_FLOAT */:
            switch (format) {
              case 0x1907 /* GL_RGB */:
                sizePerPixel = 3*4;
                break;
              case 0x1908 /* GL_RGBA */:
                sizePerPixel = 4*4;
                break;
              default:
                throw 'Invalid format (' + format + ')';
            }
            internalFormat = Module.ctx.RGBA;
            break;
          default:
            throw 'Invalid type (' + type + ')';
        }
        var bytes = GL.computeImageSize(width, height, sizePerPixel, GL.unpackAlignment);
        if (type == 0x1401 /* GL_UNSIGNED_BYTE */) {
          pixels = HEAPU8.subarray((pixels),(pixels+bytes));
        } else if (type == 0x1406 /* GL_FLOAT */) {
          pixels = HEAPF32.subarray((pixels)>>2,(pixels+bytes)>>2);
        } else if (type == 0x1405 /* GL_UNSIGNED_INT */ || type == 0x84FA /* UNSIGNED_INT_24_8_WEBGL */) {
          pixels = HEAPU32.subarray((pixels)>>2,(pixels+bytes)>>2);
        } else {
          pixels = HEAPU16.subarray((pixels)>>1,(pixels+bytes)>>1);
        }
        return {
          pixels: pixels,
          internalFormat: internalFormat
        }
      },initExtensions:function () {
        if (GL.initExtensions.done) return;
        GL.initExtensions.done = true;
        if (!Module.useWebGL) return; // an app might link both gl and 2d backends
        GL.miniTempBuffer = new Float32Array(GL.MINI_TEMP_BUFFER_SIZE);
        for (var i = 0; i < GL.MINI_TEMP_BUFFER_SIZE; i++) {
          GL.miniTempBufferViews[i] = GL.miniTempBuffer.subarray(0, i+1);
        }
        GL.maxVertexAttribs = Module.ctx.getParameter(Module.ctx.MAX_VERTEX_ATTRIBS);
        // Detect the presence of a few extensions manually, this GL interop layer itself will need to know if they exist. 
        GL.compressionExt = Module.ctx.getExtension('WEBGL_compressed_texture_s3tc') ||
                            Module.ctx.getExtension('MOZ_WEBGL_compressed_texture_s3tc') ||
                            Module.ctx.getExtension('WEBKIT_WEBGL_compressed_texture_s3tc');
        GL.anisotropicExt = Module.ctx.getExtension('EXT_texture_filter_anisotropic') ||
                            Module.ctx.getExtension('MOZ_EXT_texture_filter_anisotropic') ||
                            Module.ctx.getExtension('WEBKIT_EXT_texture_filter_anisotropic');
        GL.floatExt = Module.ctx.getExtension('OES_texture_float');
        // These are the 'safe' feature-enabling extensions that don't add any performance impact related to e.g. debugging, and
        // should be enabled by default so that client GLES2/GL code will not need to go through extra hoops to get its stuff working.
        // As new extensions are ratified at http://www.khronos.org/registry/webgl/extensions/ , feel free to add your new extensions
        // here, as long as they don't produce a performance impact for users that might not be using those extensions.
        // E.g. debugging-related extensions should probably be off by default.
        var automaticallyEnabledExtensions = [ "OES_texture_float", "OES_texture_half_float", "OES_standard_derivatives",
                                               "OES_vertex_array_object", "WEBGL_compressed_texture_s3tc", "WEBGL_depth_texture",
                                               "OES_element_index_uint", "EXT_texture_filter_anisotropic", "ANGLE_instanced_arrays",
                                               "OES_texture_float_linear", "OES_texture_half_float_linear", "WEBGL_compressed_texture_atc",
                                               "WEBGL_compressed_texture_pvrtc", "EXT_color_buffer_half_float", "WEBGL_color_buffer_float",
                                               "EXT_frag_depth", "EXT_sRGB", "WEBGL_draw_buffers", "WEBGL_shared_resources" ];
        function shouldEnableAutomatically(extension) {
          for(var i in automaticallyEnabledExtensions) {
            var include = automaticallyEnabledExtensions[i];
            if (ext.indexOf(include) != -1) {
              return true;
            }
          }
          return false;
        }
        var extensions = Module.ctx.getSupportedExtensions();
        for(var e in extensions) {
          var ext = extensions[e].replace('MOZ_', '').replace('WEBKIT_', '');
          if (automaticallyEnabledExtensions.indexOf(ext) != -1) {
            Module.ctx.getExtension(ext); // Calling .getExtension enables that extension permanently, no need to store the return value to be enabled.
          }
        }
      },populateUniformTable:function (program) {
        var p = GL.programs[program];
        GL.programInfos[program] = {
          uniforms: {},
          maxUniformLength: 0, // This is eagerly computed below, since we already enumerate all uniforms anyway.
          maxAttributeLength: -1 // This is lazily computed and cached, computed when/if first asked, "-1" meaning not computed yet.
        };
        var ptable = GL.programInfos[program];
        var utable = ptable.uniforms;
        // A program's uniform table maps the string name of an uniform to an integer location of that uniform.
        // The global GL.uniforms map maps integer locations to WebGLUniformLocations.
        var numUniforms = Module.ctx.getProgramParameter(p, Module.ctx.ACTIVE_UNIFORMS);
        for (var i = 0; i < numUniforms; ++i) {
          var u = Module.ctx.getActiveUniform(p, i);
          var name = u.name;
          ptable.maxUniformLength = Math.max(ptable.maxUniformLength, name.length+1);
          // Strip off any trailing array specifier we might have got, e.g. "[0]".
          if (name.indexOf(']', name.length-1) !== -1) {
            var ls = name.lastIndexOf('[');
            name = name.slice(0, ls);
          }
          // Optimize memory usage slightly: If we have an array of uniforms, e.g. 'vec3 colors[3];', then 
          // only store the string 'colors' in utable, and 'colors[0]', 'colors[1]' and 'colors[2]' will be parsed as 'colors'+i.
          // Note that for the GL.uniforms table, we still need to fetch the all WebGLUniformLocations for all the indices.
          var loc = Module.ctx.getUniformLocation(p, name);
          var id = GL.getNewId(GL.uniforms);
          utable[name] = [u.size, id];
          GL.uniforms[id] = loc;
          for (var j = 1; j < u.size; ++j) {
            var n = name + '['+j+']';
            loc = Module.ctx.getUniformLocation(p, n);
            id = GL.getNewId(GL.uniforms);
            GL.uniforms[id] = loc;
          }
        }
      }};function _glGetString(name_) {
      if (GL.stringCache[name_]) return GL.stringCache[name_];
      var ret; 
      switch(name_) {
        case 0x1F00 /* GL_VENDOR */:
        case 0x1F01 /* GL_RENDERER */:
        case 0x1F02 /* GL_VERSION */:
          ret = allocate(intArrayFromString(Module.ctx.getParameter(name_)), 'i8', ALLOC_NORMAL);
          break;
        case 0x1F03 /* GL_EXTENSIONS */:
          var exts = Module.ctx.getSupportedExtensions();
          var gl_exts = [];
          for (i in exts) {
            gl_exts.push(exts[i]);
            gl_exts.push("GL_" + exts[i]);
          }
          ret = allocate(intArrayFromString(gl_exts.join(' ')), 'i8', ALLOC_NORMAL);
          break;
        case 0x8B8C /* GL_SHADING_LANGUAGE_VERSION */:
          ret = allocate(intArrayFromString('OpenGL ES GLSL 1.00 (WebGL)'), 'i8', ALLOC_NORMAL);
          break;
        default:
          GL.recordError(0x0500/*GL_INVALID_ENUM*/);
          return 0;
      }
      GL.stringCache[name_] = ret;
      return ret;
    }
  function _glGetIntegerv(name_, p) {
      switch(name_) { // Handle a few trivial GLES values
        case 0x8DFA: // GL_SHADER_COMPILER
          HEAP32[((p)>>2)]=1;
          return;
        case 0x8DF8: // GL_SHADER_BINARY_FORMATS
        case 0x8DF9: // GL_NUM_SHADER_BINARY_FORMATS
          HEAP32[((p)>>2)]=0;
          return;
        case 0x86A2: // GL_NUM_COMPRESSED_TEXTURE_FORMATS
          // WebGL doesn't have GL_NUM_COMPRESSED_TEXTURE_FORMATS (it's obsolete since GL_COMPRESSED_TEXTURE_FORMATS returns a JS array that can be queried for length),
          // so implement it ourselves to allow C++ GLES2 code get the length.
          var formats = Module.ctx.getParameter(0x86A3 /*GL_COMPRESSED_TEXTURE_FORMATS*/);
          HEAP32[((p)>>2)]=formats.length;
          return;
      }
      var result = Module.ctx.getParameter(name_);
      switch (typeof(result)) {
        case "number":
          HEAP32[((p)>>2)]=result;
          break;
        case "boolean":
          HEAP8[(p)]=result ? 1 : 0;
          break;
        case "string":
          GL.recordError(0x0500/*GL_INVALID_ENUM*/);
          return;
        case "object":
          if (result === null) {
            HEAP32[((p)>>2)]=0;
          } else if (result instanceof Float32Array ||
                     result instanceof Uint32Array ||
                     result instanceof Int32Array ||
                     result instanceof Array) {
            for (var i = 0; i < result.length; ++i) {
              HEAP32[(((p)+(i*4))>>2)]=result[i];
            }
          } else if (result instanceof WebGLBuffer) {
            HEAP32[((p)>>2)]=result.name | 0;
          } else if (result instanceof WebGLProgram) {
            HEAP32[((p)>>2)]=result.name | 0;
          } else if (result instanceof WebGLFramebuffer) {
            HEAP32[((p)>>2)]=result.name | 0;
          } else if (result instanceof WebGLRenderbuffer) {
            HEAP32[((p)>>2)]=result.name | 0;
          } else if (result instanceof WebGLTexture) {
            HEAP32[((p)>>2)]=result.name | 0;
          } else {
            GL.recordError(0x0500/*GL_INVALID_ENUM*/);
            return;
          }
          break;
        default:
          GL.recordError(0x0500/*GL_INVALID_ENUM*/);
          return;
      }
    }
  function _glViewport(x0, x1, x2, x3) { Module.ctx.viewport(x0, x1, x2, x3) }
  function _glDisable(x0) { Module.ctx.disable(x0) }
  function _glScissor(x0, x1, x2, x3) { Module.ctx.scissor(x0, x1, x2, x3) }
  function _glEnable(x0) { Module.ctx.enable(x0) }
  function _glClearColor(x0, x1, x2, x3) { Module.ctx.clearColor(x0, x1, x2, x3) }
  function _glDepthMask(x0) { Module.ctx.depthMask(x0) }
  function _glClear(x0) { Module.ctx.clear(x0) }
  function _glBindBuffer(target, buffer) {
      var bufferObj = buffer ? GL.buffers[buffer] : null;
      if (target == Module.ctx.ARRAY_BUFFER) {
        GL.currArrayBuffer = buffer;
      } else if (target == Module.ctx.ELEMENT_ARRAY_BUFFER) {
        GL.currElementArrayBuffer = buffer;
      }
      Module.ctx.bindBuffer(target, bufferObj);
    }
  function _glDrawElements(mode, count, type, indices) {
      Module.ctx.drawElements(mode, count, type, indices);
    }
  function _glDrawArrays(mode, first, count) {
      Module.ctx.drawArrays(mode, first, count);
    }
  function _glBindTexture(target, texture) {
      Module.ctx.bindTexture(target, texture ? GL.textures[texture] : null);
    }
  function _glCopyTexSubImage2D(x0, x1, x2, x3, x4, x5, x6, x7) { Module.ctx.copyTexSubImage2D(x0, x1, x2, x3, x4, x5, x6, x7) }
  function _glBlendFunc(x0, x1) { Module.ctx.blendFunc(x0, x1) }
  function _glColorMask(x0, x1, x2, x3) { Module.ctx.colorMask(x0, x1, x2, x3) }
  function _glDepthFunc(x0) { Module.ctx.depthFunc(x0) }
  function _glTexParameteri(x0, x1, x2) { Module.ctx.texParameteri(x0, x1, x2) }
  function _glCullFace(x0) { Module.ctx.cullFace(x0) }
  function _glPolygonOffset(x0, x1) { Module.ctx.polygonOffset(x0, x1) }
  function _glLineWidth(x0) { Module.ctx.lineWidth(x0) }
  function _glVertexAttribPointer(index, size, type, normalized, stride, ptr) {
      Module.ctx.vertexAttribPointer(index, size, type, normalized, stride, ptr);
    }
  function _glEnableVertexAttribArray(index) {
      Module.ctx.enableVertexAttribArray(index);
    }
  function _glDisableVertexAttribArray(index) {
      Module.ctx.disableVertexAttribArray(index);
    }
  function _glActiveTexture(x0) { Module.ctx.activeTexture(x0) }
  function _glCreateShader(shaderType) {
      var id = GL.getNewId(GL.shaders);
      GL.shaders[id] = Module.ctx.createShader(shaderType);
      return id;
    }
  function _glShaderSource(shader, count, string, length) {
      var source = GL.getSource(shader, count, string, length);
      Module.ctx.shaderSource(GL.shaders[shader], source);
    }
  function _glCompileShader(shader) {
      Module.ctx.compileShader(GL.shaders[shader]);
    }
  function _glGetShaderiv(shader, pname, p) {
      if (pname == 0x8B84) { // GL_INFO_LOG_LENGTH
        HEAP32[((p)>>2)]=Module.ctx.getShaderInfoLog(GL.shaders[shader]).length + 1;
      } else {
        HEAP32[((p)>>2)]=Module.ctx.getShaderParameter(GL.shaders[shader], pname);
      }
    }
  function _glGetShaderInfoLog(shader, maxLength, length, infoLog) {
      var log = Module.ctx.getShaderInfoLog(GL.shaders[shader]);
      // Work around a bug in Chromium which causes getShaderInfoLog to return null
      if (!log) {
        log = "";
      }
      log = log.substr(0, maxLength - 1);
      writeStringToMemory(log, infoLog);
      if (length) {
        HEAP32[((length)>>2)]=log.length
      }
    }
  function _glDeleteShader(shader) {
      Module.ctx.deleteShader(GL.shaders[shader]);
      GL.shaders[shader] = null;
    }
  function _glDetachShader(program, shader) {
      Module.ctx.detachShader(GL.programs[program],
                              GL.shaders[shader]);
    }
  function _glAttachShader(program, shader) {
      Module.ctx.attachShader(GL.programs[program],
                              GL.shaders[shader]);
    }
  function _glUseProgram(program) {
      Module.ctx.useProgram(program ? GL.programs[program] : null);
    }
  function _glCreateProgram() {
      var id = GL.getNewId(GL.programs);
      var program = Module.ctx.createProgram();
      program.name = id;
      GL.programs[id] = program;
      return id;
    }
  function _glDeleteProgram(program) {
      var program = GL.programs[program];
      Module.ctx.deleteProgram(program);
      program.name = 0;
      GL.programs[program] = null;
      GL.programInfos[program] = null;
    }
  function _glLinkProgram(program) {
      Module.ctx.linkProgram(GL.programs[program]);
      GL.programInfos[program] = null; // uniforms no longer keep the same names after linking
      GL.populateUniformTable(program);
    }
  function _glGetProgramiv(program, pname, p) {
      if (pname == 0x8B84) { // GL_INFO_LOG_LENGTH
        HEAP32[((p)>>2)]=Module.ctx.getProgramInfoLog(GL.programs[program]).length + 1;
      } else if (pname == 0x8B87 /* GL_ACTIVE_UNIFORM_MAX_LENGTH */) {
        var ptable = GL.programInfos[program];
        if (ptable) {
          HEAP32[((p)>>2)]=ptable.maxUniformLength;
          return;
        } else if (program < GL.counter) {
          GL.recordError(0x0502 /* GL_INVALID_OPERATION */);
        } else {
          GL.recordError(0x0501 /* GL_INVALID_VALUE */);
        }
      } else if (pname == 0x8B8A /* GL_ACTIVE_ATTRIBUTE_MAX_LENGTH */) {
        var ptable = GL.programInfos[program];
        if (ptable) {
          if (ptable.maxAttributeLength == -1) {
            var program = GL.programs[program];
            var numAttribs = Module.ctx.getProgramParameter(program, Module.ctx.ACTIVE_ATTRIBUTES);
            ptable.maxAttributeLength = 0; // Spec says if there are no active attribs, 0 must be returned.
            for(var i = 0; i < numAttribs; ++i) {
              var activeAttrib = Module.ctx.getActiveAttrib(program, i);
              ptable.maxAttributeLength = Math.max(ptable.maxAttributeLength, activeAttrib.name.length+1);
            }
          }
          HEAP32[((p)>>2)]=ptable.maxAttributeLength;
          return;
        } else if (program < GL.counter) {
          GL.recordError(0x0502 /* GL_INVALID_OPERATION */);
        } else {
          GL.recordError(0x0501 /* GL_INVALID_VALUE */);
        }
      } else {
        HEAP32[((p)>>2)]=Module.ctx.getProgramParameter(GL.programs[program], pname);
      }
    }
  function _glGetProgramInfoLog(program, maxLength, length, infoLog) {
      var log = Module.ctx.getProgramInfoLog(GL.programs[program]);
      // Work around a bug in Chromium which causes getProgramInfoLog to return null
      if (!log) {
        log = "";
      }
      log = log.substr(0, maxLength - 1);
      writeStringToMemory(log, infoLog);
      if (length) {
        HEAP32[((length)>>2)]=log.length
      }
    }
  function _glGetActiveUniform(program, index, bufSize, length, size, type, name) {
      program = GL.programs[program];
      var info = Module.ctx.getActiveUniform(program, index);
      var infoname = info.name.slice(0, Math.max(0, bufSize - 1));
      writeStringToMemory(infoname, name);
      if (length) {
        HEAP32[((length)>>2)]=infoname.length;
      }
      if (size) {
        HEAP32[((size)>>2)]=info.size;
      }
      if (type) {
        HEAP32[((type)>>2)]=info.type;
      }
    }
  function _glGetUniformLocation(program, name) {
      name = Pointer_stringify(name);
      var arrayOffset = 0;
      // If user passed an array accessor "[index]", parse the array index off the accessor.
      if (name.indexOf(']', name.length-1) !== -1) {
        var ls = name.lastIndexOf('[');
        var arrayIndex = name.slice(ls+1, -1);
        if (arrayIndex.length > 0) {
          arrayOffset = parseInt(arrayIndex);
          if (arrayOffset < 0) {
            return -1;
          }
        }
        name = name.slice(0, ls);
      }
      var ptable = GL.programInfos[program];
      if (!ptable) {
        return -1;
      }
      var utable = ptable.uniforms;
      var uniformInfo = utable[name]; // returns pair [ dimension_of_uniform_array, uniform_location ]
      if (uniformInfo && arrayOffset < uniformInfo[0]) { // Check if user asked for an out-of-bounds element, i.e. for 'vec4 colors[3];' user could ask for 'colors[10]' which should return -1.
        return uniformInfo[1]+arrayOffset;
      } else {
        return -1;
      }
    }
  function _glGetAttribLocation(program, name) {
      program = GL.programs[program];
      name = Pointer_stringify(name);
      return Module.ctx.getAttribLocation(program, name);
    }
  function _glGenTextures(n, textures) {
      for (var i = 0; i < n; i++) {
        var id = GL.getNewId(GL.textures);
        var texture = Module.ctx.createTexture();
        texture.name = id;
        GL.textures[id] = texture;
        HEAP32[(((textures)+(i*4))>>2)]=id;
      }
    }
  function _glPixelStorei(pname, param) {
      if (pname == 0x0D05 /* GL_PACK_ALIGNMENT */) {
        GL.packAlignment = param;
      } else if (pname == 0x0cf5 /* GL_UNPACK_ALIGNMENT */) {
        GL.unpackAlignment = param;
      }
      Module.ctx.pixelStorei(pname, param);
    }
  function _glTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels) {
      if (pixels) {
        var data = GL.getTexPixelData(type, format, width, height, pixels, internalFormat);
        pixels = data.pixels;
        internalFormat = data.internalFormat;
      } else {
        pixels = null;
      }
      Module.ctx.texImage2D(target, level, internalFormat, width, height, border, format, type, pixels);
    }
  function _glCompressedTexImage2D(target, level, internalFormat, width, height, border, imageSize, data) {
      if (data) {
        data = HEAPU8.subarray((data),(data+imageSize));
      } else {
        data = null;
      }
      Module.ctx['compressedTexImage2D'](target, level, internalFormat, width, height, border, data);
    }
  function _glDeleteTextures(n, textures) {
      for (var i = 0; i < n; i++) {
        var id = HEAP32[(((textures)+(i*4))>>2)];
        var texture = GL.textures[id];
        Module.ctx.deleteTexture(texture);
        texture.name = 0;
        GL.textures[id] = null;
      }
    }
  function _glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels) {
      if (pixels) {
        var data = GL.getTexPixelData(type, format, width, height, pixels, -1);
        pixels = data.pixels;
      } else {
        pixels = null;
      }
      Module.ctx.texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    }
  function _glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data) {
      if (data) {
        data = HEAPU8.subarray((data),(data+imageSize));
      } else {
        data = null;
      }
      Module.ctx['compressedTexSubImage2D'](target, level, xoffset, yoffset, width, height, data);
    }
  function _glGenerateMipmap(x0) { Module.ctx.generateMipmap(x0) }
  function _glFlush() { Module.ctx.flush() }
  var GLUT={initTime:null,idleFunc:null,displayFunc:null,keyboardFunc:null,keyboardUpFunc:null,specialFunc:null,specialUpFunc:null,reshapeFunc:null,motionFunc:null,passiveMotionFunc:null,mouseFunc:null,buttons:0,modifiers:0,initWindowWidth:256,initWindowHeight:256,initDisplayMode:18,windowX:0,windowY:0,windowWidth:0,windowHeight:0,saveModifiers:function (event) {
        GLUT.modifiers = 0;
        if (event['shiftKey'])
          GLUT.modifiers += 1; /* GLUT_ACTIVE_SHIFT */
        if (event['ctrlKey'])
          GLUT.modifiers += 2; /* GLUT_ACTIVE_CTRL */
        if (event['altKey'])
          GLUT.modifiers += 4; /* GLUT_ACTIVE_ALT */
      },onMousemove:function (event) {
        /* Send motion event only if the motion changed, prevents
         * spamming our app with uncessary callback call. It does happen in
         * Chrome on Windows.
         */
        var lastX = Browser.mouseX;
        var lastY = Browser.mouseY;
        Browser.calculateMouseEvent(event);
        var newX = Browser.mouseX;
        var newY = Browser.mouseY;
        if (newX == lastX && newY == lastY) return;
        if (GLUT.buttons == 0 && event.target == Module["canvas"] && GLUT.passiveMotionFunc) {
          event.preventDefault();
          GLUT.saveModifiers(event);
          Runtime.dynCall('vii', GLUT.passiveMotionFunc, [lastX, lastY]);
        } else if (GLUT.buttons != 0 && GLUT.motionFunc) {
          event.preventDefault();
          GLUT.saveModifiers(event);
          Runtime.dynCall('vii', GLUT.motionFunc, [lastX, lastY]);
        }
      },getSpecialKey:function (keycode) {
          var key = null;
          switch (keycode) {
            case 8:  key = 120 /* backspace */; break;
            case 46: key = 111 /* delete */; break;
            case 0x70 /*DOM_VK_F1*/: key = 1 /* GLUT_KEY_F1 */; break;
            case 0x71 /*DOM_VK_F2*/: key = 2 /* GLUT_KEY_F2 */; break;
            case 0x72 /*DOM_VK_F3*/: key = 3 /* GLUT_KEY_F3 */; break;
            case 0x73 /*DOM_VK_F4*/: key = 4 /* GLUT_KEY_F4 */; break;
            case 0x74 /*DOM_VK_F5*/: key = 5 /* GLUT_KEY_F5 */; break;
            case 0x75 /*DOM_VK_F6*/: key = 6 /* GLUT_KEY_F6 */; break;
            case 0x76 /*DOM_VK_F7*/: key = 7 /* GLUT_KEY_F7 */; break;
            case 0x77 /*DOM_VK_F8*/: key = 8 /* GLUT_KEY_F8 */; break;
            case 0x78 /*DOM_VK_F9*/: key = 9 /* GLUT_KEY_F9 */; break;
            case 0x79 /*DOM_VK_F10*/: key = 10 /* GLUT_KEY_F10 */; break;
            case 0x7a /*DOM_VK_F11*/: key = 11 /* GLUT_KEY_F11 */; break;
            case 0x7b /*DOM_VK_F12*/: key = 12 /* GLUT_KEY_F12 */; break;
            case 0x25 /*DOM_VK_LEFT*/: key = 100 /* GLUT_KEY_LEFT */; break;
            case 0x26 /*DOM_VK_UP*/: key = 101 /* GLUT_KEY_UP */; break;
            case 0x27 /*DOM_VK_RIGHT*/: key = 102 /* GLUT_KEY_RIGHT */; break;
            case 0x28 /*DOM_VK_DOWN*/: key = 103 /* GLUT_KEY_DOWN */; break;
            case 0x21 /*DOM_VK_PAGE_UP*/: key = 104 /* GLUT_KEY_PAGE_UP */; break;
            case 0x22 /*DOM_VK_PAGE_DOWN*/: key = 105 /* GLUT_KEY_PAGE_DOWN */; break;
            case 0x24 /*DOM_VK_HOME*/: key = 106 /* GLUT_KEY_HOME */; break;
            case 0x23 /*DOM_VK_END*/: key = 107 /* GLUT_KEY_END */; break;
            case 0x2d /*DOM_VK_INSERT*/: key = 108 /* GLUT_KEY_INSERT */; break;
            case 16   /*DOM_VK_SHIFT*/:
            case 0x05 /*DOM_VK_LEFT_SHIFT*/:
              key = 112 /* GLUT_KEY_SHIFT_L */;
              break;
            case 0x06 /*DOM_VK_RIGHT_SHIFT*/:
              key = 113 /* GLUT_KEY_SHIFT_R */;
              break;
            case 17   /*DOM_VK_CONTROL*/:
            case 0x03 /*DOM_VK_LEFT_CONTROL*/:
              key = 114 /* GLUT_KEY_CONTROL_L */;
              break;
            case 0x04 /*DOM_VK_RIGHT_CONTROL*/:
              key = 115 /* GLUT_KEY_CONTROL_R */;
              break;
            case 18   /*DOM_VK_ALT*/:
            case 0x02 /*DOM_VK_LEFT_ALT*/:
              key = 116 /* GLUT_KEY_ALT_L */;
              break;
            case 0x01 /*DOM_VK_RIGHT_ALT*/:
              key = 117 /* GLUT_KEY_ALT_R */;
              break;
          };
          return key;
      },getASCIIKey:function (event) {
        if (event['ctrlKey'] || event['altKey'] || event['metaKey']) return null;
        var keycode = event['keyCode'];
        /* The exact list is soooo hard to find in a canonical place! */
        if (48 <= keycode && keycode <= 57)
          return keycode; // numeric  TODO handle shift?
        if (65 <= keycode && keycode <= 90)
          return event['shiftKey'] ? keycode : keycode + 32;
        if (106 <= keycode && keycode <= 111)
          return keycode - 106 + 42; // *,+-./  TODO handle shift?
        switch (keycode) {
          case 27: // escape
          case 32: // space
          case 61: // equal
            return keycode;
        }
        var s = event['shiftKey'];
        switch (keycode) {
          case 186: return s ? 58 : 59; // colon / semi-colon
          case 187: return s ? 43 : 61; // add / equal (these two may be wrong)
          case 188: return s ? 60 : 44; // less-than / comma
          case 189: return s ? 95 : 45; // dash
          case 190: return s ? 62 : 46; // greater-than / period
          case 191: return s ? 63 : 47; // forward slash
          case 219: return s ? 123 : 91; // open bracket
          case 220: return s ? 124 : 47; // back slash
          case 221: return s ? 125 : 93; // close braket
          case 222: return s ? 34 : 39; // single quote
        }
        return null;
      },onKeydown:function (event) {
        if (GLUT.specialFunc || GLUT.keyboardFunc) {
          var key = GLUT.getSpecialKey(event['keyCode']);
          if (key !== null) {
            if( GLUT.specialFunc ) {
              event.preventDefault();
              GLUT.saveModifiers(event);
              Runtime.dynCall('viii', GLUT.specialFunc, [key, Browser.mouseX, Browser.mouseY]);
            }
          }
          else
          {
            key = GLUT.getASCIIKey(event);
            if( key !== null && GLUT.keyboardFunc ) {
              event.preventDefault();
              GLUT.saveModifiers(event);
              Runtime.dynCall('viii', GLUT.keyboardFunc, [key, Browser.mouseX, Browser.mouseY]);
            }
          }
        }
      },onKeyup:function (event) {
        if (GLUT.specialUpFunc || GLUT.keyboardUpFunc) {
          var key = GLUT.getSpecialKey(event['keyCode']);
          if (key !== null) {
            if(GLUT.specialUpFunc) {
              event.preventDefault ();
              GLUT.saveModifiers(event);
              Runtime.dynCall('viii', GLUT.specialUpFunc, [key, Browser.mouseX, Browser.mouseY]);
            }
          }
          else
          {
            key = GLUT.getASCIIKey(event);
            if( key !== null && GLUT.keyboardUpFunc ) {
              event.preventDefault ();
              GLUT.saveModifiers(event);
              Runtime.dynCall('viii', GLUT.keyboardUpFunc, [key, Browser.mouseX, Browser.mouseY]);
            }
          }
        }
      },onMouseButtonDown:function (event) {
        Browser.calculateMouseEvent(event);
        GLUT.buttons |= (1 << event['button']);
        if (event.target == Module["canvas"] && GLUT.mouseFunc) {
          try {
            event.target.setCapture();
          } catch (e) {}
          event.preventDefault();
          GLUT.saveModifiers(event);
          Runtime.dynCall('viiii', GLUT.mouseFunc, [event['button'], 0/*GLUT_DOWN*/, Browser.mouseX, Browser.mouseY]);
        }
      },onMouseButtonUp:function (event) {
        Browser.calculateMouseEvent(event);
        GLUT.buttons &= ~(1 << event['button']);
        if (GLUT.mouseFunc) {
          event.preventDefault();
          GLUT.saveModifiers(event);
          Runtime.dynCall('viiii', GLUT.mouseFunc, [event['button'], 1/*GLUT_UP*/, Browser.mouseX, Browser.mouseY]);
        }
      },onMouseWheel:function (event) {
        Browser.calculateMouseEvent(event);
        // cross-browser wheel delta
        var e = window.event || event; // old IE support
        var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
        var button = 3; // wheel up
        if (delta < 0) {
          button = 4; // wheel down
        }
        if (GLUT.mouseFunc) {
          event.preventDefault();
          GLUT.saveModifiers(event);
          Runtime.dynCall('viiii', GLUT.mouseFunc, [button, 0/*GLUT_DOWN*/, Browser.mouseX, Browser.mouseY]);
        }
      },onFullScreenEventChange:function (event) {
        var width;
        var height;
        if (document["fullScreen"] || document["mozFullScreen"] || document["webkitIsFullScreen"]) {
          width = screen["width"];
          height = screen["height"];
        } else {
          width = GLUT.windowWidth;
          height = GLUT.windowHeight;
          // TODO set position
          document.removeEventListener('fullscreenchange', GLUT.onFullScreenEventChange, true);
          document.removeEventListener('mozfullscreenchange', GLUT.onFullScreenEventChange, true);
          document.removeEventListener('webkitfullscreenchange', GLUT.onFullScreenEventChange, true);
        }
        Browser.setCanvasSize(width, height);
        /* Can't call _glutReshapeWindow as that requests cancelling fullscreen. */
        if (GLUT.reshapeFunc) {
          // console.log("GLUT.reshapeFunc (from FS): " + width + ", " + height);
          Runtime.dynCall('vii', GLUT.reshapeFunc, [width, height]);
        }
        _glutPostRedisplay();
      },requestFullScreen:function () {
        var RFS = Module["canvas"]['requestFullscreen'] ||
                  Module["canvas"]['requestFullScreen'] ||
                  Module["canvas"]['mozRequestFullScreen'] ||
                  Module["canvas"]['webkitRequestFullScreen'] ||
                  (function() {});
        RFS.apply(Module["canvas"], []);
      },cancelFullScreen:function () {
        var CFS = document['exitFullscreen'] ||
                  document['cancelFullScreen'] ||
                  document['mozCancelFullScreen'] ||
                  document['webkitCancelFullScreen'] ||
                  (function() {});
        CFS.apply(document, []);
      }};function _glutInitDisplayMode(mode) {
      GLUT.initDisplayMode = mode;
    }
  var Browser={mainLoop:{scheduler:null,shouldPause:false,paused:false,queue:[],pause:function () {
          Browser.mainLoop.shouldPause = true;
        },resume:function () {
          if (Browser.mainLoop.paused) {
            Browser.mainLoop.paused = false;
            Browser.mainLoop.scheduler();
          }
          Browser.mainLoop.shouldPause = false;
        },updateStatus:function () {
          if (Module['setStatus']) {
            var message = Module['statusMessage'] || 'Please wait...';
            var remaining = Browser.mainLoop.remainingBlockers;
            var expected = Browser.mainLoop.expectedBlockers;
            if (remaining) {
              if (remaining < expected) {
                Module['setStatus'](message + ' (' + (expected - remaining) + '/' + expected + ')');
              } else {
                Module['setStatus'](message);
              }
            } else {
              Module['setStatus']('');
            }
          }
        }},isFullScreen:false,pointerLock:false,moduleContextCreatedCallbacks:[],workers:[],init:function () {
        if (!Module["preloadPlugins"]) Module["preloadPlugins"] = []; // needs to exist even in workers
        if (Browser.initted || ENVIRONMENT_IS_WORKER) return;
        Browser.initted = true;
        try {
          new Blob();
          Browser.hasBlobConstructor = true;
        } catch(e) {
          Browser.hasBlobConstructor = false;
          console.log("warning: no blob constructor, cannot create blobs with mimetypes");
        }
        Browser.BlobBuilder = typeof MozBlobBuilder != "undefined" ? MozBlobBuilder : (typeof WebKitBlobBuilder != "undefined" ? WebKitBlobBuilder : (!Browser.hasBlobConstructor ? console.log("warning: no BlobBuilder") : null));
        Browser.URLObject = typeof window != "undefined" ? (window.URL ? window.URL : window.webkitURL) : undefined;
        if (!Module.noImageDecoding && typeof Browser.URLObject === 'undefined') {
          console.log("warning: Browser does not support creating object URLs. Built-in browser image decoding will not be available.");
          Module.noImageDecoding = true;
        }
        // Support for plugins that can process preloaded files. You can add more of these to
        // your app by creating and appending to Module.preloadPlugins.
        //
        // Each plugin is asked if it can handle a file based on the file's name. If it can,
        // it is given the file's raw data. When it is done, it calls a callback with the file's
        // (possibly modified) data. For example, a plugin might decompress a file, or it
        // might create some side data structure for use later (like an Image element, etc.).
        var imagePlugin = {};
        imagePlugin['canHandle'] = function imagePlugin_canHandle(name) {
          return !Module.noImageDecoding && /\.(jpg|jpeg|png|bmp)$/i.test(name);
        };
        imagePlugin['handle'] = function imagePlugin_handle(byteArray, name, onload, onerror) {
          var b = null;
          if (Browser.hasBlobConstructor) {
            try {
              b = new Blob([byteArray], { type: Browser.getMimetype(name) });
              if (b.size !== byteArray.length) { // Safari bug #118630
                // Safari's Blob can only take an ArrayBuffer
                b = new Blob([(new Uint8Array(byteArray)).buffer], { type: Browser.getMimetype(name) });
              }
            } catch(e) {
              Runtime.warnOnce('Blob constructor present but fails: ' + e + '; falling back to blob builder');
            }
          }
          if (!b) {
            var bb = new Browser.BlobBuilder();
            bb.append((new Uint8Array(byteArray)).buffer); // we need to pass a buffer, and must copy the array to get the right data range
            b = bb.getBlob();
          }
          var url = Browser.URLObject.createObjectURL(b);
          var img = new Image();
          img.onload = function img_onload() {
            assert(img.complete, 'Image ' + name + ' could not be decoded');
            var canvas = document.createElement('canvas');
            canvas.width = img.width;
            canvas.height = img.height;
            var ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0);
            Module["preloadedImages"][name] = canvas;
            Browser.URLObject.revokeObjectURL(url);
            if (onload) onload(byteArray);
          };
          img.onerror = function img_onerror(event) {
            console.log('Image ' + url + ' could not be decoded');
            if (onerror) onerror();
          };
          img.src = url;
        };
        Module['preloadPlugins'].push(imagePlugin);
        var audioPlugin = {};
        audioPlugin['canHandle'] = function audioPlugin_canHandle(name) {
          return !Module.noAudioDecoding && name.substr(-4) in { '.ogg': 1, '.wav': 1, '.mp3': 1 };
        };
        audioPlugin['handle'] = function audioPlugin_handle(byteArray, name, onload, onerror) {
          var done = false;
          function finish(audio) {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = audio;
            if (onload) onload(byteArray);
          }
          function fail() {
            if (done) return;
            done = true;
            Module["preloadedAudios"][name] = new Audio(); // empty shim
            if (onerror) onerror();
          }
          if (Browser.hasBlobConstructor) {
            try {
              var b = new Blob([byteArray], { type: Browser.getMimetype(name) });
            } catch(e) {
              return fail();
            }
            var url = Browser.URLObject.createObjectURL(b); // XXX we never revoke this!
            var audio = new Audio();
            audio.addEventListener('canplaythrough', function() { finish(audio) }, false); // use addEventListener due to chromium bug 124926
            audio.onerror = function audio_onerror(event) {
              if (done) return;
              console.log('warning: browser could not fully decode audio ' + name + ', trying slower base64 approach');
              function encode64(data) {
                var BASE = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
                var PAD = '=';
                var ret = '';
                var leftchar = 0;
                var leftbits = 0;
                for (var i = 0; i < data.length; i++) {
                  leftchar = (leftchar << 8) | data[i];
                  leftbits += 8;
                  while (leftbits >= 6) {
                    var curr = (leftchar >> (leftbits-6)) & 0x3f;
                    leftbits -= 6;
                    ret += BASE[curr];
                  }
                }
                if (leftbits == 2) {
                  ret += BASE[(leftchar&3) << 4];
                  ret += PAD + PAD;
                } else if (leftbits == 4) {
                  ret += BASE[(leftchar&0xf) << 2];
                  ret += PAD;
                }
                return ret;
              }
              audio.src = 'data:audio/x-' + name.substr(-3) + ';base64,' + encode64(byteArray);
              finish(audio); // we don't wait for confirmation this worked - but it's worth trying
            };
            audio.src = url;
            // workaround for chrome bug 124926 - we do not always get oncanplaythrough or onerror
            Browser.safeSetTimeout(function() {
              finish(audio); // try to use it even though it is not necessarily ready to play
            }, 10000);
          } else {
            return fail();
          }
        };
        Module['preloadPlugins'].push(audioPlugin);
        // Canvas event setup
        var canvas = Module['canvas'];
        canvas.requestPointerLock = canvas['requestPointerLock'] ||
                                    canvas['mozRequestPointerLock'] ||
                                    canvas['webkitRequestPointerLock'];
        canvas.exitPointerLock = document['exitPointerLock'] ||
                                 document['mozExitPointerLock'] ||
                                 document['webkitExitPointerLock'] ||
                                 function(){}; // no-op if function does not exist
        canvas.exitPointerLock = canvas.exitPointerLock.bind(document);
        function pointerLockChange() {
          Browser.pointerLock = document['pointerLockElement'] === canvas ||
                                document['mozPointerLockElement'] === canvas ||
                                document['webkitPointerLockElement'] === canvas;
        }
        document.addEventListener('pointerlockchange', pointerLockChange, false);
        document.addEventListener('mozpointerlockchange', pointerLockChange, false);
        document.addEventListener('webkitpointerlockchange', pointerLockChange, false);
        if (Module['elementPointerLock']) {
          canvas.addEventListener("click", function(ev) {
            if (!Browser.pointerLock && canvas.requestPointerLock) {
              canvas.requestPointerLock();
              ev.preventDefault();
            }
          }, false);
        }
      },createContext:function (canvas, useWebGL, setInModule, webGLContextAttributes) {
        var ctx;
        try {
          if (useWebGL) {
            var contextAttributes = {
              antialias: false,
              alpha: false
            };
            if (webGLContextAttributes) {
              for (var attribute in webGLContextAttributes) {
                contextAttributes[attribute] = webGLContextAttributes[attribute];
              }
            }
            ['experimental-webgl', 'webgl'].some(function(webglId) {
              return ctx = canvas.getContext(webglId, contextAttributes);
            });
          } else {
            ctx = canvas.getContext('2d');
          }
          if (!ctx) throw ':(';
        } catch (e) {
          Module.print('Could not create canvas - ' + e);
          return null;
        }
        if (useWebGL) {
          // Set the background of the WebGL canvas to black
          canvas.style.backgroundColor = "black";
          // Warn on context loss
          canvas.addEventListener('webglcontextlost', function(event) {
            alert('WebGL context lost. You will need to reload the page.');
          }, false);
        }
        if (setInModule) {
          Module.ctx = ctx;
          Module.useWebGL = useWebGL;
          Browser.moduleContextCreatedCallbacks.forEach(function(callback) { callback() });
          Browser.init();
        }
        return ctx;
      },destroyContext:function (canvas, useWebGL, setInModule) {},fullScreenHandlersInstalled:false,lockPointer:undefined,resizeCanvas:undefined,requestFullScreen:function (lockPointer, resizeCanvas) {
        Browser.lockPointer = lockPointer;
        Browser.resizeCanvas = resizeCanvas;
        if (typeof Browser.lockPointer === 'undefined') Browser.lockPointer = true;
        if (typeof Browser.resizeCanvas === 'undefined') Browser.resizeCanvas = false;
        var canvas = Module['canvas'];
        function fullScreenChange() {
          Browser.isFullScreen = false;
          if ((document['webkitFullScreenElement'] || document['webkitFullscreenElement'] ||
               document['mozFullScreenElement'] || document['mozFullscreenElement'] ||
               document['fullScreenElement'] || document['fullscreenElement']) === canvas) {
            canvas.cancelFullScreen = document['cancelFullScreen'] ||
                                      document['mozCancelFullScreen'] ||
                                      document['webkitCancelFullScreen'];
            canvas.cancelFullScreen = canvas.cancelFullScreen.bind(document);
            if (Browser.lockPointer) canvas.requestPointerLock();
            Browser.isFullScreen = true;
            if (Browser.resizeCanvas) Browser.setFullScreenCanvasSize();
          } else if (Browser.resizeCanvas){
            Browser.setWindowedCanvasSize();
          }
          if (Module['onFullScreen']) Module['onFullScreen'](Browser.isFullScreen);
        }
        if (!Browser.fullScreenHandlersInstalled) {
          Browser.fullScreenHandlersInstalled = true;
          document.addEventListener('fullscreenchange', fullScreenChange, false);
          document.addEventListener('mozfullscreenchange', fullScreenChange, false);
          document.addEventListener('webkitfullscreenchange', fullScreenChange, false);
        }
        canvas.requestFullScreen = canvas['requestFullScreen'] ||
                                   canvas['mozRequestFullScreen'] ||
                                   (canvas['webkitRequestFullScreen'] ? function() { canvas['webkitRequestFullScreen'](Element['ALLOW_KEYBOARD_INPUT']) } : null);
        canvas.requestFullScreen();
      },requestAnimationFrame:function requestAnimationFrame(func) {
        if (typeof window === 'undefined') { // Provide fallback to setTimeout if window is undefined (e.g. in Node.js)
          setTimeout(func, 1000/60);
        } else {
          if (!window.requestAnimationFrame) {
            window.requestAnimationFrame = window['requestAnimationFrame'] ||
                                           window['mozRequestAnimationFrame'] ||
                                           window['webkitRequestAnimationFrame'] ||
                                           window['msRequestAnimationFrame'] ||
                                           window['oRequestAnimationFrame'] ||
                                           window['setTimeout'];
          }
          window.requestAnimationFrame(func);
        }
      },safeCallback:function (func) {
        return function() {
          if (!ABORT) return func.apply(null, arguments);
        };
      },safeRequestAnimationFrame:function (func) {
        return Browser.requestAnimationFrame(function() {
          if (!ABORT) func();
        });
      },safeSetTimeout:function (func, timeout) {
        return setTimeout(function() {
          if (!ABORT) func();
        }, timeout);
      },safeSetInterval:function (func, timeout) {
        return setInterval(function() {
          if (!ABORT) func();
        }, timeout);
      },getMimetype:function (name) {
        return {
          'jpg': 'image/jpeg',
          'jpeg': 'image/jpeg',
          'png': 'image/png',
          'bmp': 'image/bmp',
          'ogg': 'audio/ogg',
          'wav': 'audio/wav',
          'mp3': 'audio/mpeg'
        }[name.substr(name.lastIndexOf('.')+1)];
      },getUserMedia:function (func) {
        if(!window.getUserMedia) {
          window.getUserMedia = navigator['getUserMedia'] ||
                                navigator['mozGetUserMedia'];
        }
        window.getUserMedia(func);
      },getMovementX:function (event) {
        return event['movementX'] ||
               event['mozMovementX'] ||
               event['webkitMovementX'] ||
               0;
      },getMovementY:function (event) {
        return event['movementY'] ||
               event['mozMovementY'] ||
               event['webkitMovementY'] ||
               0;
      },mouseX:0,mouseY:0,mouseMovementX:0,mouseMovementY:0,calculateMouseEvent:function (event) { // event should be mousemove, mousedown or mouseup
        if (Browser.pointerLock) {
          // When the pointer is locked, calculate the coordinates
          // based on the movement of the mouse.
          // Workaround for Firefox bug 764498
          if (event.type != 'mousemove' &&
              ('mozMovementX' in event)) {
            Browser.mouseMovementX = Browser.mouseMovementY = 0;
          } else {
            Browser.mouseMovementX = Browser.getMovementX(event);
            Browser.mouseMovementY = Browser.getMovementY(event);
          }
          // check if SDL is available
          if (typeof SDL != "undefined") {
          	Browser.mouseX = SDL.mouseX + Browser.mouseMovementX;
          	Browser.mouseY = SDL.mouseY + Browser.mouseMovementY;
          } else {
          	// just add the mouse delta to the current absolut mouse position
          	// FIXME: ideally this should be clamped against the canvas size and zero
          	Browser.mouseX += Browser.mouseMovementX;
          	Browser.mouseY += Browser.mouseMovementY;
          }        
        } else {
          // Otherwise, calculate the movement based on the changes
          // in the coordinates.
          var rect = Module["canvas"].getBoundingClientRect();
          var x, y;
          if (event.type == 'touchstart' ||
              event.type == 'touchend' ||
              event.type == 'touchmove') {
            var t = event.touches.item(0);
            if (t) {
              x = t.pageX - (window.scrollX + rect.left);
              y = t.pageY - (window.scrollY + rect.top);
            } else {
              return;
            }
          } else {
            x = event.pageX - (window.scrollX + rect.left);
            y = event.pageY - (window.scrollY + rect.top);
          }
          // the canvas might be CSS-scaled compared to its backbuffer;
          // SDL-using content will want mouse coordinates in terms
          // of backbuffer units.
          var cw = Module["canvas"].width;
          var ch = Module["canvas"].height;
          x = x * (cw / rect.width);
          y = y * (ch / rect.height);
          Browser.mouseMovementX = x - Browser.mouseX;
          Browser.mouseMovementY = y - Browser.mouseY;
          Browser.mouseX = x;
          Browser.mouseY = y;
        }
      },xhrLoad:function (url, onload, onerror) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'arraybuffer';
        xhr.onload = function xhr_onload() {
          if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            onload(xhr.response);
          } else {
            onerror();
          }
        };
        xhr.onerror = onerror;
        xhr.send(null);
      },asyncLoad:function (url, onload, onerror, noRunDep) {
        Browser.xhrLoad(url, function(arrayBuffer) {
          assert(arrayBuffer, 'Loading data file "' + url + '" failed (no arrayBuffer).');
          onload(new Uint8Array(arrayBuffer));
          if (!noRunDep) removeRunDependency('al ' + url);
        }, function(event) {
          if (onerror) {
            onerror();
          } else {
            throw 'Loading data file "' + url + '" failed.';
          }
        });
        if (!noRunDep) addRunDependency('al ' + url);
      },resizeListeners:[],updateResizeListeners:function () {
        var canvas = Module['canvas'];
        Browser.resizeListeners.forEach(function(listener) {
          listener(canvas.width, canvas.height);
        });
      },setCanvasSize:function (width, height, noUpdates) {
        var canvas = Module['canvas'];
        canvas.width = width;
        canvas.height = height;
        if (!noUpdates) Browser.updateResizeListeners();
      },windowedWidth:0,windowedHeight:0,setFullScreenCanvasSize:function () {
        var canvas = Module['canvas'];
        this.windowedWidth = canvas.width;
        this.windowedHeight = canvas.height;
        canvas.width = screen.width;
        canvas.height = screen.height;
        // check if SDL is available   
        if (typeof SDL != "undefined") {
        	var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        	flags = flags | 0x00800000; // set SDL_FULLSCREEN flag
        	HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        }
        Browser.updateResizeListeners();
      },setWindowedCanvasSize:function () {
        var canvas = Module['canvas'];
        canvas.width = this.windowedWidth;
        canvas.height = this.windowedHeight;
        // check if SDL is available       
        if (typeof SDL != "undefined") {
        	var flags = HEAPU32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)];
        	flags = flags & ~0x00800000; // clear SDL_FULLSCREEN flag
        	HEAP32[((SDL.screen+Runtime.QUANTUM_SIZE*0)>>2)]=flags
        }
        Browser.updateResizeListeners();
      }};function _glutCreateWindow(name) {
      var contextAttributes = {
        antialias: ((GLUT.initDisplayMode & 0x0080 /*GLUT_MULTISAMPLE*/) != 0),
        depth: ((GLUT.initDisplayMode & 0x0010 /*GLUT_DEPTH*/) != 0),
        stencil: ((GLUT.initDisplayMode & 0x0020 /*GLUT_STENCIL*/) != 0)
      };
      Module.ctx = Browser.createContext(Module['canvas'], true, true, contextAttributes);
      return Module.ctx ? 1 /* a new GLUT window ID for the created context */ : 0 /* failure */;
    }function _eglCreateContext(display, config, hmm, contextAttribs) {
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      _glutInitDisplayMode(0x92 /* GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE */);
      EGL.windowID = _glutCreateWindow();
      if (EGL.windowID != 0) {
        EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
        // Note: This function only creates a context, but it shall not make it active.
        return 62004; // Magic ID for Emscripten EGLContext
      } else {
        EGL.setErrorCode(0x3009 /* EGL_BAD_MATCH */); // By the EGL 1.4 spec, an implementation that does not support GLES2 (WebGL in this case), this error code is set.
        return 0; /* EGL_NO_CONTEXT */
      }
    }
  var EGL={errorCode:12288,defaultDisplayInitialized:false,currentContext:0,currentReadSurface:0,currentDrawSurface:0,stringCache:{},setErrorCode:function (code) {
        EGL.errorCode = code;
      },chooseConfig:function (display, attribList, config, config_size, numConfigs) { 
        if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
          EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
          return 0;
        }
        // TODO: read attribList.
        if ((!config || !config_size) && !numConfigs) {
          EGL.setErrorCode(0x300C /* EGL_BAD_PARAMETER */);
          return 0;
        }
        if (numConfigs) {
          HEAP32[((numConfigs)>>2)]=1; // Total number of supported configs: 1.
        }
        if (config && config_size > 0) {
          HEAP32[((config)>>2)]=62002; 
        }
        EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
        return 1;
      }};function _eglQuerySurface(display, surface, attribute, value) { 
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      if (surface != 62006 /* Magic ID for Emscripten 'default surface' */) {
        EGL.setErrorCode(0x300D /* EGL_BAD_SURFACE */);
        return 0;
      }
      if (!value) {
        EGL.setErrorCode(0x300C /* EGL_BAD_PARAMETER */);
        return 0;
      }
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      switch(attribute) {
      case 0x3028: // EGL_CONFIG_ID
        HEAP32[((value)>>2)]=62002;
          return 1;
      case 0x3058: // EGL_LARGEST_PBUFFER
        // Odd EGL API: If surface is not a pbuffer surface, 'value' should not be written to. It's not specified as an error, so true should(?) be returned.
        // Existing Android implementation seems to do so at least.
        return 1;
      case 0x3057: // EGL_WIDTH
        HEAP32[((value)>>2)]=Module.canvas.width;
        return 1;
      case 0x3056: // EGL_HEIGHT
        HEAP32[((value)>>2)]=Module.canvas.height;
        return 1;
      case 0x3090: // EGL_HORIZONTAL_RESOLUTION
        HEAP32[((value)>>2)]=-1;
        return 1;
      case 0x3091: // EGL_VERTICAL_RESOLUTION
        HEAP32[((value)>>2)]=-1;
        return 1;
      case 0x3092: // EGL_PIXEL_ASPECT_RATIO
        HEAP32[((value)>>2)]=-1;
        return 1;
      case 0x3086: // EGL_RENDER_BUFFER
        // The main surface is bound to the visible canvas window - it's always backbuffered. 
        // Alternative to EGL_BACK_BUFFER would be EGL_SINGLE_BUFFER.
        HEAP32[((value)>>2)]=0x3084; 
        return 1;
      case 0x3099: // EGL_MULTISAMPLE_RESOLVE
        HEAP32[((value)>>2)]=0x309A; 
        return 1;
      case 0x3093: // EGL_SWAP_BEHAVIOR
        // The two possibilities are EGL_BUFFER_PRESERVED and EGL_BUFFER_DESTROYED. Slightly unsure which is the
        // case for browser environment, but advertise the 'weaker' behavior to be sure.
        HEAP32[((value)>>2)]=0x3095;
        return 1;
      case 0x3080: // EGL_TEXTURE_FORMAT
      case 0x3081: // EGL_TEXTURE_TARGET
      case 0x3082: // EGL_MIPMAP_TEXTURE
      case 0x3083: // EGL_MIPMAP_LEVEL
        // This is a window surface, not a pbuffer surface. Spec:
        // "Querying EGL_TEXTURE_FORMAT, EGL_TEXTURE_TARGET, EGL_MIPMAP_TEXTURE, or EGL_MIPMAP_LEVEL for a non-pbuffer surface is not an error, but value is not modified."
        // So pass-through.
        return 1;
      default:
        EGL.setErrorCode(0x3004 /* EGL_BAD_ATTRIBUTE */);
        return 0;
      }
    }
  function _eglGetCurrentContext() {
      return EGL.currentContext;
    }
  function _glutDestroyWindow(name) {
      Module.ctx = Browser.destroyContext(Module['canvas'], true, true);
      return 1;
    }function _eglDestroyContext(display, context) {
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      if (context != 62004 /* Magic ID for Emscripten EGLContext */) {
        EGL.setErrorCode(0x3006 /* EGL_BAD_CONTEXT */);
        return 0;
      }
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      return 1;
    }
  function _eglDestroySurface(display, surface) { 
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0; 
      }
      if (surface != 62006 /* Magic ID for the only EGLSurface supported by Emscripten */) {
        EGL.setErrorCode(0x300D /* EGL_BAD_SURFACE */);
        return 1;
      }
      if (EGL.currentReadSurface == surface) {
        EGL.currentReadSurface = 0;
      }
      if (EGL.currentDrawSurface == surface) {
        EGL.currentDrawSurface = 0;
      }
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      return 1; /* Magic ID for Emscripten 'default surface' */
    }
  function _eglMakeCurrent(display, draw, read, context) { 
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0 /* EGL_FALSE */;
      }
      //\todo An EGL_NOT_INITIALIZED error is generated if EGL is not initialized for dpy. 
      if (context != 0 && context != 62004 /* Magic ID for Emscripten EGLContext */) {
        EGL.setErrorCode(0x3006 /* EGL_BAD_CONTEXT */);
        return 0;
      }
      if ((read != 0 && read != 62006) || (draw != 0 && draw != 62006 /* Magic ID for Emscripten 'default surface' */)) {
        EGL.setErrorCode(0x300D /* EGL_BAD_SURFACE */);
        return 0;
      }
      EGL.currentContext = context;
      EGL.currentDrawSurface = draw;
      EGL.currentReadSurface = read;
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      return 1 /* EGL_TRUE */;
    }
  function _eglGetConfigs(display, configs, config_size, numConfigs) { 
      return EGL.chooseConfig(display, 0, configs, config_size, numConfigs);
    }
  function _eglGetError() { 
      return EGL.errorCode;
    }
  function _eglChooseConfig(display, attrib_list, configs, config_size, numConfigs) { 
      return EGL.chooseConfig(display, attrib_list, configs, config_size, numConfigs);
    }
  function _eglGetConfigAttrib(display, config, attribute, value) {
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      if (config != 62002 /* Magic ID for the only EGLConfig supported by Emscripten */) {
        EGL.setErrorCode(0x3005 /* EGL_BAD_CONFIG */);
        return 0;
      }
      if (!value) {
        EGL.setErrorCode(0x300C /* EGL_BAD_PARAMETER */);
        return 0;
      }
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      switch(attribute) {
      case 0x3020: // EGL_BUFFER_SIZE
        HEAP32[((value)>>2)]=32;
        return 1;
      case 0x3021: // EGL_ALPHA_SIZE
        HEAP32[((value)>>2)]=8;
        return 1;
      case 0x3022: // EGL_BLUE_SIZE
        HEAP32[((value)>>2)]=8;
        return 1;
      case 0x3023: // EGL_GREEN_SIZE
        HEAP32[((value)>>2)]=8;
        return 1;
      case 0x3024: // EGL_RED_SIZE
        HEAP32[((value)>>2)]=8;
        return 1;
      case 0x3025: // EGL_DEPTH_SIZE
        HEAP32[((value)>>2)]=24;
        return 1;
      case 0x3026: // EGL_STENCIL_SIZE
        HEAP32[((value)>>2)]=8;
        return 1;
      case 0x3027: // EGL_CONFIG_CAVEAT
        // We can return here one of EGL_NONE (0x3038), EGL_SLOW_CONFIG (0x3050) or EGL_NON_CONFORMANT_CONFIG (0x3051).
        HEAP32[((value)>>2)]=0x3038;
        return 1;
      case 0x3028: // EGL_CONFIG_ID
        HEAP32[((value)>>2)]=62002;
        return 1;
      case 0x3029: // EGL_LEVEL
        HEAP32[((value)>>2)]=0;
        return 1;
      case 0x302A: // EGL_MAX_PBUFFER_HEIGHT
        HEAP32[((value)>>2)]=4096;
        return 1;
      case 0x302B: // EGL_MAX_PBUFFER_PIXELS
        HEAP32[((value)>>2)]=16777216;
        return 1;
      case 0x302C: // EGL_MAX_PBUFFER_WIDTH
        HEAP32[((value)>>2)]=4096;
        return 1;
      case 0x302D: // EGL_NATIVE_RENDERABLE
        HEAP32[((value)>>2)]=0;
        return 1;
      case 0x302E: // EGL_NATIVE_VISUAL_ID
        HEAP32[((value)>>2)]=0;
        return 1;
      case 0x302F: // EGL_NATIVE_VISUAL_TYPE
        HEAP32[((value)>>2)]=0x3038;
        return 1;
      case 0x3031: // EGL_SAMPLES
        HEAP32[((value)>>2)]=4;
        return 1;
      case 0x3032: // EGL_SAMPLE_BUFFERS
        HEAP32[((value)>>2)]=1;
        return 1;
      case 0x3033: // EGL_SURFACE_TYPE
        HEAP32[((value)>>2)]=0x0004;
        return 1;
      case 0x3034: // EGL_TRANSPARENT_TYPE
        // If this returns EGL_TRANSPARENT_RGB (0x3052), transparency is used through color-keying. No such thing applies to Emscripten canvas.
        HEAP32[((value)>>2)]=0x3038;
        return 1;
      case 0x3035: // EGL_TRANSPARENT_BLUE_VALUE
      case 0x3036: // EGL_TRANSPARENT_GREEN_VALUE
      case 0x3037: // EGL_TRANSPARENT_RED_VALUE
        // "If EGL_TRANSPARENT_TYPE is EGL_NONE, then the values for EGL_TRANSPARENT_RED_VALUE, EGL_TRANSPARENT_GREEN_VALUE, and EGL_TRANSPARENT_BLUE_VALUE are undefined."
        HEAP32[((value)>>2)]=-1;
        return 1;
      case 0x3039: // EGL_BIND_TO_TEXTURE_RGB
      case 0x303A: // EGL_BIND_TO_TEXTURE_RGBA
        HEAP32[((value)>>2)]=0;
        return 1;
      case 0x303B: // EGL_MIN_SWAP_INTERVAL
      case 0x303C: // EGL_MAX_SWAP_INTERVAL
        HEAP32[((value)>>2)]=1;
        return 1;
      case 0x303D: // EGL_LUMINANCE_SIZE
      case 0x303E: // EGL_ALPHA_MASK_SIZE
        HEAP32[((value)>>2)]=0;
        return 1;
      case 0x303F: // EGL_COLOR_BUFFER_TYPE
        // EGL has two types of buffers: EGL_RGB_BUFFER and EGL_LUMINANCE_BUFFER.
        HEAP32[((value)>>2)]=0x308E;
        return 1;
      case 0x3040: // EGL_RENDERABLE_TYPE
        // A bit combination of EGL_OPENGL_ES_BIT,EGL_OPENVG_BIT,EGL_OPENGL_ES2_BIT and EGL_OPENGL_BIT.
        HEAP32[((value)>>2)]=0x0004;
        return 1;
      case 0x3042: // EGL_CONFORMANT
        // "EGL_CONFORMANT is a mask indicating if a client API context created with respect to the corresponding EGLConfig will pass the required conformance tests for that API."
        HEAP32[((value)>>2)]=0;
        return 1;
      default:
        EGL.setErrorCode(0x3004 /* EGL_BAD_ATTRIBUTE */);
        return 0;
      }
    }
  function _eglGetDisplay(nativeDisplayType) {
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      // Note: As a 'conformant' implementation of EGL, we would prefer to init here only if the user
      //       calls this function with EGL_DEFAULT_DISPLAY. Other display IDs would be preferred to be unsupported
      //       and EGL_NO_DISPLAY returned. Uncomment the following code lines to do this.
      // Instead, an alternative route has been preferred, namely that the Emscripten EGL implementation
      // "emulates" X11, and eglGetDisplay is expected to accept/receive a pointer to an X11 Display object.
      // Therefore, be lax and allow anything to be passed in, and return the magic handle to our default EGLDisplay object.
  //    if (nativeDisplayType == 0 /* EGL_DEFAULT_DISPLAY */) {
          return 62000; // Magic ID for Emscripten 'default display'
  //    }
  //    else
  //      return 0; // EGL_NO_DISPLAY
    }
  function _eglInitialize(display, majorVersion, minorVersion) {
      if (display == 62000 /* Magic ID for Emscripten 'default display' */) {
        if (majorVersion) {
          HEAP32[((majorVersion)>>2)]=1; // Advertise EGL Major version: '1'
        }
        if (minorVersion) {
          HEAP32[((minorVersion)>>2)]=4; // Advertise EGL Minor version: '4'
        }
        EGL.defaultDisplayInitialized = true;
        EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
        return 1;
      } 
      else {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
    }
  function _eglQueryString(display, name) {
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      //\todo An EGL_NOT_INITIALIZED error is generated if EGL is not initialized for dpy. 
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      if (EGL.stringCache[name]) return EGL.stringCache[name];
      var ret;
      switch(name) {
        case 0x3053 /* EGL_VENDOR */: ret = allocate(intArrayFromString("Emscripten"), 'i8', ALLOC_NORMAL); break;
        case 0x3054 /* EGL_VERSION */: ret = allocate(intArrayFromString("1.4 Emscripten EGL"), 'i8', ALLOC_NORMAL); break;
        case 0x3055 /* EGL_EXTENSIONS */:  ret = allocate(intArrayFromString(""), 'i8', ALLOC_NORMAL); break; // Currently not supporting any EGL extensions.
        case 0x308D /* EGL_CLIENT_APIS */: ret = allocate(intArrayFromString("OpenGL_ES"), 'i8', ALLOC_NORMAL); break;
        default:
          EGL.setErrorCode(0x300C /* EGL_BAD_PARAMETER */);
          return 0;
      }
      EGL.stringCache[name] = ret;
      return ret;
    }
  function _eglCreateWindowSurface(display, config, win, attrib_list) { 
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      if (config != 62002 /* Magic ID for the only EGLConfig supported by Emscripten */) {
        EGL.setErrorCode(0x3005 /* EGL_BAD_CONFIG */);
        return 0;
      }
      // TODO: Examine attrib_list! Parameters that can be present there are:
      // - EGL_RENDER_BUFFER (must be EGL_BACK_BUFFER)
      // - EGL_VG_COLORSPACE (can't be set)
      // - EGL_VG_ALPHA_FORMAT (can't be set)
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      return 62006; /* Magic ID for Emscripten 'default surface' */
    }
  function _eglTerminate(display) {
      if (display != 62000 /* Magic ID for Emscripten 'default display' */) {
        EGL.setErrorCode(0x3008 /* EGL_BAD_DISPLAY */);
        return 0;
      }
      EGL.currentContext = 0;
      EGL.currentReadSurface = 0;
      EGL.currentDrawSurface = 0;
      EGL.defaultDisplayInitialized = false;
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
      return 1;
    }
  function _eglSwapBuffers() {
      EGL.setErrorCode(0x3000 /* EGL_SUCCESS */);
    }
  function _glGenBuffers(n, buffers) {
      for (var i = 0; i < n; i++) {
        var id = GL.getNewId(GL.buffers);
        var buffer = Module.ctx.createBuffer();
        buffer.name = id;
        GL.buffers[id] = buffer;
        HEAP32[(((buffers)+(i*4))>>2)]=id;
      }
    }
  function _glBufferData(target, size, data, usage) {
      switch (usage) { // fix usages, WebGL only has *_DRAW
        case 0x88E1: // GL_STREAM_READ
        case 0x88E2: // GL_STREAM_COPY
          usage = 0x88E0; // GL_STREAM_DRAW
          break;
        case 0x88E5: // GL_STATIC_READ
        case 0x88E6: // GL_STATIC_COPY
          usage = 0x88E4; // GL_STATIC_DRAW
          break;
        case 0x88E9: // GL_DYNAMIC_READ
        case 0x88EA: // GL_DYNAMIC_COPY
          usage = 0x88E8; // GL_DYNAMIC_DRAW
          break;
      }
      Module.ctx.bufferData(target, HEAPU8.subarray(data, data+size), usage);
    }
  function _glDeleteBuffers(n, buffers) {
      for (var i = 0; i < n; i++) {
        var id = HEAP32[(((buffers)+(i*4))>>2)];
        var buffer = GL.buffers[id];
        // From spec: "glDeleteBuffers silently ignores 0's and names that do not
        // correspond to existing buffer objects."
        if (!buffer) continue;
        Module.ctx.deleteBuffer(buffer);
        buffer.name = 0;
        GL.buffers[id] = null;
        if (id == GL.currArrayBuffer) GL.currArrayBuffer = 0;
        if (id == GL.currElementArrayBuffer) GL.currElementArrayBuffer = 0;
      }
    }
  function _glBufferSubData(target, offset, size, data) {
      Module.ctx.bufferSubData(target, offset, HEAPU8.subarray(data, data+size));
    }
  function _glUniform1fv(location, count, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform
        view = GL.miniTempBufferViews[0];
        view[0] = HEAPF32[((value)>>2)];
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*4)>>2);
      }
      Module.ctx.uniform1fv(location, view);
    }
  function _glUniform2fv(location, count, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform
        view = GL.miniTempBufferViews[1];
        view[0] = HEAPF32[((value)>>2)];
        view[1] = HEAPF32[(((value)+(4))>>2)];
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*8)>>2);
      }
      Module.ctx.uniform2fv(location, view);
    }
  function _glUniform3fv(location, count, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform
        view = GL.miniTempBufferViews[2];
        view[0] = HEAPF32[((value)>>2)];
        view[1] = HEAPF32[(((value)+(4))>>2)];
        view[2] = HEAPF32[(((value)+(8))>>2)];
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*12)>>2);
      }
      Module.ctx.uniform3fv(location, view);
    }
  function _glUniform4fv(location, count, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform
        view = GL.miniTempBufferViews[3];
        view[0] = HEAPF32[((value)>>2)];
        view[1] = HEAPF32[(((value)+(4))>>2)];
        view[2] = HEAPF32[(((value)+(8))>>2)];
        view[3] = HEAPF32[(((value)+(12))>>2)];
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*16)>>2);
      }
      Module.ctx.uniform4fv(location, view);
    }
  function _glUniform1iv(location, count, value) {
      location = GL.uniforms[location];
      value = HEAP32.subarray((value)>>2,(value+count*4)>>2);
      Module.ctx.uniform1iv(location, value);
    }
  function _glUniform2iv(location, count, value) {
      location = GL.uniforms[location];
      count *= 2;
      value = HEAP32.subarray((value)>>2,(value+count*4)>>2);
      Module.ctx.uniform2iv(location, value);
    }
  function _glUniform3iv(location, count, value) {
      location = GL.uniforms[location];
      count *= 3;
      value = HEAP32.subarray((value)>>2,(value+count*4)>>2);
      Module.ctx.uniform3iv(location, value);
    }
  function _glUniform4iv(location, count, value) {
      location = GL.uniforms[location];
      count *= 4;
      value = HEAP32.subarray((value)>>2,(value+count*4)>>2);
      Module.ctx.uniform4iv(location, value);
    }
  function _glUniformMatrix2fv(location, count, transpose, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform matrix
        view = GL.miniTempBufferViews[3];
        for (var i = 0; i < 4; i++) {
          view[i] = HEAPF32[(((value)+(i*4))>>2)];
        }
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*16)>>2);
      }
      Module.ctx.uniformMatrix2fv(location, transpose, view);
    }
  function _glUniformMatrix3fv(location, count, transpose, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform matrix
        view = GL.miniTempBufferViews[8];
        for (var i = 0; i < 9; i++) {
          view[i] = HEAPF32[(((value)+(i*4))>>2)];
        }
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*36)>>2);
      }
      Module.ctx.uniformMatrix3fv(location, transpose, view);
    }
  function _glUniformMatrix4fv(location, count, transpose, value) {
      location = GL.uniforms[location];
      var view;
      if (count == 1) {
        // avoid allocation for the common case of uploading one uniform matrix
        view = GL.miniTempBufferViews[15];
        for (var i = 0; i < 16; i++) {
          view[i] = HEAPF32[(((value)+(i*4))>>2)];
        }
      } else {
        view = HEAPF32.subarray((value)>>2,(value+count*64)>>2);
      }
      Module.ctx.uniformMatrix4fv(location, transpose, view);
    }
  function _glUniform1i(location, v0) {
      location = GL.uniforms[location];
      Module.ctx.uniform1i(location, v0);
    }
  function _glGenFramebuffers(n, ids) {
      for (var i = 0; i < n; ++i) {
        var id = GL.getNewId(GL.framebuffers);
        var framebuffer = Module.ctx.createFramebuffer();
        framebuffer.name = id;
        GL.framebuffers[id] = framebuffer;
        HEAP32[(((ids)+(i*4))>>2)]=id;
      }
    }
  function _glBindFramebuffer(target, framebuffer) {
      Module.ctx.bindFramebuffer(target, framebuffer ? GL.framebuffers[framebuffer] : null);
    }
  function _glDeleteFramebuffers(n, framebuffers) {
      for (var i = 0; i < n; ++i) {
        var id = HEAP32[(((framebuffers)+(i*4))>>2)];
        var framebuffer = GL.framebuffers[id];
        Module.ctx.deleteFramebuffer(framebuffer);
        framebuffer.name = 0;
        GL.framebuffers[id] = null;
      }
    }
  function _glFramebufferTexture2D(target, attachment, textarget, texture, level) {
      Module.ctx.framebufferTexture2D(target, attachment, textarget,
                                      GL.textures[texture], level);
    }
  function _glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer) {
      Module.ctx.framebufferRenderbuffer(target, attachment, renderbuffertarget,
                                         GL.renderbuffers[renderbuffer]);
    }
  function _glCheckFramebufferStatus(x0) { return Module.ctx.checkFramebufferStatus(x0) }
  function _glGenRenderbuffers(n, renderbuffers) {
      for (var i = 0; i < n; i++) {
        var id = GL.getNewId(GL.renderbuffers);
        var renderbuffer = Module.ctx.createRenderbuffer();
        renderbuffer.name = id;
        GL.renderbuffers[id] = renderbuffer;
        HEAP32[(((renderbuffers)+(i*4))>>2)]=id;
      }
    }
  function _glBindRenderbuffer(target, renderbuffer) {
      Module.ctx.bindRenderbuffer(target, renderbuffer ? GL.renderbuffers[renderbuffer] : null);
    }
  function _glRenderbufferStorage(x0, x1, x2, x3) { Module.ctx.renderbufferStorage(x0, x1, x2, x3) }
  function _glDeleteRenderbuffers(n, renderbuffers) {
      for (var i = 0; i < n; i++) {
        var id = HEAP32[(((renderbuffers)+(i*4))>>2)];
        var renderbuffer = GL.renderbuffers[id];
        Module.ctx.deleteRenderbuffer(renderbuffer);
        renderbuffer.name = 0;
        GL.renderbuffers[id] = null;
      }
    }
  function ___cxa_guard_acquire(variable) {
      if (!HEAP8[(variable)]) { // ignore SAFE_HEAP stuff because llvm mixes i64 and i8 here
        HEAP8[(variable)]=1;
        return 1;
      }
      return 0;
    }
  function ___cxa_guard_abort() {}
  function ___cxa_guard_release() {}
  var _abs=Math_abs;
  var _cos=Math_cos;
  var _sin=Math_sin;
  function _strcpy(pdest, psrc) {
      pdest = pdest|0; psrc = psrc|0;
      var i = 0;
      do {
        HEAP8[(((pdest+i)|0)|0)]=HEAP8[(((psrc+i)|0)|0)];
        i = (i+1)|0;
      } while (HEAP8[(((psrc)+(i-1))|0)]);
      return pdest|0;
    }
  function _strncmp(px, py, n) {
      var i = 0;
      while (i < n) {
        var x = HEAPU8[(((px)+(i))|0)];
        var y = HEAPU8[(((py)+(i))|0)];
        if (x == y && x == 0) return 0;
        if (x == 0) return -1;
        if (y == 0) return 1;
        if (x == y) {
          i ++;
          continue;
        } else {
          return x > y ? 1 : -1;
        }
      }
      return 0;
    }
  function _memmove(dest, src, num) {
      dest = dest|0; src = src|0; num = num|0;
      if (((src|0) < (dest|0)) & ((dest|0) < ((src + num)|0))) {
        // Unlikely case: Copy backwards in a safe manner
        src = (src + num)|0;
        dest = (dest + num)|0;
        while ((num|0) > 0) {
          dest = (dest - 1)|0;
          src = (src - 1)|0;
          num = (num - 1)|0;
          HEAP8[(dest)]=HEAP8[(src)];
        }
      } else {
        _memcpy(dest, src, num) | 0;
      }
    }var _llvm_memmove_p0i8_p0i8_i32=_memmove;
  function _memcmp(p1, p2, num) {
      p1 = p1|0; p2 = p2|0; num = num|0;
      var i = 0, v1 = 0, v2 = 0;
      while ((i|0) < (num|0)) {
        v1 = HEAPU8[(((p1)+(i))|0)];
        v2 = HEAPU8[(((p2)+(i))|0)];
        if ((v1|0) != (v2|0)) return ((v1|0) > (v2|0) ? 1 : -1)|0;
        i = (i+1)|0;
      }
      return 0;
    }
  function _open(path, oflag, varargs) {
      // int open(const char *path, int oflag, ...);
      // http://pubs.opengroup.org/onlinepubs/009695399/functions/open.html
      var mode = HEAP32[((varargs)>>2)];
      path = Pointer_stringify(path);
      try {
        var stream = FS.open(path, oflag, mode);
        return stream.fd;
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }function _fopen(filename, mode) {
      // FILE *fopen(const char *restrict filename, const char *restrict mode);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fopen.html
      var flags;
      mode = Pointer_stringify(mode);
      if (mode[0] == 'r') {
        if (mode.indexOf('+') != -1) {
          flags = 2;
        } else {
          flags = 0;
        }
      } else if (mode[0] == 'w') {
        if (mode.indexOf('+') != -1) {
          flags = 2;
        } else {
          flags = 1;
        }
        flags |= 64;
        flags |= 512;
      } else if (mode[0] == 'a') {
        if (mode.indexOf('+') != -1) {
          flags = 2;
        } else {
          flags = 1;
        }
        flags |= 64;
        flags |= 1024;
      } else {
        ___setErrNo(ERRNO_CODES.EINVAL);
        return 0;
      }
      var ret = _open(filename, flags, allocate([0x1FF, 0, 0, 0], 'i32', ALLOC_STACK));  // All creation permissions.
      return (ret == -1) ? 0 : ret;
    }
  function _close(fildes) {
      // int close(int fildes);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/close.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        FS.close(stream);
        return 0;
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }
  function _fsync(fildes) {
      // int fsync(int fildes);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fsync.html
      var stream = FS.getStream(fildes);
      if (stream) {
        // We write directly to the file system, so there's nothing to do here.
        return 0;
      } else {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
    }function _fclose(stream) {
      // int fclose(FILE *stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fclose.html
      _fsync(stream);
      return _close(stream);
    }
  function _recv(fd, buf, len, flags) {
      var sock = SOCKFS.getSocket(fd);
      if (!sock) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      // TODO honor flags
      return _read(fd, buf, len);
    }
  function _pread(fildes, buf, nbyte, offset) {
      // ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/read.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        var slab = HEAP8;
        return FS.read(stream, slab, buf, nbyte, offset);
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }function _read(fildes, buf, nbyte) {
      // ssize_t read(int fildes, void *buf, size_t nbyte);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/read.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        var slab = HEAP8;
        return FS.read(stream, slab, buf, nbyte);
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }function _fread(ptr, size, nitems, stream) {
      // size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fread.html
      var bytesToRead = nitems * size;
      if (bytesToRead == 0) {
        return 0;
      }
      var bytesRead = 0;
      var streamObj = FS.getStream(stream);
      while (streamObj.ungotten.length && bytesToRead > 0) {
        HEAP8[((ptr++)|0)]=streamObj.ungotten.pop()
        bytesToRead--;
        bytesRead++;
      }
      var err = _read(stream, ptr, bytesToRead);
      if (err == -1) {
        if (streamObj) streamObj.error = true;
        return 0;
      }
      bytesRead += err;
      if (bytesRead < bytesToRead) streamObj.eof = true;
      return Math.floor(bytesRead / size);
    }
  function _fwrite(ptr, size, nitems, stream) {
      // size_t fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fwrite.html
      var bytesToWrite = nitems * size;
      if (bytesToWrite == 0) return 0;
      var bytesWritten = _write(stream, ptr, bytesToWrite);
      if (bytesWritten == -1) {
        var streamObj = FS.getStream(stream);
        if (streamObj) streamObj.error = true;
        return 0;
      } else {
        return Math.floor(bytesWritten / size);
      }
    }
  function _ftell(stream) {
      // long ftell(FILE *stream);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/ftell.html
      stream = FS.getStream(stream);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      if (FS.isChrdev(stream.node.mode)) {
        ___setErrNo(ERRNO_CODES.ESPIPE);
        return -1;
      } else {
        return stream.position;
      }
    }
  function _lseek(fildes, offset, whence) {
      // off_t lseek(int fildes, off_t offset, int whence);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/lseek.html
      var stream = FS.getStream(fildes);
      if (!stream) {
        ___setErrNo(ERRNO_CODES.EBADF);
        return -1;
      }
      try {
        return FS.llseek(stream, offset, whence);
      } catch (e) {
        FS.handleFSError(e);
        return -1;
      }
    }function _fseek(stream, offset, whence) {
      // int fseek(FILE *stream, long offset, int whence);
      // http://pubs.opengroup.org/onlinepubs/000095399/functions/fseek.html
      var ret = _lseek(stream, offset, whence);
      if (ret == -1) {
        return -1;
      }
      stream = FS.getStream(stream);
      stream.eof = false;
      return 0;
    }
  var _tan=Math_tan;
  function _strdup(ptr) {
      var len = _strlen(ptr);
      var newStr = _malloc(len + 1);
      (_memcpy(newStr, ptr, len)|0);
      HEAP8[(((newStr)+(len))|0)]=0;
      return newStr;
    }
;
;
;
;
;
;
;
  function ___errno_location() {
      return ___errno_state;
    }
  function _llvm_lifetime_start() {}
  function _llvm_lifetime_end() {}
  var _llvm_memset_p0i8_i64=_memset;
  function _sbrk(bytes) {
      // Implement a Linux-like 'memory area' for our 'process'.
      // Changes the size of the memory area by |bytes|; returns the
      // address of the previous top ('break') of the memory area
      // We control the "dynamic" memory - DYNAMIC_BASE to DYNAMICTOP
      var self = _sbrk;
      if (!self.called) {
        DYNAMICTOP = alignMemoryPage(DYNAMICTOP); // make sure we start out aligned
        self.called = true;
        assert(Runtime.dynamicAlloc);
        self.alloc = Runtime.dynamicAlloc;
        Runtime.dynamicAlloc = function() { abort('cannot dynamically allocate, sbrk now has control') };
      }
      var ret = DYNAMICTOP;
      if (bytes != 0) self.alloc(bytes);
      return ret;  // Previous break location.
    }
  function _sysconf(name) {
      // long sysconf(int name);
      // http://pubs.opengroup.org/onlinepubs/009695399/functions/sysconf.html
      switch(name) {
        case 30: return PAGE_SIZE;
        case 132:
        case 133:
        case 12:
        case 137:
        case 138:
        case 15:
        case 235:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 149:
        case 13:
        case 10:
        case 236:
        case 153:
        case 9:
        case 21:
        case 22:
        case 159:
        case 154:
        case 14:
        case 77:
        case 78:
        case 139:
        case 80:
        case 81:
        case 79:
        case 82:
        case 68:
        case 67:
        case 164:
        case 11:
        case 29:
        case 47:
        case 48:
        case 95:
        case 52:
        case 51:
        case 46:
          return 200809;
        case 27:
        case 246:
        case 127:
        case 128:
        case 23:
        case 24:
        case 160:
        case 161:
        case 181:
        case 182:
        case 242:
        case 183:
        case 184:
        case 243:
        case 244:
        case 245:
        case 165:
        case 178:
        case 179:
        case 49:
        case 50:
        case 168:
        case 169:
        case 175:
        case 170:
        case 171:
        case 172:
        case 97:
        case 76:
        case 32:
        case 173:
        case 35:
          return -1;
        case 176:
        case 177:
        case 7:
        case 155:
        case 8:
        case 157:
        case 125:
        case 126:
        case 92:
        case 93:
        case 129:
        case 130:
        case 131:
        case 94:
        case 91:
          return 1;
        case 74:
        case 60:
        case 69:
        case 70:
        case 4:
          return 1024;
        case 31:
        case 42:
        case 72:
          return 32;
        case 87:
        case 26:
        case 33:
          return 2147483647;
        case 34:
        case 1:
          return 47839;
        case 38:
        case 36:
          return 99;
        case 43:
        case 37:
          return 2048;
        case 0: return 2097152;
        case 3: return 65536;
        case 28: return 32768;
        case 44: return 32767;
        case 75: return 16384;
        case 39: return 1000;
        case 89: return 700;
        case 71: return 256;
        case 40: return 255;
        case 2: return 100;
        case 180: return 64;
        case 25: return 20;
        case 5: return 16;
        case 6: return 6;
        case 73: return 4;
        case 84: return 1;
      }
      ___setErrNo(ERRNO_CODES.EINVAL);
      return -1;
    }
  function _time(ptr) {
      var ret = Math.floor(Date.now()/1000);
      if (ptr) {
        HEAP32[((ptr)>>2)]=ret
      }
      return ret;
    }
  function ___cxa_allocate_exception(size) {
      return _malloc(size);
    }
  function _llvm_eh_exception() {
      return HEAP32[((_llvm_eh_exception.buf)>>2)];
    }
  function __ZSt18uncaught_exceptionv() { // std::uncaught_exception()
      return !!__ZSt18uncaught_exceptionv.uncaught_exception;
    }
  function ___cxa_is_number_type(type) {
      var isNumber = false;
      try { if (type == __ZTIi) isNumber = true } catch(e){}
      try { if (type == __ZTIj) isNumber = true } catch(e){}
      try { if (type == __ZTIl) isNumber = true } catch(e){}
      try { if (type == __ZTIm) isNumber = true } catch(e){}
      try { if (type == __ZTIx) isNumber = true } catch(e){}
      try { if (type == __ZTIy) isNumber = true } catch(e){}
      try { if (type == __ZTIf) isNumber = true } catch(e){}
      try { if (type == __ZTId) isNumber = true } catch(e){}
      try { if (type == __ZTIe) isNumber = true } catch(e){}
      try { if (type == __ZTIc) isNumber = true } catch(e){}
      try { if (type == __ZTIa) isNumber = true } catch(e){}
      try { if (type == __ZTIh) isNumber = true } catch(e){}
      try { if (type == __ZTIs) isNumber = true } catch(e){}
      try { if (type == __ZTIt) isNumber = true } catch(e){}
      return isNumber;
    }function ___cxa_does_inherit(definiteType, possibilityType, possibility) {
      if (possibility == 0) return false;
      if (possibilityType == 0 || possibilityType == definiteType)
        return true;
      var possibility_type_info;
      if (___cxa_is_number_type(possibilityType)) {
        possibility_type_info = possibilityType;
      } else {
        var possibility_type_infoAddr = HEAP32[((possibilityType)>>2)] - 8;
        possibility_type_info = HEAP32[((possibility_type_infoAddr)>>2)];
      }
      switch (possibility_type_info) {
      case 0: // possibility is a pointer
        // See if definite type is a pointer
        var definite_type_infoAddr = HEAP32[((definiteType)>>2)] - 8;
        var definite_type_info = HEAP32[((definite_type_infoAddr)>>2)];
        if (definite_type_info == 0) {
          // Also a pointer; compare base types of pointers
          var defPointerBaseAddr = definiteType+8;
          var defPointerBaseType = HEAP32[((defPointerBaseAddr)>>2)];
          var possPointerBaseAddr = possibilityType+8;
          var possPointerBaseType = HEAP32[((possPointerBaseAddr)>>2)];
          return ___cxa_does_inherit(defPointerBaseType, possPointerBaseType, possibility);
        } else
          return false; // one pointer and one non-pointer
      case 1: // class with no base class
        return false;
      case 2: // class with base class
        var parentTypeAddr = possibilityType + 8;
        var parentType = HEAP32[((parentTypeAddr)>>2)];
        return ___cxa_does_inherit(definiteType, parentType, possibility);
      default:
        return false; // some unencountered type
      }
    }
  function ___resumeException(ptr) {
      if (HEAP32[((_llvm_eh_exception.buf)>>2)] == 0) HEAP32[((_llvm_eh_exception.buf)>>2)]=ptr;
      throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";;
    }function ___cxa_find_matching_catch(thrown, throwntype) {
      if (thrown == -1) thrown = HEAP32[((_llvm_eh_exception.buf)>>2)];
      if (throwntype == -1) throwntype = HEAP32[(((_llvm_eh_exception.buf)+(4))>>2)];
      var typeArray = Array.prototype.slice.call(arguments, 2);
      // If throwntype is a pointer, this means a pointer has been
      // thrown. When a pointer is thrown, actually what's thrown
      // is a pointer to the pointer. We'll dereference it.
      if (throwntype != 0 && !___cxa_is_number_type(throwntype)) {
        var throwntypeInfoAddr= HEAP32[((throwntype)>>2)] - 8;
        var throwntypeInfo= HEAP32[((throwntypeInfoAddr)>>2)];
        if (throwntypeInfo == 0)
          thrown = HEAP32[((thrown)>>2)];
      }
      // The different catch blocks are denoted by different types.
      // Due to inheritance, those types may not precisely match the
      // type of the thrown object. Find one which matches, and
      // return the type of the catch block which should be called.
      for (var i = 0; i < typeArray.length; i++) {
        if (___cxa_does_inherit(typeArray[i], throwntype, thrown))
          return tempRet0 = typeArray[i],thrown;
      }
      // Shouldn't happen unless we have bogus data in typeArray
      // or encounter a type for which emscripten doesn't have suitable
      // typeinfo defined. Best-efforts match just in case.
      return tempRet0 = throwntype,thrown;
    }function ___cxa_throw(ptr, type, destructor) {
      if (!___cxa_throw.initialized) {
        try {
          HEAP32[((__ZTVN10__cxxabiv119__pointer_type_infoE)>>2)]=0; // Workaround for libcxxabi integration bug
        } catch(e){}
        try {
          HEAP32[((__ZTVN10__cxxabiv117__class_type_infoE)>>2)]=1; // Workaround for libcxxabi integration bug
        } catch(e){}
        try {
          HEAP32[((__ZTVN10__cxxabiv120__si_class_type_infoE)>>2)]=2; // Workaround for libcxxabi integration bug
        } catch(e){}
        ___cxa_throw.initialized = true;
      }
      HEAP32[((_llvm_eh_exception.buf)>>2)]=ptr
      HEAP32[(((_llvm_eh_exception.buf)+(4))>>2)]=type
      HEAP32[(((_llvm_eh_exception.buf)+(8))>>2)]=destructor
      if (!("uncaught_exception" in __ZSt18uncaught_exceptionv)) {
        __ZSt18uncaught_exceptionv.uncaught_exception = 1;
      } else {
        __ZSt18uncaught_exceptionv.uncaught_exception++;
      }
      throw ptr + " - Exception catching is disabled, this exception cannot be caught. Compile with -s DISABLE_EXCEPTION_CATCHING=0 or DISABLE_EXCEPTION_CATCHING=2 to catch.";;
    }
  var _sqrtf=Math_sqrt;
  function _llvm_bswap_i32(x) {
      return ((x&0xff)<<24) | (((x>>8)&0xff)<<16) | (((x>>16)&0xff)<<8) | (x>>>24);
    }
FS.staticInit();__ATINIT__.unshift({ func: function() { if (!Module["noFSInit"] && !FS.init.initialized) FS.init() } });__ATMAIN__.push({ func: function() { FS.ignorePermissions = false } });__ATEXIT__.push({ func: function() { FS.quit() } });Module["FS_createFolder"] = FS.createFolder;Module["FS_createPath"] = FS.createPath;Module["FS_createDataFile"] = FS.createDataFile;Module["FS_createPreloadedFile"] = FS.createPreloadedFile;Module["FS_createLazyFile"] = FS.createLazyFile;Module["FS_createLink"] = FS.createLink;Module["FS_createDevice"] = FS.createDevice;
___errno_state = Runtime.staticAlloc(4); HEAP32[((___errno_state)>>2)]=0;
__ATINIT__.unshift({ func: function() { TTY.init() } });__ATEXIT__.push({ func: function() { TTY.shutdown() } });TTY.utf8 = new Runtime.UTF8Processor();
if (ENVIRONMENT_IS_NODE) { var fs = require("fs"); NODEFS.staticInit(); }
__ATINIT__.push({ func: function() { SOCKFS.root = FS.mount(SOCKFS, {}, null); } });
GL.init()
Module["requestFullScreen"] = function Module_requestFullScreen(lockPointer, resizeCanvas) { Browser.requestFullScreen(lockPointer, resizeCanvas) };
  Module["requestAnimationFrame"] = function Module_requestAnimationFrame(func) { Browser.requestAnimationFrame(func) };
  Module["setCanvasSize"] = function Module_setCanvasSize(width, height, noUpdates) { Browser.setCanvasSize(width, height, noUpdates) };
  Module["pauseMainLoop"] = function Module_pauseMainLoop() { Browser.mainLoop.pause() };
  Module["resumeMainLoop"] = function Module_resumeMainLoop() { Browser.mainLoop.resume() };
  Module["getUserMedia"] = function Module_getUserMedia() { Browser.getUserMedia() }
_llvm_eh_exception.buf = allocate(12, "void*", ALLOC_STATIC);
STACK_BASE = STACKTOP = Runtime.alignMemory(STATICTOP);
staticSealed = true; // seal the static portion of memory
STACK_MAX = STACK_BASE + 5242880;
DYNAMIC_BASE = DYNAMICTOP = Runtime.alignMemory(STACK_MAX);
assert(DYNAMIC_BASE < TOTAL_MEMORY); // Stack must fit in TOTAL_MEMORY; allocations from here on may enlarge TOTAL_MEMORY
var FUNCTION_TABLE = [0,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole6CameraEJPNS2_6EngineEEEENS1_3egg16IntrusivePointerIT_NS6_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper20BackableVertexFormat11findElementEi,0,__ZThn12_N7toadlet7tadpole13MeshComponent11frameUpdateEii,0,__ZThn12_NK7toadlet6peeper12GLES2Texture7getNameEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState19getNumTextureStatesENS0_6Shader10ShaderTypeE,0,__ZThn36_N7toadlet6peeper11GLES2Buffer6updateEPhii,0,__ZN7toadlet6peeper16GLES2RenderState7destroyEv,0,__ZThn40_NK7toadlet6peeper14BackableBuffer9getAccessEv,0,__ZNK7toadlet7tadpole4Node10getVisibleEi,0,__ZN7toadlet6peeper14BackableBuffer7setBackENS_3egg16IntrusivePointerINS0_12VertexBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper19BackableRenderState16setMaterialStateERKNS0_13MaterialStateE,0,__ZN7toadlet7tadpole4Mesh7SubMesh7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet3egg2io10DataStream8readableEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer6createEiiNS0_11IndexBuffer11IndexFormatEi,0,__ZThn36_N7toadlet6peeper15BackableTexture6retainEv,0,__ZN7toadlet7tadpole11TGAStreamerD0Ev,0,__ZThn36_N7toadlet7tadpole12HopComponent8setScopeEi,0,__ZN10emscripten4baseIN7toadlet7tadpole13BaseComponentEE14convertPointerINS2_4NodeES3_EEPT0_PT_,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole6action13BaseAnimation8getScopeEv,0,__ZN7toadlet7tadpole8material23MaterialDiffuseVariableD0Ev,0,__ZNK7toadlet6peeper17GLES2RenderTarget9isPrimaryEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer6unlockEv,0,__ZN7toadlet6peeper14BackableBuffer11resetCreateEv,0,__ZN7toadlet3egg12BaseResource20setDestroyedListenerEPNS0_25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper19BackableShaderStateD1Ev,0,__ZN7toadlet7tadpole12WaveStreamerD1Ev,0,__ZThn36_N7toadlet6peeper14BackableShader7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole4Node16componentRemovedEPNS0_9ComponentE,0,__ZN7toadlet7tadpole4Mesh24internal_setUniqueHandleEi,0,__ZNK7toadlet6peeper15BackableTexture15getUniqueHandleEv,0,__ZN7toadlet7tadpole6sensor13SensorResultsD0Ev,0,__ZN7toadlet7tadpole11FontManagerD0Ev,0,__ZN7toadlet7tadpole13BaseComponent11logicUpdateEii,0,__ZThn48_N7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_20VariableBufferFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn44_NK7toadlet7tadpole17SkeletonComponent17getRenderMaterialEv,0,__ZThn12_N7toadlet7tadpole6sensor13SensorResults11resultFoundEPNS0_4NodeEf,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice8endSceneEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimationD0Ev,0,__ZThn12_N7toadlet7tadpole12TMSHStreamerD1Ev,0,__ZThn36_N7toadlet7tadpole4NodeD1Ev,0,__ZN7toadlet3egg6LoggerD0Ev,0,__ZThn12_NK7toadlet7tadpole8Sequence7getNameEv,0,__ZNK7toadlet7tadpole17SkeletonComponent17getWorldTransformEv,0,__ZThn12_N7toadlet3egg2io10DataStream5writeEPKhi,0,__ZN7toadlet6peeper19BackableShaderState7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8material25LightViewPositionVariable9getFormatEv,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole13BaseComponentENS2_9ComponentEED0Ev,0,__ZThn12_NK7toadlet7tadpole13BaseComponent7getTypeEv,0,__ZN7toadlet6peeper11GLES2Buffer18getRootPixelBufferEv,0,__ZN7toadlet7tadpole8material21MaterialLightVariableD0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice25getShaderProfileSupportedERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBufferD0Ev,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole6EngineENS3_25DefaultIntrusiveSemanticsEEEJEE6invokeEPFS8_vE,0,__ZN7toadlet7tadpole13BaseComponent7destroyEv,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer7getNameEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget10deactivateEv,0,__ZN7toadlet7tadpole8material18TextureSetVariable6linkedEPNS1_10RenderPassE,0,__ZN7toadlet7tadpole8material22CameraPositionVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer6updateEPhii,0,__ZThn36_N7toadlet6peeper12GLES2Texture7destroyEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9ComponentEKFRKNS2_3egg6StringEvES8_PKS4_JEE6invokeERKSA_SC_,0,__ZN7toadlet6peeper11GLES2Buffer6unlockEv,0,__ZN7toadlet6peeper17GLES2RenderTarget12resourceThisEv,0,__ZThn12_NK7toadlet6peeper19BackableRenderState7getNameEv,0,__ZNK7toadlet6peeper16GLES2RenderState19getNumTextureStatesENS0_6Shader10ShaderTypeE,0,__ZThn36_N7toadlet6peeper13GLES2SLShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper13GLES2SLShaderD1Ev,0,__ZThn12_N7toadlet7tadpole13BufferManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZN7toadlet3hop5SolidD1Ev,0,__ZN7toadlet6peeper19BackableRenderState6createEv,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState16getMaterialStateERNS0_13MaterialStateE,0,__ZN7toadlet3egg6Object13fullyReleasedEv,0,__ZThn12_N7toadlet3egg2io10FileStreamD1Ev,0,__ZN7toadlet7tadpole10HopManager10setGravityERKNS_3egg4math7Vector3E,0,__ZN7toadlet7tadpole10HopManager9traceNodeERNS0_16PhysicsCollisionEPNS0_4NodeERKNS_3egg4math7SegmentEi,0,__ZN7toadlet6peeper15BackableTexture7destroyEv,0,__ZN7toadlet7tadpole12WaveStreamer6retainEv,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget7getNameEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole6CameraENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn12_NK7toadlet7tadpole4Node7getTypeEv,0,__ZNK7toadlet7tadpole6action13BaseAnimation9getWeightEv,0,__ZN7toadlet7tadpole12HopComponent7setMassEf,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat10addElementERKNS_3egg6StringEii,0,__ZN7toadlet7tadpole8material20LightDiffuseVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn12_N7toadlet3egg2io12MemoryStream7releaseEv,0,__ZThn48_NK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent7getTypeEv,0,__ZThn12_N7toadlet7tadpole15GridMeshCreator7destroyEv,0,__ZThn12_N7toadlet7tadpole13PartitionNodeD1Ev,0,__ZNK10__cxxabiv120__si_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib,0,__ZThn36_NK7toadlet7tadpole5Track8getUsageEv,0,__ZN7toadlet3egg2io10FileStream8readableEv,0,__ZN7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper17GLES2RenderDevice13setLightStateEiRKNS0_10LightStateE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer12resetDestroyEv,0,__ZN7toadlet7tadpole11BMPStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper17GLES2RenderDevice28copyFrameBufferToPixelBufferEPNS0_11PixelBufferE,0,__ZN7toadlet7tadpole12HopComponent11setPositionERKNS_3egg4math7Vector3E,0,__ZN7toadlet6peeper14BackableBuffer7setBackENS_3egg16IntrusivePointerINS0_14VariableBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn44_N7toadlet6peeper14BackableBuffer11resetCreateEv,0,__ZThn12_N7toadlet6peeper16GLES2RenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn48_N7toadlet6peeper11GLES2Buffer6unlockEv,0,__ZN7toadlet7tadpole8Skeleton7destroyEv,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget9isPrimaryEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget6createEv,0,__ZN7toadlet7tadpole12HopComponent8getBoundERNS_3egg4math5AABoxE,0,__ZN7toadlet7tadpole11WaveDecoder6lengthEv,0,__ZNK7toadlet7tadpole12HopComponent13getWorldBoundEv,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole13MeshComponentENS2_9ComponentEED0Ev,0,__ZN7toadlet6peeper17GLES2RenderDevice9setMatrixENS0_12RenderDevice10MatrixTypeERKNS_3egg4math9Matrix4x4E,0,__ZThn12_N7toadlet7tadpole4Mesh6retainEv,0,__ZN7toadlet7tadpole12HopComponent23removeCollisionListenerEPNS0_24PhysicsCollisionListenerE,0,__ZN7toadlet7tadpole6Camera13setClearColorERKNS_3egg4math7Vector4E,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormatD0Ev,0,__ZThn36_NK7toadlet6peeper19BackableRenderState16getGeometryStateERNS0_13GeometryStateE,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget14getAdaptorInfoEv,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget7releaseEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer11resetCreateEv,0,__ZN7toadlet7tadpole8Sequence12resourceThisEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer6retainEv,0,__ZThn36_N7toadlet6peeper15BackableTextureD1Ev,0,__ZN7toadlet7tadpole15GridMeshCreator7destroyEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState7setNameERKNS_3egg6StringE,0,__ZNK7toadlet7tadpole5Scene16getLogicFractionEv,0,__ZN10emscripten4baseIN7toadlet7tadpole13BaseComponentEE14convertPointerIS3_NS2_4NodeEEEPT0_PT_,0,__ZN7toadlet7tadpole6Camera7releaseEv,0,__ZN7toadlet7tadpole14TextureManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZN7toadlet7tadpole4Node12lightRemovedEPNS0_14LightComponentE,0,__ZN7toadlet6peeper19BackableRenderState13setDepthStateERKNS0_10DepthStateE,0,__ZThn36_N7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget6attachENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEENS0_23PixelBufferRenderTarget10AttachmentE,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimation7releaseEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState9setShaderENS0_6Shader10ShaderTypeENS_3egg16IntrusivePointerIS2_NS4_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget8getWidthEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTargetD0Ev,0,__ZThn12_N7toadlet3egg2io10DataStream6closedEv,0,__ZN7toadlet7tadpole6sensor13SensorResults11resultFoundEPNS0_4NodeEf,0,__ZThn12_N7toadlet7tadpole5Scene19postContextActivateEPNS_6peeper12RenderDeviceE,0,__ZThn36_NK7toadlet6peeper19BackableRenderState12getSetStatesEv,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat11findElementERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper20VariableBufferFormat6retainEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget7destroyEv,0,__ZThn36_N7toadlet7tadpole12HopComponent19setCollideWithScopeEi,0,__ZThn36_N7toadlet6peeper15BackableTexture7destroyEv,0,__ZN7toadlet7tadpole6Camera15setDefaultStateEPNS_6peeper11RenderStateE,0,__ZNK7toadlet6peeper22EGL2WindowRenderTarget9getHeightEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer12resetDestroyEv,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer8getUsageEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper12RenderDeviceENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget9isPrimaryEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState23getVariableBufferFormatENS0_6Shader10ShaderTypeEi,0,__ZN7toadlet6peeper19BackableRenderState7setNameERKNS_3egg6StringE,0,__ZN10emscripten8internal13getActualTypeIN7toadlet6peeper12RenderTargetEEEPKNS0_7_TYPEIDEPT_,0,__ZN10emscripten11RangeAccessIN7toadlet3egg20PointerIteratorRangeINS1_7tadpole4NodeEEEE5atEndERKS6_,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget30getRootPixelBufferRenderTargetEv,0,__ZThn12_N7toadlet7tadpole12HopComponent11frameUpdateEii,0,__ZN7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole4MeshD0Ev,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer18getRootPixelBufferEv,0,__ZN7toadlet7tadpole10HopManager11logicUpdateEiiPNS0_4NodeE,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreator7destroyEv,0,__ZThn12_N7toadlet3egg2io10FileStream5writeEPKhi,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget7isValidEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13BaseComponentENS2_9ComponentEE11newInstanceEv,0,__ZN7toadlet7tadpole11TGAStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget15getUniqueHandleEv,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimationD0Ev,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer11getDataSizeEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTargetD1Ev,0,__ZNK10__cxxabiv116__shim_type_info5noop1Ev,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTargetD0Ev,0,__ZNK7toadlet7tadpole5Track9getAccessEv,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget9isPrimaryEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget6retainEv,0,__ZN7toadlet6peeper19BackableRenderState7getBackEv,0,__ZN10emscripten4baseIN7toadlet7tadpole4NodeEE14convertPointerIS3_NS2_13PartitionNodeEEEPT0_PT_,0,__ZN7toadlet7tadpole8material21LightSpecularVariableD1Ev,0,__ZN7toadlet7tadpole8material16FogColorVariableD0Ev,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole15CameraComponentENS3_25DefaultIntrusiveSemanticsEEEJOPNS5_6CameraEEE6invokeEPFS8_SB_ESA_,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer7destroyEv,0,__ZThn12_NK7toadlet7tadpole8Sequence15getUniqueHandleEv,0,__ZThn36_N7toadlet6peeper11GLES2Buffer6unlockEv,0,__ZThn12_NK7toadlet6peeper23EGL2PBufferRenderTarget15getUniqueHandleEv,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation9getWeightEv,0,__ZN7toadlet6peeper18GLES2SLShaderState24internal_setUniqueHandleEi,0,__ZN7toadlet3egg2io10DataStream8positionEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer12resetDestroyEv,0,__ZThn44_N7toadlet7tadpole13MeshComponentD0Ev,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation9setWeightEf,0,__ZN7toadlet7tadpole8material24MaterialSpecularVariable9getFormatEv,0,__ZNK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZThn12_NK7toadlet6peeper18GLES2SLShaderState15getUniqueHandleEv,0,__ZN7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZThn12_NK7toadlet7tadpole12HopComponent9getActiveEv,0,__ZN7toadlet7tadpole27NormalizationTextureCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet7tadpole13ShaderManager6retainEv,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget8getWidthEv,0,__ZN7toadlet3egg17PosixErrorHandler12handleFramesEPPvi,0,__ZThn12_N7toadlet3egg12BaseResource24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState15getSamplerStateENS0_6Shader10ShaderTypeEiRNS0_12SamplerStateE,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer18getRootPixelBufferEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer4lockEi,0,__ZN7toadlet6peeper19BackableRenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper12GLES2Texture6retainEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimation9setWeightEf,0,__ZThn36_N7toadlet6peeper12GLES2Texture6retainEv,0,__ZThn12_N7toadlet7tadpole8SequenceD1Ev,0,__ZThn48_N7toadlet6peeper11GLES2Buffer6retainEv,0,__ZN7toadlet6peeper20VertexBufferAccessorD0Ev,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole4MeshENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet3egg2io12MemoryStream5writeEPKhi,0,__ZN7toadlet6peeper14BackableShaderD0Ev,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer7releaseEv,0,__ZN7toadlet3egg12BaseResourceD0Ev,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole4MeshENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer8getUsageEv,0,__ZNK10__cxxabiv117__class_type_info9can_catchEPKNS_16__shim_type_infoERPv,0,__ZN7toadlet7tadpole4Node7destroyEv,0,__ZN7toadlet6peeper11GLES2Buffer12resourceThisEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet3egg3LogEEEPKNS0_7_TYPEIDEPT_,0,__ZThn40_N7toadlet6peeper14BackableBuffer12resetDestroyEv,0,__ZThn12_N7toadlet7tadpole11BMPStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet7tadpole8material10RenderPassD1Ev,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent9getActiveEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState7getNameEv,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat7getNameEv,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMeshD1Ev,0,__ZThn36_NK7toadlet7tadpole12HopComponent17getCollisionScopeEv,0,__ZThn12_N7toadlet7tadpole13BaseComponent6retainEv,0,__ZN7toadlet7tadpole13BaseComponent20notifyParentAttachedEv,0,__ZN7toadlet7tadpole13BaseComponent7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper17GLES2RenderDevice7releaseEv,0,__ZThn48_N7toadlet6peeper14BackableBufferD0Ev,0,__ZN7toadlet7tadpole4Mesh7SubMesh6retainEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder4readEPhi,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice9setBufferENS0_6Shader10ShaderTypeEiPNS0_14VariableBufferE,0,__ZN7toadlet7tadpole6sensor6SensorD0Ev,0,__ZN7toadlet6peeper20GLES2FBORenderTarget7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole19SimpleRenderManager17renderRenderablesEPNS0_13RenderableSetEPNS_6peeper12RenderDeviceEPNS0_6CameraEb,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice9setMatrixENS0_12RenderDevice10MatrixTypeERKNS_3egg4math9Matrix4x4E,0,__ZN7toadlet7tadpole4Node13lightAttachedEPNS0_14LightComponentE,0,__ZN7toadlet7tadpole4Mesh7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget7destroyEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole9ComponentENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice17createRenderStateEv,0,__ZN7toadlet7tadpole8Sequence24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper17GLES2RenderTargetD1Ev,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer7destroyEv,0,__ZN7toadlet7tadpole8material18TextureSetVariableD0Ev,0,__ZN7toadlet7tadpole10HopManager11intraUpdateEPNS_3hop5SolidEif,0,__ZN7toadlet7tadpole12AudioManagerD0Ev,0,__ZN7toadlet7tadpole11NodeManagerD1Ev,0,__ZN7toadlet7tadpole11TGAStreamer6retainEv,0,__ZN7toadlet7tadpole19SimpleRenderManagerD0Ev,0,__ZN7toadlet7tadpole11NodeManager7destroyEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget12resourceThisEv,0,__ZThn12_N7toadlet7tadpole12HopComponent11handleEventEPNS_3egg5EventE,0,__ZN7toadlet6peeper17GLES2VertexFormatD1Ev,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget14getAdaptorInfoEv,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget15getUniqueHandleEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole9ComponentEEEPKNS0_7_TYPEIDEPT_,0,__ZThn36_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderStateD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet6peeper7TextureEEEvPT_,0,__ZN7toadlet6peeper12GLES2TextureD1Ev,0,__ZThn36_N7toadlet6peeper15BackableTexture17getMipPixelBufferEii,0,__ZN7toadlet7tadpole8material17MVPMatrixVariableD1Ev,0,__ZN7toadlet6peeper13TextureFormat6retainEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper11RenderStateENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet3egg12BaseResource7destroyEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState16getMaterialStateERNS0_13MaterialStateE,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer8getUsageEv,0,__ZN7toadlet7tadpole5BoundD0Ev,0,__ZNK10__cxxabiv117__class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib,0,__ZN7toadlet6peeper14BackableShader7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole6Engine7releaseEv,0,__ZNK7toadlet6peeper20BackableVertexFormat13getVertexSizeEv,0,__ZThn36_N7toadlet6peeper15BackableTexture14getRootTextureEv,0,__ZN7toadlet7tadpole6Camera11setViewportERKNS_6peeper8ViewportE,0,__ZN7toadlet6peeper17GLES2RenderDevice15getRenderTargetEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole5SceneEFPNS3_13PartitionNodeEvES6_PS4_JEE6invokeERKS8_S9_,0,__ZThn12_N7toadlet7tadpole8material8Material7releaseEv,0,__ZThn12_N7toadlet3egg2io10FileStream4seekEi,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer9getAccessEv,0,__ZThn36_N7toadlet6peeper16GLES2RenderState15setTextureStateENS0_6Shader10ShaderTypeEiRKNS0_12TextureStateE,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getSizeEv,0,__ZN7toadlet7tadpole4Node12nodeAttachedEPS1_,0,__ZThn36_N7toadlet6peeper13GLES2SLShader7releaseEv,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState16getGeometryStateERNS0_13GeometryStateE,0,__ZN7toadlet6peeper17GLES2RenderTarget24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet7tadpole13MeshComponent16transformChangedEPNS0_9TransformE,0,__ZThn36_N7toadlet6peeper17GLES2RenderTargetD1Ev,0,__ZNK7toadlet7tadpole8material8Material7getNameEv,0,__ZN7toadlet7tadpole13BufferManagerD2Ev,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget27deactivateAdditionalContextEv,0,__ZN7toadlet7tadpole13MeshComponent17gatherRenderablesEPNS0_6CameraEPNS0_13RenderableSetE,0,__ZN7toadlet7tadpole14TextureManagerD0Ev,0,__ZThn12_NK7toadlet6peeper20BackableVertexFormat7getNameEv,0,__ZNK7toadlet6peeper19BackableRenderState15getTextureStateENS0_6Shader10ShaderTypeEiRNS0_12TextureStateE,0,__ZThn48_N7toadlet6peeper14BackableBuffer6retainEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet6peeper12RenderDeviceEFvvEvPS4_JEE6invokeERKS6_S7_,0,__ZThn12_N7toadlet7tadpole8material8MaterialD1Ev,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer9getAccessEv,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManagerD1Ev,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBufferD1Ev,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState6retainEv,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper14BackableBuffer18getRootIndexBufferEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEKFPS4_vES5_PKS4_JEE6invokeERKS7_S9_,0,__ZThn12_N7toadlet6peeper13GLES2SLShader7releaseEv,0,__ZThn16_N7toadlet7tadpole10HopManager11intraUpdateEPNS_3hop5SolidEif,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreatorD1Ev,0,__ZThn44_N7toadlet7tadpole13MeshComponent11setRenderedEb,0,__ZThn36_N7toadlet6peeper14BackableBuffer6updateEPhii,0,__ZThn12_N7toadlet3egg2io10DataStream6retainEv,0,__ZThn12_NK7toadlet6peeper15BackableTexture15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensor7releaseEv,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole12AudioManagerD1Ev,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer7getSizeEv,0,__ZN7toadlet7tadpole5Scene7destroyEv,0,__ZThn12_N7toadlet7tadpole15CameraComponentD1Ev,0,__ZN7toadlet7tadpole4Node9setRotateERKNS_3egg4math10QuaternionE,0,__ZN7toadlet7tadpole8material20LightDiffuseVariable9getFormatEv,0,__ZN7toadlet7tadpole5Scene19postContextActivateEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet6peeper19BackableShaderState12resourceThisEv,0,__ZNK7toadlet7tadpole5Track11getDataSizeEv,0,__ZThn12_N7toadlet7tadpole8material8Material7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole15CameraComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet7tadpole16AABoxMeshCreator6retainEv,0,__ZN7toadlet3egg2io10FileStream6lengthEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_6peeper11RenderStateENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet7tadpole4Node6createEPNS0_5SceneE,0,__ZN7toadlet6peeper20BackableVertexFormat10addElementEiRKNS_3egg6StringEii,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer8getUsageEv,0,__ZNK7toadlet7tadpole12HopComponent12getTransformEv,0,__ZN7toadlet7tadpole8material10RenderPassD0Ev,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreator7destroyEv,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreator7destroyEv,0,__ZThn40_N7toadlet7tadpole13MeshComponent16transformChangedEPNS0_9TransformE,0,__ZN7toadlet7tadpole4Node13tryDeactivateEv,0,__ZN10emscripten8internal7InvokerIvJbEE6invokeEPFvbEb,0,__ZThn36_NK7toadlet6peeper19BackableRenderState13getDepthStateERNS0_10DepthStateE,0,__ZN7toadlet3egg2io10FileStream5resetEv,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation8getValueEv,0,__ZN7toadlet7tadpole4Node9setRotateERKNS_3egg4math9Matrix3x3E,0,__ZN7toadlet6peeper19BackableRenderState13setBlendStateERKNS0_10BlendStateE,0,__ZThn12_N7toadlet7tadpole12AudioManagerD0Ev,0,__ZN7toadlet7tadpole27NormalizationTextureCreator7destroyEv,0,__ZNK7toadlet7tadpole13MeshComponent17getWorldTransformEv,0,__ZThn44_N7toadlet6peeper14BackableBufferD0Ev,0,__ZThn44_NK7toadlet7tadpole13MeshComponent11getRenderedEv,0,__ZN7toadlet7tadpole8material10RenderPath7destroyEv,0,__ZN7toadlet7tadpole18SkyDomeMeshCreator6retainEv,0,__ZThn36_NK7toadlet6peeper14BackableShader7getNameEv,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreator7releaseEv,0,__ZThn12_N7toadlet7tadpole5Track24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState19getNumTextureStatesENS0_6Shader10ShaderTypeE,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper14BackableBuffer12resourceThisEv,0,__ZNK7toadlet6peeper14BackableBuffer15getVertexFormatEv,0,__ZThn12_N7toadlet6peeper13GLES2SLShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole8material10RenderPathD0Ev,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector4EfE7setWireIS5_EEvRKMS5_fRT_f,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState7getNameEv,0,__ZNK7toadlet6peeper12GLES2Texture7getNameEv,0,__ZN7toadlet6peeper16GLES2RenderState13setBlendStateERKNS0_10BlendStateE,0,__ZThn36_N7toadlet6peeper16GLES2RenderState16setMaterialStateERKNS0_13MaterialStateE,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState6createEv,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget14getAdaptorInfoEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer7destroyEv,0,__ZN7toadlet7tadpole22DiffuseMaterialCreator7destroyEv,0,__ZThn12_N7toadlet6peeper12GLES2Texture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper19BackableShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper11GLES2Buffer18getRootIndexBufferEv,0,__ZN7toadlet7tadpole12TMSHStreamer7releaseEv,0,__ZThn12_N7toadlet7tadpole5TrackD1Ev,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFvbEvPS4_JbEE6invokeERKS6_S7_b,0,__ZN7toadlet6peeper11GLES2Buffer21getRootVariableBufferEv,0,__ZN7toadlet6peeper15BackableTexture6createEiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEEPPh,0,__ZThn12_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZThn12_N7toadlet6peeper12GLES2TextureD1Ev,0,__ZN7toadlet7tadpole5Scene9getEngineEv,0,__ZN7toadlet7tadpole13MeshComponent7releaseEv,0,__ZNK7toadlet6peeper22EGL2WindowRenderTarget8getWidthEv,0,__ZN10emscripten8internal15raw_constructorIN7toadlet3egg4math7Vector4EJEEEPT_DpNS0_11BindingTypeIT0_E8WireTypeE,0,__ZN7toadlet7tadpole6EngineD0Ev,0,__ZN7toadlet7tadpole8material14ScalarVariable9getFormatEv,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer11getDataSizeEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice20createVariableBufferEv,0,__ZN7toadlet7tadpole17SkeletonComponent11frameUpdateEii,0,__ZN7toadlet6peeper22EGL2WindowRenderTargetD0Ev,0,__ZN7toadlet7tadpole12WaveStreamer7releaseEv,0,__ZNK7toadlet7tadpole4Mesh7getNameEv,0,__ZThn12_N7toadlet3egg12BaseResource20setDestroyedListenerEPNS0_25ResourceDestroyedListenerE,0,__ZNK7toadlet7tadpole15CameraComponent8getBoundEv,0,__ZN7toadlet7tadpole17SkeletonComponent7destroyEv,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer9getAccessEv,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensor5senseEPNS0_21SensorResultsListenerE,0,__ZThn36_N7toadlet6peeper16GLES2RenderState7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole15MaterialManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZNK7toadlet7tadpole6action13BaseAnimation11getMaxValueEv,0,__ZThn36_NK7toadlet6peeper14BackableBuffer9getAccessEv,0,__ZThn12_N7toadlet7tadpole12HopComponent11logicUpdateEii,0,__ZN7toadlet6peeper19BackableShaderState23getVariableBufferFormatENS0_6Shader10ShaderTypeEi,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation11getMaxValueEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole15CameraComponentENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZThn12_N7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole9TransformENS3_25DefaultIntrusiveSemanticsEEEJEE6invokeEPFS8_vE,0,__ZN7toadlet7tadpole4Node10stopActionERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8material25LightViewPositionVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn36_N7toadlet6peeper15BackableTexture4readEPNS0_13TextureFormatEPh,0,__ZThn36_NK7toadlet6peeper14BackableShader15getUniqueHandleEv,0,__ZN7toadlet7tadpole4MeshD0Ev,0,__ZThn12_N7toadlet7tadpole4Node11rootChangedEPS1_,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreator6retainEv,0,__ZN7toadlet7tadpole10HopManager10traceSolidERNS_3hop9CollisionEPNS2_5SolidERKNS_3egg4math7SegmentEi,0,__ZN7toadlet7tadpole11WaveDecoder9writeableEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole13MeshComponentEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole5Scene12traceSegmentERNS0_16PhysicsCollisionERKNS_3egg4math7SegmentEiPNS0_4NodeE,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBufferD1Ev,0,__ZThn12_N7toadlet7tadpole8Skeleton7destroyEv,0,__ZThn12_NK7toadlet7tadpole8Skeleton7getNameEv,0,__ZN7toadlet6peeper16GLES2RenderState16setGeometryStateERKNS0_13GeometryStateE,0,__ZThn12_NK7toadlet7tadpole4Mesh7getNameEv,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget9getHeightEv,0,__ZNK7toadlet7tadpole4Node14getNumSpacialsEv,0,__ZN7toadlet7tadpole8material20NormalMatrixVariableD0Ev,0,__ZNK7toadlet7tadpole5Scene7getTimeEv,0,__ZN7toadlet7tadpole17SkeletonComponent13parentChangedEPNS0_4NodeE,0,__ZThn36_N7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer11getDataSizeEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer6unlockEv,0,__ZThn36_NK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZThn12_N7toadlet7tadpole4Mesh7releaseEv,0,__ZThn12_N7toadlet7tadpole8Sequence24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState9getShaderENS0_6Shader10ShaderTypeE,0,__ZN7toadlet6peeper15BackableTexture7releaseEv,0,__ZThn12_NK7toadlet7tadpole13BaseComponent7getNameEv,0,__ZN7toadlet7tadpole4Node14spacialRemovedEPNS0_7SpacialE,0,__ZThn12_NK7toadlet7tadpole10HopManager26getTraceableCollisionScopeEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole4MeshENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet3egg7Errorer10startTraceEv,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget7releaseEv,0,__ZThn12_N7toadlet3egg2io10DataStream6lengthEv,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerINS2_15CameraComponentES3_EEPT0_PT_,0,__ZN7toadlet7tadpole8material18FogDensityVariable9getFormatEv,0,__ZN7toadlet6peeper18GLES2SLShaderState7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8Skeleton4Bone6retainEv,0,__ZThn12_N7toadlet7tadpole15MaterialManagerD1Ev,0,__ZThn36_N7toadlet6peeper19BackableRenderStateD1Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderState18setRasterizerStateERKNS0_15RasterizerStateE,0,__ZN7toadlet7tadpole4Mesh12resourceThisEv,0,__ZThn40_NK7toadlet7tadpole13MeshComponent17getWorldTransformEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper12RenderTargetENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMesh6retainEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder6retainEv,0,__ZN7toadlet7tadpole6sensor6Sensor9senseNodeEPNS0_4NodeE,0,__ZN7toadlet7tadpole4Node6attachEPNS0_9ComponentE,0,__ZThn12_NK7toadlet6peeper12GLES2Texture15getUniqueHandleEv,0,__ZThn12_NK7toadlet6peeper17GLES2VertexFormat7getNameEv,0,__ZThn12_N7toadlet7tadpole8material8Material6retainEv,0,__ZN7toadlet3egg5Error13unimplementedERKNS0_6StringE,0,__ZNK7toadlet7tadpole4Node10getPhysicsEv,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreatorD0Ev,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation11getMinValueEv,0,__ZNK7toadlet7tadpole17SkeletonComponent8getBoundEv,0,__ZThn40_NK7toadlet7tadpole17SkeletonComponent8getBoundEv,0,__ZN7toadlet7tadpole19SimpleRenderManager9setupPassEPNS0_8material10RenderPassEPNS_6peeper12RenderDeviceE,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget15getUniqueHandleEv,0,__ZN7toadlet7tadpole13ShaderManagerD0Ev,0,__ZN7toadlet7tadpole8material23MaterialDiffuseVariableD1Ev,0,__ZN7toadlet7tadpole6action13BaseAnimationD0Ev,0,__ZThn36_N7toadlet6peeper13GLES2SLShaderD0Ev,0,__ZN7toadlet7tadpole8material25LightViewPositionVariableD1Ev,0,__ZThn12_N7toadlet7tadpole12HopComponentD0Ev,0,__ZN7toadlet7tadpole6Camera6retainEv,0,__ZThn36_NK7toadlet7tadpole12HopComponent7getMassEv,0,__ZThn36_N7toadlet7tadpole5Track7releaseEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer6updateEPhii,0,__ZN7toadlet3egg6StringD2Ev,0,__ZN10emscripten8internal7InvokerIPN7toadlet6peeper12RenderTargetEJNS_3valEEE6invokeEPFS5_S6_EPNS0_7_EM_VALE,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormatD0Ev,0,__ZN7toadlet7tadpole17SkyBoxMeshCreator6retainEv,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat16getElementOffsetEi,0,__ZThn48_N7toadlet7tadpole12HopComponent8getBoundERNS_3egg4math5AABoxE,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat16getElementFormatEi,0,__ZN7toadlet7tadpole12HopComponent11logicUpdateEii,0,__ZN7toadlet7tadpole19SimpleRenderManager7releaseEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBufferD0Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderState6retainEv,0,__ZThn12_N7toadlet6peeper19BackableRenderState7destroyEv,0,__ZN7toadlet7tadpole8material8MaterialD0Ev,0,__ZN7toadlet7tadpole16AABoxMeshCreatorD0Ev,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole5SceneD0Ev,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMesh20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn16_N7toadlet7tadpole10HopManager12traceSegmentERNS_3hop9CollisionERKNS_3egg4math7SegmentEi,0,__ZThn40_NK7toadlet6peeper14BackableBuffer11getDataSizeEv,0,__ZNK7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEE5cloneEv,0,__ZN7toadlet7tadpole4Node8setScaleERKNS_3egg4math7Vector3E,0,__ZN7toadlet7tadpole16ResourceStreamer7destroyEv,0,__ZN7toadlet7tadpole10HopManager18SolidSensorResultsD0Ev,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat15getElementIndexEi,0,__ZThn36_N7toadlet7tadpole12HopComponent12setTraceableEPNS0_16PhysicsTraceableE,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState7setNameERKNS_3egg6StringE,0,__ZThn12_NK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZThn12_NK7toadlet7tadpole12HopComponent7getTypeEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTargetD1Ev,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole13MeshComponentENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole22DiffuseMaterialCreator6retainEv,0,__ZN7toadlet6peeper17GLES2VertexFormat7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper15BackableTextureD0Ev,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget7isValidEv,0,__ZN7toadlet7tadpole15GridMeshCreatorD0Ev,0,__ZN7toadlet6peeper19BackableShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper14BackableShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn44_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat7destroyEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimation8setValueEf,0,__ZThn36_N7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole9TransformENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole8material22CameraPositionVariableD0Ev,0,__ZN7toadlet7tadpole9Transform6retainEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder8readableEv,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat15getElementIndexEi,0,__ZN7toadlet7tadpole13PartitionNode20senseBoundingVolumesEPNS0_21SensorResultsListenerEPNS0_5BoundE,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget7destroyEv,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6unlockEv,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer7destroyEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer11resetCreateEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget10deactivateEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole13PartitionNodeENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet7tadpole10HopManager17findSolidsInAABoxERKNS_3egg4math5AABoxEPPNS_3hop5SolidEi,0,__ZThn12_N7toadlet7tadpole8Sequence7destroyEv,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole9TransformD1Ev,0,__ZNK7toadlet6peeper16GLES2RenderState15getSamplerStateENS0_6Shader10ShaderTypeEiRNS0_12SamplerStateE,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget6createEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEFvvEvPS4_JEE6invokeERKS6_S7_,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9TransformEKFRKNS2_3egg4math7Vector3EvES9_PKS4_JEE6invokeERKSB_SD_,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMesh24internal_setUniqueHandleEi,0,__ZN7toadlet3egg9ExceptionD0Ev,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math10QuaternionEfE7setWireIS5_EEvRKMS5_fRT_f,0,__ZNK7toadlet6peeper19BackableRenderState13getBlendStateERNS0_10BlendStateE,0,__ZNK7toadlet6peeper19BackableRenderState16getMaterialStateERNS0_13MaterialStateE,0,__ZN7toadlet7tadpole4Mesh7destroyEv,0,__ZN7toadlet3egg16InstantiableTypeINS_7tadpole4NodeENS2_9ComponentEED1Ev,0,__ZN7toadlet7tadpole4Mesh7SubMeshD1Ev,0,__ZN7toadlet7tadpole11FontManagerD1Ev,0,__ZN7toadlet7tadpole6action13BaseAnimation6retainEv,0,__ZN7toadlet6peeper20VariableBufferFormatD1Ev,0,__ZN7toadlet7tadpole8material25MaterialShininessVariable9getFormatEv,0,__ZThn12_N7toadlet7tadpole6sensor13SensorResultsD0Ev,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget19getRootRenderTargetEv,0,__ZNK10__cxxabiv117__class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib,0,__ZN7toadlet7tadpole12HopComponent8setBoundEPNS0_5BoundE,0,__ZNK7toadlet7tadpole4Node15getActionActiveERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole13BaseComponent11handleEventEPNS_3egg5EventE,0,__ZN7toadlet3hop5Solid7releaseEv,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer7getSizeEv,0,__ZN7toadlet7tadpole8material17MVPMatrixVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreatorD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget6retainEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole15CameraComponentEFvRKNS2_3egg4math7Vector3ES9_S9_EvPS4_JS9_S9_S9_EE6invokeERKSB_SC_PS7_SG_SG_,0,__ZN7toadlet7tadpole13MeshComponent7SubMesh6retainEv,0,__ZNK7toadlet7tadpole5Scene13getMaxLogicDTEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimation6retainEv,0,__ZN7toadlet7tadpole9Transform7releaseEv,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreator7destroyEv,0,__ZN7toadlet7tadpole8material21TextureMatrixVariableD1Ev,0,__ZThn48_NK7toadlet6peeper14BackableBuffer8getUsageEv,0,__ZN7toadlet6peeper17GLES2VertexFormat24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole11WaveDecoder4seekEi,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZN10emscripten8internal15FunctionInvokerIPFbRKN7toadlet3egg20PointerIteratorRangeINS2_7tadpole4NodeEEEEbS9_JEE6invokeEPSB_PS7_,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9TransformEFvRKNS2_3egg4math10QuaternionEEvPS4_JS9_EE6invokeERKSB_SC_PS7_,0,__ZN7toadlet7tadpole9Transform9setRotateERKNS_3egg4math10QuaternionE,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreator7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice18createVertexFormatEv,0,__ZN7toadlet7tadpole17SkyBoxMeshCreator7destroyEv,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreatorD1Ev,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet7tadpole12HopComponent9collisionERKNS0_16PhysicsCollisionE,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole13RenderableSet10endQueuingEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer6updateEPhii,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_6peeper12RenderDeviceENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet6peeper19BackableRenderState18setRasterizerStateERKNS0_15RasterizerStateE,0,__ZN7toadlet6peeper12GLES2Texture7destroyEv,0,__ZNK7toadlet6peeper12GLES2Texture15getUniqueHandleEv,0,__ZN7toadlet6peeper11GLES2Buffer11resetCreateEv,0,__ZN7toadlet6peeper19BackableRenderState7releaseEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole9TransformENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn12_NK7toadlet7tadpole4Mesh7SubMesh7getNameEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper7TextureENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet7tadpole5Scene14setCreateAudioEb,0,__ZN7toadlet3egg17PosixErrorHandler13signalHandlerEiP9siginfo_tPv,0,__ZThn12_N7toadlet6peeper14BackableBufferD1Ev,0,__ZThn48_N7toadlet7tadpole17SkeletonComponent17getNumAttachmentsEv,0,__ZN7toadlet7tadpole17SkyBoxMeshCreatorD0Ev,0,__ZThn12_N7toadlet6peeper19BackableRenderState7setNameERKNS_3egg6StringE,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFvvEvPS4_JEE6invokeERKS6_S7_,0,__ZThn12_N7toadlet7tadpole13PartitionNodeD0Ev,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole6EngineEEEPKNS0_7_TYPEIDEPT_,0,__ZThn12_N7toadlet7tadpole10HopManager9traceNodeERNS0_16PhysicsCollisionEPNS0_4NodeERKNS_3egg4math7SegmentEi,0,__ZNK7toadlet7tadpole12HopComponent7getNameEv,0,__ZNK7toadlet7tadpole4Node7getRootEv,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget9getHeightEv,0,__ZN7toadlet3egg4math15MathInitializer9referenceEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole8material8MaterialENS2_25DefaultIntrusiveSemanticsEEEE3getERKS8_,0,__ZN7toadlet7tadpole11DDSStreamer7releaseEv,0,__ZN7toadlet6peeper15BackableTexture7setBackENS_3egg16IntrusivePointerINS0_7TextureENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceE,0,__ZN7toadlet6peeper16EGL2RenderTarget8activateEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimation8setScopeEi,0,__ZN7toadlet6peeper19BackableShaderState7getBackEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole4NodeENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn36_N7toadlet6peeper19BackableShaderState6retainEv,0,__ZThn36_N7toadlet6peeper14BackableShader24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper11GLES2BufferD1Ev,0,__ZThn44_N7toadlet6peeper14BackableBuffer4lockEi,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreator7releaseEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer14castToGLBufferEv,0,__ZThn12_N7toadlet3egg2io10DataStream5flushEv,0,__ZThn36_N7toadlet6peeper19BackableShaderStateD0Ev,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6CameraEKFRKNS2_6peeper8ViewportEvES8_PKS4_JEE6invokeERKSA_SC_,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer15getVertexFormatEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet7tadpole12HopComponent17getWorldTransformEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_20VariableBufferFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper17GLES2RenderDeviceD1Ev,0,__ZThn12_NK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZThn12_N7toadlet3egg2io12MemoryStream6closedEv,0,__ZN7toadlet3hop5Shape6retainEv,0,__ZThn12_N7toadlet7tadpole10HopManager7releaseEv,0,__ZThn36_N7toadlet6peeper11GLES2Buffer12resetDestroyEv,0,__ZThn12_N7toadlet7tadpole13ShaderManagerD0Ev,0,__ZN7toadlet7tadpole12TMSHStreamerD1Ev,0,__ZN7toadlet7tadpole4Node17componentAttachedEPNS0_9ComponentE,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimation6retainEv,0,__ZThn12_N7toadlet7tadpole11DDSStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet3egg2io10DataStream5resetEv,0,__ZThn12_N7toadlet7tadpole8SkeletonD1Ev,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer9getAccessEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat14getNumElementsEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat18getElementSemanticEi,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice12createShaderEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTargetD1Ev,0,__ZN7toadlet7tadpole11FontManager6manageEPNS_3egg8ResourceERKNS2_6StringE,0,__ZN7toadlet3egg16StandardListener11addLogEntryEPNS0_6Logger8CategoryENS2_5LevelEyRKNS0_6StringE,0,__ZN7toadlet3egg2io10DataStreamD2Ev,0,__ZN7toadlet3hop9Simulator6retainEv,0,__ZN7toadlet3egg16InstantiableTypeINS_7tadpole13PartitionNodeENS2_9ComponentEED0Ev,0,__ZN7toadlet6peeper11GLES2Buffer6createEiiNS0_11IndexBuffer11IndexFormatEi,0,__ZThn12_N7toadlet7tadpole11TGAStreamer7releaseEv,0,__ZN7toadlet7tadpole11WaveDecoder6closedEv,0,__ZN7toadlet7tadpole17SphereMeshCreatorD1Ev,0,__ZThn40_N7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole13PartitionNode6retainEv,0,__ZThn12_N7toadlet6peeper12GLES2Texture7destroyEv,0,__ZN7toadlet3egg17PosixErrorHandler21setStackTraceListenerEPNS0_18StackTraceListenerE,0,__ZN7toadlet7tadpole14ArchiveManager6retainEv,0,__ZN7toadlet3egg3Log6excessERKNS0_6StringE,0,__ZThn36_NK7toadlet7tadpole12HopComponent11getPositionEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole4MeshD1Ev,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole8material14ScalarVariableD0Ev,0,__ZNK7toadlet6peeper17GLES2VertexFormat7getNameEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer6unlockEv,0,__ZThn36_N7toadlet6peeper11GLES2Buffer4lockEi,0,__ZN7toadlet7tadpole8material16FogColorVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer18getRootPixelBufferEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState9getShaderENS0_6Shader10ShaderTypeE,0,__ZN7toadlet6peeper12GLES2Texture4loadEPNS0_13TextureFormatEPh,0,__ZThn48_N7toadlet6peeper11GLES2BufferD0Ev,0,__ZN7toadlet7tadpole15ResourceManager3getEi,0,__ZN7toadlet6peeper17GLES2RenderDevice17createIndexBufferEv,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget7getNameEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTargetD1Ev,0,__ZThn12_N7toadlet6peeper12GLES2Texture6retainEv,0,__ZN7toadlet7tadpole10HopManager11frameUpdateEiiPNS0_4NodeE,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer6unlockEv,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer16getTextureFormatEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTargetD0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15setDefaultStateEv,0,__ZThn12_N7toadlet6peeper13GLES2SLShader6retainEv,0,__ZN7toadlet7tadpole10HopManager12traceSegmentERNS_3hop9CollisionERKNS_3egg4math7SegmentEi,0,__ZThn36_N7toadlet7tadpole5Track7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer6retainEv,0,__ZN7toadlet6peeper17GLES2RenderDevice11createQueryEv,0,__ZNK7toadlet6peeper17GLES2RenderTarget14getAdaptorInfoEv,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerINS2_13MeshComponentES3_EEPT0_PT_,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole9ComponentEEEvPT_,0,__ZThn12_N7toadlet7tadpole14ArchiveManagerD0Ev,0,__ZThn36_N7toadlet7tadpole12HopComponentD0Ev,0,__ZN7toadlet7tadpole5Scene20preContextDeactivateEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet6peeper20GLES2FBORenderTarget7destroyEv,0,__ZN7toadlet3egg2io12MemoryStream5resetEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget6retainEv,0,__ZN7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_20VariableBufferFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole4Mesh24setVertexBoneAssignmentsERKNS_3egg10CollectionINS3_INS1_20VertexBoneAssignmentEEEEE,0,__ZN7toadlet6peeper18GLES2SLShaderState21getNumVariableBuffersENS0_6Shader10ShaderTypeE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice29createPixelBufferRenderTargetEv,0,__ZThn12_N7toadlet7tadpole4Node11handleEventEPNS_3egg5EventE,0,__ZN7toadlet7tadpole17SkeletonComponent17getAttachmentNameEi,0,__ZThn12_N7toadlet7tadpole11BMPStreamerD1Ev,0,___ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole13BaseComponentEFvRKNS2_3egg6StringEEvPS4_JS8_EE6invokeERKSA_SB_PNS0_11BindingTypeIS6_E3$_0E_,0,__ZN10__cxxabiv117__class_type_infoD0Ev,0,__ZThn36_NK7toadlet7tadpole5Track7getSizeEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13BaseComponentENS2_9ComponentEE11getFullNameEv,0,__ZThn12_N7toadlet7tadpole11DDSStreamer6retainEv,0,__ZThn36_NK7toadlet7tadpole12HopComponent9getActiveEv,0,__ZThn12_N7toadlet7tadpole13MeshComponentD1Ev,0,__ZN7toadlet7tadpole4NodeD2Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget7releaseEv,0,__ZN7toadlet6peeper19BackableRenderState24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState13getDepthStateERNS0_10DepthStateE,0,__ZN7toadlet7tadpole13MeshComponent7setMeshERKNS_3egg6StringE,0,__ZNK7toadlet7tadpole12HopComponent11getPositionEv,0,__ZN7toadlet7tadpole10HopManager18SolidSensorResults16sensingBeginningEv,0,__ZThn36_N7toadlet7tadpole12HopComponent7destroyEv,0,__ZThn48_N7toadlet7tadpole12HopComponent12traceSegmentERNS_3hop9CollisionERKNS_3egg4math7Vector3ERKNS6_7SegmentE,0,__ZN7toadlet7tadpole12HopComponent11setVelocityERKNS_3egg4math7Vector3E,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreator7destroyEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_6peeper7TextureENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn36_N7toadlet6peeper11GLES2Buffer11resetCreateEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole13MeshComponentEFvPNS3_4MeshEEvPS4_JS6_EE6invokeERKS8_S9_S6_,0,__ZThn12_N7toadlet7tadpole11TGAStreamerD0Ev,0,___getTypeName,0,__ZThn12_N7toadlet7tadpole5Track7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderTarget19getRootRenderTargetEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole9TransformEEEvPT_,0,__ZN10emscripten8internal14raw_destructorIN7toadlet6peeper8ViewportEEEvPT_,0,__ZThn12_N7toadlet6peeper15BackableTexture7destroyEv,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation8setScopeEi,0,__ZN7toadlet6peeper16GLES2RenderState24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole12HopComponent7releaseEv,0,__ZN7toadlet7tadpole16AABoxMeshCreatorD1Ev,0,__ZThn12_N7toadlet6peeper16GLES2RenderState24internal_setUniqueHandleEi,0,__ZThn52_N7toadlet7tadpole17SkeletonComponent12getAnimationERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper13GLES2SLShader6createENS0_6Shader10ShaderTypeERKNS_3egg6StringES7_,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole9TransformEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole13BaseComponentD1Ev,0,__ZN7toadlet7tadpole4Node8activateEv,0,__ZThn12_N7toadlet7tadpole4Mesh24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole5Track20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_NK7toadlet6peeper15BackableTexture7getNameEv,0,__ZN7toadlet6peeper12GLES2Texture6createEiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEEPPh,0,__ZN7toadlet6peeper19BackableShaderState18getRootShaderStateEv,0,__ZN7toadlet7tadpole8material21LightPositionVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn44_N7toadlet6peeper14BackableBuffer6retainEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget12resourceThisEv,0,__ZThn36_N7toadlet7tadpole12HopComponent11handleEventEPNS_3egg5EventE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice7destroyEv,0,__ZN7toadlet7tadpole6Engine7destroyEv,0,__ZN7toadlet7tadpole5Bound6retainEv,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget8getWidthEv,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget9isPrimaryEv,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState13getBlendStateERNS0_10BlendStateE,0,__ZN7toadlet7tadpole13MeshComponent7SubMeshD1Ev,0,__ZN7toadlet7tadpole6action13BaseAnimation7releaseEv,0,__ZThn12_N7toadlet7tadpole14TextureManagerD1Ev,0,__ZNK7toadlet6peeper14BackableShader7getNameEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv,0,__ZN7toadlet6peeper20BackableVertexFormat12resourceThisEv,0,__ZN7toadlet6peeper17GLES2RenderDevice15setAmbientColorERKNS_3egg4math7Vector4E,0,__ZThn48_N7toadlet6peeper14BackableBuffer6unlockEv,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole13MeshComponentEJPNS2_6EngineEEEENS1_3egg16IntrusivePointerIT_NS6_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet7tadpole4Mesh6retainEv,0,__ZN7toadlet7tadpole15ResourceManagerD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice5clearEiRKNS_3egg4math7Vector4E,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole6CameraENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet6peeper11GLES2Buffer6retainEv,0,__ZN7toadlet7tadpole12HopComponent17setCollisionScopeEi,0,__ZThn12_N7toadlet3egg2io12MemoryStream5resetEv,0,__ZN7toadlet7tadpole12HopComponent8addForceERKNS_3egg4math7Vector3E,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer7setBackENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole11WaveDecoder5resetEv,0,__ZThn12_N7toadlet7tadpole15GridMeshCreator7releaseEv,0,__ZThn36_N7toadlet6peeper12GLES2Texture7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole11FontManager4findERKNS_3egg6StringEPNS0_12ResourceDataE,0,__ZN7toadlet7tadpole8material21LightPositionVariableD0Ev,0,__ZN7toadlet3hop9SimulatorD0Ev,0,__ZN7toadlet7tadpole4Node14actionAttachedEPNS0_15ActionComponentE,0,__ZN7toadlet6peeper13TextureFormat7releaseEv,0,__ZN7toadlet6peeper17GLES2VertexFormat10addElementEiii,0,__ZThn44_NK7toadlet6peeper14BackableBuffer9getAccessEv,0,__ZThn12_NK7toadlet7tadpole8material8Material15getUniqueHandleEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole4NodeEEEPKNS0_7_TYPEIDEPT_,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole17SkeletonComponentENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZThn16_N7toadlet7tadpole10HopManager9preUpdateEif,0,__ZN7toadlet7tadpole5Scene7releaseEv,0,__ZN7toadlet7tadpole8material8Material6retainEv,0,__ZN7toadlet7tadpole17SphereMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn44_NK7toadlet6peeper14BackableBuffer11getDataSizeEv,0,__ZN7toadlet6peeper20GLES2FBORenderTargetD1Ev,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensorD1Ev,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13BaseComponentENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation9getWeightEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEKFivEiPKS4_JEE6invokeERKS6_S8_,0,__ZThn36_N7toadlet6peeper12GLES2Texture4readEPNS0_13TextureFormatEPh,0,__ZThn12_NK7toadlet6peeper13GLES2SLShader7getNameEv,0,__ZNK7toadlet7tadpole12HopComponent7getTypeEv,0,__ZN7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEEi,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole6EngineEJEEENS1_3egg16IntrusivePointerIT_NS4_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet7tadpole15ResourceManager11setStreamerEPNS0_16ResourceStreamerERKNS_3egg6StringE,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget6createEv,0,__ZN7toadlet6peeper17GLES2RenderDevice25activateAdditionalContextEv,0,__ZN7toadlet3egg14PointerCounter7destroyEv,0,__ZN7toadlet6peeper22EGL2WindowRenderTarget4swapEv,0,__ZN7toadlet7tadpole8material26MaterialTrackColorVariable9getFormatEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole6EngineENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet3egg2io10DataStream6retainEv,0,__ZN7toadlet7tadpole13BufferManager7releaseEv,0,__ZNK7toadlet6peeper16GLES2RenderState13getBlendStateERNS0_10BlendStateE,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole5SceneEFvvEvPS4_JEE6invokeERKS6_S7_,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget7isValidEv,0,__ZN7toadlet7tadpole8material10RenderPath7releaseEv,0,__ZThn40_NK7toadlet7tadpole12HopComponent12getTransformEv,0,__ZNK7toadlet6peeper16GLES2RenderState11getFogStateERNS0_8FogStateE,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBufferD0Ev,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManagerD0Ev,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper12RenderTargetENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet6peeper14BackableBuffer4lockEi,0,__ZN7toadlet7tadpole15ResourceManager7releaseEv,0,__ZThn12_N7toadlet7tadpole8material8MaterialD0Ev,0,__ZN7toadlet7tadpole15MaterialManager7releaseEv,0,__ZN7toadlet3egg2io12MemoryStream4readEPhi,0,__ZThn36_N7toadlet6peeper14BackableShader6retainEv,0,__ZN7toadlet6peeper20BackableVertexFormat10addElementEiii,0,__ZNK7toadlet6peeper14BackableShader13getShaderTypeEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer18getRootIndexBufferEv,0,__ZThn36_NK7toadlet7tadpole12HopComponent7getTypeEv,0,__ZN7toadlet6peeper12GLES2Texture11resetCreateEv,0,__ZNK7toadlet6peeper11GLES2Buffer8getUsageEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState18getRootShaderStateEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat16getElementOffsetEi,0,__ZN7toadlet7tadpole16AABoxMeshCreator7destroyEv,0,__ZN7toadlet6peeper19BackableShaderStateD0Ev,0,__ZThn36_N7toadlet7tadpole4NodeD0Ev,0,__ZThn40_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZThn12_N7toadlet3egg2io10DataStreamD0Ev,0,__ZN7toadlet6peeper19BackableRenderState15setSamplerStateENS0_6Shader10ShaderTypeEiRKNS0_12SamplerStateE,0,__ZThn12_N7toadlet3egg2io12MemoryStream5closeEv,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer6retainEv,0,__ZThn12_N7toadlet3egg2io10DataStream4readEPhi,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice13setLightStateEiRKNS0_10LightStateE,0,__ZThn44_N7toadlet7tadpole12HopComponentD0Ev,0,__ZThn12_NK7toadlet6peeper14BackableShader7getNameEv,0,__ZN7toadlet7tadpole11WaveDecoder5flushEv,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer7getSizeEv,0,__ZN7toadlet7tadpole8material20LightDiffuseVariableD1Ev,0,__ZNK7toadlet7tadpole13MeshComponent7SubMesh14getRenderBoundEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer12resetDestroyEv,0,__ZThn48_N7toadlet7tadpole12HopComponentD1Ev,0,__ZN7toadlet7tadpole16TorusMeshCreator7destroyEv,0,__ZN7toadlet7tadpole11WaveDecoder4readEPhi,0,__ZN7toadlet7tadpole8material15SceneParameters6retainEv,0,__ZN7toadlet7tadpole13BaseComponent11rootChangedEPNS0_4NodeE,0,__ZThn12_N7toadlet7tadpole11BMPStreamer7releaseEv,0,__ZN7toadlet7tadpole19SimpleRenderManager22setupPassForRenderableEPNS0_8material10RenderPassEPNS_6peeper12RenderDeviceEPNS0_10RenderableERKNS_3egg4math7Vector4E,0,__ZThn12_N7toadlet3egg2io10FileStream8positionEv,0,__ZN7toadlet7tadpole5Track6updateEPhii,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation8getScopeEv,0,__ZNK7toadlet7tadpole6action13BaseAnimation7getNameEv,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE,0,__ZNK7toadlet7tadpole13MeshComponent7SubMesh18getRenderTransformEv,0,__ZN7toadlet7tadpole15CameraComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet3egg14PointerCounterD1Ev,0,__ZThn12_N7toadlet7tadpole10HopManager12setTraceableEPNS0_16PhysicsTraceableE,0,__ZN7toadlet7tadpole13RenderableSet9setCameraEPNS0_6CameraE,0,__ZThn36_NK7toadlet7tadpole12HopComponent19getCollideWithScopeEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet6peeper12RenderTargetEEEvPT_,0,__ZN7toadlet6peeper13GLES2SLShaderD0Ev,0,__ZThn36_NK7toadlet6peeper13GLES2SLShader7getNameEv,0,__ZThn12_N7toadlet7tadpole13BaseComponent11rootChangedEPNS0_4NodeE,0,__ZThn36_NK7toadlet6peeper19BackableRenderState11getFogStateERNS0_8FogStateE,0,__ZN7toadlet6peeper17GLES2RenderTarget4swapEv,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole8material21TextureMatrixVariable9getFormatEv,0,__ZNK7toadlet7tadpole5Scene8getFrameEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole13PartitionNodeENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn12_N7toadlet7tadpole11WaveDecoder5closeEv,0,__ZN7toadlet7tadpole8material8Material24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper14BackableShader7destroyEv,0,__ZNK7toadlet6peeper19BackableRenderState15getUniqueHandleEv,0,__ZThn48_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer8getUsageEv,0,__ZThn12_N7toadlet3egg2io12MemoryStream4seekEi,0,__ZThn36_NK7toadlet6peeper14BackableBuffer11getDataSizeEv,0,__ZN7toadlet6peeper14BackableBufferD0Ev,0,__ZN7toadlet3egg6LoggerD2Ev,0,__ZThn36_N7toadlet7tadpole12HopComponent11frameUpdateEii,0,__ZNK7toadlet6peeper13GLES2SLShader13getShaderTypeEv,0,__ZNK7toadlet7tadpole4Node9getActiveEv,0,__ZN7toadlet6peeper13GLES2SLShader24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole4Node11logicUpdateEii,0,__ZN7toadlet7tadpole11WaveDecoder6retainEv,0,__ZThn16_N7toadlet7tadpole10HopManager17findSolidsInAABoxERKNS_3egg4math5AABoxEPPNS_3hop5SolidEi,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole13MeshComponentENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet6peeper20BackableVertexFormat7setBackENS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice18createVertexFormatEv,0,__ZN7toadlet7tadpole4NodeD0Ev,0,__ZThn12_N7toadlet3egg2io10FileStream6retainEv,0,__ZThn12_NK7toadlet7tadpole5Track15getUniqueHandleEv,0,__ZThn16_N7toadlet7tadpole10HopManagerD0Ev,0,__ZN7toadlet7tadpole6Engine15setRenderDeviceEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet7tadpole11FontManager6retainEv,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation7getNameEv,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimationD0Ev,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBufferD1Ev,0,__ZN7toadlet7tadpole5Scene11frameUpdateEiiPNS0_4NodeE,0,__ZNK7toadlet7tadpole10HopManager26getTraceableCollisionScopeEv,0,__ZThn12_N7toadlet7tadpole10HopManager12traceSegmentERNS0_16PhysicsCollisionERKNS_3egg4math7SegmentEiPNS0_4NodeE,0,__ZN7toadlet7tadpole4Node10deactivateEv,0,__ZNK7toadlet6peeper22EGL2WindowRenderTarget9isPrimaryEv,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv,0,__ZThn44_N7toadlet7tadpole12HopComponent9collisionERKNS_3hop9CollisionE,0,__ZThn36_NK7toadlet6peeper14BackableBuffer7getSizeEv,0,__ZThn12_N7toadlet6peeper13GLES2SLShaderD1Ev,0,__ZN7toadlet7tadpole8material21LightPositionVariable9getFormatEv,0,__ZN10__cxxabiv119__pointer_type_infoD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole13MeshComponentENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet6peeper17GLES2RenderTarget7releaseEv,0,__ZThn12_N7toadlet7tadpole15GridMeshCreatorD0Ev,0,__ZN7toadlet6peeper18GLES2SLShaderState18getRootShaderStateEv,0,__ZThn12_N7toadlet3egg2io10FileStream4readEPhi,0,__ZThn36_N7toadlet6peeper14BackableShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole10HopManager7releaseEv,0,__ZThn12_N7toadlet7tadpole11TGAStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer8getUsageEv,0,__ZThn16_N7toadlet7tadpole10HopManager10postUpdateEif,0,__ZNK7toadlet7tadpole15CameraComponent7getTypeEv,0,__ZN7toadlet7tadpole6action13BaseAnimationD2Ev,0,__ZThn12_NK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv,0,__ZNK7toadlet7tadpole13MeshComponent11getRenderedEv,0,__ZN7toadlet7tadpole10HopManager18SolidSensorResults13sensingEndingEv,0,__ZThn12_N7toadlet3egg2io10DataStream7releaseEv,0,__ZN7toadlet7tadpole8material8MaterialD1Ev,0,__ZThn12_N7toadlet6peeper16GLES2RenderState7releaseEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget25activateAdditionalContextEv,0,__ZThn12_N7toadlet7tadpole12WaveStreamer7releaseEv,0,__ZN7toadlet6peeper16GLES2RenderState6createEv,0,__ZN7toadlet6peeper15BackableTexture4readEPNS0_13TextureFormatEPh,0,__ZThn52_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZThn12_N7toadlet6peeper22EGL2WindowRenderTargetD0Ev,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole15CameraComponentENS2_9ComponentEED0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice25activateAdditionalContextEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget8activateEv,0,__ZN7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole5SceneEEEvPT_,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFbPNS2_6ribbit11AudioDeviceEEbPS4_JS7_EE6invokeERKS9_SA_S7_,0,__ZN7toadlet7tadpole5Scene14setNodeManagerEPNS0_11NodeManagerE,0,__ZN7toadlet6peeper20GLES2FBORenderTarget24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv,0,__ZN7toadlet6peeper11GLES2Buffer12resetDestroyEv,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreator7destroyEv,0,__ZNK7toadlet7tadpole12HopComponent10getGravityEv,0,__ZThn12_N7toadlet3egg2io12MemoryStream6retainEv,0,__ZThn12_NK7toadlet6peeper19BackableShaderState15getUniqueHandleEv,0,__ZN7toadlet7tadpole8material14RenderVariable6linkedEPNS1_10RenderPassE,0,__ZN7toadlet7tadpole6sensor6Sensor5senseEPNS0_21SensorResultsListenerE,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimationD1Ev,0,__ZNK7toadlet7tadpole5Scene13getMinLogicDTEv,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZNK7toadlet7tadpole9Transform12getTranslateEv,0,__ZN7toadlet7tadpole4Mesh7releaseEv,0,__ZThn44_N7toadlet7tadpole13MeshComponentD1Ev,0,__ZN7toadlet3egg6Object6retainEv,0,__ZN7toadlet3egg2io10FileStream6closedEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer21getRootVariableBufferEv,0,__ZN7toadlet7tadpole5Scene14preFrameUpdateEi,0,__ZThn12_N7toadlet7tadpole15CameraComponent7releaseEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget11resetCreateEv,0,__ZThn12_NK7toadlet6peeper23EGL2PBufferRenderTarget7getNameEv,0,__ZThn36_N7toadlet6peeper14BackableShaderD1Ev,0,__ZThn36_NK7toadlet7tadpole15CameraComponent12getTransformEv,0,__ZNK7toadlet7tadpole4Node8getChildEPKNS_3egg4TypeINS0_9ComponentEEE,0,__ZN7toadlet7tadpole14ArchiveManagerD1Ev,0,__ZThn12_N7toadlet7tadpole12HopComponentD1Ev,0,__ZN7toadlet7tadpole13MeshComponent7SubMesh7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice17createShaderStateEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderTargetD1Ev,0,__ZN7toadlet7tadpole8material19FogDistanceVariableD0Ev,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat6retainEv,0,__ZThn12_NK7toadlet3egg12BaseResource7getNameEv,0,__ZN7toadlet7tadpole6action13BaseAnimation20setAnimationListenerEPNS1_17AnimationListenerE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBufferD0Ev,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer18getRootPixelBufferEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat14getNumElementsEv,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer4lockEi,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole12HopComponentENS2_9ComponentEED1Ev,0,__ZN7toadlet7tadpole13MeshComponent20setSharedRenderStateENS_3egg16IntrusivePointerINS_6peeper11RenderStateENS2_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet6peeper18GLES2SLShaderState15getUniqueHandleEv,0,__ZN7toadlet3egg16StandardListenerD1Ev,0,__ZThn36_N7toadlet6peeper13GLES2SLShader7setNameERKNS_3egg6StringE,0,__ZThn12_NK7toadlet7tadpole4Mesh15getUniqueHandleEv,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget7getNameEv,0,__ZN7toadlet7tadpole20WaterMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat6createEv,0,__ZN7toadlet3egg2io10FileStreamD2Ev,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole4Node16transformChangedEPNS0_9TransformE,0,__ZThn12_N7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreator7destroyEv,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormatD1Ev,0,__ZN7toadlet6peeper12GLES2Texture12resetDestroyEv,0,__ZThn12_N7toadlet3egg2io10FileStreamD0Ev,0,__ZN7toadlet6peeper17GLES2RenderDevice8endSceneEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer7getNameEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer6updateEPhii,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation8getScopeEv,0,__ZThn12_NK7toadlet6peeper18GLES2SLShaderState7getNameEv,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZN7toadlet7tadpole6Engine6retainEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole15GridMeshCreator6retainEv,0,__ZThn12_N7toadlet7tadpole8Sequence7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet6peeper14BackableBuffer8getUsageEv,0,__ZThn36_N7toadlet7tadpole15CameraComponentD1Ev,0,__ZNK7toadlet6peeper14BackableBuffer7getSizeEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat16getElementFormatEi,0,__ZN7toadlet6peeper17GLES2RenderDevice10setTextureENS0_6Shader10ShaderTypeEiPNS0_7TextureE,0,__ZN7toadlet7tadpole12AudioManagerD1Ev,0,__ZN7toadlet7tadpole4Node12boundChangedEv,0,__ZN7toadlet7tadpole11NodeManagerD0Ev,0,__ZThn36_N7toadlet6peeper14BackableBuffer12resetDestroyEv,0,__ZNK10__cxxabiv121__vmi_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi,0,__ZN7toadlet7tadpole8material17MVPMatrixVariableD0Ev,0,__ZN7toadlet6peeper19BackableRenderState6retainEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState6createEv,0,__ZN7toadlet7tadpole6CameraD1Ev,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole13PartitionNodeENS2_9ComponentEE11newInstanceEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg4math7Vector4EEEvPT_,0,__ZN7toadlet7tadpole8material10RenderPath6retainEv,0,__ZN7toadlet7tadpole15GridMeshCreatorD1Ev,0,__ZN7toadlet7tadpole12WaveStreamerD0Ev,0,__ZN7toadlet7tadpole8SequenceD1Ev,0,__ZN7toadlet6peeper19GLES2FBOPixelBufferD0Ev,0,__ZThn12_N7toadlet7tadpole10HopManagerD1Ev,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreatorD1Ev,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer8getUsageEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget6attachENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEENS0_23PixelBufferRenderTarget10AttachmentE,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager9setupPassEPNS0_8material10RenderPassEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole13BaseComponentEEEvPT_,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget9getHeightEv,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensorD1Ev,0,__ZThn48_N7toadlet7tadpole13MeshComponent18getAttachmentIndexERKNS_3egg6StringE,0,__ZNK7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEE6equalsEPNS0_8IteratorIS6_EE,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn48_N7toadlet6peeper14BackableBuffer7releaseEv,0,__ZN7toadlet6peeper20BackableVertexFormat11findElementERKNS_3egg6StringE,0,__ZN10emscripten8internal13getActualTypeIN7toadlet3egg5ErrorEEEPKNS0_7_TYPEIDEPT_,0,__ZNK7toadlet6peeper20BackableVertexFormat14getElementNameEi,0,__ZThn12_N7toadlet7tadpole15ResourceManagerD0Ev,0,__ZNK7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEE11dereferenceEv,0,__ZN7toadlet7tadpole6Camera8setScopeEi,0,__ZThn44_N7toadlet7tadpole13MeshComponent17gatherRenderablesEPNS0_6CameraEPNS0_13RenderableSetE,0,__ZThn36_N7toadlet6peeper12GLES2Texture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn44_N7toadlet6peeper11GLES2Buffer29castToGLTextureMipPixelBufferEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer6retainEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer29castToGLTextureMipPixelBufferEv,0,__ZN7toadlet7tadpole5Scene14setExcessiveDTEi,0,__ZN7toadlet6peeper19IndexBufferAccessorD2Ev,0,__ZThn40_N7toadlet6peeper11GLES2Buffer12resetDestroyEv,0,__ZThn36_N7toadlet7tadpole12HopComponent20addCollisionListenerEPNS0_24PhysicsCollisionListenerE,0,__ZN7toadlet7tadpole8material21LightSpecularVariableD0Ev,0,__ZNK7toadlet7tadpole13BaseComponent7getNameEv,0,__ZThn48_N7toadlet7tadpole17SkeletonComponent18getAttachmentIndexERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8Skeleton7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper13GLES2SLShader7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper20BackableVertexFormat7releaseEv,0,__ZThn12_NK7toadlet6peeper17GLES2RenderTarget7getNameEv,0,__ZN7toadlet7tadpole15ResourceManager20unableToFindStreamerERKNS_3egg6StringEPNS0_12ResourceDataE,0,__ZThn12_N7toadlet7tadpole11DDSStreamerD0Ev,0,__ZN7toadlet7tadpole4Mesh19setStaticVertexDataENS_3egg16IntrusivePointerINS_6peeper10VertexDataENS2_25DefaultIntrusiveSemanticsEEE,0,___ZN10emscripten8internal7InvokerIvJRKN7toadlet3egg6StringEEE6invokeEPFvS6_EPNS0_11BindingTypeIS4_E3$_0E_,0,__ZNK7toadlet6peeper16GLES2RenderState19getNumSamplerStatesENS0_6Shader10ShaderTypeE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDeviceD1Ev,0,__ZNK7toadlet7tadpole13BaseComponent7getTypeEv,0,__ZN7toadlet7tadpole8material25MaterialShininessVariable6updateEPhPNS1_15SceneParametersE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole6CameraENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer22castToGLFBOPixelBufferEv,0,__ZN7toadlet7tadpole13MeshComponent17getNumAttachmentsEv,0,__ZNK7toadlet6peeper15BackableTexture9getFormatEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet6peeper20BackableVertexFormatD1Ev,0,__ZN7toadlet7tadpole12HopComponent9collisionERKNS0_16PhysicsCollisionE,0,__ZN7toadlet7tadpole13BaseComponent7releaseEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole13PartitionNodeENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole15CameraComponentENS2_9ComponentEE11getFullNameEv,0,__ZThn48_NK7toadlet6peeper14BackableBuffer23getVariableBufferFormatEv,0,__ZThn12_N7toadlet3egg2io12MemoryStreamD1Ev,0,__ZThn40_N7toadlet6peeper11GLES2BufferD0Ev,0,__ZNK7toadlet7tadpole13MeshComponent20getSharedRenderStateEv,0,__ZThn12_N7toadlet7tadpole13MeshComponent7SubMeshD0Ev,0,__ZN7toadlet3egg5Error17invalidParametersERKNS0_6StringE,0,__ZN7toadlet7tadpole8material17MVPMatrixVariable9getFormatEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole13BaseComponentEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole15MaterialManager7destroyEv,0,__ZThn36_NK7toadlet6peeper22EGL2WindowRenderTarget9getHeightEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer12resetDestroyEv,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreator7releaseEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK10__cxxabiv117__class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi,0,__ZNK7toadlet6peeper15BackableTexture8getUsageEv,0,__ZN7toadlet3egg2io10DataStream7releaseEv,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper19BackableShaderState24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole15ResourceManager3getERKNS_3egg6StringE,0,__ZThn12_NK7toadlet7tadpole13MeshComponent7getTypeEv,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat14getElementNameEi,0,__ZNK7toadlet6peeper16GLES2RenderState15getUniqueHandleEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget12resetDestroyEv,0,__ZThn48_NK7toadlet6peeper14BackableBuffer9getAccessEv,0,__ZN7toadlet7tadpole11FontManager7releaseEv,0,__ZNK7toadlet7tadpole17SkeletonComponent6renderEPNS0_13RenderManagerE,0,__ZThn12_N7toadlet6peeper14BackableBufferD0Ev,0,__ZN7toadlet3egg2io10FileStream5writeEPKhi,0,__ZN7toadlet6peeper11GLES2Buffer22castToGLFBOPixelBufferEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFbPNS2_6peeper12RenderDeviceEEbPS4_JS7_EE6invokeERKS9_SA_S7_,0,__ZN7toadlet6peeper19BackableRenderState11setFogStateERKNS0_8FogStateE,0,__ZNK7toadlet7tadpole4Node12getTransformEv,0,__ZN7toadlet6peeper20BackableVertexFormat7destroyEv,0,__ZN7toadlet7tadpole4Node11logicUpdateEii,0,__ZNK7toadlet6peeper20BackableVertexFormat16getElementOffsetEi,0,__ZN7toadlet7tadpole8Skeleton6retainEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole20WaterMaterialCreator7releaseEv,0,__ZThn16_N7toadlet7tadpole10HopManager10traceSolidERNS_3hop9CollisionEPNS2_5SolidERKNS_3egg4math7SegmentEi,0,__ZN7toadlet6peeper11GLES2Buffer29castToGLTextureMipPixelBufferEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState21getNumVariableBuffersENS0_6Shader10ShaderTypeE,0,__ZThn12_N7toadlet7tadpole8Skeleton20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole5Scene14preLogicUpdateEi,0,__ZN7toadlet6peeper17GLES2RenderDevice8activateEv,0,__ZThn36_N7toadlet6peeper22EGL2WindowRenderTargetD1Ev,0,__ZNK7toadlet6peeper14BackableBuffer9getAccessEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice6createEPNS0_12RenderTargetEi,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMeshD0Ev,0,__ZThn48_NK7toadlet6peeper14BackableBuffer7getSizeEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_3hop9SimulatorENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn12_NK7toadlet6peeper20GLES2FBORenderTarget15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole11BMPStreamer6retainEv,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat10addElementEiii,0,__ZThn44_N7toadlet6peeper11GLES2Buffer6updateEPhii,0,__ZN7toadlet7tadpole6Engine15installHandlersEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet7tadpole11DDSStreamer7releaseEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13MeshComponentENS2_9ComponentEE11getFullNameEv,0,__ZN10__cxxabiv120__si_class_type_infoD0Ev,0,__ZN7toadlet6peeper17GLES2VertexFormat10addElementERKNS_3egg6StringEii,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFNS2_3egg16IntrusivePointerINS3_8material8MaterialENS5_25DefaultIntrusiveSemanticsEEEPNS2_6peeper7TextureEPNSB_11RenderStateEESA_PS4_JSD_SF_EE6invokeERKSH_SI_SD_SF_,0,__ZThn12_NK7toadlet7tadpole4Node9getActiveEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer22castToGLFBOPixelBufferEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole15CameraComponentEFPNS3_6CameraEvES6_PS4_JEE6invokeERKS8_S9_,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager11renderSceneEPNS_6peeper12RenderDeviceEPNS0_4NodeEPNS0_6CameraE,0,__ZN7toadlet7tadpole15ResourceManager4findERKNS_3egg6StringEPNS0_12ResourceDataE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer4lockEi,0,__ZN7toadlet7tadpole4Node8setScaleEf,0,__ZThn12_N7toadlet7tadpole8SkeletonD0Ev,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat11findElementEi,0,__ZThn12_N7toadlet6peeper17GLES2RenderDeviceD0Ev,0,__ZN7toadlet7tadpole10HopManagerD0Ev,0,__ZThn48_N7toadlet6peeper11GLES2Buffer6updateEPhii,0,__ZN7toadlet6peeper20BackableVertexFormat6retainEv,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation9getWeightEv,0,__ZN7toadlet7tadpole8material23MaterialAmbientVariableD1Ev,0,__ZThn12_N7toadlet7tadpole11WaveDecoder6closedEv,0,__ZThn36_N7toadlet6peeper12GLES2Texture4loadEPNS0_13TextureFormatEPh,0,__ZThn36_N7toadlet6peeper13GLES2SLShader6retainEv,0,__ZN7toadlet7tadpole6sensor13SensorResults7releaseEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole16AABoxMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn12_N7toadlet7tadpole5TrackD0Ev,0,__ZNK7toadlet7tadpole12HopComponent8getBoundEv,0,__ZN7toadlet7tadpole12HopComponent7destroyEv,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreatorD0Ev,0,__ZThn12_N7toadlet7tadpole11TGAStreamer6retainEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole5SceneENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet7tadpole12HopComponent6retainEv,0,__ZN7toadlet3egg17PosixErrorHandler12errorHandledEv,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreatorD1Ev,0,__ZN7toadlet7tadpole15ResourceManager12findFromFileERKNS_3egg6StringEPNS0_12ResourceDataE,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation7getNameEv,0,__ZThn48_N7toadlet6peeper11GLES2BufferD1Ev,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimation8setScopeEi,0,__ZN7toadlet7tadpole12HopComponent11frameUpdateEii,0,__ZThn36_N7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper12GLES2Texture11resetCreateEv,0,__ZThn12_N7toadlet7tadpole10HopManager22createPhysicsComponentEv,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper17GLES2RenderDevice13getRenderCapsERNS0_10RenderCapsE,0,__ZN7toadlet7tadpole11BMPStreamerD0Ev,0,__ZThn36_N7toadlet6peeper19BackableRenderState7releaseEv,0,__ZThn12_N7toadlet7tadpole11TGAStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole5SceneEFPNS3_6EngineEvES6_PS4_JEE6invokeERKS8_S9_,0,__ZN7toadlet7tadpole8material18TextureSetVariable9getFormatEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole12HopComponentENS2_9ComponentEE11getFullNameEv,0,__ZN7toadlet6peeper10VertexData6retainEv,0,__ZNK7toadlet7tadpole17SkeletonComponent14getRenderBoundEv,0,__ZN7toadlet7tadpole12HopComponent16transformChangedEPNS0_9TransformE,0,__ZN7toadlet6peeper17GLES2RenderDevice6retainEv,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState18getRootShaderStateEv,0,__ZNK7toadlet7tadpole8Skeleton15getUniqueHandleEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer6retainEv,0,__ZThn36_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole13PartitionNodeENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget7isValidEv,0,__ZThn12_N7toadlet6peeper11GLES2BufferD1Ev,0,__ZThn12_N7toadlet6peeper14BackableBuffer7releaseEv,0,__ZN7toadlet6peeper14BackableShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole11BMPStreamerD0Ev,0,__ZN7toadlet7tadpole8Skeleton4BoneD1Ev,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat6retainEv,0,__ZN7toadlet7tadpole4Node6removeEPNS0_9ComponentE,0,__ZN7toadlet7tadpole8material22CameraPositionVariable9getFormatEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice17createIndexBufferEv,0,__ZThn12_N7toadlet7tadpole5Track7destroyEv,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer16getTextureFormatEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole15CameraComponentENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet7tadpole5TrackD0Ev,0,__ZN7toadlet7tadpole11WaveDecoder4seekEi,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer7getSizeEv,0,__ZNK7toadlet6peeper17GLES2RenderTarget7getNameEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole12HopComponent8getScopeEv,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer11getDataSizeEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder8positionEv,0,__ZThn36_N7toadlet7tadpole5TrackD0Ev,0,__ZThn40_N7toadlet6peeper11GLES2Buffer4lockEi,0,__ZThn40_NK7toadlet7tadpole13MeshComponent13getWorldBoundEv,0,__ZN7toadlet7tadpole8material23MaterialDiffuseVariable6updateEPhPNS1_15SceneParametersE,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEFvNS2_3egg16IntrusivePointerINS3_9TransformENS5_25DefaultIntrusiveSemanticsEEEEvPS4_JS9_EE6invokeERKSB_SC_PS9_,0,__ZThn12_N7toadlet7tadpole11WaveDecoder5flushEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole15MaterialManagerD0Ev,0,__ZN7toadlet7tadpole11WaveDecoder8positionEv,0,__ZThn12_N7toadlet7tadpole8Skeleton7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper18GLES2SLShaderState7destroyEv,0,__ZN7toadlet7tadpole13PartitionNode7releaseEv,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreator6retainEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole4MeshENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZThn12_N7toadlet7tadpole5Scene15preContextResetEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet6peeper19BackableShaderState7destroyEv,0,__ZN7toadlet7tadpole13BaseComponentD0Ev,0,__ZN7toadlet7tadpole6sensor6Sensor5senseEv,0,__ZN7toadlet7tadpole13MeshComponent22getAttachmentTransformEPNS0_9TransformEi,0,__ZThn40_N7toadlet7tadpole12HopComponent16transformChangedEPNS0_9TransformE,0,__ZN7toadlet7tadpole4Node13actionRemovedEPNS0_15ActionComponentE,0,__ZN7toadlet7tadpole13PartitionNodeD0Ev,0,__ZThn36_NK7toadlet6peeper22EGL2WindowRenderTarget7isValidEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreatorD1Ev,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget7destroyEv,0,__ZThn12_N7toadlet7tadpole8Skeleton24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet7tadpole12HopComponent10getGravityEv,0,__ZThn12_N7toadlet6peeper12GLES2Texture7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice6retainEv,0,__ZN7toadlet7tadpole5Bound7releaseEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState7releaseEv,0,__ZThn12_N7toadlet7tadpole4Node7destroyEv,0,__ZN7toadlet6peeper16GLES2RenderState6retainEv,0,__ZN7toadlet7tadpole13MeshComponent7SubMeshD0Ev,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerIS3_NS2_13MeshComponentEEEPT0_PT_,0,__ZThn36_N7toadlet6peeper14BackableBuffer7releaseEv,0,__ZThn12_N7toadlet3egg12BaseResource7destroyEv,0,__ZN7toadlet7tadpole20WaterMaterialCreatorD1Ev,0,__ZN7toadlet6peeper18GLES2SLShaderState6createEv,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreator6retainEv,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet7tadpole15CameraComponent13getWorldBoundEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer12resetDestroyEv,0,__ZThn12_NK7toadlet7tadpole13MeshComponent7SubMesh6renderEPNS0_13RenderManagerE,0,__ZN7toadlet6peeper19BackableRenderState16setGeometryStateERKNS0_13GeometryStateE,0,__ZNK7toadlet7tadpole17SkeletonComponent18getRenderTransformEv,0,__ZThn36_NK7toadlet6peeper15BackableTexture8getUsageEv,0,__ZN7toadlet6peeper17GLES2RenderDevice9setBufferENS0_6Shader10ShaderTypeEiPNS0_14VariableBufferE,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget14getAdaptorInfoEv,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer16getTextureFormatEv,0,__ZNK7toadlet7tadpole5Track8getUsageEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer11resetCreateEv,0,__ZThn36_NK7toadlet6peeper13GLES2SLShader15getUniqueHandleEv,0,__ZN7toadlet3egg14PointerCounterD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_6peeper12RenderDeviceENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet7tadpole10HopManager18SolidSensorResultsD1Ev,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat19getRootVertexFormatEv,0,__ZNK7toadlet6peeper12GLES2Texture8getUsageEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer4lockEi,0,__ZThn40_N7toadlet6peeper14BackableBuffer6updateEPhii,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensorD0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15copyPixelBufferEPNS0_11PixelBufferES3_,0,__ZThn12_N7toadlet6peeper15BackableTextureD1Ev,0,__ZN7toadlet6peeper20GLES2FBORenderTargetD0Ev,0,__ZN7toadlet3egg2io10DataStream8readableEv,0,__ZNK7toadlet6peeper19BackableRenderState7getNameEv,0,__ZThn12_NK7toadlet6peeper20GLES2FBORenderTarget7getNameEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget7destroyEv,0,__ZThn16_N7toadlet7tadpole10HopManagerD1Ev,0,__ZNK7toadlet7tadpole6action13BaseAnimation8getValueEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole15GridMeshCreator7releaseEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet6peeper8ViewportEiE7setWireIS4_EEvRKMS4_iRT_i,0,__ZNK7toadlet7tadpole4Node8getLightEi,0,__ZThn40_N7toadlet6peeper14BackableBufferD1Ev,0,__ZThn12_N7toadlet6peeper14BackableShaderD0Ev,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer7releaseEv,0,__ZThn12_N7toadlet7tadpole15ResourceManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZThn44_N7toadlet7tadpole12HopComponentD1Ev,0,__ZThn36_N7toadlet6peeper15BackableTexture11resetCreateEv,0,__ZN7toadlet6peeper17GLES2VertexFormatD0Ev,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer12resetDestroyEv,0,__ZN7toadlet6peeper18GLES2SLShaderStateD1Ev,0,__ZThn36_N7toadlet7tadpole13PartitionNodeD0Ev,0,__ZN7toadlet7tadpole8material15AmbientVariable9getFormatEv,0,__ZN7toadlet7tadpole17SkeletonComponent16getNumAnimationsEv,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreator6retainEv,0,__ZThn12_N7toadlet7tadpole14TextureManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZThn12_N7toadlet6peeper15BackableTexture6retainEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9TransformEFvRKNS2_3egg4math7Vector3EEvPS4_JS9_EE6invokeERKSB_SC_PS7_,0,__ZN7toadlet7tadpole4Node17gatherRenderablesEPNS0_6CameraEPNS0_13RenderableSetE,0,__ZThn36_N7toadlet7tadpole12HopComponent11setPositionERKNS_3egg4math7Vector3E,0,__ZThn12_NK7toadlet7tadpole13MeshComponent7SubMesh14getRenderBoundEv,0,__ZN7toadlet7tadpole4Node12setTranslateERKNS_3egg4math7Vector3E,0,__ZThn16_N7toadlet7tadpole10HopManager17collisionResponseEPNS_3hop5SolidERNS_3egg4math7Vector3ES8_RNS2_9CollisionE,0,__ZN7toadlet6peeper9IndexDataD1Ev,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer14castToGLBufferEv,0,__ZN7toadlet7tadpole11TGAStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi,0,__ZN7toadlet3egg2io10DataStream5flushEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer7destroyEv,0,__ZThn36_NK7toadlet6peeper18GLES2SLShaderState15getUniqueHandleEv,0,__ZN7toadlet6peeper19BackableShaderState6createEv,0,__ZN7toadlet6peeper20VariableBufferFormatD0Ev,0,__ZThn12_N7toadlet7tadpole15CameraComponentD0Ev,0,__ZThn44_NK7toadlet7tadpole17SkeletonComponent6renderEPNS0_13RenderManagerE,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget7releaseEv,0,__ZNK7toadlet6peeper19BackableRenderState16getGeometryStateERNS0_13GeometryStateE,0,__ZThn36_N7toadlet6peeper19BackableRenderState6createEv,0,__ZThn12_N7toadlet7tadpole15GridMeshCreatorD1Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet6peeper12RenderDeviceEEEvPT_,0,__ZNK7toadlet7tadpole6action13BaseAnimation11getMinValueEv,0,__ZN7toadlet6peeper17GLES2VertexFormat6createEv,0,__ZThn12_N7toadlet7tadpole4Node7releaseEv,0,__ZThn12_NK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBufferD1Ev,0,__ZNK7toadlet6peeper11GLES2Buffer9getAccessEv,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerIS3_NS2_13BaseComponentEEEPT0_PT_,0,__ZThn36_N7toadlet6peeper16GLES2RenderState15setSamplerStateENS0_6Shader10ShaderTypeEiRKNS0_12SamplerStateE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole5SceneENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZNK7toadlet7tadpole8Sequence15getUniqueHandleEv,0,__ZN7toadlet6peeper19BackableShaderState7releaseEv,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget7getNameEv,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreator7destroyEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget6attachENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEENS0_23PixelBufferRenderTarget10AttachmentE,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole6EngineENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole13BaseComponent19notifyParentRemovedEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet7tadpole11WaveDecoder5writeEPKhi,0,__ZN7toadlet7tadpole12HopComponent13componentThisEv,0,__ZN7toadlet7tadpole8material19FogDistanceVariable9getFormatEv,0,__ZN7toadlet3egg2io12MemoryStream5closeEv,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat13getVertexSizeEv,0,__ZThn40_N7toadlet7tadpole17SkeletonComponent16transformChangedEPNS0_9TransformE,0,__ZN7toadlet7tadpole18SkyDomeMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_NK7toadlet7tadpole15CameraComponent17getWorldTransformEv,0,__ZN7toadlet7tadpole15ResourceManager15logAllResourcesEv,0,__ZN7toadlet7tadpole4Mesh20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat7releaseEv,0,__ZThn12_N7toadlet3egg2io10FileStream5closeEv,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole9TransformEJEEENS1_3egg16IntrusivePointerIT_NS4_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet6peeper18GLES2SLShaderState12resourceThisEv,0,__ZN7toadlet7tadpole4Node15spacialAttachedEPNS0_7SpacialE,0,__ZN7toadlet7tadpole5Scene7destroyEPNS0_9ComponentE,0,__ZNK7toadlet6peeper16GLES2RenderState16getGeometryStateERNS0_13GeometryStateE,0,__ZThn48_N7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget9getHeightEv,0,__ZN7toadlet6peeper19BackableShaderState24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole15MaterialManagerD0Ev,0,__ZN7toadlet6peeper15BackableTexture11resetCreateEv,0,__ZN7toadlet7tadpole8Skeleton24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper14BackableBuffer6createEiiNS0_11IndexBuffer11IndexFormatEi,0,__ZN7toadlet6peeper15BackableTexture6retainEv,0,__ZThn16_N7toadlet7tadpole10HopManager9preUpdateEPNS_3hop5SolidEif,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimationD1Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg4math10QuaternionEEEvPT_,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation8getScopeEv,0,__ZN7toadlet7tadpole4Node20notifyParentAttachedEv,0,__ZN7toadlet7tadpole8material8Material7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole9TransformD0Ev,0,__ZN7toadlet6peeper17GLES2RenderDeviceD0Ev,0,__ZN7toadlet7tadpole5Track6retainEv,0,__ZN7toadlet7tadpole15ResourceManager11getStreamerERKNS_3egg6StringE,0,__ZNK7toadlet7tadpole13MeshComponent7getTypeEv,0,__ZN7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer12resourceThisEv,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer7destroyEv,0,__ZN7toadlet7tadpole15MaterialManager6retainEv,0,__ZThn44_NK7toadlet7tadpole17SkeletonComponent14getRenderBoundEv,0,__ZThn48_N7toadlet6peeper11GLES2Buffer11resetCreateEv,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerINS2_13BaseComponentES3_EEPT0_PT_,0,__ZN7toadlet7tadpole8material20NormalMatrixVariable9getFormatEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer14castToGLBufferEv,0,__ZN7toadlet6peeper14BackableBuffer21getRootVariableBufferEv,0,__ZN7toadlet6peeper17GLES2VertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole13BaseComponent11frameUpdateEii,0,__ZN7toadlet6peeper16GLES2RenderState16setMaterialStateERKNS0_13MaterialStateE,0,__ZN7toadlet7tadpole8material26MaterialTrackColorVariableD0Ev,0,__ZThn12_N7toadlet6peeper22EGL2WindowRenderTargetD1Ev,0,__ZN7toadlet7tadpole17SkeletonComponent18getAttachmentIndexERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper14BackableShaderD0Ev,0,__ZN7toadlet7tadpole17SphereMeshCreatorD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg4math7Vector2EEEvPT_,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer6unlockEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,_new_GLES2RenderDevice,0,__ZNK7toadlet7tadpole12HopComponent9getActiveEv,0,__ZN7toadlet7tadpole15ResourceManager21removeResourceArchiveEPNS_3egg2io7ArchiveE,0,__ZN7toadlet7tadpole4Mesh7SubMesh20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat18getElementSemanticEi,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer29castToGLTextureMipPixelBufferEv,0,__ZN10__cxxabiv116__shim_type_infoD2Ev,0,__ZThn12_N7toadlet7tadpole11FontManagerD1Ev,0,__ZN7toadlet7tadpole6EngineD2Ev,0,__ZN7toadlet7tadpole6sensor6SensorD2Ev,0,__ZThn12_N7toadlet7tadpole12WaveStreamer17createAudioStreamEPNS_3egg2io6StreamEPNS0_12ResourceDataE,0,__ZThn12_NK7toadlet6peeper14BackableShader15getUniqueHandleEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer6unlockEv,0,__ZThn12_N7toadlet7tadpole5Track7setNameERKNS_3egg6StringE,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole12HopComponentENS2_9ComponentEE11newInstanceEv,0,__ZThn36_N7toadlet6peeper16GLES2RenderState13setDepthStateERKNS0_10DepthStateE,0,__ZThn36_N7toadlet7tadpole4Node16transformChangedEPNS0_9TransformE,0,__ZThn12_N7toadlet7tadpole5Track6retainEv,0,__ZN7toadlet7tadpole8material23MaterialDiffuseVariable9getFormatEv,0,__ZN7toadlet7tadpole6sensor13SensorResults13sensingEndingEv,0,__ZNK7toadlet7tadpole4Mesh7SubMesh7getNameEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer6unlockEv,0,__ZN7toadlet3egg2io10FileStreamD0Ev,0,__ZThn36_N7toadlet6peeper13GLES2SLShader7destroyEv,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat19getRootVertexFormatEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimation8setValueEf,0,__ZNK7toadlet7tadpole4Node17getWorldTransformEv,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreator6retainEv,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat7destroyEv,0,__ZThn36_N7toadlet7tadpole12HopComponent11rootChangedEPNS0_4NodeE,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer8getUsageEv,0,__ZN7toadlet7tadpole15ResourceManager18getDefaultStreamerEv,0,__ZThn12_NK7toadlet7tadpole13MeshComponent7SubMesh18getRenderTransformEv,0,__ZN7toadlet7tadpole14TextureManager6retainEv,0,__ZN7toadlet3egg3Log10initializeEb,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensor7releaseEv,0,__ZN7toadlet7tadpole4Node16nodeBoundChangedEPS1_,0,__ZThn12_N7toadlet3egg2io10DataStream5resetEv,0,__ZNK7toadlet7tadpole4Node7getTypeEv,0,__ZThn12_N7toadlet7tadpole11TGAStreamerD1Ev,0,__ZN7toadlet6peeper14BackableBuffer18getRootPixelBufferEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget12resetDestroyEv,0,__ZThn12_N7toadlet7tadpole13ShaderManagerD1Ev,0,__ZN7toadlet7tadpole5Track7releaseEv,0,__ZN7toadlet6peeper19BackableRenderState7setBackENS_3egg16IntrusivePointerINS0_11RenderStateENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper17GLES2RenderDevice29createPixelBufferRenderTargetEv,0,__ZN7toadlet7tadpole15ResourceManager19getDefaultExtensionEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector3EfE7setWireIS5_EEvRKMS5_fRT_f,0,__ZN7toadlet6peeper14BackableBuffer12resetDestroyEv,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat16getElementFormatEi,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget7destroyEv,0,__ZThn12_N7toadlet6peeper19BackableRenderState24internal_setUniqueHandleEi,0,__ZNK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer7releaseEv,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager7releaseEv,0,__ZThn12_N7toadlet7tadpole4Node6retainEv,0,__ZN7toadlet7tadpole12HopComponentD1Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderState6createEv,0,__ZN7toadlet7tadpole6Engine21createDiffuseMaterialEPNS_6peeper7TextureEPNS2_11RenderStateE,0,__ZThn12_N7toadlet3egg2io10DataStream8positionEv,0,__ZN7toadlet7tadpole6Engine14createGridMeshEffiiPNS0_8material8MaterialE,0,__ZN7toadlet3egg5Error11nullPointerERKNS0_6StringE,0,__ZN7toadlet7tadpole10HopManager8testNodeERNS0_16PhysicsCollisionEPNS0_4NodeERKNS_3egg4math7SegmentES5_,0,__ZThn48_NK7toadlet6peeper14BackableBuffer11getDataSizeEv,0,__ZNK7toadlet7tadpole10HopManager10getGravityEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState23getVariableBufferFormatENS0_6Shader10ShaderTypeEi,0,__ZN7toadlet7tadpole16TorusMeshCreator6retainEv,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv,0,__ZN7toadlet7tadpole13BaseComponent13componentThisEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer4lockEi,0,__ZN7toadlet7tadpole15CameraComponent9setLookAtERKNS_3egg4math7Vector3ES6_S6_,0,__ZThn12_N7toadlet7tadpole11WaveDecoder6lengthEv,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole4NodeENS2_9ComponentEE11newInstanceEv,0,__ZThn44_N7toadlet6peeper11GLES2BufferD0Ev,0,__ZThn12_N7toadlet7tadpole14TextureManagerD0Ev,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole4MeshEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet6peeper20GLES2FBORenderTarget30getRootPixelBufferRenderTargetEv,0,__ZN7toadlet7tadpole13RenderableSet15queueRenderableEPNS0_10RenderableE,0,__ZNK7toadlet7tadpole12HopComponent19getCollideWithScopeEv,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget7isValidEv,0,__ZNK7toadlet7tadpole5Scene17getUpdateListenerEv,0,__ZN7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper16GLES2RenderState13setBlendStateERKNS0_10BlendStateE,0,__ZThn12_N7toadlet7tadpole4Mesh20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEE9incrementEv,0,__ZThn36_N7toadlet7tadpole12HopComponent11setVelocityERKNS_3egg4math7Vector3E,0,__ZN7toadlet6peeper18GLES2SLShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2RenderTargetD0Ev,0,__ZNK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZN7toadlet6peeper15BackableTexture14getRootTextureEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole8SequenceD0Ev,0,__ZThn12_N7toadlet7tadpole13BaseComponentD0Ev,0,__ZThn36_NK7toadlet6peeper12GLES2Texture7getNameEv,0,__ZThn44_NK7toadlet6peeper14BackableBuffer8getUsageEv,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat11findElementERKNS_3egg6StringE,0,__ZNK10__cxxabiv121__vmi_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib,0,__ZThn12_N7toadlet7tadpole10HopManagerD0Ev,0,__ZNK7toadlet7tadpole5Scene12getLogicTimeEv,0,__ZThn36_NK7toadlet7tadpole12HopComponent7getNameEv,0,__ZN7toadlet7tadpole6Camera20updateWorldTransformEv,0,__ZN7toadlet3egg2io10FileStream9writeableEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget19getRootRenderTargetEv,0,__ZThn36_N7toadlet6peeper14BackableShader7releaseEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice18createVertexBufferEv,0,__ZN7toadlet6peeper13GLES2SLShader13getRootShaderEv,0,__ZThn40_NK7toadlet7tadpole13MeshComponent12getTransformEv,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer7getSizeEv,0,__ZN7toadlet7tadpole11BMPStreamer7releaseEv,0,__ZN7toadlet7tadpole15ResourceManagerD0Ev,0,__ZN7toadlet6peeper11GLES2Buffer19getRootVertexBufferEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper19BackableRenderState13setDepthStateERKNS0_10DepthStateE,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTargetD0Ev,0,__ZThn12_N7toadlet7tadpole15ResourceManagerD1Ev,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation11getMaxValueEv,0,__ZThn36_NK7toadlet6peeper15BackableTexture9getFormatEv,0,__ZThn16_N7toadlet7tadpole10HopManager10postUpdateEPNS_3hop5SolidEif,0,__ZN7toadlet7tadpole13RenderableSetD0Ev,0,__ZN7toadlet7tadpole5Scene16postContextResetEPNS_6peeper12RenderDeviceE,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTargetD1Ev,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat7getNameEv,0,__ZN7toadlet7tadpole17SkeletonComponent12getAnimationEi,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole4NodeD0Ev,0,__ZThn36_N7toadlet7tadpole5Track6updateEPhii,0,__ZThn12_N7toadlet7tadpole8Sequence20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole12HopComponent11handleEventEPNS_3egg5EventE,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMesh7releaseEv,0,__ZN7toadlet7tadpole5Scene10setEpsilonEf,0,__ZThn12_NK7toadlet6peeper26GLES2TextureMipPixelBuffer15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensor5senseEv,0,__ZThn12_N7toadlet7tadpole13PartitionNode6retainEv,0,__ZN7toadlet6peeper12GLES2Texture12resourceThisEv,0,__ZNK7toadlet7tadpole5Scene17getPhysicsManagerEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget6removeENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet7tadpole6sensor13SensorResults13sensingEndingEv,0,__ZN7toadlet7tadpole8SkeletonD0Ev,0,__ZThn12_N7toadlet3egg2io10FileStream5flushEv,0,__ZNK7toadlet6peeper19BackableShaderState7getNameEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer11resetCreateEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState7destroyEv,0,__ZN7toadlet3egg7ErrorerD2Ev,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole4NodeENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn36_N7toadlet7tadpole12HopComponent7setMassEf,0,__ZN7toadlet7tadpole5Track7destroyEv,0,__ZThn36_N7toadlet6peeper22EGL2WindowRenderTarget7destroyEv,0,__ZThn36_NK7toadlet6peeper17GLES2RenderTarget9isPrimaryEv,0,__ZThn12_N7toadlet7tadpole13BufferManagerD0Ev,0,__ZN7toadlet7tadpole4Node8setScopeEi,0,__ZThn44_N7toadlet6peeper11GLES2Buffer18getRootPixelBufferEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole6EngineENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet7tadpole5Scene16setRenderManagerEPNS0_13RenderManagerE,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper13GLES2SLShader7setNameERKNS_3egg6StringE,0,__ZN7toadlet3hop5ShapeD1Ev,0,__ZNK7toadlet6peeper20BackableVertexFormat14getNumElementsEv,0,__ZN7toadlet7tadpole8material14ScalarVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget6retainEv,0,__ZThn12_N7toadlet3egg2io10DataStreamD1Ev,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer7getNameEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole9ComponentENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn12_NK7toadlet7tadpole4Mesh7SubMesh15getUniqueHandleEv,0,__ZN7toadlet7tadpole5Scene11logicUpdateEiiPNS0_4NodeE,0,__ZThn12_N7toadlet6peeper12GLES2Texture24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet3egg2io10DataStream4seekEi,0,__ZN7toadlet7tadpole10HopManager22createPhysicsComponentEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget19getRootRenderTargetEv,0,__ZN7toadlet7tadpole8material21MaterialLightVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn48_N7toadlet7tadpole12HopComponentD0Ev,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZN7toadlet7tadpole8material24MaterialSpecularVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn12_N7toadlet7tadpole12WaveStreamer6retainEv,0,__ZThn12_NK7toadlet6peeper17GLES2VertexFormat15getUniqueHandleEv,0,__ZN7toadlet7tadpole8material16FogColorVariableD1Ev,0,__ZN7toadlet6peeper13GLES2SLShader20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet6peeper11GLES2Buffer11getDataSizeEv,0,__ZThn36_NK7toadlet6peeper18GLES2SLShaderState7getNameEv,0,__ZN7toadlet6peeper14BackableShader7getBackEv,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget9isPrimaryEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole17SkeletonComponentENS2_9ComponentEE11getFullNameEv,0,__ZN7toadlet3egg2io10DataStream6lengthEv,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat7releaseEv,0,__ZThn36_N7toadlet6peeper22EGL2WindowRenderTargetD0Ev,0,__ZN7toadlet7tadpole12HopComponent19setCollideWithScopeEi,0,__ZThn36_NK7toadlet7tadpole5Track9getAccessEv,0,__ZThn40_NK7toadlet7tadpole17SkeletonComponent17getWorldTransformEv,0,__ZThn36_N7toadlet6peeper16GLES2RenderState16setGeometryStateERKNS0_13GeometryStateE,0,__ZN7toadlet7tadpole13BaseComponent11handleEventEPNS_3egg5EventE,0,__ZNK7toadlet6peeper16GLES2RenderState13getDepthStateERNS0_10DepthStateE,0,__ZN7toadlet7tadpole15ResourceManager14resourceLoadedERKNS_3egg16IntrusivePointerINS2_8ResourceENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn44_N7toadlet6peeper14BackableBuffer7releaseEv,0,__ZN7toadlet7tadpole14TextureManager7releaseEv,0,__ZThn12_N7toadlet6peeper14BackableShader7destroyEv,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget7getNameEv,0,__ZThn12_NK7toadlet6peeper16GLES2RenderState7getNameEv,0,__ZThn36_N7toadlet6peeper15BackableTexture6createEiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEEPPh,0,__ZN7toadlet7tadpole8material19ModelMatrixVariable9getFormatEv,0,__ZThn12_N7toadlet7tadpole13MeshComponent7destroyEv,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState15getUniqueHandleEv,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole13BaseComponent11logicUpdateEii,0,__ZThn12_N7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole19SimpleRenderManager28renderDepthSortedRenderablesERKNS_3egg10CollectionINS0_13RenderableSet19RenderableQueueItemEEEb,0,__ZN7toadlet7tadpole8Skeleton4Bone7releaseEv,0,__ZN7toadlet6peeper16GLES2RenderState12resourceThisEv,0,__ZN7toadlet6peeper14BackableBuffer7releaseEv,0,__ZN7toadlet7tadpole12AudioManager6retainEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEFbPNS3_9ComponentEEbPS4_JS6_EE6invokeERKS8_S9_S6_,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_6peeper12RenderTargetENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet7tadpole5Scene15postFrameUpdateEi,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget7releaseEv,0,__ZN7toadlet7tadpole19SimpleRenderManager11renderSceneEPNS_6peeper12RenderDeviceEPNS0_4NodeEPNS0_6CameraE,0,__ZN7toadlet7tadpole16AABoxMeshCreator7releaseEv,0,__ZN7toadlet6peeper16GLES2RenderState15setSamplerStateENS0_6Shader10ShaderTypeEiRKNS0_12SamplerStateE,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget9getHeightEv,0,__ZN7toadlet3egg17PosixErrorHandler16uninstallHandlerEv,0,__ZThn48_N7toadlet7tadpole13MeshComponentD0Ev,0,__ZThn36_N7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper17GLES2RenderDevice6createEPNS0_12RenderTargetEi,0,__ZThn36_NK7toadlet6peeper15BackableTexture7getNameEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer7destroyEv,0,__ZN7toadlet7tadpole4Mesh7SubMeshD0Ev,0,__ZN7toadlet7tadpole5Scene15postLogicUpdateEi,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget9getHeightEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTargetD0Ev,0,__ZN7toadlet6peeper19BackableShaderState6retainEv,0,__ZN7toadlet6peeper15BackableTexture12resetDestroyEv,0,__ZN7toadlet7tadpole17SkeletonComponent17getNumAttachmentsEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent6retainEv,0,__ZNK7toadlet7tadpole4Node7getNodeERKNS_3egg6StringE,0,__ZN7toadlet3egg6Object7destroyEv,0,__ZN7toadlet7tadpole8Skeleton12resourceThisEv,0,__ZThn12_N7toadlet7tadpole12TMSHStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet3egg2io10DataStream5writeEPKhi,0,__ZN7toadlet6peeper17GLES2VertexFormat12resourceThisEv,0,__ZN7toadlet7tadpole8material15SceneParametersD1Ev,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat7releaseEv,0,__ZThn48_N7toadlet7tadpole13MeshComponent22getAttachmentTransformEPNS0_9TransformEi,0,__ZThn36_NK7toadlet6peeper22EGL2WindowRenderTarget9isPrimaryEv,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation6retainEv,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv,0,__ZThn48_N7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZThn12_N7toadlet7tadpole4Node11frameUpdateEii,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState7destroyEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6CameraEFvRKNS2_6peeper8ViewportEEvPS4_JS8_EE6invokeERKSA_SB_PS6_,0,__ZThn36_N7toadlet7tadpole12HopComponent8addForceERKNS_3egg4math7Vector3E,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice22getPrimaryRenderTargetEv,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat13getVertexSizeEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet6peeper8ViewportEbE7getWireIS4_EEbRKMS4_bRKT_,0,__ZThn36_N7toadlet7tadpole13MeshComponentD1Ev,0,__ZN10__cxxabiv123__fundamental_type_infoD0Ev,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget15getUniqueHandleEv,0,__ZNK7toadlet6peeper14BackableBuffer14getIndexFormatEv,0,__ZN7toadlet7tadpole4Node15visibleAttachedEPNS0_7VisibleE,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget8getWidthEv,0,__ZN10emscripten4baseIN7toadlet7tadpole9ComponentEE14convertPointerIS3_NS2_15CameraComponentEEEPT0_PT_,0,__ZNK7toadlet7tadpole11WaveDecoder14getAudioFormatEv,0,__ZThn52_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZThn12_N7toadlet6peeper11GLES2BufferD0Ev,0,__ZN7toadlet3hop5Solid6retainEv,0,__ZN7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole6sensor13SensorResultsD1Ev,0,__ZThn36_NK7toadlet6peeper19BackableShaderState15getUniqueHandleEv,0,__ZThn44_NK7toadlet6peeper14BackableBuffer7getSizeEv,0,__ZThn12_N7toadlet7tadpole10HopManager11logicUpdateEiiPNS0_4NodeE,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget14getAdaptorInfoEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_6peeper11RenderStateENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet6peeper17GLES2RenderDevice15setDefaultStateEv,0,__ZThn12_N7toadlet7tadpole12TMSHStreamer7releaseEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer4lockEi,0,__ZNK10__cxxabiv120__si_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib,0,__ZN7toadlet7tadpole8material20LightDiffuseVariableD0Ev,0,__ZN7toadlet7tadpole17SkeletonComponent22getAttachmentTransformEPNS0_9TransformEi,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreatorD0Ev,0,__ZNK7toadlet6peeper23EGL2PBufferRenderTarget8getWidthEv,0,__ZNK7toadlet7tadpole4Node14getNumVisiblesEv,0,__ZNK7toadlet6peeper17GLES2RenderTarget8getWidthEv,0,__ZN7toadlet6peeper20BackableVertexFormatD0Ev,0,__ZThn36_NK7toadlet6peeper12GLES2Texture9getFormatEv,0,__ZN7toadlet6peeper17GLES2RenderTarget25activateAdditionalContextEv,0,__ZN7toadlet7tadpole17SphereMeshCreator7releaseEv,0,__ZThn12_N7toadlet6peeper19BackableRenderStateD1Ev,0,__ZThn36_N7toadlet6peeper12GLES2Texture14getRootTextureEv,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget7destroyEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper17GLES2RenderDevice22getPrimaryRenderTargetEv,0,__ZN7toadlet7tadpole27NormalizationTextureCreatorD1Ev,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole4NodeENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole5TrackD1Ev,0,__ZThn36_N7toadlet6peeper14BackableBuffer11resetCreateEv,0,__ZThn36_N7toadlet7tadpole5Track4lockEi,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper11RenderStateENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet6peeper20GLES2FBORenderTarget6removeENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole5Track24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole13MeshComponent7SubMesh6renderEPNS0_13RenderManagerE,0,__ZN7toadlet3egg2io12MemoryStream7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice17createPixelBufferEv,0,__ZN7toadlet7tadpole4Node14calculateBoundEv,0,__ZN7toadlet7tadpole6Camera13setClearFlagsEi,0,__ZN7toadlet7tadpole14ArchiveManagerD0Ev,0,__ZNK7toadlet7tadpole17SkeletonComponent7getTypeEv,0,__ZThn12_N7toadlet6peeper19BackableRenderState6retainEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZThn44_N7toadlet6peeper11GLES2Buffer11resetCreateEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole12HopComponentENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer9getAccessEv,0,__ZN7toadlet3egg2io10FileStream8positionEv,0,__ZN7toadlet6peeper15BackableTexture17getMipPixelBufferEii,0,__ZThn36_N7toadlet7tadpole13PartitionNodeD1Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole8material8MaterialENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZThn12_N7toadlet6peeper14BackableShader7releaseEv,0,__ZN10emscripten11RangeAccessIN7toadlet3egg20PointerIteratorRangeINS1_7tadpole4NodeEEEE4prevERS6_,0,__ZN7toadlet7tadpole8material21TextureMatrixVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensor5senseEv,0,__ZN7toadlet7tadpole8material23MaterialAmbientVariable6updateEPhPNS1_15SceneParametersE,0,__ZNK7toadlet3egg12BaseResource15getUniqueHandleEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer7destroyEv,0,__ZN7toadlet7tadpole8Skeleton7releaseEv,0,__ZThn36_N7toadlet6peeper16GLES2RenderState7destroyEv,0,__ZN7toadlet7tadpole17SkeletonComponent6retainEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat15getElementIndexEi,0,__ZThn12_N7toadlet7tadpole10HopManager11frameUpdateEiiPNS0_4NodeE,0,__ZThn36_N7toadlet6peeper20BackableVertexFormatD0Ev,0,__ZN7toadlet7tadpole13AudioStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper11GLES2Buffer6updateEPhii,0,__ZNK7toadlet7tadpole17SkeletonComponent17getRenderMaterialEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole8material8MaterialENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS6_PNS_8internal7_EM_VALE,0,__ZNK7toadlet6peeper22EGL2WindowRenderTarget7isValidEv,0,__ZThn12_NK7toadlet6peeper16GLES2RenderState15getUniqueHandleEv,0,__ZN7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEED0Ev,0,__ZThn12_N7toadlet7tadpole12HopComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget25activateAdditionalContextEv,0,__ZN7toadlet7tadpole8material16FogColorVariable9getFormatEv,0,__ZN7toadlet3egg12BaseResource7setNameERKNS0_6StringE,0,__ZThn40_N7toadlet7tadpole13MeshComponentD0Ev,0,__ZThn36_N7toadlet7tadpole15CameraComponentD0Ev,0,__ZN7toadlet6peeper16GLES2RenderState7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8material19FogDistanceVariableD1Ev,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat6createEv,0,__ZN7toadlet7tadpole6CameraD0Ev,0,__ZN7toadlet6peeper17GLES2VertexFormat19getRootVertexFormatEv,0,__ZN7toadlet7tadpole8material19ModelMatrixVariableD1Ev,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensor6retainEv,0,__ZThn12_N7toadlet7tadpole5Scene20preContextDeactivateEPNS_6peeper12RenderDeviceE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice13createTextureEv,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreator6retainEv,0,__ZN7toadlet6peeper17GLES2RenderTarget5resetEv,0,__ZNSt9bad_allocD0Ev,0,__ZN7toadlet7tadpole13RenderableSet9queueNodeEPNS0_4NodeE,0,__ZN7toadlet7tadpole12HopComponent10traceSolidERNS_3hop9CollisionEPNS2_5SolidERKNS_3egg4math7Vector3ERKNS8_7SegmentE,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat16getElementOffsetEi,0,__ZThn12_N7toadlet6peeper13GLES2SLShader7destroyEv,0,__ZNK7toadlet6peeper14BackableBuffer11getDataSizeEv,0,__ZThn12_N7toadlet7tadpole14ArchiveManagerD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet3egg2io10DataStream9writeableEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper22EGL2WindowRenderTarget19getRootRenderTargetEv,0,__ZThn12_NK7toadlet7tadpole8Skeleton15getUniqueHandleEv,0,__ZN7toadlet7tadpole12HopComponent12setTraceableEPNS0_16PhysicsTraceableE,0,__ZThn36_N7toadlet6peeper14BackableBufferD1Ev,0,__ZN7toadlet6peeper17GLES2RenderTarget27deactivateAdditionalContextEv,0,__ZN7toadlet7tadpole5Scene7getRootEv,0,__ZThn12_N7toadlet6peeper19BackableRenderState7releaseEv,0,__ZN7toadlet7tadpole12AudioManager7releaseEv,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget6retainEv,0,__ZN7toadlet7tadpole8material21MaterialLightVariableD1Ev,0,__ZN7toadlet6peeper19BackableRenderState18getRootRenderStateEv,0,__ZN7toadlet3egg18BaseLoggerListener5flushEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole8material21LightPositionVariableD1Ev,0,__ZThn12_N7toadlet7tadpole12WaveStreamerD0Ev,0,__ZN7toadlet7tadpole4Mesh8setBoundENS_3egg16IntrusivePointerINS0_5BoundENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg4math7Vector3EEEvPT_,0,__ZThn12_N7toadlet6peeper15BackableTexture7releaseEv,0,__ZN7toadlet7tadpole17SkyBoxMeshCreator7releaseEv,0,__ZThn36_N7toadlet7tadpole5Track6retainEv,0,__ZN7toadlet7tadpole5Scene17setUpdateListenerEPNS0_14UpdateListenerE,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole13BaseComponentENS2_9ComponentEED1Ev,0,__ZThn36_N7toadlet7tadpole15CameraComponent16transformChangedEPNS0_9TransformE,0,__ZThn12_NK7toadlet7tadpole8material8Material7getNameEv,0,__ZN7toadlet3egg12BaseResource12resourceThisEv,0,__ZN7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEE9decrementEv,0,__ZN7toadlet7tadpole8material10RenderPathD1Ev,0,__ZN7toadlet7tadpole4Node12setTranslateEfff,0,__ZN7toadlet6peeper17GLES2VertexFormat11findElementERKNS_3egg6StringE,0,__ZN7toadlet7tadpole8material21TextureMatrixVariable6linkedEPNS1_10RenderPassE,0,__ZThn48_N7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget7releaseEv,0,__ZN7toadlet7tadpole13ShaderManager7releaseEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState6retainEv,0,__ZN7toadlet6peeper16GLES2RenderState18setRasterizerStateERKNS0_15RasterizerStateE,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat7releaseEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget8activateEv,0,__ZN7toadlet3egg3Log7warningERKNS0_6StringE,0,__ZThn12_N7toadlet7tadpole10HopManager26setTraceableCollisionScopeEi,0,__ZThn40_N7toadlet7tadpole12HopComponentD1Ev,0,__ZThn40_N7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole13MeshComponent11frameUpdateEii,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation11getMaxValueEv,0,__ZThn36_N7toadlet6peeper11GLES2Buffer6createEiiNS0_11IndexBuffer11IndexFormatEi,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget9getHeightEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13MeshComponentENS2_9ComponentEE11newInstanceEv,0,__ZThn12_N7toadlet6peeper22EGL2WindowRenderTarget7destroyEv,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget9isPrimaryEv,0,__ZN7toadlet3hop9SimulatorD2Ev,0,__ZThn48_N7toadlet6peeper14BackableBuffer4lockEi,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer9getAccessEv,0,__ZN7toadlet7tadpole10HopManager10postUpdateEPNS_3hop5SolidEif,0,__ZN7toadlet7tadpole5Scene7setRootEPNS0_13PartitionNodeE,0,__ZThn36_NK7toadlet6peeper13GLES2SLShader10getProfileEv,0,__ZN7toadlet7tadpole14TextureManager7destroyEv,0,__ZNK7toadlet6peeper17GLES2VertexFormat14getElementNameEi,0,__ZThn12_N7toadlet7tadpole5Scene21postContextDeactivateEPNS_6peeper12RenderDeviceE,0,__ZNK7toadlet7tadpole5Scene14getExcessiveDTEv,0,__ZN7toadlet7tadpole10HopManager12setTraceableEPNS0_16PhysicsTraceableE,0,__ZThn12_NK7toadlet7tadpole17SkeletonComponent17SequenceAnimation11getMinValueEv,0,__ZThn12_N7toadlet6peeper20BackableVertexFormatD0Ev,0,__ZNK7toadlet7tadpole13BaseComponent9getActiveEv,0,__ZN7toadlet7tadpole4Node11rootChangedEPS1_,0,__ZN7toadlet7tadpole16TorusMeshCreator7releaseEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15setAmbientColorERKNS_3egg4math7Vector4E,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer7getSizeEv,0,__ZN7toadlet7tadpole13PartitionNode19findAmbientForBoundERNS_3egg4math7Vector4EPNS0_5BoundE,0,__ZN7toadlet7tadpole5Scene18preContextActivateEPNS_6peeper12RenderDeviceE,0,__ZN7toadlet3egg12BaseResource24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn36_N7toadlet7tadpole12HopComponent23removeCollisionListenerEPNS0_24PhysicsCollisionListenerE,0,__ZN7toadlet6peeper19GLES2SLVertexLayoutD0Ev,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer7getSizeEv,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget7getNameEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet7tadpole16TorusMeshCreatorD0Ev,0,__ZNK7toadlet7tadpole13BaseComponent9getParentEv,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreator6retainEv,0,__ZThn12_N7toadlet7tadpole11DDSStreamerD1Ev,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole10HopManager10setGravityERKNS_3egg4math7Vector3E,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper19IndexBufferAccessorD0Ev,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer7destroyEv,0,__ZNK10__cxxabiv116__shim_type_info5noop2Ev,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreatorD1Ev,0,__ZNK7toadlet6peeper17GLES2RenderTarget7isValidEv,0,__ZN7toadlet7tadpole14ArchiveManager8unmanageEPNS_3egg8ResourceE,0,__ZN7toadlet7tadpole4Node12setTransformENS_3egg16IntrusivePointerINS0_9TransformENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole18SkyDomeMeshCreator7releaseEv,0,__ZN7toadlet7tadpole13MeshComponentD1Ev,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderStateD1Ev,0,__ZN7toadlet3egg6Object7releaseEv,0,__ZNK7toadlet7tadpole5Track7getNameEv,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreator7releaseEv,0,__ZN10emscripten8internal7InvokerIPN7toadlet6peeper12RenderDeviceEJEE6invokeEPFS5_vE,0,__ZNK7toadlet6peeper13GLES2SLShader7getNameEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer6updateEPhii,0,__ZN7toadlet7tadpole10HopManager26setTraceableCollisionScopeEi,0,__ZThn44_NK7toadlet7tadpole17SkeletonComponent18getRenderTransformEv,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_3hop9SimulatorENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole8material8MaterialENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZN7toadlet7tadpole13MeshComponent7destroyEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEFviEvPS4_JiEE6invokeERKS6_S7_i,0,__ZN7toadlet7tadpole12HopComponent13parentChangedEPNS0_4NodeE,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreatorD1Ev,0,__ZN7toadlet6peeper22EGL2WindowRenderTarget27deactivateAdditionalContextEv,0,__ZN7toadlet7tadpole11WaveDecoderD0Ev,0,__ZN7toadlet6peeper17GLES2RenderDevice15setLightEnabledEib,0,__ZThn12_N7toadlet3egg12BaseResourceD0Ev,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer11resetCreateEv,0,__ZN7toadlet7tadpole8material24MaterialSpecularVariableD0Ev,0,__ZThn36_NK7toadlet6peeper19BackableRenderState15getUniqueHandleEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState7releaseEv,0,__ZN7toadlet7tadpole8material18FogDensityVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn36_N7toadlet6peeper15BackableTexture12resetDestroyEv,0,__ZThn12_N7toadlet3egg2io10FileStream6lengthEv,0,__ZThn12_N7toadlet7tadpole5Track20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn40_NK7toadlet6peeper11GLES2Buffer8getUsageEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet3egg20PointerIteratorRangeINS2_7tadpole4NodeEEEEEPKNS0_7_TYPEIDEPT_,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget7destroyEv,0,__ZThn36_N7toadlet6peeper11GLES2BufferD0Ev,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector4EfE7getWireIS5_EEfRKMS5_fRKT_,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState6retainEv,0,__ZN7toadlet7tadpole15ResourceManager18addResourceArchiveEPNS_3egg2io7ArchiveE,0,__ZN7toadlet6peeper19BackableRenderState12resourceThisEv,0,__ZN7toadlet6peeper14BackableShader7releaseEv,0,__ZN7toadlet3egg16StandardListenerD0Ev,0,__ZN7toadlet7tadpole10HopManagerD1Ev,0,__ZN7toadlet6peeper13GLES2SLShader7destroyEv,0,__ZN7toadlet6peeper16GLES2RenderState13setDepthStateERKNS0_10DepthStateE,0,__ZN7toadlet6peeper10VertexDataD1Ev,0,__ZThn36_N7toadlet6peeper11GLES2Buffer6retainEv,0,__ZThn12_N7toadlet7tadpole12TMSHStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBufferD1Ev,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget19getRootRenderTargetEv,0,__ZN7toadlet7tadpole8material15SceneParameters7releaseEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer14castToGLBufferEv,0,__ZNK7toadlet7tadpole6Camera11getViewportEv,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole15CameraComponentEJPNS2_6CameraEEEENS1_3egg16IntrusivePointerIT_NS6_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet7tadpole15ResourceManager6retainEv,0,__ZThn12_NK7toadlet6peeper17GLES2RenderTarget15getUniqueHandleEv,0,__ZN7toadlet7tadpole5Scene21postContextDeactivateEPNS_6peeper12RenderDeviceE,0,__ZThn40_N7toadlet6peeper14BackableBuffer11resetCreateEv,0,__ZN7toadlet6peeper19BackableRenderStateD0Ev,0,__ZN7toadlet7tadpole4Node8setScaleEfff,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole9TransformENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet6peeper14BackableShader6retainEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreatorD1Ev,0,__ZThn12_N7toadlet7tadpole8Skeleton7releaseEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer6unlockEv,0,__ZN7toadlet7tadpole14ArchiveManager7releaseEv,0,__ZN7toadlet7tadpole6Engine15createTorusMeshEffiiPNS0_8material8MaterialE,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_3hop9SimulatorENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole22DiffuseMaterialCreator7releaseEv,0,__ZN10emscripten8internal15raw_constructorIN7toadlet3egg4math10QuaternionEJEEEPT_DpNS0_11BindingTypeIT0_E8WireTypeE,0,__ZN7toadlet3egg2io12MemoryStreamD0Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderStateD1Ev,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole15ResourceManager16resourceUnloadedERKNS_3egg16IntrusivePointerINS2_8ResourceENS2_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet6peeper19BackableRenderState12getSetStatesEv,0,__ZN7toadlet6peeper17GLES2RenderTarget7destroyEv,0,__ZN7toadlet7tadpole8material18FogDensityVariableD1Ev,0,__ZThn12_N7toadlet6peeper19BackableShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2RenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper16GLES2RenderState18getRootRenderStateEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole13MeshComponentENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice14setShaderStateEPNS0_11ShaderStateE,0,__ZThn36_N7toadlet6peeper19BackableShaderStateD1Ev,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper12RenderDeviceENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole15CameraComponentENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZN7toadlet7tadpole22DiffuseMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn48_N7toadlet7tadpole12HopComponent10traceSolidERNS_3hop9CollisionEPNS2_5SolidERKNS_3egg4math7Vector3ERKNS8_7SegmentE,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZNK7toadlet7tadpole13MeshComponent12getTransformEv,0,__ZThn12_N7toadlet6peeper14BackableShader24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole13MeshComponent7SubMeshD1Ev,0,___cxa_pure_virtual,0,__ZN7toadlet7tadpole11WaveDecoder5closeEv,0,__ZN7toadlet7tadpole15GridMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState12getSetStatesEv,0,__ZN7toadlet7tadpole13PartitionNode23sensePotentiallyVisibleEPNS0_21SensorResultsListenerERKNS_3egg4math7Vector3E,0,__ZN7toadlet7tadpole11NodeManager7releaseEv,0,__ZN7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet7tadpole5Track20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK10__cxxabiv119__pointer_type_info9can_catchEPKNS_16__shim_type_infoERPv,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole13PartitionNodeENS2_9ComponentEE11getFullNameEv,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer16getTextureFormatEv,0,___ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole13MeshComponentEFvRKNS2_3egg6StringEEvPS4_JS8_EE6invokeERKSA_SB_PNS0_11BindingTypeIS6_E3$_0E_,0,__ZN7toadlet7tadpole10HopManager6retainEv,0,__ZN7toadlet6peeper12GLES2Texture24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper15BackableTexture7releaseEv,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreator6retainEv,0,__ZN7toadlet7tadpole11DDSStreamer6retainEv,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBufferD0Ev,0,__ZN7toadlet7tadpole9Transform12setTranslateERKNS_3egg4math7Vector3E,0,__ZN7toadlet7tadpole18SkyDomeMeshCreatorD1Ev,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensor6retainEv,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer9getAccessEv,0,__ZN7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole11BMPStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn12_NK7toadlet6peeper19BackableShaderState7getNameEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer6retainEv,0,__ZN7toadlet7tadpole13RenderableSet10queueLightEPNS0_14LightComponentE,0,__ZN10emscripten8internal15raw_constructorIN7toadlet3egg4math7Vector3EJEEEPT_DpNS0_11BindingTypeIT0_E8WireTypeE,0,__ZN7toadlet7tadpole4Mesh13removeSubMeshENS_3egg16IntrusivePointerINS1_7SubMeshENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole4NodeENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole8material8MaterialEEEvPT_,0,__ZNK7toadlet7tadpole15CameraComponent12getTransformEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState13setBlendStateERKNS0_10BlendStateE,0,__ZThn12_N7toadlet7tadpole6sensor20BoundingVolumeSensor5senseEPNS0_21SensorResultsListenerE,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole13MeshComponentENS2_9ComponentEED1Ev,0,__ZN7toadlet6peeper12GLES2Texture7setNameERKNS_3egg6StringE,0,__ZThn12_NK7toadlet6peeper13GLES2SLShader15getUniqueHandleEv,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState15getTextureStateENS0_6Shader10ShaderTypeEiRNS0_12TextureStateE,0,__ZThn12_NK7toadlet7tadpole11WaveDecoder14getAudioFormatEv,0,__ZThn36_N7toadlet6peeper12GLES2Texture12resetDestroyEv,0,__ZN7toadlet7tadpole12TMSHStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet3egg7ErrorerD0Ev,0,__ZN10__cxxabiv121__vmi_class_type_infoD0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15renderPrimitiveEPNS0_10VertexDataEPNS0_9IndexDataE,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreator6retainEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState16setGeometryStateERKNS0_13GeometryStateE,0,__ZThn12_N7toadlet3egg2io12MemoryStream5writeEPKhi,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer7getSizeEv,0,__ZN7toadlet7tadpole12TMSHStreamer6retainEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole15CameraComponentENS2_9ComponentEE11newInstanceEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole5SceneENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZThn12_N7toadlet7tadpole8Sequence7releaseEv,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation7getNameEv,0,__ZN7toadlet7tadpole4Mesh7SubMesh7releaseEv,0,__ZNK7toadlet6peeper20BackableVertexFormat7getNameEv,0,__ZN7toadlet6peeper17GLES2RenderTarget6retainEv,0,__ZN7toadlet6peeper18GLES2SLShaderState7releaseEv,0,__ZN7toadlet6peeper18GLES2SLShaderState6retainEv,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer16getTextureFormatEv,0,__ZN7toadlet7tadpole5Track12resourceThisEv,0,__ZN7toadlet7tadpole4Node14physicsRemovedEPNS0_16PhysicsComponentE,0,__ZThn36_N7toadlet7tadpole5Track24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole8material23MaterialAmbientVariable9getFormatEv,0,__ZThn36_N7toadlet6peeper19BackableShaderState7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice4swapEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZThn52_N7toadlet7tadpole17SkeletonComponent12getAnimationEi,0,__ZNK7toadlet6peeper19BackableRenderState13getDepthStateERNS0_10DepthStateE,0,__ZN7toadlet6peeper17GLES2RenderDevice25getShaderProfileSupportedERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreator7releaseEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer6updateEPhii,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole5SceneENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet6peeper16GLES2RenderState18getRootRenderStateEv,0,__ZN7toadlet7tadpole10HopManager9preUpdateEif,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole17SkeletonComponentENS2_9ComponentEED0Ev,0,__ZN7toadlet6peeper14BackableShader6createENS0_6Shader10ShaderTypeEPKNS_3egg6StringES7_i,0,__ZN7toadlet7tadpole13BufferManager7destroyEv,0,__ZN7toadlet7tadpole20WaterMaterialCreatorD0Ev,0,__ZN7toadlet6peeper14BackableBuffer7setBackENS_3egg16IntrusivePointerINS0_11IndexBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn12_N7toadlet7tadpole13BaseComponent7releaseEv,0,__ZN7toadlet3egg2io12MemoryStream5flushEv,0,__ZThn12_N7toadlet7tadpole13BaseComponent7destroyEv,0,__ZThn12_N7toadlet7tadpole13BaseComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet7tadpole11BMPStreamerD1Ev,0,__ZThn36_N7toadlet6peeper15BackableTextureD0Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderState24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTargetD0Ev,0,__ZN7toadlet6peeper10VertexData7releaseEv,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderStateD0Ev,0,__ZThn12_N7toadlet7tadpole13BaseComponent11frameUpdateEii,0,__ZThn12_NK7toadlet7tadpole12HopComponent7getNameEv,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv,0,__ZThn36_N7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9ComponentEFvvEvPS4_JEE6invokeERKS6_S7_,0,__ZN7toadlet7tadpole19SimpleRenderManager6retainEv,0,__ZN7toadlet7tadpole13BaseComponent13parentChangedEPNS0_4NodeE,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer14getIndexFormatEv,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole11WaveDecoder9writeableEv,0,__Z26new_EGLWindowRenderTarget2N10emscripten3valE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice17createPixelBufferEv,0,__ZN7toadlet7tadpole16TorusMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn12_N7toadlet7tadpole6sensor13SensorResults16sensingBeginningEv,0,__ZN7toadlet7tadpole6Camera17setSkipFirstClearEb,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimation9setWeightEf,0,__ZThn12_N7toadlet6peeper14BackableShaderD1Ev,0,__ZThn36_N7toadlet6peeper19BackableRenderState11setFogStateERKNS0_8FogStateE,0,__ZNK7toadlet6peeper20GLES2FBORenderTarget15getUniqueHandleEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper17GLES2VertexFormat7destroyEv,0,__ZThn44_N7toadlet6peeper11GLES2BufferD1Ev,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreatorD0Ev,0,__ZThn12_N7toadlet6peeper14BackableBuffer7destroyEv,0,__ZN7toadlet7tadpole8material8Material12resourceThisEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTargetD0Ev,0,__ZN7toadlet7tadpole12TMSHStreamerD0Ev,0,__ZN7toadlet7tadpole8material8Material20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet3egg3Log5errorERKNS0_6StringE,0,__ZN10emscripten8internal13getActualTypeIN7toadlet6peeper12RenderDeviceEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet6peeper20GLES2FBORenderTarget6retainEv,0,__ZThn44_N7toadlet6peeper14BackableBuffer18getRootPixelBufferEv,0,__ZThn40_N7toadlet6peeper14BackableBufferD0Ev,0,__ZNK7toadlet7tadpole12HopComponent11getVelocityEv,0,__ZThn12_N7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget11resetCreateEv,0,__ZN7toadlet7tadpole4Node6retainEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder5resetEv,0,__ZNK7toadlet6peeper19BackableRenderState19getNumTextureStatesENS0_6Shader10ShaderTypeE,0,__ZNK10__cxxabiv121__vmi_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer11getDataSizeEv,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat14getNumElementsEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBufferD1Ev,0,__ZThn12_N7toadlet6peeper26GLES2TextureMipPixelBuffer7setNameERKNS_3egg6StringE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole16AABoxMeshCreatorD0Ev,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation20setAnimationListenerEPNS1_17AnimationListenerE,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat7destroyEv,0,__ZThn12_N7toadlet7tadpole8material8Material24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole4Node11handleEventEPNS_3egg5EventE,0,__ZN7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper16GLES2RenderStateD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15getRenderTargetEv,0,__ZN7toadlet6peeper19BackableShaderState9getShaderENS0_6Shader10ShaderTypeE,0,__ZThn48_N7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZThn44_NK7toadlet6peeper14BackableBuffer16getTextureFormatEv,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer16getTextureFormatEv,0,__ZN7toadlet7tadpole4Node9setRotateERKNS_3egg4math7Vector3Ef,0,__ZThn36_NK7toadlet7tadpole5Track15getUniqueHandleEv,0,__ZN7toadlet7tadpole5Scene15preContextResetEPNS_6peeper12RenderDeviceE,0,__ZNK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole8material20NormalMatrixVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet6peeper11GLES2Buffer4lockEi,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat7setNameERKNS_3egg6StringE,0,__ZNK7toadlet6peeper19BackableRenderState18getRasterizerStateERNS0_15RasterizerStateE,0,__ZN7toadlet7tadpole4Mesh7SubMesh24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper18GLES2SLShaderStateD0Ev,0,__ZN7toadlet6peeper20VertexBufferAccessorD2Ev,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget15getUniqueHandleEv,0,__ZThn36_N7toadlet7tadpole17SkeletonComponent16transformChangedEPNS0_9TransformE,0,__ZNK7toadlet6peeper16GLES2RenderState16getMaterialStateERNS0_13MaterialStateE,0,__ZN7toadlet7tadpole6action13BaseAnimation8setScopeEi,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice19getClosePixelFormatEii,0,__ZN7toadlet6peeper20BackableVertexFormat6createEv,0,__ZThn12_N7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper12GLES2TextureD0Ev,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEKFPNS3_9TransformEvES6_PKS4_JEE6invokeERKS8_SA_,0,__ZThn36_N7toadlet7tadpole5Track6unlockEv,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer12resetDestroyEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBufferD0Ev,0,__ZN7toadlet7tadpole4Node8setBoundENS_3egg16IntrusivePointerINS0_5BoundENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn36_N7toadlet7tadpole5Track7destroyEv,0,__ZNK7toadlet6peeper11GLES2Buffer15getVertexFormatEv,0,__ZN7toadlet7tadpole5Track6unlockEv,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat6retainEv,0,__ZThn40_NK7toadlet6peeper14BackableBuffer15getVertexFormatEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer22castToGLFBOPixelBufferEv,0,__ZThn12_N7toadlet7tadpole11BMPStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole9ComponentENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet6peeper17GLES2RenderDevice14setRenderStateEPNS0_11RenderStateE,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormat7setNameERKNS_3egg6StringE,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole5SceneENS3_25DefaultIntrusiveSemanticsEEEJOPNS5_6EngineEEE6invokeEPFS8_SB_ESA_,0,__ZN7toadlet6peeper17GLES2RenderDevice5resetEv,0,__ZN7toadlet7tadpole13RenderableSet12startQueuingEv,0,__ZN7toadlet7tadpole8material10RenderPass6retainEv,0,__ZNK7toadlet7tadpole5Scene13getLogicFrameEv,0,__ZN7toadlet7tadpole8material19FogDistanceVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet6peeper14BackableShader6createENS0_6Shader10ShaderTypeERKNS_3egg6StringES7_,0,__ZThn40_NK7toadlet7tadpole17SkeletonComponent12getTransformEv,0,__ZN7toadlet6peeper13GLES2SLShader6createENS0_6Shader10ShaderTypeERKNS_3egg6StringES7_,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole15CameraComponentEEEvPT_,0,__ZN7toadlet3egg7Errorer10traceFrameEPKc,0,__ZN7toadlet7tadpole8material25MaterialShininessVariableD1Ev,0,__ZThn36_N7toadlet6peeper12GLES2TextureD0Ev,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState24internal_setUniqueHandleEi,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer7releaseEv,0,__ZNK7toadlet6peeper14BackableShader15getUniqueHandleEv,0,__ZN7toadlet3hop5SolidD0Ev,0,__ZN7toadlet7tadpole4Node11frameUpdateEii,0,__ZN7toadlet7tadpole8material15AmbientVariableD1Ev,0,__ZN7toadlet7tadpole9Transform8setScaleERKNS_3egg4math7Vector3E,0,__ZN7toadlet7tadpole27NormalizationTextureCreator6retainEv,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole4NodeEJPNS2_5SceneEEEENS1_3egg16IntrusivePointerIT_NS6_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet6peeper17GLES2VertexFormat11findElementEi,0,__ZNK7toadlet7tadpole5Track15getUniqueHandleEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector3EfE7getWireIS5_EEfRKMS5_fRKT_,0,__ZN7toadlet3egg2io10DataStream6closedEv,0,__ZThn12_NK7toadlet7tadpole13BaseComponent9getActiveEv,0,__ZThn36_N7toadlet7tadpole5TrackD1Ev,0,__ZNK7toadlet6peeper19BackableRenderState11getFogStateERNS0_8FogStateE,0,__ZThn48_N7toadlet7tadpole13MeshComponentD1Ev,0,__ZN7toadlet7tadpole15CameraComponentD1Ev,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer7getSizeEv,0,__ZN7toadlet6peeper12GLES2Texture14getRootTextureEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet6peeper8ViewportEbE7setWireIS4_EEvRKMS4_bRT_b,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreator7destroyEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer6retainEv,0,__ZN7toadlet6peeper19BackableRenderState7destroyEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState18setRasterizerStateERKNS0_15RasterizerStateE,0,__ZThn12_N7toadlet7tadpole8material8Material20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole16TorusMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet7tadpole11TGAStreamerD1Ev,0,__ZN7toadlet7tadpole10HopManager12traceSegmentERNS0_16PhysicsCollisionERKNS_3egg4math7SegmentEiPNS0_4NodeE,0,__ZN7toadlet7tadpole8SequenceD0Ev,0,__ZN7toadlet7tadpole4Node11startActionERKNS_3egg6StringE,0,__ZN7toadlet7tadpole15MaterialManagerD1Ev,0,__ZThn36_N7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZN7toadlet7tadpole8material18TextureSetVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn12_NK7toadlet7tadpole10HopManager10getGravityEv,0,__ZThn48_NK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager22setupPassForRenderableEPNS0_8material10RenderPassEPNS_6peeper12RenderDeviceEPNS0_10RenderableERKNS_3egg4math7Vector4E,0,__ZThn36_N7toadlet6peeper12GLES2Texture6createEiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEEPPh,0,__ZThn40_NK7toadlet6peeper14BackableBuffer7getSizeEv,0,__ZN7toadlet7tadpole4Node14visibleRemovedEPNS0_7VisibleE,0,__ZN7toadlet7tadpole11FontManager8unmanageEPNS_3egg8ResourceE,0,__ZN7toadlet6peeper15BackableTexture4loadEPNS0_13TextureFormatEPh,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole4NodeENS3_25DefaultIntrusiveSemanticsEEEJOPNS5_5SceneEEE6invokeEPFS8_SB_ESA_,0,__ZThn36_N7toadlet6peeper12GLES2Texture24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper13GLES2SLShader24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole15CameraComponent13getWorldBoundEv,0,__ZN7toadlet7tadpole6sensor13SensorResultsD1Ev,0,__ZN7toadlet6peeper17GLES2RenderDevice15setRenderTargetEPNS0_12RenderTargetE,0,__ZN7toadlet7tadpole4Mesh7SubMesh12resourceThisEv,0,__ZNK7toadlet6peeper29BackableTextureMipPixelBuffer9getAccessEv,0,__ZN7toadlet3egg3Log5alertERKNS0_6StringE,0,__ZThn12_N7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZN10emscripten11RangeAccessIN7toadlet3egg20PointerIteratorRangeINS1_7tadpole4NodeEEEE4nextERS6_,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole9ComponentENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet7tadpole8material10RenderPass7releaseEv,0,__ZN7toadlet6peeper20BackableVertexFormatD1Ev,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole12WaveStreamer17createAudioStreamEPNS_3egg2io6StreamEPNS0_12ResourceDataE,0,__ZN7toadlet3egg9ExceptionD2Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice19getShadowBiasMatrixEPKNS0_7TextureERNS_3egg4math9Matrix4x4E,0,__ZThn36_NK7toadlet6peeper17GLES2VertexFormat15getUniqueHandleEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZThn36_N7toadlet7tadpole12HopComponentD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice7releaseEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget6retainEv,0,__ZNK7toadlet7tadpole13MeshComponent13getWorldBoundEv,0,__ZN7toadlet6peeper15BackableTexture7getBackEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector2EfE7getWireIS5_EEfRKMS5_fRKT_,0,__ZThn36_N7toadlet6peeper14BackableShader7destroyEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget19getRootRenderTargetEv,0,__ZN7toadlet7tadpole17SphereMeshCreator7destroyEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole6EngineENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn36_N7toadlet7tadpole12HopComponent7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice20createVariableBufferEv,0,__ZThn36_N7toadlet6peeper14BackableBuffer4lockEi,0,__ZThn12_N7toadlet7tadpole5Scene16postContextResetEPNS_6peeper12RenderDeviceE,0,__ZThn12_N7toadlet3egg2io12MemoryStream8positionEv,0,__ZThn12_N7toadlet7tadpole11FontManagerD0Ev,0,__ZThn12_N7toadlet7tadpole11WaveDecoderD0Ev,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation8getValueEv,0,__ZN7toadlet7tadpole15ResourceManager7destroyEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg20PointerIteratorRangeINS2_7tadpole4NodeEEEEEvPT_,0,__ZThn12_N7toadlet6peeper19BackableRenderStateD0Ev,0,__ZN7toadlet3egg5Error7tassertERKNS0_6StringE,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState19getNumSamplerStatesENS0_6Shader10ShaderTypeE,0,__ZN7toadlet6peeper22EGL2WindowRenderTarget25activateAdditionalContextEv,0,__ZNK7toadlet7tadpole17SkeletonComponent17SequenceAnimation8getValueEv,0,__ZThn12_N7toadlet7tadpole12TMSHStreamer6retainEv,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole13PartitionNodeENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget7setNameERKNS_3egg6StringE,0,__ZThn36_NK7toadlet6peeper20GLES2FBORenderTarget7getNameEv,0,__ZThn12_N7toadlet6peeper19BackableShaderStateD0Ev,0,__ZThn12_N7toadlet6peeper16GLES2RenderState7destroyEv,0,__ZN7toadlet6peeper16EGL2RenderTarget10deactivateEv,0,__ZNK7toadlet7tadpole4Node9getActionERKNS_3egg6StringE,0,__ZN7toadlet7tadpole5Scene6renderEPNS_6peeper12RenderDeviceEPNS0_6CameraEPNS0_4NodeE,0,__ZN7toadlet7tadpole15ResourceManager8unmanageEPNS_3egg8ResourceE,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState11getFogStateERNS0_8FogStateE,0,__ZThn12_N7toadlet6peeper20GLES2FBORenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole6EngineEEEvPT_,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget6retainEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice10setTextureENS0_6Shader10ShaderTypeEiPNS0_7TextureE,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat10addElementEiii,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget7destroyEv,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreator7destroyEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget6retainEv,0,__ZThn36_N7toadlet7tadpole12HopComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer6updateEPhii,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole13PartitionNodeEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole8material19ModelMatrixVariableD0Ev,0,__ZThn44_N7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZThn12_NK7toadlet7tadpole15CameraComponent7getTypeEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6CameraEFvPNS2_6peeper12RenderDeviceEPNS3_5SceneEPNS3_4NodeEEvPS4_JS7_S9_SB_EE6invokeERKSD_SE_S7_S9_SB_,0,__ZNK7toadlet7tadpole5Scene14getCreateAudioEv,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer11getDataSizeEv,0,__ZN7toadlet7tadpole8material25LightViewPositionVariableD0Ev,0,__ZNK7toadlet7tadpole15CameraComponent17getWorldTransformEv,0,__ZN7toadlet7tadpole6Camera20autoUpdateProjectionEPNS_6peeper12RenderTargetE,0,__ZNK7toadlet7tadpole5Scene14getNodeManagerEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice7destroyEv,0,__ZN7toadlet7tadpole4Mesh11setSkeletonENS_3egg16IntrusivePointerINS0_8SkeletonENS2_25DefaultIntrusiveSemanticsEEE,0,__ZNSt9bad_allocD2Ev,0,__ZN7toadlet6peeper17GLES2RenderDevice17createRenderStateEv,0,__ZN7toadlet7tadpole5Scene13getResetFrameEv,0,__ZN7toadlet6peeper9IndexData7releaseEv,0,__ZThn36_NK7toadlet7tadpole15CameraComponent8getBoundEv,0,__ZN7toadlet7tadpole12HopComponent8setScopeEi,0,__ZThn12_N7toadlet7tadpole13MeshComponent7releaseEv,0,__ZN10emscripten8internal15FunctionInvokerIPFNS_3valERN7toadlet3egg20PointerIteratorRangeINS3_7tadpole4NodeEEEES2_S9_JEE6invokeEPSB_PS8_,0,__ZN7toadlet6peeper20BackableVertexFormat7getBackEv,0,__ZN7toadlet3egg5Error7unknownERKNS0_6StringE,0,__ZThn12_NK7toadlet6peeper19BackableRenderState15getUniqueHandleEv,0,__ZN7toadlet7tadpole6Camera6renderEPNS_6peeper12RenderDeviceEPNS0_5SceneEPNS0_4NodeE,0,__ZThn36_N7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole5Scene17setPhysicsManagerEPNS0_14PhysicsManagerE,0,__ZN7toadlet6peeper20GLES2FBORenderTarget27deactivateAdditionalContextEv,0,__ZN7toadlet7tadpole12HopComponent11rootChangedEPNS0_4NodeE,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer7setNameERKNS_3egg6StringE,0,__ZN10emscripten8make_ptrIN7toadlet3hop9SimulatorEJEEENS1_3egg16IntrusivePointerIT_NS4_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZThn12_N7toadlet7tadpole8Skeleton6retainEv,0,__ZThn12_NK7toadlet7tadpole6action13BaseAnimation11getMinValueEv,0,__ZN7toadlet7tadpole4Node15physicsAttachedEPNS0_16PhysicsComponentE,0,__ZNK7toadlet7tadpole5Scene16getRenderManagerEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice17createShaderStateEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg5ErrorEEEvPT_,0,__ZN7toadlet7tadpole8material22CameraPositionVariableD1Ev,0,__ZThn12_N7toadlet6peeper16GLES2RenderStateD1Ev,0,__ZN7toadlet3egg2io12MemoryStream9writeableEv,0,__ZN7toadlet6peeper17GLES2VertexFormat6retainEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEEi,0,__ZNK7toadlet6peeper20BackableVertexFormat16getElementFormatEi,0,__ZN7toadlet7tadpole13BufferManagerD0Ev,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole4NodeENS2_9ComponentEE11getFullNameEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet6peeper11RenderStateEEEvPT_,0,__ZNK7toadlet7tadpole12HopComponent17getCollisionScopeEv,0,__ZThn12_N7toadlet6peeper16GLES2RenderState7setNameERKNS_3egg6StringE,0,__ZThn48_N7toadlet6peeper11GLES2Buffer21getRootVariableBufferEv,0,__ZN7toadlet7tadpole8material23MaterialAmbientVariableD0Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice28copyFrameBufferToPixelBufferEPNS0_11PixelBufferE,0,__ZN7toadlet6peeper15BackableTextureD1Ev,0,__ZThn36_N7toadlet6peeper14BackableBuffer7destroyEv,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget7releaseEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget7releaseEv,0,__ZN7toadlet7tadpole15CameraComponent7releaseEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer19getRootVertexBufferEv,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole8material25MaterialShininessVariableD0Ev,0,__ZThn36_N7toadlet6peeper19BackableRenderState18getRootRenderStateEv,0,__ZThn12_N7toadlet7tadpole15GridMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet6peeper17GLES2RenderTarget7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderState7releaseEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState13getBlendStateERNS0_10BlendStateE,0,__ZThn40_N7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEEi,0,__ZN7toadlet7tadpole5BoundD1Ev,0,__ZThn12_N7toadlet3egg2io10DataStream5closeEv,0,__ZN7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZNK7toadlet7tadpole4Node12getNumLightsEv,0,__ZN7toadlet7tadpole5Scene15setRangeLogicDTEii,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6EngineEFNS2_3egg16IntrusivePointerINS3_4MeshENS5_25DefaultIntrusiveSemanticsEEEffiiPNS3_8material8MaterialEES9_PS4_JffiiSC_EE6invokeERKSE_SF_ffiiSC_,0,__ZThn44_NK7toadlet7tadpole13MeshComponent20getSharedRenderStateEv,0,__ZN7toadlet6peeper13TextureFormatD1Ev,0,__ZThn44_N7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole11WaveDecoderD2Ev,0,__ZThn48_N7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZN7toadlet7tadpole13MeshComponent17getAttachmentNameEi,0,__ZThn40_NK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer6retainEv,0,__ZNK7toadlet7tadpole17SkeletonComponent13getWorldBoundEv,0,__ZN7toadlet7tadpole11WaveDecoder8readableEv,0,__ZN7toadlet6peeper18GLES2SLShaderState9setShaderENS0_6Shader10ShaderTypeENS_3egg16IntrusivePointerIS2_NS4_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper17GLES2RenderDevice19getClosePixelFormatEii,0,__ZN7toadlet7tadpole8material10RenderPass7destroyEv,0,__ZN7toadlet7tadpole8material21TextureMatrixVariableD0Ev,0,__ZN7toadlet7tadpole5SceneD1Ev,0,__ZN7toadlet3egg2io10DataStream4readEPhi,0,__ZThn40_NK7toadlet7tadpole12HopComponent17getWorldTransformEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget4swapEv,0,__ZN7toadlet7tadpole13MeshComponent16transformChangedEPNS0_9TransformE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer12resourceThisEv,0,__ZN7toadlet7tadpole10HopManager9preUpdateEPNS_3hop5SolidEif,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_6peeper12RenderTargetENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZNK7toadlet6peeper16GLES2RenderState18getRasterizerStateERNS0_15RasterizerStateE,0,__ZN7toadlet7tadpole8SkeletonD1Ev,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole15CameraComponentEEEPKNS0_7_TYPEIDEPT_,0,__ZThn36_N7toadlet6peeper11GLES2Buffer7destroyEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer12resetDestroyEv,0,__ZN7toadlet7tadpole17SkyBoxMeshCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet3egg2io10DataStream4seekEi,0,__ZThn12_N7toadlet7tadpole13BufferManagerD1Ev,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat11findElementEi,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole5SceneEFviEvPS4_JiEE6invokeERKS6_S7_i,0,__ZN7toadlet7tadpole10HopManager17collisionResponseEPNS_3hop5SolidERNS_3egg4math7Vector3ES8_RNS2_9CollisionE,0,__ZN7toadlet6peeper19GLES2SLVertexLayoutD1Ev,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreator6createERKNS_3egg6StringEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZN7toadlet7tadpole8Sequence20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole8material8Material7destroyEv,0,__ZNK7toadlet6peeper15BackableTexture7getNameEv,0,__ZN7toadlet7tadpole22DiffuseMaterialCreatorD1Ev,0,__ZN7toadlet7tadpole6action13BaseAnimation8setValueEf,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer15getUniqueHandleEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole6CameraEEEvPT_,0,__ZNK7toadlet6peeper14BackableBuffer16getTextureFormatEv,0,__ZThn12_NK7toadlet7tadpole13MeshComponent9getActiveEv,0,__ZN7toadlet7tadpole17SkeletonComponent12getAnimationERKNS_3egg6StringE,0,__ZN7toadlet7tadpole16TorusMeshCreatorD1Ev,0,__ZN7toadlet7tadpole15ResourceManager18setDefaultStreamerEPNS0_16ResourceStreamerE,0,__ZThn36_N7toadlet7tadpole12HopComponent6retainEv,0,__ZN7toadlet6peeper16GLES2RenderState15setTextureStateENS0_6Shader10ShaderTypeEiRKNS0_12TextureStateE,0,__ZN7toadlet3hop5ShapeD0Ev,0,__ZN7toadlet7tadpole4Node19notifyParentRemovedEv,0,__ZThn40_N7toadlet6peeper11GLES2BufferD1Ev,0,__ZN7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZThn40_NK7toadlet7tadpole12HopComponent13getWorldBoundEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet6peeper12RenderDeviceEFbPNS3_12RenderTargetEiEbPS4_JS6_iEE6invokeERKS8_S9_S6_i,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderStateD0Ev,0,__ZN7toadlet6peeper19BackableShaderState21getNumVariableBuffersENS0_6Shader10ShaderTypeE,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer16getTextureFormatEv,0,__ZN7toadlet7tadpole8material21LightSpecularVariable9getFormatEv,0,__ZThn36_N7toadlet6peeper16GLES2RenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper12GLES2Texture17getMipPixelBufferEii,0,__ZThn12_N7toadlet7tadpole15CameraComponent6retainEv,0,__ZThn12_N7toadlet6peeper12GLES2TextureD0Ev,0,__ZNK7toadlet6peeper14BackableShader10getProfileEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_3hop9SimulatorENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet7tadpole8Sequence7destroyEv,0,__ZNK7toadlet7tadpole9Transform9getRotateEv,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15setRenderTargetEPNS0_12RenderTargetE,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreatorD0Ev,0,__ZNK7toadlet3egg16InstantiableTypeINS_7tadpole4NodeENS2_9ComponentEE10instanceOfERKNS0_6StringE,0,__ZN7toadlet7tadpole4Node18animatableAttachedEPNS0_10AnimatableE,0,__ZN7toadlet6peeper11GLES2BufferD0Ev,0,__ZN7toadlet7tadpole13PartitionNodeD1Ev,0,__ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet7tadpole8Skeleton7getNameEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole4MeshEEEvPT_,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole19SimpleRenderManager9getDeviceEv,0,__ZN7toadlet7tadpole8Sequence6retainEv,0,__ZThn48_N7toadlet7tadpole13MeshComponent17getAttachmentNameEi,0,__ZThn12_N7toadlet7tadpole4Mesh7destroyEv,0,__ZN7toadlet7tadpole6Engine14setAudioDeviceEPNS_6ribbit11AudioDeviceE,0,__ZN7toadlet7tadpole8material15AmbientVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn12_N7toadlet3egg2io10FileStream7releaseEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget6removeENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole8material24MaterialSpecularVariableD1Ev,0,__ZThn12_N7toadlet3egg12BaseResourceD1Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3hop9SimulatorEEEvPT_,0,__ZN10emscripten11RangeAccessIN7toadlet3egg20PointerIteratorRangeINS1_7tadpole4NodeEEEE7atBeginERKS6_,0,__ZThn36_NK7toadlet6peeper19BackableRenderState15getTextureStateENS0_6Shader10ShaderTypeEiRNS0_12TextureStateE,0,__ZThn12_N7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZThn12_N7toadlet7tadpole8material8Material7destroyEv,0,__ZN7toadlet7tadpole10HopManager18SolidSensorResults11resultFoundEPNS0_4NodeEf,0,__ZThn36_NK7toadlet6peeper19BackableShaderState7getNameEv,0,__ZNK7toadlet7tadpole17SkeletonComponent9getActiveEv,0,__ZThn36_N7toadlet7tadpole12HopComponent11logicUpdateEii,0,__ZN7toadlet6peeper22EGL2WindowRenderTarget7destroyEv,0,__ZThn12_N7toadlet7tadpole12TMSHStreamerD0Ev,0,__ZN7toadlet7tadpole6Camera20updateClippingPlanesEv,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTargetD1Ev,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager9getDeviceEv,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer23getVariableBufferFormatEv,0,__ZN7toadlet3egg3Log5debugERKNS0_6StringE,0,__ZThn44_N7toadlet7tadpole13MeshComponent20setSharedRenderStateENS_3egg16IntrusivePointerINS_6peeper11RenderStateENS2_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet6peeper17GLES2RenderTarget15getUniqueHandleEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole6CameraEFvRKNS2_3egg4math7Vector4EEvPS4_JS9_EE6invokeERKSB_SC_PS7_,0,__ZThn12_N7toadlet6peeper14BackableShader7setNameERKNS_3egg6StringE,0,__ZNK7toadlet7tadpole4Node8getChildERKNS_3egg6StringE,0,__ZN7toadlet3egg2io12MemoryStream8readableEv,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat24internal_setUniqueHandleEi,0,__ZThn12_N7toadlet7tadpole12HopComponent7destroyEv,0,__ZN7toadlet6peeper17GLES2RenderDevice13createTextureEv,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreator6retainEv,0,__ZN7toadlet6peeper22EGL2WindowRenderTarget19getRootRenderTargetEv,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer15getUniqueHandleEv,0,__ZThn44_NK7toadlet6peeper20GLES2FBORenderTarget7isValidEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer12resourceThisEv,0,__ZN7toadlet3egg16InstantiableTypeINS_7tadpole13PartitionNodeENS2_9ComponentEED1Ev,0,__ZN7toadlet6peeper19BackableRenderState15setTextureStateENS0_6Shader10ShaderTypeEiRKNS0_12TextureStateE,0,__ZThn36_N7toadlet6peeper19BackableRenderState15setTextureStateENS0_6Shader10ShaderTypeEiRKNS0_12TextureStateE,0,__ZNK7toadlet3egg12BaseResource7getNameEv,0,__ZN7toadlet3egg2io12MemoryStream8positionEv,0,__ZN7toadlet7tadpole4Node13setStayActiveEb,0,__ZN7toadlet7tadpole16ResourceStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget7isValidEv,0,__ZThn36_N7toadlet7tadpole13MeshComponentD0Ev,0,__ZN7toadlet7tadpole8material18TextureSetVariableD1Ev,0,__ZN7toadlet7tadpole8material18FogDensityVariableD0Ev,0,__ZThn36_N7toadlet6peeper14BackableBufferD0Ev,0,__ZThn48_N7toadlet6peeper11GLES2Buffer12resetDestroyEv,0,__ZN7toadlet7tadpole11DDSStreamerD1Ev,0,__ZThn40_N7toadlet6peeper11GLES2Buffer19getRootVertexBufferEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState19getNumSamplerStatesENS0_6Shader10ShaderTypeE,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget30getRootPixelBufferRenderTargetEv,0,__ZThn12_N7toadlet3egg2io10FileStream5resetEv,0,__ZThn12_N7toadlet7tadpole13BaseComponentD1Ev,0,__ZNK7toadlet7tadpole13MeshComponent7SubMesh17getRenderMaterialEv,0,__ZNK7toadlet6peeper11GLES2Buffer14getIndexFormatEv,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer8getUsageEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation7releaseEv,0,__ZN7toadlet3egg10CollectionIPNS0_14LoggerListenerEED1Ev,0,__ZThn36_NK7toadlet6peeper12GLES2Texture15getUniqueHandleEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent17SequenceAnimationD1Ev,0,__ZThn12_N7toadlet3egg2io10FileStream6closedEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6updateEPhii,0,__ZThn36_N7toadlet7tadpole12HopComponent8setBoundEPNS0_5BoundE,0,__ZThn12_N7toadlet7tadpole4Mesh7setNameERKNS_3egg6StringE,0,__ZThn40_NK7toadlet7tadpole13MeshComponent8getBoundEv,0,__ZN7toadlet7tadpole15GridMeshCreator6retainEv,0,__ZNK7toadlet7tadpole17SkeletonComponent12getTransformEv,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole9TransformEKFRKNS2_3egg4math10QuaternionEvES9_PKS4_JEE6invokeERKSB_SD_,0,__ZN7toadlet7tadpole5Track7setNameERKNS_3egg6StringE,0,__ZNK7toadlet6peeper13GLES2SLShader15getUniqueHandleEv,0,__ZN7toadlet7tadpole13MeshComponent7setMeshEPNS0_4MeshE,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent11frameUpdateEii,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole15CameraComponentENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet3egg2io10DataStream5closeEv,0,__ZN7toadlet7tadpole8material20NormalMatrixVariableD1Ev,0,__ZN7toadlet7tadpole27NormalizationTextureCreator7releaseEv,0,__ZN7toadlet7tadpole4Node11nodeRemovedEPS1_,0,__ZThn12_N7toadlet7tadpole20WaterMaterialCreatorD1Ev,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget30getRootPixelBufferRenderTargetEv,0,__ZN7toadlet6peeper14BackableShader7setBackENS_3egg16IntrusivePointerINS0_6ShaderENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceE,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget6removeENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet7tadpole6Engine20setHasBackableShaderEb,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBufferD0Ev,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBufferD1Ev,0,__ZN7toadlet7tadpole18SkyDomeMeshCreatorD0Ev,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet6peeper18GLES2SLShaderState7getNameEv,0,__ZNK7toadlet7tadpole8material8Material15getUniqueHandleEv,0,__ZN7toadlet6peeper20BackableVertexFormat10addElementERKNS_3egg6StringEii,0,__ZThn36_NK7toadlet7tadpole12HopComponent8getScopeEv,0,__ZN10emscripten8internal12operator_newIN7toadlet3egg16IntrusivePointerINS2_7tadpole6CameraENS3_25DefaultIntrusiveSemanticsEEEJEEEPT_DpT0_,0,__ZThn44_NK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZN7toadlet3egg2io10DataStreamD0Ev,0,__ZNK10__cxxabiv120__si_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole4NodeEFNS2_3egg20PointerIteratorRangeIS4_EEvES7_PS4_JEE6invokeERKS9_SA_,0,__ZThn12_N7toadlet7tadpole8Sequence6retainEv,0,__ZThn12_NK7toadlet6peeper26GLES2TextureMipPixelBuffer7getNameEv,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer9getAccessEv,0,__ZN7toadlet7tadpole17SkeletonComponent17SequenceAnimation7releaseEv,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat14getElementNameEi,0,__ZN7toadlet7tadpole4Node17animatableRemovedEPNS0_10AnimatableE,0,__ZThn36_N7toadlet6peeper14BackableShader6createENS0_6Shader10ShaderTypeERKNS_3egg6StringES7_,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer14castToGLBufferEv,0,__ZN7toadlet3egg2io12MemoryStream6lengthEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState16setMaterialStateERKNS0_13MaterialStateE,0,__ZThn36_NK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZN7toadlet3egg19WrapPointerIteratorINS0_16IntrusivePointerINS_7tadpole4NodeENS0_25DefaultIntrusiveSemanticsEEENS0_17PointerCollectionIS4_E8iteratorEED1Ev,0,__ZN7toadlet6peeper12GLES2Texture7releaseEv,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer18getRootPixelBufferEv,0,__ZN7toadlet7tadpole8Sequence7releaseEv,0,__ZThn12_N7toadlet7tadpole4Mesh7SubMesh7setNameERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice8activateEv,0,__ZN7toadlet7tadpole13MeshComponent6retainEv,0,__ZThn48_N7toadlet7tadpole17SkeletonComponent17getAttachmentNameEi,0,__ZN7toadlet6peeper14BackableBuffer6retainEv,0,__ZN7toadlet3egg16InstantiableTypeINS_7tadpole4NodeENS2_9ComponentEED0Ev,0,__ZThn44_N7toadlet6peeper14BackableBufferD1Ev,0,__ZThn12_N7toadlet7tadpole10HopManager8testNodeERNS0_16PhysicsCollisionEPNS0_4NodeERKNS_3egg4math7SegmentES5_,0,__ZN7toadlet7tadpole11DDSStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv,0,__ZNK7toadlet3egg19NonInstantiableTypeINS_7tadpole17SkeletonComponentENS2_9ComponentEE11newInstanceEv,0,__ZThn36_NK7toadlet6peeper14BackableShader13getShaderTypeEv,0,__ZThn12_NK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZThn12_N7toadlet7tadpole21SkyBoxMaterialCreator7releaseEv,0,__ZThn12_N7toadlet6peeper11GLES2Buffer24internal_setUniqueHandleEi,0,__ZNK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv,0,__ZN7toadlet7tadpole11TGAStreamer7releaseEv,0,__ZN7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_20VariableBufferFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZThn36_NK7toadlet6peeper19BackableRenderState18getRasterizerStateERNS0_15RasterizerStateE,0,__ZNK7toadlet6peeper20BackableVertexFormat15getElementIndexEi,0,__ZNK7toadlet6peeper16GLES2RenderState15getTextureStateENS0_6Shader10ShaderTypeEiRNS0_12TextureStateE,0,__ZThn60_N7toadlet6peeper23EGL2PBufferRenderTarget24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole4Node17getNumAnimatablesEv,0,__ZN7toadlet7tadpole6Camera17projectionUpdatedEv,0,__ZN7toadlet7tadpole15ResourceManager6manageEPNS_3egg8ResourceERKNS2_6StringE,0,__ZN7toadlet7tadpole17SphereMeshCreator6retainEv,0,__ZN7toadlet7tadpole8Skeleton4BoneD0Ev,0,__ZN7toadlet7tadpole15MaterialManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer29castToGLTextureMipPixelBufferEv,0,__ZN7toadlet7tadpole8material15SceneParametersD0Ev,0,__ZN7toadlet6peeper18GLES2SLShaderState23getVariableBufferFormatENS0_6Shader10ShaderTypeEi,0,__ZN7toadlet6peeper22EGL2WindowRenderTargetD1Ev,0,__ZNK7toadlet6peeper14BackableBuffer8getUsageEv,0,__ZThn36_N7toadlet6peeper20BackableVertexFormatD1Ev,0,__ZN7toadlet7tadpole8material26MaterialTrackColorVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet6peeper11GLES2Buffer14castToGLBufferEv,0,__ZN10emscripten4baseIN7toadlet7tadpole4NodeEE14convertPointerINS2_13PartitionNodeES3_EEPT0_PT_,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget8getWidthEv,0,__ZThn48_N7toadlet7tadpole17SkeletonComponent22getAttachmentTransformEPNS0_9TransformEi,0,__ZThn12_N7toadlet6peeper16GLES2RenderState6retainEv,0,__ZN7toadlet7tadpole17SkyBoxMeshCreatorD1Ev,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget12resetDestroyEv,0,__ZThn36_N7toadlet6peeper19BackableRenderState15setSamplerStateENS0_6Shader10ShaderTypeEiRKNS0_12SamplerStateE,0,__ZThn12_N7toadlet3egg2io12MemoryStream6lengthEv,0,__ZThn36_NK7toadlet6peeper19BackableRenderState15getSamplerStateENS0_6Shader10ShaderTypeEiRNS0_12SamplerStateE,0,__ZThn40_N7toadlet7tadpole13MeshComponentD1Ev,0,__ZThn12_N7toadlet3egg2io10FileStream9writeableEv,0,__ZThn12_N7toadlet7tadpole6action13BaseAnimation8setValueEf,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole8material8MaterialEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole17SkeletonComponent7releaseEv,0,__ZThn12_N7toadlet7tadpole12HopComponent6retainEv,0,__ZN7toadlet7tadpole20WaterMaterialCreator6retainEv,0,__ZN7toadlet6peeper12GLES2Texture4readEPNS0_13TextureFormatEPh,0,__ZThn12_N7toadlet3egg12BaseResource7setNameERKNS0_6StringE,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer7getBackEv,0,__ZThn12_NK7toadlet6peeper19GLES2FBOPixelBuffer7getNameEv,0,__ZN7toadlet7tadpole13MeshComponentD0Ev,0,__ZThn40_NK7toadlet7tadpole17SkeletonComponent13getWorldBoundEv,0,__ZN7toadlet7tadpole4MeshD1Ev,0,__ZN7toadlet7tadpole8material26MaterialTrackColorVariableD1Ev,0,__ZThn12_N7toadlet7tadpole12HopComponent7releaseEv,0,__ZN7toadlet7tadpole5Scene6retainEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTargetD1Ev,0,__ZN7toadlet6peeper20VariableBufferFormat7releaseEv,0,__ZThn36_N7toadlet6peeper15BackableTexture4loadEPNS0_13TextureFormatEPh,0,__ZThn12_N7toadlet6peeper19BackableShaderState7destroyEv,0,__ZN7toadlet3egg2io12MemoryStream4seekEi,0,__ZThn36_NK7toadlet6peeper19GLES2FBOPixelBuffer11getDataSizeEv,0,__ZThn12_N7toadlet6peeper18GLES2SLShaderStateD1Ev,0,__ZThn36_N7toadlet6peeper16GLES2RenderState7releaseEv,0,__ZThn12_N7toadlet6peeper11GLES2Buffer6retainEv,0,__ZThn40_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole15CameraComponentENS2_9ComponentEED1Ev,0,__ZN7toadlet6peeper11GLES2Buffer18getRootIndexBufferEv,0,__ZThn36_NK7toadlet6peeper11GLES2Buffer7getNameEv,0,__ZThn12_N7toadlet7tadpole12WaveStreamerD1Ev,0,__ZThn48_N7toadlet7tadpole13MeshComponent17getNumAttachmentsEv,0,__ZN7toadlet6peeper17GLES2RenderDevice11setViewportERKNS0_8ViewportE,0,__ZN7toadlet7tadpole15CameraComponent6retainEv,0,__ZThn12_N7toadlet6peeper17GLES2VertexFormatD1Ev,0,__ZThn12_N7toadlet6peeper19BackableShaderState7setNameERKNS_3egg6StringE,0,__ZN7toadlet6peeper19BackableShaderState7setBackENS_3egg16IntrusivePointerINS0_11ShaderStateENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper17GLES2RenderDevice14setShaderStateEPNS0_11ShaderStateE,0,__ZThn12_N7toadlet6peeper19BackableShaderState7releaseEv,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet6peeper16GLES2RenderState11setFogStateERKNS0_8FogStateE,0,__ZN7toadlet7tadpole15ResourceManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZThn12_N7toadlet6peeper14BackableShader6retainEv,0,__ZNK7toadlet6peeper11GLES2Buffer16getTextureFormatEv,0,__ZN7toadlet6peeper16GLES2RenderStateD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg3LogEEEvPT_,0,__ZThn40_NK7toadlet6peeper14BackableBuffer15getUniqueHandleEv,0,__ZN7toadlet6peeper17GLES2RenderDevice4swapEv,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreator7releaseEv,0,__ZThn36_N7toadlet6peeper12GLES2TextureD1Ev,0,__ZThn36_N7toadlet6peeper19BackableRenderStateD0Ev,0,__ZN7toadlet7tadpole12HopComponent20addCollisionListenerEPNS0_24PhysicsCollisionListenerE,0,__ZN7toadlet7tadpole6action13BaseAnimation9setWeightEf,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole18SkyDomeMeshCreatorD0Ev,0,__ZN7toadlet3egg17PosixErrorHandler21getStackTraceListenerEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent7destroyEv,0,__ZN7toadlet7tadpole15CameraComponent9getCameraEv,0,__ZThn36_N7toadlet7tadpole12HopComponent17setCollisionScopeEi,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer29castToGLTextureMipPixelBufferEv,0,__ZN7toadlet7tadpole18SkyDomeMeshCreator7destroyEv,0,__ZThn12_N7toadlet7tadpole17SphereMeshCreatorD0Ev,0,__ZThn12_N7toadlet7tadpole19SimpleRenderManager6retainEv,0,__ZN7toadlet6peeper12GLES2Texture17getMipPixelBufferEii,0,__ZN7toadlet7tadpole17SkeletonComponent16transformChangedEPNS0_9TransformE,0,__ZThn12_N7toadlet6peeper13GLES2SLShaderD0Ev,0,__ZN7toadlet6peeper19GLES2SLVertexLayout7destroyEv,0,__ZN7toadlet6peeper18GLES2SLShaderState9getShaderENS0_6Shader10ShaderTypeE,0,__ZThn36_N7toadlet6peeper13GLES2SLShaderD1Ev,0,__ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi,0,__ZN10emscripten8internal13getActualTypeIN7toadlet3hop9SimulatorEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole8material21MaterialLightVariable9getFormatEv,0,__ZNK7toadlet6peeper14BackableBuffer23getVariableBufferFormatEv,0,__ZN7toadlet7tadpole13ShaderManagerD1Ev,0,__ZThn44_N7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZThn44_NK7toadlet6peeper11GLES2Buffer11getDataSizeEv,0,__ZN7toadlet3egg12BaseResourceD1Ev,0,__ZN7toadlet6peeper19GLES2FBOPixelBuffer7releaseEv,0,__ZThn12_N7toadlet6peeper19BackableRenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_NK7toadlet7tadpole13MeshComponent7SubMesh17getRenderMaterialEv,0,__ZN7toadlet3egg17PosixErrorHandler14installHandlerEv,0,__ZN7toadlet3hop9Simulator7releaseEv,0,__ZN7toadlet7tadpole15ResourceManager19setDefaultExtensionERKNS_3egg6StringE,0,__ZN7toadlet7tadpole4Node7releaseEv,0,__ZN7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet6peeper14BackableBuffer19getRootVertexBufferEv,0,__ZN7toadlet7tadpole15MaterialManager20unableToFindStreamerERKNS_3egg6StringEPNS0_12ResourceDataE,0,__ZN7toadlet6peeper19GLES2SLVertexLayout6retainEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_7tadpole15CameraComponentENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN7toadlet7tadpole8material15AmbientVariableD0Ev,0,__ZNK7toadlet7tadpole13MeshComponent8getBoundEv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_6peeper7TextureENS2_25DefaultIntrusiveSemanticsEEEE5shareEPS5_PNS_8internal7_EM_VALE,0,__ZN7toadlet3egg2io10FileStream6retainEv,0,__ZN7toadlet7tadpole12HopComponentD0Ev,0,__ZN7toadlet7tadpole11DDSStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZNK7toadlet7tadpole4Node7getNodeEPKNS_3egg4TypeINS0_9ComponentEEE,0,__ZThn36_N7toadlet6peeper14BackableShader13getRootShaderEv,0,__ZThn48_N7toadlet6peeper14BackableBufferD1Ev,0,__ZN7toadlet3egg2io12MemoryStream6closedEv,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBuffer7releaseEv,0,__ZN7toadlet7tadpole6sensor13SensorResults6retainEv,0,__ZN7toadlet7tadpole5Track4lockEi,0,__ZN7toadlet7tadpole12TMSHStreamer4saveENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEENS3_INS2_8ResourceES6_EEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState21getNumVariableBuffersENS0_6Shader10ShaderTypeE,0,__ZN10emscripten8internal13getActualTypeIN7toadlet6peeper11RenderStateEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet7tadpole15CameraComponentD0Ev,0,__ZN10emscripten8internal13MethodInvokerIMN7toadlet7tadpole5SceneEFvPNS2_6peeper12RenderDeviceEPNS3_6CameraEPNS3_4NodeEEvPS4_JS7_S9_SB_EE6invokeERKSD_SE_S7_S9_SB_,0,__ZThn36_N7toadlet6peeper13GLES2SLShader13getRootShaderEv,0,__ZN7toadlet7tadpole6sensor6Sensor10senseNodesEPNS0_4NodeEPNS0_21SensorResultsListenerE,0,__ZN7toadlet6peeper16GLES2RenderState20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_NK7toadlet6peeper19GLES2FBOPixelBuffer15getUniqueHandleEv,0,__ZN7toadlet6peeper14BackableShader24internal_setUniqueHandleEi,0,__ZThn48_NK7toadlet6peeper11GLES2Buffer9getAccessEv,0,__ZNK7toadlet6peeper19BackableShaderState15getUniqueHandleEv,0,__ZThn44_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZN7toadlet6peeper17GLES2RenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZNK7toadlet7tadpole12HopComponent7getMassEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet6peeper8ViewportEiE7getWireIS4_EEiRKMS4_iRKT_,0,__ZN10emscripten8internal15raw_constructorIN7toadlet3egg4math7Vector2EJEEEPT_DpNS0_11BindingTypeIT0_E8WireTypeE,0,__ZN7toadlet6peeper17GLES2VertexFormat7releaseEv,0,__ZThn12_N7toadlet6peeper23EGL2PBufferRenderTarget7releaseEv,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole6CameraENS3_25DefaultIntrusiveSemanticsEEEJOPNS5_6EngineEEE6invokeEPFS8_SB_ESA_,0,__ZN7toadlet6peeper17GLES2RenderTargetD0Ev,0,__ZN7toadlet6peeper26GLES2TextureMipPixelBuffer6unlockEv,0,__ZN7toadlet6peeper14BackableShaderD1Ev,0,__ZN7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi,0,__ZNK7toadlet6peeper17GLES2VertexFormat15getUniqueHandleEv,0,__ZN7toadlet6peeper17GLES2RenderDevice12createShaderEv,0,__ZThn12_N7toadlet3egg2io12MemoryStream4readEPhi,0,__ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv,0,__ZN7toadlet6peeper13GLES2SLShader6retainEv,0,__ZN7toadlet7tadpole5Scene20setLogicTimeAndFrameEii,0,__ZNK7toadlet6peeper11GLES2Buffer23getVariableBufferFormatEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice13getRenderCapsERNS0_10RenderCapsE,0,__ZNK7toadlet6peeper16GLES2RenderState12getSetStatesEv,0,__ZN7toadlet6peeper10VertexDataD0Ev,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole13PartitionNodeEEEvPT_,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice5resetEv,0,__ZThn36_N7toadlet6peeper11GLES2BufferD1Ev,0,__ZThn36_NK7toadlet6peeper13GLES2SLShader13getShaderTypeEv,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget19getRootRenderTargetEv,0,__ZNK7toadlet6peeper19BackableRenderState19getNumSamplerStatesENS0_6Shader10ShaderTypeE,0,__ZThn12_N7toadlet7tadpole11WaveDecoderD1Ev,0,__ZThn12_N7toadlet3egg2io12MemoryStream8readableEv,0,__ZThn36_N7toadlet6peeper20GLES2FBORenderTarget7releaseEv,0,__ZNK7toadlet7tadpole12HopComponent9getParentEv,0,__ZThn12_N7toadlet7tadpole5Scene18preContextActivateEPNS_6peeper12RenderDeviceE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice15setLightEnabledEib,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi,0,__ZNK7toadlet7tadpole13MeshComponent9getActiveEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer22castToGLFBOPixelBufferEv,0,__ZThn12_NK7toadlet3egg12BaseResource15getUniqueHandleEv,0,__ZN7toadlet7tadpole19SimpleRenderManagerD2Ev,0,__ZThn36_N7toadlet6peeper20BackableVertexFormat7destroyEv,0,__ZThn36_N7toadlet6peeper26GLES2TextureMipPixelBuffer6retainEv,0,__ZThn52_N7toadlet7tadpole17SkeletonComponent16getNumAnimationsEv,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math10QuaternionEfE7getWireIS5_EEfRKMS5_fRKT_,0,__ZN7toadlet3hop5Shape7releaseEv,0,__ZThn36_N7toadlet6peeper17GLES2RenderTargetD0Ev,0,__ZN7toadlet7tadpole13RenderableSetD2Ev,0,__ZN7toadlet3egg2io10FileStream4readEPhi,0,__ZThn12_N7toadlet7tadpole13PartitionNode7releaseEv,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_7tadpole13MeshComponentENS3_25DefaultIntrusiveSemanticsEEEJOPNS5_6EngineEEE6invokeEPFS8_SB_ESA_,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget6retainEv,0,__ZN7toadlet7tadpole11WaveDecoder5writeEPKhi,0,__ZN7toadlet7tadpole13MeshComponent11setRenderedEb,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole8material21LightSpecularVariable6updateEPhPNS1_15SceneParametersE,0,__ZN7toadlet6peeper13GLES2SLShader7releaseEv,0,__ZN7toadlet7tadpole21SkyBoxMaterialCreatorD0Ev,0,__ZThn12_N7toadlet7tadpole27NormalizationTextureCreatorD0Ev,0,__ZThn12_N7toadlet6peeper19BackableShaderStateD1Ev,0,__ZN7toadlet6peeper29BackableTextureMipPixelBuffer6unlockEv,0,__ZN7toadlet3egg7Errorer8endTraceEv,0,__ZThn12_N7toadlet6peeper14BackableBuffer6retainEv,0,__ZN7toadlet6peeper17GLES2RenderDevice15copyPixelBufferEPNS0_11PixelBufferES3_,0,__ZNK7toadlet7tadpole4Node10getSpacialEi,0,__ZN7toadlet7tadpole22DiffuseMaterialCreatorD0Ev,0,__ZN7toadlet6peeper20BackableVertexFormat19getRootVertexFormatEv,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer18getRootPixelBufferEv,0,__ZNK7toadlet6peeper20BackableVertexFormat18getElementSemanticEi,0,__ZN7toadlet7tadpole6Camera15setRenderTargetEPNS_6peeper12RenderTargetE,0,__ZN7toadlet7tadpole12HopComponent10setGravityEf,0,__ZN7toadlet7tadpole13BaseComponent6retainEv,0,__ZN7toadlet7tadpole13MeshComponent18getAttachmentIndexERKNS_3egg6StringE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice14setRenderStateEPNS0_11RenderStateE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice10beginSceneEv,0,__ZThn40_NK7toadlet6peeper14BackableBuffer8getUsageEv,0,__ZThn44_N7toadlet6peeper11GLES2Buffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole11DDSStreamerD0Ev,0,__ZThn36_NK7toadlet6peeper12GLES2Texture8getUsageEv,0,__ZThn48_N7toadlet6peeper14BackableBuffer7setNameERKNS_3egg6StringE,0,__ZN7toadlet7tadpole19SimpleRenderManager22interRenderRenderablesEPNS0_13RenderableSetEPNS_6peeper12RenderDeviceEPNS0_6CameraEb,0,__ZThn12_N7toadlet6peeper19GLES2FBOPixelBufferD0Ev,0,__ZThn36_N7toadlet6peeper19BackableShaderState7destroyEv,0,__ZNK7toadlet7tadpole4Node8getScopeEv,0,__ZN7toadlet7tadpole6sensor13SensorResults16sensingBeginningEv,0,__ZThn12_N7toadlet7tadpole12HopComponent11rootChangedEPNS0_4NodeE,0,__ZThn12_N7toadlet7tadpole17SkyBoxMeshCreator7releaseEv,0,__ZN10emscripten8make_ptrIN7toadlet7tadpole5SceneEJPNS2_6EngineEEEENS1_3egg16IntrusivePointerIT_NS6_25DefaultIntrusiveSemanticsEEEDpOT0_,0,__ZN7toadlet6peeper14BackableShader13getRootShaderEv,0,__ZN7toadlet7tadpole14TextureManagerD2Ev,0,__ZNKSt9bad_alloc4whatEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponent7releaseEv,0,__ZThn12_N7toadlet7tadpole11WaveDecoder7releaseEv,0,__ZNK10__cxxabiv123__fundamental_type_info9can_catchEPKNS_16__shim_type_infoERPv,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole9TransformENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet7tadpole12HopComponent9collisionERKNS_3hop9CollisionE,0,__ZThn12_N7toadlet6peeper13GLES2SLShader24internal_setUniqueHandleEi,0,__ZThn40_N7toadlet6peeper11GLES2Buffer6updateEPhii,0,__ZN7toadlet3egg2io12MemoryStreamD2Ev,0,__ZN7toadlet7tadpole19SimpleRenderManager16renderQueueItemsEPNS0_8material8MaterialEPKNS0_13RenderableSet19RenderableQueueItemEi,0,__ZThn36_NK7toadlet7tadpole12HopComponent11getVelocityEv,0,__ZN7toadlet7tadpole6Engine19setHasBackableFixedEb,0,__ZN7toadlet6peeper11GLES2Buffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet7tadpole13PartitionNode7getTypeEv,0,__ZN7toadlet3egg2io10FileStream5flushEv,0,__ZN7toadlet6peeper14BackableBuffer6updateEPhii,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer22castToGLFBOPixelBufferEv,0,__ZThn36_NK7toadlet6peeper20BackableVertexFormat18getElementSemanticEi,0,__ZThn36_N7toadlet6peeper19BackableShaderState9setShaderENS0_6Shader10ShaderTypeENS_3egg16IntrusivePointerIS2_NS4_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet7tadpole4Mesh7SubMesh15getUniqueHandleEv,0,__ZN7toadlet3egg2io10FileStream5closeEv,0,__ZN7toadlet7tadpole4Node8getNodesEv,0,__ZN7toadlet6peeper19BackableShaderState9setShaderENS0_6Shader10ShaderTypeENS_3egg16IntrusivePointerIS2_NS4_25DefaultIntrusiveSemanticsEEE,0,__ZNK7toadlet6peeper19BackableRenderState15getSamplerStateENS0_6Shader10ShaderTypeEiRNS0_12SamplerStateE,0,__ZThn36_N7toadlet6peeper17GLES2VertexFormat10addElementERKNS_3egg6StringEii,0,__ZThn12_N7toadlet7tadpole11DDSStreamer4loadENS_3egg16IntrusivePointerINS2_2io6StreamENS2_25DefaultIntrusiveSemanticsEEEPNS0_12ResourceDataEPNS0_16ProgressListenerE,0,__ZThn36_N7toadlet6peeper17GLES2RenderTarget19getRootRenderTargetEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget11resetCreateEv,0,__ZNK7toadlet7tadpole5Scene10getEpsilonEv,0,__ZThn36_NK7toadlet6peeper22EGL2WindowRenderTarget8getWidthEv,0,__ZN7toadlet7tadpole4Node12setMatrix4x4ERKNS_3egg4math9Matrix4x4E,0,__ZThn12_N7toadlet7tadpole4NodeD1Ev,0,__ZN7toadlet7tadpole19SimpleRenderManager17gatherRenderablesEPNS0_13RenderableSetEPNS0_4NodeEPNS0_6CameraE,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice11createQueryEv,0,__ZN7toadlet3egg2io10FileStream7releaseEv,0,__ZN7toadlet7tadpole13BufferManager17resourceDestroyedEPNS_3egg8ResourceE,0,__ZN7toadlet6peeper14BackableBuffer7setBackENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceE,0,__ZN7toadlet7tadpole4Mesh10addSubMeshENS_3egg16IntrusivePointerINS1_7SubMeshENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN10emscripten8internal13getActualTypeIN7toadlet6peeper7TextureEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet6peeper13GLES2SLShader12resourceThisEv,0,__ZN7toadlet7tadpole10HopManager10postUpdateEif,0,__ZThn12_N7toadlet6peeper16GLES2RenderStateD0Ev,0,__ZThn12_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv,0,__ZN7toadlet3egg2io10FileStream4seekEi,0,__ZThn40_N7toadlet6peeper14BackableBuffer6retainEv,0,__ZN7toadlet7tadpole11WaveDecoder7releaseEv,0,__ZN7toadlet7tadpole4Mesh12setTransformENS_3egg16IntrusivePointerINS0_9TransformENS2_25DefaultIntrusiveSemanticsEEE,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole17SkeletonComponentENS2_9ComponentEED1Ev,0,__ZN7toadlet7tadpole8material14ScalarVariableD1Ev,0,__ZThn12_N7toadlet6peeper20BackableVertexFormat6retainEv,0,__ZN7toadlet6peeper14BackableBuffer7destroyEv,0,__ZN7toadlet6peeper20GLES2FBORenderTarget6attachENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEENS0_23PixelBufferRenderTarget10AttachmentE,0,__ZThn40_N7toadlet7tadpole12HopComponentD0Ev,0,__ZN10emscripten8internal12MemberAccessIN7toadlet3egg4math7Vector2EfE7setWireIS5_EEvRKMS5_fRT_f,0,__ZN7toadlet7tadpole8Sequence7setNameERKNS_3egg6StringE,0,__ZThn40_NK7toadlet7tadpole12HopComponent8getBoundEv,0,__ZN7toadlet6peeper15BackableTexture12resourceThisEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer7releaseEv,0,__ZThn36_N7toadlet6peeper12GLES2Texture7releaseEv,0,__ZN7toadlet6peeper17GLES2RenderDevice15renderPrimitiveEPNS0_10VertexDataEPNS0_9IndexDataE,0,__ZN7toadlet6peeper19GLES2FBOPixelBufferD1Ev,0,__ZNK7toadlet6peeper17GLES2VertexFormat13getVertexSizeEv,0,__ZN7toadlet7tadpole14ArchiveManager6manageEPNS_3egg8ResourceERKNS2_6StringE,0,__ZN7toadlet6peeper12GLES2Texture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZN7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZThn12_N7toadlet7tadpole13MeshComponent13parentChangedEPNS0_4NodeE,0,__ZN7toadlet6peeper13TextureFormatD0Ev,0,__ZN7toadlet6peeper14BackableBuffer24internal_setUniqueHandleEi,0,__ZThn36_NK7toadlet6peeper16GLES2RenderState18getRasterizerStateERNS0_15RasterizerStateE,0,__ZNK7toadlet7tadpole9Transform8getScaleEv,0,__ZNK7toadlet6peeper11GLES2Buffer7getSizeEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole13MeshComponentEEEvPT_,0,__ZN7toadlet6peeper20GLES2FBORenderTarget4swapEv,0,__ZThn36_NK7toadlet7tadpole5Track7getNameEv,0,__ZThn44_NK7toadlet6peeper14BackableBuffer7getNameEv,0,__ZN7toadlet7tadpole11NodeManager6retainEv,0,__ZNK7toadlet6peeper16GLES2RenderState7getNameEv,0,__ZN7toadlet3hop5Solid7destroyEv,0,__ZN7toadlet7tadpole12HopComponent12traceSegmentERNS_3hop9CollisionERKNS_3egg4math7Vector3ERKNS6_7SegmentE,0,__ZN10emscripten8internal14raw_destructorIN7toadlet3egg16IntrusivePointerINS2_6peeper7TextureENS3_25DefaultIntrusiveSemanticsEEEEEvPT_,0,__ZN10emscripten8internal15raw_constructorIN7toadlet6peeper8ViewportEJEEEPT_DpNS0_11BindingTypeIT0_E8WireTypeE,0,__ZN7toadlet6peeper17GLES2RenderDevice5clearEiRKNS_3egg4math7Vector4E,0,__ZThn12_N7toadlet7tadpole10HopManager6retainEv,0,__ZNK7toadlet6peeper26GLES2TextureMipPixelBuffer11getDataSizeEv,0,__ZThn12_N7toadlet6peeper12GLES2Texture7releaseEv,0,__ZN7toadlet7tadpole27NormalizationTextureCreatorD0Ev,0,__ZN7toadlet6peeper14BackableBuffer6unlockEv,0,__ZN7toadlet7tadpole13BufferManager6retainEv,0,__ZThn40_N7toadlet6peeper11GLES2Buffer11resetCreateEv,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget14getAdaptorInfoEv,0,__ZN7toadlet6peeper17GLES2RenderDevice10beginSceneEv,0,__ZN7toadlet6peeper17GLES2RenderTarget8activateEv,0,__ZN7toadlet6peeper14BackableBuffer7getBackEv,0,__ZN7toadlet7tadpole8Skeleton20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE,0,__ZThn12_N7toadlet7tadpole13MeshComponentD0Ev,0,__ZN7toadlet6peeper9IndexDataD0Ev,0,__ZN7toadlet7tadpole4Node14stopAllActionsEv,0,__ZThn48_N7toadlet6peeper11GLES2Buffer4lockEi,0,__ZN7toadlet6peeper20GLES2FBORenderTarget6createEv,0,__ZN10emscripten8internal14raw_destructorIN7toadlet7tadpole4NodeEEEvPT_,0,__ZNK7toadlet7tadpole8Sequence7getNameEv,0,__ZN7toadlet6peeper15BackableTextureD0Ev,0,__ZThn12_N7toadlet3egg2io12MemoryStream9writeableEv,0,__ZN7toadlet6peeper17GLES2RenderTarget10deactivateEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole5SceneEEEPKNS0_7_TYPEIDEPT_,0,__ZN7toadlet3egg19NonInstantiableTypeINS_7tadpole12HopComponentENS2_9ComponentEED0Ev,0,__ZThn36_NK7toadlet6peeper14BackableShader10getProfileEv,0,__ZN10emscripten8internal13getActualTypeIN7toadlet7tadpole6CameraEEEPKNS0_7_TYPEIDEPT_,0,__ZThn12_N7toadlet7tadpole22DiffuseMaterialCreator7releaseEv,0,__ZNK7toadlet7tadpole5Track7getSizeEv,0,__ZN7toadlet6peeper9IndexData6retainEv,0,__ZThn36_NK7toadlet6peeper23EGL2PBufferRenderTarget14getAdaptorInfoEv,0,__ZN7toadlet6peeper17GLES2RenderDevice19getShadowBiasMatrixEPKNS0_7TextureERNS_3egg4math9Matrix4x4E,0,__ZN7toadlet6peeper14BackableBufferD1Ev,0,__ZN7toadlet6peeper14BackableBuffer6createEiiNS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEEi,0,__ZThn36_NK7toadlet6peeper14BackableBuffer14getIndexFormatEv,0,__ZThn12_NK7toadlet7tadpole5Track7getNameEv,0,__ZThn12_N7toadlet7tadpole13MeshComponent6retainEv,0,__ZThn12_N7toadlet7tadpole17SkeletonComponentD0Ev,0,__ZNK7toadlet7tadpole4Node13getAnimatableEi,0,__ZN7toadlet6peeper19GLES2SLVertexLayout7releaseEv,0,__ZN7toadlet6peeper14BackableShader12resourceThisEv,0,__ZN7toadlet7tadpole8material8Material7releaseEv,0,__ZN7toadlet6peeper19BackableRenderStateD1Ev,0,__ZThn12_N7toadlet6peeper17GLES2RenderDevice11setViewportERKNS0_8ViewportE,0,__ZThn36_NK7toadlet6peeper26GLES2TextureMipPixelBuffer11getDataSizeEv,0,__ZN7toadlet7tadpole8material19ModelMatrixVariable6updateEPhPNS1_15SceneParametersE,0,__ZThn44_N7toadlet6peeper20GLES2FBORenderTarget12resetDestroyEv,0,__ZThn12_N7toadlet3egg2io10FileStream8readableEv,0,__ZNK7toadlet7tadpole4Mesh15getUniqueHandleEv,0,__ZN7toadlet7tadpole11BMPStreamer6retainEv,0,__ZThn12_N7toadlet6peeper19BackableShaderState6retainEv,0,__ZThn40_N7toadlet6peeper14BackableBuffer4lockEi,0,__ZThn36_N7toadlet6peeper23EGL2PBufferRenderTarget24internal_setUniqueHandleEi,0,__ZN7toadlet3egg2io12MemoryStream6retainEv,0,__ZN7toadlet7tadpole6sensor20BoundingVolumeSensorD0Ev,0,__ZN7toadlet7tadpole5Scene6updateEi,0,__ZThn12_NK7toadlet6peeper11GLES2Buffer15getUniqueHandleEv,0,__ZThn36_NK7toadlet7tadpole5Track11getDataSizeEv,0,__ZThn12_N7toadlet6peeper19BackableShaderState24internal_setUniqueHandleEi,0,__ZNK7toadlet6peeper12GLES2Texture9getFormatEv,0,__ZThn12_N7toadlet3egg2io12MemoryStreamD0Ev,0,__ZNK7toadlet6peeper17GLES2RenderTarget9getHeightEv,0,__ZN7toadlet6peeper23EGL2PBufferRenderTarget11resetCreateEv,0,__ZNK7toadlet6peeper19GLES2FBOPixelBuffer7getNameEv,0,__ZThn36_NK7toadlet6peeper15BackableTexture15getUniqueHandleEv,0,__ZN10emscripten8internal7InvokerIN7toadlet3egg16IntrusivePointerINS2_3hop9SimulatorENS3_25DefaultIntrusiveSemanticsEEEJEE6invokeEPFS8_vE,0,__ZThn36_N7toadlet6peeper16GLES2RenderState11setFogStateERKNS0_8FogStateE,0,__ZN10emscripten15smart_ptr_traitIN7toadlet3egg16IntrusivePointerINS1_7tadpole13MeshComponentENS2_25DefaultIntrusiveSemanticsEEEE3getERKS7_,0,__ZN7toadlet7tadpole15CameraComponent16transformChangedEPNS0_9TransformE,0,__ZThn12_N7toadlet3egg2io12MemoryStream5flushEv,0,__ZThn60_NK7toadlet6peeper23EGL2PBufferRenderTarget8getWidthEv,0,__ZNK7toadlet6peeper13GLES2SLShader10getProfileEv,0,__ZThn36_N7toadlet7tadpole12HopComponent10setGravityEf,0,__ZThn48_N7toadlet7tadpole17SkeletonComponentD1Ev,0,__ZThn12_NK7toadlet7tadpole13PartitionNode7getTypeEv,0,__ZN7toadlet6peeper17GLES2RenderDevice18createVertexBufferEv,0,__ZN7toadlet7tadpole13MeshComponent13parentChangedEPNS0_4NodeE,0,__ZThn36_N7toadlet6peeper19GLES2FBOPixelBuffer4lockEi,0,__ZThn12_N7toadlet3egg2io10DataStream9writeableEv,0,__ZN7toadlet6peeper16GLES2RenderState7releaseEv,0,__ZN7toadlet7tadpole20WaterMaterialCreator7destroyEv,0,__ZThn36_N7toadlet6peeper18GLES2SLShaderState7destroyEv,0];
// EMSCRIPTEN_START_FUNCS
function __ZN7toadlet6peeper15BackableTexture7destroyEv(r1){var r2,r3,r4,r5;r2=r1+56|0;r3=r1+64|0;r4=0;while(1){if((r4|0)>=(HEAP32[r2>>2]|0)){break}r5=HEAP32[HEAP32[r3>>2]+(r4<<2)>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+16>>2]](r5)}r4=r4+1|0}__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper11PixelBufferENS0_25DefaultIntrusiveSemanticsEEEE5clearEv(r1+56|0);r4=HEAP32[r1+52>>2];if((r4|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+16>>2]](r4);r4=r1+52|0;r3=HEAP32[r4>>2];if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r4>>2]=0}HEAP32[r4>>2]=0}r4=r1+48|0;r3=HEAP32[r4>>2];if((r3|0)!=0){_free(r3);HEAP32[r4>>2]=0}r4=r1+16|0;r3=HEAP32[r4>>2];if((r3|0)==0){return}r2=HEAP32[HEAP32[r3>>2]+8>>2];r5=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+24>>2]](r1|0);FUNCTION_TABLE[r2](r3,r5);HEAP32[r4>>2]=0;return}function __ZThn12_N7toadlet6peeper15BackableTexture7destroyEv(r1){__ZN7toadlet6peeper15BackableTexture7destroyEv(r1-84+72|0);return}function __ZThn36_N7toadlet6peeper15BackableTexture7destroyEv(r1){__ZN7toadlet6peeper15BackableTexture7destroyEv(r1-84+48|0);return}function __ZN7toadlet6peeper15BackableTexture11resetCreateEv(r1){var r2,r3,r4;r2=HEAP32[r1+52>>2];if((r2|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+48>>2]](r2);r2=r1+56|0;r3=r1+64|0;r1=0;while(1){if((r1|0)>=(HEAP32[r2>>2]|0)){break}r4=HEAP32[HEAP32[r3>>2]+(r1<<2)>>2];if((r4|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+76>>2]](r4)}r1=r1+1|0}return}function __ZThn36_N7toadlet6peeper15BackableTexture11resetCreateEv(r1){var r2,r3,r4;r2=r1-84+48|0;r1=HEAP32[r2+52>>2];if((r1|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+48>>2]](r1);r1=r2+56|0;r3=r2+64|0;r2=0;while(1){if((r2|0)>=(HEAP32[r1>>2]|0)){break}r4=HEAP32[HEAP32[r3>>2]+(r2<<2)>>2];if((r4|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+76>>2]](r4)}r2=r2+1|0}return}function __ZN7toadlet6peeper15BackableTexture12resetDestroyEv(r1){var r2,r3,r4,r5;r2=r1+52|0;if((HEAP32[r2>>2]|0)==0){return}r3=r1+56|0;r4=r1+64|0;r1=0;while(1){if((r1|0)>=(HEAP32[r3>>2]|0)){break}r5=HEAP32[HEAP32[r4>>2]+(r1<<2)>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+80>>2]](r5)}r1=r1+1|0}r1=HEAP32[r2>>2];FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+52>>2]](r1);return}function __ZThn36_N7toadlet6peeper15BackableTexture12resetDestroyEv(r1){var r2,r3,r4,r5;r2=r1-84+48|0;r1=r2+52|0;if((HEAP32[r1>>2]|0)==0){return}r3=r2+56|0;r4=r2+64|0;r2=0;while(1){if((r2|0)>=(HEAP32[r3>>2]|0)){break}r5=HEAP32[HEAP32[r4>>2]+(r2<<2)>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+80>>2]](r5)}r2=r2+1|0}r2=HEAP32[r1>>2];FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+52>>2]](r2);return}function __ZN7toadlet6peeper15BackableTexture17getMipPixelBufferEii(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10,r11,r12,r13;r5=STACKTOP;STACKTOP=STACKTOP+8|0;r6=r5;if((HEAP32[HEAP32[r2+44>>2]+12>>2]|0)==4){r7=(r3*6&-1)+r4|0}else{r7=r3}if((HEAP32[r2+56>>2]|0)<=(r7|0)){__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper11PixelBufferENS0_25DefaultIntrusiveSemanticsEEEE6resizeEi(r2+56|0,r7+1|0)}r8=r2+64|0;do{if((HEAP32[HEAP32[r8>>2]+(r7<<2)>>2]|0)==0){r9=__Z14toadlet_mallocii(68);HEAP32[r9+4>>2]=0;HEAP32[r9+8>>2]=0;HEAP32[r9+16>>2]=0;HEAP32[r9+20>>2]=0;HEAP32[r9+24>>2]=52432;HEAP32[r9+28>>2]=52432;HEAP32[r9+32>>2]=0;r10=r9+36|0;HEAP32[r9>>2]=27904;HEAP32[r9+12>>2]=28016;HEAP32[r10>>2]=28064;HEAP32[r9+40>>2]=r2;HEAP32[r9+44>>2]=r3;HEAP32[r9+48>>2]=r4;r11=r9+52|0;HEAP32[r11>>2]=0;HEAP32[r11+4>>2]=0;HEAP32[r11+8>>2]=0;HEAP32[r11+12>>2]=0;__ZN7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv(r9);do{if((r9|0)==0){HEAP32[r6>>2]=0}else{HEAP32[r6>>2]=r10;if((r10|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+8>>2]](r10)}}while(0);r10=HEAP32[r8>>2]+(r7<<2)|0;do{if((r10|0)==(r6|0)){r12=r6|0}else{r9=r10|0;r11=HEAP32[r9>>2];if((r11|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r11>>2]+12>>2]](r11|0);HEAP32[r9>>2]=0}r11=r6|0;r13=HEAP32[r11>>2];HEAP32[r9>>2]=r13;if((r13|0)==0){r12=r11;break}FUNCTION_TABLE[HEAP32[HEAP32[r13>>2]+8>>2]](r13|0);r12=r11}}while(0);r10=HEAP32[r12>>2];if((r10|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+12>>2]](r10|0);HEAP32[r12>>2]=0}}while(0);r12=HEAP32[HEAP32[r8>>2]+(r7<<2)>>2];HEAP32[r1>>2]=r12;if((r12|0)==0){STACKTOP=r5;return}FUNCTION_TABLE[HEAP32[HEAP32[r12>>2]+8>>2]](r12|0);STACKTOP=r5;return}function __ZThn36_N7toadlet6peeper15BackableTexture17getMipPixelBufferEii(r1,r2,r3,r4){__ZN7toadlet6peeper15BackableTexture17getMipPixelBufferEii(r1,r2-84+48|0,r3,r4);return}function __ZN7toadlet6peeper15BackableTexture4loadEPNS0_13TextureFormatEPh(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41,r42,r43,r44,r45,r46,r47,r48,r49,r50,r51,r52,r53,r54,r55,r56,r57,r58,r59,r60,r61,r62,r63,r64,r65,r66,r67,r68,r69,r70,r71,r72,r73,r74,r75,r76,r77,r78,r79,r80,r81,r82,r83,r84,r85,r86,r87,r88,r89,r90,r91,r92,r93,r94,r95,r96,r97,r98,r99,r100,r101,r102,r103,r104,r105,r106,r107,r108,r109,r110,r111,r112,r113,r114,r115,r116,r117,r118,r119,r120,r121,r122,r123,r124,r125,r126,r127,r128,r129,r130,r131,r132,r133,r134,r135,r136,r137,r138,r139,r140,r141,r142,r143,r144,r145,r146,r147,r148,r149,r150,r151,r152,r153,r154,r155,r156,r157,r158,r159,r160,r161,r162,r163,r164,r165,r166,r167,r168,r169,r170,r171,r172,r173,r174,r175,r176,r177,r178,r179,r180,r181,r182,r183,r184,r185,r186,r187,r188,r189,r190,r191,r192,r193,r194,r195,r196,r197,r198,r199,r200,r201,r202,r203,r204,r205,r206,r207,r208,r209,r210,r211,r212,r213,r214,r215,r216,r217,r218,r219,r220,r221,r222,r223,r224;r4=0;r5=STACKTOP;STACKTOP=STACKTOP+16|0;r6=r5;r7=r5+8;r8=r1+52|0;r9=HEAP32[r8>>2];r10=(r9|0)==0;if(r10){r11=0}else{r12=r9|0;r13=r9;r14=HEAP32[r13>>2];r15=r14+8|0;r16=HEAP32[r15>>2];r17=FUNCTION_TABLE[r16](r12);r18=r6;r19=r7;r20=r9;r21=HEAP32[r20>>2];r22=r21+60|0;r23=HEAP32[r22>>2];FUNCTION_TABLE[r23](r6,r9);r24=r6|0;r25=HEAP32[r24>>2];r26=r25+16|0;r27=HEAP32[r26>>2];r28=r2+16|0;r29=HEAP32[r28>>2];r30=(r27|0)==(r29|0);r31=(r25|0)==0;if(!r31){r32=r25;r33=HEAP32[r32>>2];r34=r33+12|0;r35=HEAP32[r34>>2];r36=FUNCTION_TABLE[r35](r25);HEAP32[r24>>2]=0}do{if(r30){r37=r9;r38=HEAP32[r37>>2];r39=r38+68|0;r40=HEAP32[r39>>2];r41=FUNCTION_TABLE[r40](r9,r2,r3);r42=r41}else{r43=__Z14toadlet_mallocii(64);r44=r43;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r44,r2,-1);r45=HEAP32[r20>>2];r46=r45+60|0;r47=HEAP32[r46>>2];FUNCTION_TABLE[r47](r7,r9);r48=r7|0;r49=HEAP32[r48>>2];r50=r49+16|0;r51=HEAP32[r50>>2];r52=r43+16|0;r53=r52;HEAP32[r53>>2]=r51;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r44);r54=HEAP32[r48>>2];r55=(r54|0)==0;if(!r55){r56=r54;r57=HEAP32[r56>>2];r58=r57+12|0;r59=HEAP32[r58>>2];r60=FUNCTION_TABLE[r59](r54);HEAP32[r48>>2]=0}r61=r43+52|0;r62=r61;r63=HEAP32[r62>>2];r64=(r63|0)>-1;r65=r64?r63:-1;r66=(r65|0)==0;r67=r66?1:r65;while(1){r68=_malloc(r67);r69=(r68|0)==0;if(!r69){break}r70=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);r71=(r70|0)==0;if(r71){r4=831;break}r72=r70;FUNCTION_TABLE[r72]()}if(r4==831){r73=___cxa_allocate_exception(4);r74=r73;HEAP32[r74>>2]=19680;___cxa_throw(r73,46792,4624)}r75=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r3,r2,r68,r44);r76=r9;r77=HEAP32[r76>>2];r78=r77+68|0;r79=HEAP32[r78>>2];r80=FUNCTION_TABLE[r79](r9,r44,r68);_free(r68);r81=(r43|0)==0;if(r81){r42=r80;break}r82=r43;r83=HEAP32[r82>>2];r84=r83+4|0;r85=HEAP32[r84>>2];FUNCTION_TABLE[r85](r44);r42=r80}}while(0);r86=r42&1;r87=r9|0;r88=r9;r89=HEAP32[r88>>2];r90=r89+12|0;r91=HEAP32[r90>>2];r92=FUNCTION_TABLE[r91](r87);r11=r86}r93=r2+56|0;r94=HEAP32[r93>>2];r95=(r94|0)==0;if(!r95){r96=r11;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}r99=r1+48|0;r100=HEAP32[r99>>2];r101=(r100|0)==0;if(r101){r96=r11;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}r102=r2+32|0;r103=HEAP32[r102>>2];r104=r2+20|0;r105=HEAP32[r104>>2];r106=r103-r105|0;r107=r1+44|0;r108=HEAP32[r107>>2];r109=r108+32|0;r110=HEAP32[r109>>2];r111=r108+20|0;r112=HEAP32[r111>>2];r113=r110-r112|0;r114=(r106|0)==(r113|0);do{if(r114){r115=r2+36|0;r116=HEAP32[r115>>2];r117=r2+24|0;r118=HEAP32[r117>>2];r119=r116-r118|0;r120=r108+36|0;r121=HEAP32[r120>>2];r122=r108+24|0;r123=HEAP32[r122>>2];r124=r121-r123|0;r125=(r119|0)==(r124|0);if(!r125){break}r126=r2+40|0;r127=HEAP32[r126>>2];r128=r2+28|0;r129=HEAP32[r128>>2];r130=r127-r129|0;r131=r108+40|0;r132=HEAP32[r131>>2];r133=r108+28|0;r134=HEAP32[r133>>2];r135=r132-r134|0;r136=(r130|0)==(r135|0);if(!r136){break}r137=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r3,r2,r100,r108);r138=r137&1;r96=r138;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}}while(0);r139=r2+16|0;r140=HEAP32[r139>>2];r141=r108+16|0;r142=HEAP32[r141>>2];r143=(r140|0)==(r142|0);do{if(r143){r144=r11;r145=r3;r146=r2}else{r147=__Z14toadlet_mallocii(64);r148=r147;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r148,r2,-1);r149=HEAP32[r107>>2];r150=r149+16|0;r151=HEAP32[r150>>2];r152=r147+16|0;r153=r152;HEAP32[r153>>2]=r151;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r148);r154=r147+52|0;r155=r154;r156=HEAP32[r155>>2];r157=(r156|0)>-1;r158=r157?r156:-1;r159=(r158|0)==0;r160=r159?1:r158;while(1){r161=_malloc(r160);r162=(r161|0)==0;if(!r162){r4=867;break}r163=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);r164=(r163|0)==0;if(r164){break}r165=r163;FUNCTION_TABLE[r165]()}if(r4==867){r166=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r3,r2,r161,r148);r167=r166&1;r144=r167;r145=r161;r146=r148;break}r168=___cxa_allocate_exception(4);r169=r168;HEAP32[r169>>2]=19680;___cxa_throw(r168,46792,4624)}}while(0);r170=r146+40|0;r171=r146+28|0;r172=r146+36|0;r173=r146+24|0;r174=r146+20|0;r175=r146+48|0;r176=r146+44|0;r177=0;while(1){r178=HEAP32[r170>>2];r179=HEAP32[r171>>2];r180=r178-r179|0;r181=(r177|0)<(r180|0);if(r181){r182=0}else{break}while(1){r183=HEAP32[r172>>2];r184=HEAP32[r173>>2];r185=r183-r184|0;r186=(r182|0)<(r185|0);if(!r186){break}r187=HEAP32[r99>>2];r188=HEAP32[r107>>2];r189=HEAP32[r174>>2];r190=r184+r182|0;r191=HEAP32[r171>>2];r192=r191+r177|0;r193=r188+48|0;r194=HEAP32[r193>>2];r195=Math_imul(r194,r192)|0;r196=r188+44|0;r197=HEAP32[r196>>2];r198=Math_imul(r197,r190)|0;r199=r195+r198|0;r200=r188+16|0;r201=HEAP32[r200>>2];switch(r201|0){case 259:{r202=2;break};case 262:case 263:{r202=3;break};case 264:case 265:{r202=4;break};case 1798:case 1799:case 2056:case 2057:case 2568:case 2569:{r202=2;break};case 1537:case 1538:{r202=4;break};case 1539:{r202=8;break};case 1542:case 1543:{r202=12;break};case 1544:case 1545:{r202=16;break};case 257:case 258:{r202=1;break};default:{r202=0}}r203=Math_imul(r189,r202)|0;r204=r199+r203|0;r205=r187+r204|0;r206=HEAP32[r175>>2];r207=Math_imul(r206,r177)|0;r208=HEAP32[r176>>2];r209=Math_imul(r208,r182)|0;r210=r207+r209|0;r211=r145+r210|0;_memcpy(r205,r211,r208)|0;r212=r182+1|0;r182=r212}r213=r177+1|0;r177=r213}r214=HEAP32[r139>>2];r215=HEAP32[r107>>2];r216=r215+16|0;r217=HEAP32[r216>>2];r218=(r214|0)==(r217|0);if(r218){r96=r144;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}r219=(r146|0)==0;if(!r219){r220=r146;r221=HEAP32[r220>>2];r222=r221+4|0;r223=HEAP32[r222>>2];FUNCTION_TABLE[r223](r146)}r224=(r145|0)==0;if(r224){r96=r144;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}_free(r145);r96=r144;r97=r96&1;r98=r97<<24>>24!=0;STACKTOP=r5;return r98}function __ZThn36_N7toadlet6peeper15BackableTexture4loadEPNS0_13TextureFormatEPh(r1,r2,r3){return __ZN7toadlet6peeper15BackableTexture4loadEPNS0_13TextureFormatEPh(r1-84+48|0,r2,r3)}function __ZN7toadlet6peeper15BackableTexture4readEPNS0_13TextureFormatEPh(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41,r42,r43,r44,r45,r46,r47,r48,r49,r50,r51,r52,r53,r54,r55,r56,r57,r58,r59,r60,r61,r62,r63,r64,r65,r66,r67,r68,r69,r70,r71,r72,r73,r74,r75,r76,r77,r78,r79,r80,r81,r82,r83,r84,r85,r86,r87,r88,r89,r90,r91,r92,r93,r94,r95,r96,r97,r98,r99,r100,r101,r102,r103,r104,r105,r106,r107,r108,r109,r110,r111,r112,r113,r114,r115,r116,r117,r118,r119,r120,r121,r122,r123,r124,r125,r126,r127,r128,r129,r130,r131,r132,r133,r134,r135,r136,r137,r138,r139,r140,r141,r142,r143,r144,r145,r146,r147,r148,r149,r150,r151,r152,r153,r154,r155,r156,r157,r158,r159,r160,r161,r162,r163,r164,r165,r166,r167,r168,r169,r170,r171,r172,r173,r174,r175,r176,r177,r178,r179,r180,r181,r182,r183,r184,r185,r186,r187,r188,r189,r190,r191,r192,r193,r194,r195,r196,r197,r198,r199,r200,r201,r202,r203,r204,r205,r206,r207,r208,r209,r210,r211,r212,r213,r214,r215,r216,r217,r218,r219,r220,r221,r222,r223,r224,r225,r226,r227,r228,r229,r230,r231,r232,r233;r4=0;r5=STACKTOP;STACKTOP=STACKTOP+24|0;r6=r5;r7=r5+8;r8=r5+16;r9=r1+52|0;r10=HEAP32[r9>>2];r11=(r10|0)==0;if(!r11){r12=r10|0;r13=r10;r14=HEAP32[r13>>2];r15=r14+8|0;r16=HEAP32[r15>>2];r17=FUNCTION_TABLE[r16](r12);r18=r6;r19=r7;r20=r8;r21=r10;r22=HEAP32[r21>>2];r23=r22+60|0;r24=HEAP32[r23>>2];FUNCTION_TABLE[r24](r6,r10);r25=r6|0;r26=HEAP32[r25>>2];r27=r26+16|0;r28=HEAP32[r27>>2];r29=r2+16|0;r30=HEAP32[r29>>2];r31=(r28|0)==(r30|0);r32=(r26|0)==0;if(!r32){r33=r26;r34=HEAP32[r33>>2];r35=r34+12|0;r36=HEAP32[r35>>2];r37=FUNCTION_TABLE[r36](r26);HEAP32[r25>>2]=0}if(r31){r38=r10;r39=HEAP32[r38>>2];r40=r39+72|0;r41=HEAP32[r40>>2];r42=FUNCTION_TABLE[r41](r10,r2,r3);r43=r42}else{r44=HEAP32[r21>>2];r45=r44+60|0;r46=HEAP32[r45>>2];FUNCTION_TABLE[r46](r7,r10);r47=r7|0;r48=HEAP32[r47>>2];r49=r48+52|0;r50=HEAP32[r49>>2];r51=(r48|0)==0;if(!r51){r52=r48;r53=HEAP32[r52>>2];r54=r53+12|0;r55=HEAP32[r54>>2];r56=FUNCTION_TABLE[r55](r48);HEAP32[r47>>2]=0}r57=(r50|0)>-1;r58=r57?r50:-1;r59=(r58|0)==0;r60=r59?1:r58;while(1){r61=_malloc(r60);r62=(r61|0)==0;if(!r62){break}r63=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);r64=(r63|0)==0;if(r64){r4=918;break}r65=r63;FUNCTION_TABLE[r65]()}if(r4==918){r66=___cxa_allocate_exception(4);r67=r66;HEAP32[r67>>2]=19680;___cxa_throw(r66,46792,4624)}r68=__Z14toadlet_mallocii(64);r69=r68;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r69,r2,-1);r70=HEAP32[r21>>2];r71=r70+60|0;r72=HEAP32[r71>>2];FUNCTION_TABLE[r72](r8,r10);r73=r8|0;r74=HEAP32[r73>>2];r75=r74+16|0;r76=HEAP32[r75>>2];r77=r68+16|0;r78=r77;HEAP32[r78>>2]=r76;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r69);r79=HEAP32[r73>>2];r80=(r79|0)==0;if(!r80){r81=r79;r82=HEAP32[r81>>2];r83=r82+12|0;r84=HEAP32[r83>>2];r85=FUNCTION_TABLE[r84](r79);HEAP32[r73>>2]=0}r86=r10;r87=HEAP32[r86>>2];r88=r87+72|0;r89=HEAP32[r88>>2];r90=FUNCTION_TABLE[r89](r10,r69,r61);r91=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r61,r69,r3,r2);r92=(r68|0)==0;if(!r92){r93=r68;r94=HEAP32[r93>>2];r95=r94+4|0;r96=HEAP32[r95>>2];FUNCTION_TABLE[r96](r69)}_free(r61);r43=r90}r97=r43&1;r98=r10|0;r99=r10;r100=HEAP32[r99>>2];r101=r100+12|0;r102=HEAP32[r101>>2];r103=FUNCTION_TABLE[r102](r98);r104=r97;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}r106=r2+60|0;r107=HEAP32[r106>>2];r108=(r107|0)<2;if(!r108){r104=0;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}r109=r1+48|0;r110=HEAP32[r109>>2];r111=(r110|0)==0;if(r111){r104=0;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}r112=r2+32|0;r113=HEAP32[r112>>2];r114=r2+20|0;r115=HEAP32[r114>>2];r116=r113-r115|0;r117=r1+44|0;r118=HEAP32[r117>>2];r119=r118+32|0;r120=HEAP32[r119>>2];r121=r118+20|0;r122=HEAP32[r121>>2];r123=r120-r122|0;r124=(r116|0)==(r123|0);do{if(r124){r125=r2+36|0;r126=HEAP32[r125>>2];r127=r2+24|0;r128=HEAP32[r127>>2];r129=r126-r128|0;r130=r118+36|0;r131=HEAP32[r130>>2];r132=r118+24|0;r133=HEAP32[r132>>2];r134=r131-r133|0;r135=(r129|0)==(r134|0);if(!r135){break}r136=r2+40|0;r137=HEAP32[r136>>2];r138=r2+28|0;r139=HEAP32[r138>>2];r140=r137-r139|0;r141=r118+40|0;r142=HEAP32[r141>>2];r143=r118+28|0;r144=HEAP32[r143>>2];r145=r142-r144|0;r146=(r140|0)==(r145|0);if(!r146){break}r147=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r110,r118,r3,r2);r148=r147&1;r104=r148;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}}while(0);r149=r2+16|0;r150=HEAP32[r149>>2];r151=r118+16|0;r152=HEAP32[r151>>2];r153=(r150|0)==(r152|0);L938:do{if(r153){r154=r3;r155=r2}else{r156=__Z14toadlet_mallocii(64);r157=r156;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r157,r2,-1);r158=HEAP32[r117>>2];r159=r158+16|0;r160=HEAP32[r159>>2];r161=r156+16|0;r162=r161;HEAP32[r162>>2]=r160;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r157);r163=r156+52|0;r164=r163;r165=HEAP32[r164>>2];r166=(r165|0)>-1;r167=r166?r165:-1;r168=(r167|0)==0;r169=r168?1:r167;while(1){r170=_malloc(r169);r171=(r170|0)==0;if(!r171){r154=r170;r155=r157;break L938}r172=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);r173=(r172|0)==0;if(r173){break}r174=r172;FUNCTION_TABLE[r174]()}r175=___cxa_allocate_exception(4);r176=r175;HEAP32[r176>>2]=19680;___cxa_throw(r175,46792,4624)}}while(0);r177=r155+40|0;r178=r155+28|0;r179=r155+36|0;r180=r155+24|0;r181=r155+48|0;r182=r155+44|0;r183=r155+20|0;r184=0;while(1){r185=HEAP32[r177>>2];r186=HEAP32[r178>>2];r187=r185-r186|0;r188=(r184|0)<(r187|0);if(r188){r189=0}else{break}while(1){r190=HEAP32[r179>>2];r191=HEAP32[r180>>2];r192=r190-r191|0;r193=(r189|0)<(r192|0);if(!r193){break}r194=HEAP32[r181>>2];r195=Math_imul(r194,r184)|0;r196=HEAP32[r182>>2];r197=Math_imul(r196,r189)|0;r198=r195+r197|0;r199=r154+r198|0;r200=HEAP32[r109>>2];r201=HEAP32[r117>>2];r202=HEAP32[r183>>2];r203=r191+r189|0;r204=HEAP32[r178>>2];r205=r204+r184|0;r206=r201+48|0;r207=HEAP32[r206>>2];r208=Math_imul(r207,r205)|0;r209=r201+44|0;r210=HEAP32[r209>>2];r211=Math_imul(r210,r203)|0;r212=r208+r211|0;r213=r201+16|0;r214=HEAP32[r213>>2];switch(r214|0){case 259:{r215=2;break};case 262:case 263:{r215=3;break};case 264:case 265:{r215=4;break};case 1798:case 1799:case 2056:case 2057:case 2568:case 2569:{r215=2;break};case 1537:case 1538:{r215=4;break};case 1539:{r215=8;break};case 1542:case 1543:{r215=12;break};case 1544:case 1545:{r215=16;break};case 257:case 258:{r215=1;break};default:{r215=0}}r216=Math_imul(r202,r215)|0;r217=r212+r216|0;r218=r200+r217|0;_memcpy(r199,r218,r196)|0;r219=r189+1|0;r189=r219}r220=r184+1|0;r184=r220}r221=HEAP32[r149>>2];r222=HEAP32[r117>>2];r223=r222+16|0;r224=HEAP32[r223>>2];r225=(r221|0)==(r224|0);if(r225){r104=0;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}r226=__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r154,r155,r3,r2);r227=r226&1;r228=(r155|0)==0;if(!r228){r229=r155;r230=HEAP32[r229>>2];r231=r230+4|0;r232=HEAP32[r231>>2];FUNCTION_TABLE[r232](r155)}r233=(r154|0)==0;if(r233){r104=r227;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}_free(r154);r104=r227;r105=r104<<24>>24!=0;STACKTOP=r5;return r105}function __ZThn36_N7toadlet6peeper15BackableTexture4readEPNS0_13TextureFormatEPh(r1,r2,r3){return __ZN7toadlet6peeper15BackableTexture4readEPNS0_13TextureFormatEPh(r1-84+48|0,r2,r3)}function __ZN7toadlet6peeper15BackableTexture7setBackENS_3egg16IntrusivePointerINS0_7TextureENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceE(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12;r4=STACKTOP;STACKTOP=STACKTOP+16|0;r5=r4;r6=r4+8;r7=r2|0;r8=r1+52|0;r9=HEAP32[r8>>2];if(!((HEAP32[r7>>2]|0)==(r9|0)|(r9|0)==0)){r9=r1+56|0;r10=r1+64|0;r11=0;while(1){if((r11|0)>=(HEAP32[r9>>2]|0)){break}r12=HEAP32[HEAP32[r10>>2]+(r11<<2)>>2];if((r12|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r12>>2]+80>>2]](r12)}r11=r11+1|0}r11=HEAP32[r8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r11>>2]+16>>2]](r11)}r11=r1+52|0;do{if((r11|0)!=(r2|0)){r10=r11|0;r9=HEAP32[r10>>2];if((r9|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+12>>2]](r9|0);HEAP32[r10>>2]=0}r9=HEAP32[r7>>2];HEAP32[r10>>2]=r9;if((r9|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+8>>2]](r9|0)}}while(0);if((HEAP32[r8>>2]|0)==0){STACKTOP=r4;return}r7=__Z14toadlet_mallocii(64);r11=r7;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r11,HEAP32[r1+44>>2],-1);if((r7|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+8>>2]](r11)}r2=r1+40|0;r9=HEAP32[r2>>2]>>>6&1^1;HEAP32[r7+56>>2]=0;HEAP32[r7+60>>2]=r9;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r11);r9=HEAP32[r8>>2];r8=r5|0;HEAP32[r8>>2]=r9;if((r9|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+8>>2]](r9|0)}r9=HEAP32[r2>>2];r2=r6|0;HEAP32[r2>>2]=r11;r10=(r7|0)==0;if(!r10){FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+8>>2]](r11)}r12=r1+48|0;__ZN7toadlet6peeper15BackableTexture13convertCreateENS_3egg16IntrusivePointerINS0_7TextureENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceEiNS3_INS0_13TextureFormatES5_EEPPh(r5,r3,r9,r6,(HEAP32[r12>>2]|0)==0?0:r12);r12=HEAP32[r2>>2];if((r12|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r12>>2]+12>>2]](r12);HEAP32[r2>>2]=0}r2=HEAP32[r8>>2];if((r2|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+12>>2]](r2|0);HEAP32[r8>>2]=0}r8=r1+56|0;r2=r1+64|0;r1=0;while(1){if((r1|0)>=(HEAP32[r8>>2]|0)){break}r12=HEAP32[HEAP32[r2>>2]+(r1<<2)>>2];if((r12|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r12>>2]+76>>2]](r12)}r1=r1+1|0}if(r10){STACKTOP=r4;return}FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+12>>2]](r11);STACKTOP=r4;return}function __ZN7toadlet6peeper15BackableTexture13convertCreateENS_3egg16IntrusivePointerINS0_7TextureENS2_25DefaultIntrusiveSemanticsEEEPNS0_12RenderDeviceEiNS3_INS0_13TextureFormatES5_EEPPh(r1,r2,r3,r4,r5){var r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41;r6=0;r7=STACKTOP;STACKTOP=STACKTOP+176|0;r8=r7;r9=r7+40;r10=r7+48;r11=r7+64;r12=r7+72;r13=r7+88;r14=r7+104;r15=r7+120;r16=r7+136;r17=r7+152;r18=r7+168;HEAP8[r8|0]=0;HEAP32[r8+4>>2]=0;HEAP32[r8+8>>2]=0;HEAP8[r8+12|0]=0;r19=r8+14|0;r20=r8+18|0;HEAP16[r19>>1]=0;HEAP16[r19+2>>1]=0;HEAP16[r19+4>>1]=0;HEAP32[r8+20>>2]=6;r19=r8+24|0;HEAP32[r8+32>>2]=0;HEAP8[r8+36|0]=0;HEAP8[r8+37|0]=0;HEAP32[r19>>2]=0;HEAP16[r19+4>>1]=0;HEAP8[r19+6|0]=0;FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+172>>2]](r2,r8);r19=r4|0;r4=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+160>>2]](r2,HEAP32[HEAP32[r19>>2]+16>>2],r3);r2=HEAP8[r8+17|0]&1;r8=HEAP32[r19>>2];r21=HEAP32[r8+32>>2];do{if((r21-1&r21|0)==0){r22=HEAP32[r8+36>>2];if((r22-1&r22|0)!=0){r23=1;break}if((HEAP32[r8+12>>2]|0)==4){r23=0;break}r22=HEAP32[r8+40>>2];r23=(r22-1&r22|0)!=0|0}else{r23=1}}while(0);r21=r23&(r2^1);r2=r3>>>6;r23=r2&((HEAP8[r20]^1)&255)&255;r20=(r4|0)!=(HEAP32[r8+16>>2]|0);if((r5|0)==0){r22=__Z14toadlet_mallocii(64);r24=r22;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r24,HEAP32[r19>>2],-1);if((r22|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r22>>2]+8>>2]](r24)}HEAP32[r22+16>>2]=r4;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r24);r25=HEAP32[r1>>2];r26=HEAP32[HEAP32[r25>>2]+44>>2];r27=r9|0;HEAP32[r27>>2]=r24;r28=(r22|0)==0;if(!r28){FUNCTION_TABLE[HEAP32[HEAP32[r22>>2]+8>>2]](r24)}r29=FUNCTION_TABLE[r26](r25,r3,r9,0)&1;r9=HEAP32[r27>>2];if((r9|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+12>>2]](r9);HEAP32[r27>>2]=0}if(r28){r30=r29;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}FUNCTION_TABLE[HEAP32[HEAP32[r22>>2]+12>>2]](r24);r30=r29;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}do{if((HEAP32[r8+60>>2]|0)==0){if((r2&1|0)!=0){break}__ZN7toadlet3egg6StringC1EPKci(r10,9992);r29=HEAP32[52920>>2];if((r29|0)==0){__ZN7toadlet3egg3Log10initializeEb(0);r32=HEAP32[52920>>2]}else{r32=r29}do{if((HEAP32[r32+36>>2]|0)>3){r29=r32;r24=__ZN7toadlet3egg6Logger11getCategoryERKNS0_6StringE(r29,65864);if((r24|0)==0){r33=6}else{r33=HEAP32[r24+12>>2]}if((r33|0)<=3){break}__ZN7toadlet3egg6Logger11addLogEntryERKNS0_6StringENS1_5LevelES4_(r29,65864,4,r10)}}while(0);r29=HEAP32[r10+4>>2];if(!((r29|0)==52432|(r29|0)==0)){_free(r29)}r29=HEAP32[r10+8>>2];if((r29|0)==52432|(r29|0)==0){break}_free(r29)}}while(0);r10=HEAP32[r19>>2];r33=HEAP32[r10+32>>2]|0;r32=HEAP32[r10+36>>2]|0;r2=HEAP32[r10+40>>2]|0;r8=r32>r2?r32:r2;r2=0;r32=(r33>r8?r33:r8)&-1;while(1){if((r32|0)<=1){break}r2=r2+1|0;r32=r32>>1}r32=r23&1;r23=r32<<24>>24==0;r8=r10+60|0;if(r23){if((HEAP32[r8>>2]|0)>0){r34=r8;r6=1090}else{r35=1;r36=0}}else{r34=r8;r6=1090}if(r6==1090){r8=HEAP32[r34>>2];r34=(r8|0)>0?r8:r2;r35=r34;r36=r34}r34=r21<<24>>24==0;if(!(r34^1|r23^1|r20)){r2=HEAP32[r1>>2];r8=HEAP32[HEAP32[r2>>2]+44>>2];r33=r11|0;HEAP32[r33>>2]=r10;if((r10|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+8>>2]](r10)}r10=FUNCTION_TABLE[r8](r2,r3,r11,r5)&1;r11=HEAP32[r33>>2];if((r11|0)==0){r30=r10;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}FUNCTION_TABLE[HEAP32[HEAP32[r11>>2]+12>>2]](r11);HEAP32[r33>>2]=0;r30=r10;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}__ZN7toadlet3egg6StringC1EPKci(r17,9e3);__ZNK7toadlet3egg6StringplEi(r16,r17,r21&255);__ZNK7toadlet3egg6StringplEPKc(r15,r16,8440);__ZNK7toadlet3egg6StringplEi(r14,r15,r32&255);__ZNK7toadlet3egg6StringplEPKc(r13,r14,8440);__ZNK7toadlet3egg6StringplEi(r12,r13,r20&1);r20=HEAP32[52920>>2];if((r20|0)==0){__ZN7toadlet3egg3Log10initializeEb(0);r37=HEAP32[52920>>2]}else{r37=r20}do{if((HEAP32[r37+36>>2]|0)>2){r20=r37;r32=__ZN7toadlet3egg6Logger11getCategoryERKNS0_6StringE(r20,65864);if((r32|0)==0){r38=6}else{r38=HEAP32[r32+12>>2]}if((r38|0)<=2){break}__ZN7toadlet3egg6Logger11addLogEntryERKNS0_6StringENS1_5LevelES4_(r20,65864,3,r12)}}while(0);r38=HEAP32[r12+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r12+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r13+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r13+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r14+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r14+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r15+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r15+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r16+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r16+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r17+4>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=HEAP32[r17+8>>2];if(!((r38|0)==52432|(r38|0)==0)){_free(r38)}r38=__Z14toadlet_mallocii(64);r17=r38;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r17,HEAP32[r19>>2],-1);if((r38|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r38>>2]+8>>2]](r17)}HEAP32[r38+16>>2]=r4;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r17);if(!r34){r34=HEAP32[r19>>2];r4=HEAP32[r34+32>>2]-HEAP32[r34+20>>2]|0;r16=1;while(1){if((r16|0)>=(r4|0)){break}r16=r16<<1}r4=HEAP32[r34+36>>2]-HEAP32[r34+24>>2]|0;r15=1;while(1){if((r15|0)>=(r4|0)){break}r15=r15<<1}r4=HEAP32[r34+40>>2]-HEAP32[r34+28>>2]|0;r34=1;while(1){if((r34|0)>=(r4|0)){break}r34=r34<<1}HEAP32[r38+32>>2]=HEAP32[r38+20>>2]+r16;HEAP32[r38+36>>2]=HEAP32[r38+24>>2]+r15;HEAP32[r38+40>>2]=HEAP32[r38+28>>2]+r34;__ZN7toadlet6peeper13TextureFormat11updatePitchEv(r17)}r34=_llvm_umul_with_overflow_i32(r35,4);r15=tempRet0?-1:r34;r34=(r15|0)==0?1:r15;while(1){r39=_malloc(r34);if((r39|0)!=0){break}r15=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r15|0)==0){r6=1194;break}FUNCTION_TABLE[r15]()}if(r6==1194){r34=___cxa_allocate_exception(4);HEAP32[r34>>2]=19680;___cxa_throw(r34,46792,4624)}r34=r39;r15=0;L1173:while(1){if((r15|0)>=(r35|0)){break}r16=__Z14toadlet_mallocii(64);r4=r16;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r4,HEAP32[r19>>2],r15);if((r16|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r16>>2]+8>>2]](r4)}r14=__Z14toadlet_mallocii(64);r13=r14;__ZN7toadlet6peeper13TextureFormatC1EPKS1_i(r13,r17,r15);if((r14|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r14>>2]+8>>2]](r13)}r12=HEAP32[r14+52>>2];r37=(r12|0)>-1?r12:-1;r12=(r37|0)==0?1:r37;while(1){r40=_malloc(r12);if((r40|0)!=0){break}r37=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r37|0)==0){r6=1215;break L1173}FUNCTION_TABLE[r37]()}HEAP32[r34+(r15<<2)>>2]=r40;__ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(HEAP32[r5+(r15<<2)>>2],r4,r40,r13);if((r14|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r14>>2]+12>>2]](r13)}if((r16|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r16>>2]+12>>2]](r4)}r15=r15+1|0}if(r6==1215){r6=___cxa_allocate_exception(4);HEAP32[r6>>2]=19680;___cxa_throw(r6,46792,4624)}r6=HEAP32[r1>>2];r1=HEAP32[HEAP32[r6>>2]+44>>2];r15=r18|0;HEAP32[r15>>2]=r17;r40=(r38|0)==0;if(!r40){FUNCTION_TABLE[HEAP32[HEAP32[r38>>2]+8>>2]](r17)}r5=FUNCTION_TABLE[r1](r6,r23?r3:r3&-65,r18,r34)&1;r18=HEAP32[r15>>2];if((r18|0)==0){r41=0}else{FUNCTION_TABLE[HEAP32[HEAP32[r18>>2]+12>>2]](r18);HEAP32[r15>>2]=0;r41=0}while(1){if((r41|0)>=(r36|0)){break}r15=HEAP32[r34+(r41<<2)>>2];if((r15|0)!=0){_free(r15)}r41=r41+1|0}if((r39|0)!=0){_free(r39)}if(r40){r30=r5;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}FUNCTION_TABLE[HEAP32[HEAP32[r38>>2]+12>>2]](r17);r30=r5;r31=r30<<24>>24!=0;STACKTOP=r7;return r31}function __ZN7toadlet6peeper15BackableTextureD1Ev(r1){var r2,r3,r4,r5;r2=r1|0;HEAP32[r2>>2]=32040;r3=r1+12|0;HEAP32[r3>>2]=32140;HEAP32[r1+36>>2]=32188;__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper11PixelBufferENS0_25DefaultIntrusiveSemanticsEEEED1Ev(HEAP32[r1+64>>2]);r4=r1+52|0;r5=HEAP32[r4>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+12>>2]](r5|0);HEAP32[r4>>2]=0}r4=r1+44|0;r5=HEAP32[r4>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+12>>2]](r5);HEAP32[r4>>2]=0}HEAP32[r2>>2]=35320;HEAP32[r3>>2]=35376;r3=HEAP32[r1+24>>2];if(!((r3|0)==52432|(r3|0)==0)){_free(r3)}r3=HEAP32[r1+28>>2];if((r3|0)==52432|(r3|0)==0){return}_free(r3);return}function __ZN7toadlet6peeper15BackableTextureD0Ev(r1){__ZN7toadlet6peeper15BackableTextureD1Ev(r1);_free(r1);return}function __ZN7toadlet6peeper15BackableTexture6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet6peeper15BackableTexture7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet6peeper15BackableTexture12resourceThisEv(r1){return r1+36|0}function __ZN7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1+16>>2]=r2;return}function __ZN7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1+20|0,r2);return}function __ZNK7toadlet6peeper15BackableTexture7getNameEv(r1){return r1+20|0}function __ZN7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi(r1,r2){HEAP32[r1+32>>2]=r2;return}function __ZNK7toadlet6peeper15BackableTexture15getUniqueHandleEv(r1){return HEAP32[r1+32>>2]}function __ZN7toadlet6peeper15BackableTexture14getRootTextureEv(r1){var r2,r3;r2=HEAP32[r1+52>>2];if((r2|0)==0){r3=0;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+40>>2]](r2);return r3}function __ZNK7toadlet6peeper15BackableTexture8getUsageEv(r1){return HEAP32[r1+40>>2]}function __ZNK7toadlet6peeper15BackableTexture9getFormatEv(r1,r2){var r3;r3=HEAP32[r2+44>>2];HEAP32[r1>>2]=r3;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r3);return}function __ZN7toadlet6peeper15BackableTexture7getBackEv(r1,r2){var r3;r3=HEAP32[r2+52>>2];HEAP32[r1>>2]=r3;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r3|0);return}function __ZThn12_N7toadlet6peeper15BackableTextureD1Ev(r1){__ZN7toadlet6peeper15BackableTextureD1Ev(r1-84+72|0);return}function __ZThn12_N7toadlet6peeper15BackableTextureD0Ev(r1){var r2;r2=r1-84+72|0;__ZN7toadlet6peeper15BackableTextureD1Ev(r2);_free(r2);return}function __ZThn12_N7toadlet6peeper15BackableTexture6retainEv(r1){var r2;r2=r1-84+76|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet6peeper15BackableTexture7releaseEv(r1){var r2,r3,r4;r2=r1-84+72|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-84+88>>2]=r2;return}function __ZThn12_N7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-84+92|0,r2);return}function __ZThn12_NK7toadlet6peeper15BackableTexture7getNameEv(r1){return r1-84+92|0}function __ZThn12_N7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-84+104>>2]=r2;return}function __ZThn12_NK7toadlet6peeper15BackableTexture15getUniqueHandleEv(r1){return HEAP32[r1-84+104>>2]}function __ZThn36_N7toadlet6peeper15BackableTextureD1Ev(r1){__ZN7toadlet6peeper15BackableTextureD1Ev(r1-84+48|0);return}function __ZThn36_N7toadlet6peeper15BackableTextureD0Ev(r1){var r2;r2=r1-84+48|0;__ZN7toadlet6peeper15BackableTextureD1Ev(r2);_free(r2);return}function __ZThn36_N7toadlet6peeper15BackableTexture6retainEv(r1){var r2;r2=r1-84+52|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn36_N7toadlet6peeper15BackableTexture7releaseEv(r1){var r2,r3,r4;r2=r1-84+48|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+4>>2]](r1);return r4}function __ZThn36_N7toadlet6peeper15BackableTexture20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-84+64>>2]=r2;return}function __ZThn36_N7toadlet6peeper15BackableTexture7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-84+68|0,r2);return}function __ZThn36_NK7toadlet6peeper15BackableTexture7getNameEv(r1){return r1-84+68|0}function __ZThn36_N7toadlet6peeper15BackableTexture24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-84+80>>2]=r2;return}function __ZThn36_NK7toadlet6peeper15BackableTexture15getUniqueHandleEv(r1){return HEAP32[r1-84+80>>2]}function __ZThn36_N7toadlet6peeper15BackableTexture14getRootTextureEv(r1){var r2,r3;r2=HEAP32[r1-84+100>>2];if((r2|0)==0){r3=0;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+40>>2]](r2);return r3}function __ZThn36_NK7toadlet6peeper15BackableTexture8getUsageEv(r1){return HEAP32[r1-84+88>>2]}function __ZThn36_NK7toadlet6peeper15BackableTexture9getFormatEv(r1,r2){var r3;r3=HEAP32[r2-84+92>>2];r2=r3;HEAP32[r1>>2]=r2;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r2);return}function __ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1){var r2,r3,r4,r5;r2=r1|0;HEAP32[r2>>2]=27904;r3=r1+12|0;HEAP32[r3>>2]=28016;HEAP32[r1+36>>2]=28064;r4=r1+64|0;r5=HEAP32[r4>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+12>>2]](r5|0);HEAP32[r4>>2]=0}r4=r1+60|0;r5=HEAP32[r4>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+12>>2]](r5);HEAP32[r4>>2]=0}HEAP32[r2>>2]=35320;HEAP32[r3>>2]=35376;r3=HEAP32[r1+24>>2];if(!((r3|0)==52432|(r3|0)==0)){_free(r3)}r3=HEAP32[r1+28>>2];if((r3|0)==52432|(r3|0)==0){return}_free(r3);return}function __ZN7toadlet6peeper29BackableTextureMipPixelBufferD0Ev(r1){__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1);_free(r1);return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv(r1){FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+60>>2]](r1);HEAP32[r1+40>>2]=0;return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer12resourceThisEv(r1){return r1+36|0}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1+16>>2]=r2;return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1+20|0,r2);return}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv(r1){return r1+20|0}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi(r1,r2){HEAP32[r1+32>>2]=r2;return}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv(r1){return HEAP32[r1+32>>2]}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer18getRootPixelBufferEv(r1){return HEAP32[r1+64>>2]}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE(r1,r2,r3,r4){return 0}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv(r1){var r2,r3,r4,r5,r6;r2=STACKTOP;STACKTOP=STACKTOP+16|0;r3=r2;r4=r2+8;r5=HEAP32[r1+40>>2];FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+88>>2]](r3,r5);r5=r3|0;r3=HEAP32[r5>>2];if((r3|0)==0){STACKTOP=r2;return}r6=HEAP32[HEAP32[r1>>2]+96>>2];FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+64>>2]](r4,r3,HEAP32[r1+44>>2],HEAP32[r1+48>>2]);FUNCTION_TABLE[r6](r1,r4);r1=r4|0;r4=HEAP32[r1>>2];if((r4|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+12>>2]](r4|0);HEAP32[r1>>2]=0}r1=HEAP32[r5>>2];if((r1|0)==0){STACKTOP=r2;return}FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+12>>2]](r1|0);HEAP32[r5>>2]=0;STACKTOP=r2;return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer12resetDestroyEv(r1){var r2,r3,r4,r5;r2=STACKTOP;STACKTOP=STACKTOP+8|0;r3=r2;r4=HEAP32[HEAP32[r1>>2]+96>>2];r5=r3|0;HEAP32[r5>>2]=0;FUNCTION_TABLE[r4](r1,r3);r3=HEAP32[r5>>2];if((r3|0)==0){STACKTOP=r2;return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r5>>2]=0;STACKTOP=r2;return}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer8getUsageEv(r1){var r2,r3;r2=HEAP32[r1+64>>2];if((r2|0)==0){r3=HEAP32[r1+52>>2];return r3}else{r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+40>>2]](r2|0);return r3}}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer9getAccessEv(r1){var r2,r3;r2=HEAP32[r1+64>>2];if((r2|0)==0){r3=HEAP32[r1+56>>2];return r3}else{r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+44>>2]](r2|0);return r3}}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer11getDataSizeEv(r1){var r2,r3;r2=HEAP32[r1+64>>2];if((r2|0)!=0){r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+48>>2]](r2|0);return r3}r2=HEAP32[r1+60>>2];if((r2|0)==0){r3=0;return r3}r3=HEAP32[r2+52>>2];return r3}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer7getSizeEv(r1){var r2,r3;r2=HEAP32[r1+64>>2];if((r2|0)!=0){r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+52>>2]](r2|0);return r3}r2=HEAP32[r1+60>>2];if((r2|0)==0){r3=0;return r3}r1=Math_imul(HEAP32[r2+32>>2]-HEAP32[r2+20>>2]|0,HEAP32[r2+36>>2]-HEAP32[r2+24>>2]|0)|0;r3=Math_imul(r1,HEAP32[r2+40>>2]-HEAP32[r2+28>>2]|0)|0;return r3}function __ZNK7toadlet6peeper29BackableTextureMipPixelBuffer16getTextureFormatEv(r1,r2){var r3;r3=HEAP32[r2+64>>2];if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+84>>2]](r1,r3);return}r3=HEAP32[r2+60>>2];HEAP32[r1>>2]=r3;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r3);return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer4lockEi(r1,r2){var r3,r4;r3=HEAP32[r1+64>>2];if((r3|0)==0){r4=0;return r4}r4=FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+56>>2]](r3|0,r2);return r4}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer6unlockEv(r1){var r2,r3;r2=HEAP32[r1+64>>2];if((r2|0)==0){r3=0;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+60>>2]](r2|0);return r3}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer6updateEPhii(r1,r2,r3,r4){var r5,r6;r5=HEAP32[r1+64>>2];if((r5|0)==0){r6=0;return r6}r6=FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+64>>2]](r5|0,r2,r3,r4);return r6}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer7setBackENS_3egg16IntrusivePointerINS0_11PixelBufferENS2_25DefaultIntrusiveSemanticsEEE(r1,r2){var r3,r4,r5,r6,r7;r3=STACKTOP;STACKTOP=STACKTOP+8|0;r4=r3;r5=r1+64|0;do{if((r5|0)!=(r2|0)){r6=r5|0;r7=HEAP32[r6>>2];if((r7|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+12>>2]](r7|0);HEAP32[r6>>2]=0}r7=HEAP32[r2>>2];HEAP32[r6>>2]=r7;if((r7|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+8>>2]](r7|0)}}while(0);r2=r1+64|0;r5=HEAP32[r2>>2];if((r5|0)==0){STACKTOP=r3;return}HEAP32[r1+52>>2]=FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+40>>2]](r5|0);r5=HEAP32[r2>>2];HEAP32[r1+56>>2]=FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+44>>2]](r5|0);r5=HEAP32[r2>>2];FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+84>>2]](r4,r5);r5=r1+60|0;r1=HEAP32[r5>>2];if((r1|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+12>>2]](r1);HEAP32[r5>>2]=0}r1=r4|0;r4=HEAP32[r1>>2];HEAP32[r5>>2]=r4;if((r4|0)==0){STACKTOP=r3;return}FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+8>>2]](r4);r4=HEAP32[r1>>2];if((r4|0)==0){STACKTOP=r3;return}FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+12>>2]](r4);HEAP32[r1>>2]=0;STACKTOP=r3;return}function __ZN7toadlet6peeper29BackableTextureMipPixelBuffer7getBackEv(r1,r2){var r3;r3=HEAP32[r2+64>>2];HEAP32[r1>>2]=r3;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r3|0);return}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1){__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1-68+56|0);return}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBufferD0Ev(r1){var r2;r2=r1-68+56|0;__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r2);_free(r2);return}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv(r1){var r2;r2=r1-68+60|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv(r1){var r2,r3,r4;r2=r1-68+56|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv(r1){var r2;r2=r1-68+56|0;FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+60>>2]](r2);HEAP32[r2+40>>2]=0;return}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-68+72>>2]=r2;return}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-68+76|0,r2);return}function __ZThn12_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv(r1){return r1-68+76|0}function __ZThn12_N7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-68+88>>2]=r2;return}function __ZThn12_NK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv(r1){return HEAP32[r1-68+88>>2]}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1){__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r1-68+32|0);return}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBufferD0Ev(r1){var r2;r2=r1-68+32|0;__ZN7toadlet6peeper29BackableTextureMipPixelBufferD1Ev(r2);_free(r2);return}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6retainEv(r1){var r2;r2=r1-68+36|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7releaseEv(r1){var r2,r3,r4;r2=r1-68+32|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7destroyEv(r1){var r2;r2=r1-68+32|0;FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+60>>2]](r2);HEAP32[r2+40>>2]=0;return}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-68+48>>2]=r2;return}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-68+52|0,r2);return}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getNameEv(r1){return r1-68+52|0}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-68+64>>2]=r2;return}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer15getUniqueHandleEv(r1){return HEAP32[r1-68+64>>2]}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer8getUsageEv(r1){var r2,r3;r2=r1-68+32|0;r1=HEAP32[r2+64>>2];if((r1|0)==0){r3=HEAP32[r2+52>>2];return r3}else{r3=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+40>>2]](r1|0);return r3}}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer9getAccessEv(r1){var r2,r3;r2=r1-68+32|0;r1=HEAP32[r2+64>>2];if((r1|0)==0){r3=HEAP32[r2+56>>2];return r3}else{r3=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+44>>2]](r1|0);return r3}}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer11getDataSizeEv(r1){var r2,r3;r2=r1-68+32|0;r1=HEAP32[r2+64>>2];if((r1|0)!=0){r3=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+48>>2]](r1|0);return r3}r1=HEAP32[r2+60>>2];if((r1|0)==0){r3=0;return r3}r3=HEAP32[r1+52>>2];return r3}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer7getSizeEv(r1){var r2,r3;r2=r1-68+32|0;r1=HEAP32[r2+64>>2];if((r1|0)!=0){r3=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+52>>2]](r1|0);return r3}r1=HEAP32[r2+60>>2];if((r1|0)==0){r3=0;return r3}r2=Math_imul(HEAP32[r1+32>>2]-HEAP32[r1+20>>2]|0,HEAP32[r1+36>>2]-HEAP32[r1+24>>2]|0)|0;r3=Math_imul(r2,HEAP32[r1+40>>2]-HEAP32[r1+28>>2]|0)|0;return r3}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer4lockEi(r1,r2){var r3,r4;r3=HEAP32[r1-68+96>>2];if((r3|0)==0){r4=0;return r4}r4=FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+56>>2]](r3|0,r2);return r4}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6unlockEv(r1){var r2,r3;r2=HEAP32[r1-68+96>>2];if((r2|0)==0){r3=0;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+60>>2]](r2|0);return r3}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6updateEPhii(r1,r2,r3,r4){var r5,r6;r5=HEAP32[r1-68+96>>2];if((r5|0)==0){r6=0;return r6}r6=FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+64>>2]](r5|0,r2,r3,r4);return r6}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer18getRootPixelBufferEv(r1){return HEAP32[r1-68+96>>2]}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer6createEiiNS_3egg16IntrusivePointerINS0_13TextureFormatENS2_25DefaultIntrusiveSemanticsEEE(r1,r2,r3,r4){return 0}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv(r1){__ZN7toadlet6peeper29BackableTextureMipPixelBuffer11resetCreateEv(r1-68+32|0);return}function __ZThn36_N7toadlet6peeper29BackableTextureMipPixelBuffer12resetDestroyEv(r1){__ZN7toadlet6peeper29BackableTextureMipPixelBuffer12resetDestroyEv(r1-68+32|0);return}function __ZThn36_NK7toadlet6peeper29BackableTextureMipPixelBuffer16getTextureFormatEv(r1,r2){var r3;r3=r2-68+32|0;r2=HEAP32[r3+64>>2];if((r2|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+84>>2]](r1,r2);return}r2=HEAP32[r3+60>>2];HEAP32[r1>>2]=r2;if((r2|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+8>>2]](r2);return}function __GLOBAL__I_a13235(){__ZN7toadlet3egg6StringC1EPKci(54088,11096);_atexit(982,54088,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73224,54088,10832);_atexit(982,73224,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61448,73224,10728);_atexit(982,61448,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67336,54088,10688);_atexit(982,67336,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71752,54088,10680);_atexit(982,71752,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68808,54088,10672);_atexit(982,68808,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65864,54088,10664);_atexit(982,65864,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64392,54088,10640);_atexit(982,64392,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62920,54088,10624);_atexit(982,62920,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59976,62920,10616);_atexit(982,59976,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58504,62920,10600);_atexit(982,58504,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(57032,62920,10592);_atexit(982,57032,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55560,62920,10568);_atexit(982,55560,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70280,54088,10560);_atexit(982,70280,___dso_handle);return}function __ZN7toadlet6peeper20BackableVertexFormat6createEv(r1){var r2,r3;r2=HEAP32[r1+184>>2];if((r2|0)==0){r3=1;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+44>>2]](r2);return r3}function __ZThn36_N7toadlet6peeper20BackableVertexFormat6createEv(r1){var r2,r3;r2=HEAP32[r1-188+336>>2];if((r2|0)==0){r3=1;return r3}r3=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+44>>2]](r2);return r3}function __ZN7toadlet6peeper20BackableVertexFormat7destroyEv(r1){var r2,r3,r4,r5;r2=HEAP32[r1+184>>2];if((r2|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+16>>2]](r2);r2=r1+184|0;r3=HEAP32[r2>>2];if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r2>>2]=0}HEAP32[r2>>2]=0}r2=r1+16|0;r3=HEAP32[r2>>2];if((r3|0)==0){return}r4=HEAP32[HEAP32[r3>>2]+8>>2];r5=FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+24>>2]](r1|0);FUNCTION_TABLE[r4](r3,r5);HEAP32[r2>>2]=0;return}function __ZThn12_N7toadlet6peeper20BackableVertexFormat7destroyEv(r1){__ZN7toadlet6peeper20BackableVertexFormat7destroyEv(r1-188+176|0);return}function __ZThn36_N7toadlet6peeper20BackableVertexFormat7destroyEv(r1){__ZN7toadlet6peeper20BackableVertexFormat7destroyEv(r1-188+152|0);return}function __ZN7toadlet6peeper20BackableVertexFormat10addElementEiRKNS_3egg6StringEii(r1,r2,r3,r4,r5){var r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18;r6=STACKTOP;STACKTOP=STACKTOP+16|0;r7=r6;r8=r1+40|0;if((r2|0)==-1){r9=__ZN7toadlet6peeper20BackableVertexFormat19getSemanticFromNameERKNS_3egg6StringE(r3);r10=r8|0;r11=HEAP32[r10>>2];if((r11+1|0)>(HEAP32[r1+44>>2]|0)){__ZN7toadlet3egg10CollectionIiE7reserveEi(r8,(r11<<1)+2|0);r12=HEAP32[r10>>2]}else{r12=r11}HEAP32[HEAP32[r1+48>>2]+(r12<<2)>>2]=r9;r9=HEAP32[r10>>2]+1|0;HEAP32[r10>>2]=r9;r10=r1+60|0;HEAP32[r10>>2]=r8;HEAP32[r10+4>>2]=r9}else{r9=r8|0;r10=HEAP32[r9>>2];if((r10+1|0)>(HEAP32[r1+44>>2]|0)){__ZN7toadlet3egg10CollectionIiE7reserveEi(r8,(r10<<1)+2|0);r13=HEAP32[r9>>2]}else{r13=r10}HEAP32[HEAP32[r1+48>>2]+(r13<<2)>>2]=r2;r13=HEAP32[r9>>2]+1|0;HEAP32[r9>>2]=r13;r9=r1+60|0;HEAP32[r9>>2]=r8;HEAP32[r9+4>>2]=r13}r13=r1+68|0;do{if(__ZNK7toadlet3egg6String6equalsEPKc(r3,0)){__ZN7toadlet6peeper20BackableVertexFormat19getNameFromSemanticEi(r7,r2);r9=r13|0;r8=HEAP32[r9>>2];if((r8+1|0)>(HEAP32[r1+72>>2]|0)){__ZN7toadlet3egg10CollectionINS0_6StringEE7reserveEi(r13,(r8<<1)+2|0);r14=HEAP32[r9>>2]}else{r14=r8}__ZN7toadlet3egg6StringaSERKS1_(HEAP32[r1+76>>2]+(r14*12&-1)|0,r7);r8=HEAP32[r9>>2]+1|0;HEAP32[r9>>2]=r8;r9=r1+88|0;HEAP32[r9>>2]=r13;HEAP32[r9+4>>2]=r8;r8=HEAP32[r7+4>>2];if(!((r8|0)==52432|(r8|0)==0)){_free(r8)}r8=HEAP32[r7+8>>2];if((r8|0)==52432|(r8|0)==0){break}_free(r8)}else{r8=r13|0;r9=HEAP32[r8>>2];if((r9+1|0)>(HEAP32[r1+72>>2]|0)){__ZN7toadlet3egg10CollectionINS0_6StringEE7reserveEi(r13,(r9<<1)+2|0);r15=HEAP32[r8>>2]}else{r15=r9}__ZN7toadlet3egg6StringaSERKS1_(HEAP32[r1+76>>2]+(r15*12&-1)|0,r3);r9=HEAP32[r8>>2]+1|0;HEAP32[r8>>2]=r9;r8=r1+88|0;HEAP32[r8>>2]=r13;HEAP32[r8+4>>2]=r9}}while(0);r13=r1+96|0;r3=r13|0;r15=HEAP32[r3>>2];if((r15+1|0)>(HEAP32[r1+100>>2]|0)){__ZN7toadlet3egg10CollectionIiE7reserveEi(r13,(r15<<1)+2|0);r16=HEAP32[r3>>2]}else{r16=r15}HEAP32[HEAP32[r1+104>>2]+(r16<<2)>>2]=r4;r16=HEAP32[r3>>2]+1|0;HEAP32[r3>>2]=r16;r3=r1+116|0;HEAP32[r3>>2]=r13;HEAP32[r3+4>>2]=r16;r16=r1+124|0;r3=r16|0;r13=HEAP32[r3>>2];if((r13+1|0)>(HEAP32[r1+128>>2]|0)){__ZN7toadlet3egg10CollectionIiE7reserveEi(r16,(r13<<1)+2|0);r17=HEAP32[r3>>2]}else{r17=r13}HEAP32[HEAP32[r1+132>>2]+(r17<<2)>>2]=r5;r17=HEAP32[r3>>2]+1|0;HEAP32[r3>>2]=r17;r3=r1+144|0;HEAP32[r3>>2]=r16;HEAP32[r3+4>>2]=r17;r17=r1+152|0;r3=r1+180|0;r16=r17|0;r13=HEAP32[r16>>2];if((r13+1|0)>(HEAP32[r1+156>>2]|0)){__ZN7toadlet3egg10CollectionIiE7reserveEi(r17,(r13<<1)+2|0);r18=HEAP32[r16>>2]}else{r18=r13}HEAP32[HEAP32[r1+160>>2]+(r18<<2)>>2]=HEAP32[r3>>2];r18=HEAP32[r16>>2]+1|0;HEAP32[r16>>2]=r18;r16=r1+172|0;HEAP32[r16>>2]=r17;HEAP32[r16+4>>2]=r18;r18=__ZN7toadlet6peeper12VertexFormat13getFormatSizeEi(r5);HEAP32[r3>>2]=HEAP32[r3>>2]+r18;r18=HEAP32[r1+184>>2];if((r18|0)==0){STACKTOP=r6;return 1}FUNCTION_TABLE[HEAP32[HEAP32[r18>>2]+48>>2]](r18,r2,r4,r5);STACKTOP=r6;return 1}function __ZN7toadlet6peeper20BackableVertexFormat19getSemanticFromNameERKNS_3egg6StringE(r1){var r2,r3;do{if(__ZNK7toadlet3egg6String6equalsEPKc(r1,9976)){r2=0}else{if(__ZNK7toadlet3egg6String6equalsEPKc(r1,8984)){r2=1;break}if(__ZNK7toadlet3egg6String6equalsEPKc(r1,8424)){r2=2;break}if(__ZNK7toadlet3egg6String6equalsEPKc(r1,6464)){r2=3;break}if(__ZNK7toadlet3egg6String6equalsEPKc(r1,4264)){r2=4;break}r3=__ZNK7toadlet3egg6String6equalsEPKc(r1,3312);return r3?5:-1}}while(0);return r2}function __ZN7toadlet6peeper20BackableVertexFormat19getNameFromSemanticEi(r1,r2){var r3,r4,r5,r6;r3=0;switch(r2|0){case 3:{__ZN7toadlet3egg6StringC1EPKci(r1,6464);return;break};case 2:{__ZN7toadlet3egg6StringC1EPKci(r1,8424);return;break};case 5:{__ZN7toadlet3egg6StringC1EPKci(r1,3312);return;break};case 4:{__ZN7toadlet3egg6StringC1EPKci(r1,4264);return;break};case 1:{__ZN7toadlet3egg6StringC1EPKci(r1,8984);return;break};case 0:{__ZN7toadlet3egg6StringC1EPKci(r1,9976);return;break};default:{r4=r1|0;HEAP32[r4>>2]=0;r5=r1+4|0;HEAP32[r5>>2]=52432;r6=r1+8|0;HEAP32[r6>>2]=52432;return}}}function __ZN7toadlet6peeper20BackableVertexFormat11findElementEi(r1,r2){var r3,r4,r5,r6;r3=0;r4=HEAP32[r1+40>>2];r5=r1+48|0;r1=0;while(1){if((r1|0)>=(r4|0)){r6=-1;r3=1703;break}if((HEAP32[HEAP32[r5>>2]+(r1<<2)>>2]|0)==(r2|0)){r6=r1;r3=1702;break}r1=r1+1|0}if(r3==1703){return r6}else if(r3==1702){return r6}}function __ZThn36_N7toadlet6peeper20BackableVertexFormat11findElementEi(r1,r2){var r3,r4,r5,r6;r3=0;r4=r1-188+192|0;r1=HEAP32[r4>>2];r5=r4+8|0;r4=0;while(1){if((r4|0)>=(r1|0)){r6=-1;r3=1709;break}if((HEAP32[HEAP32[r5>>2]+(r4<<2)>>2]|0)==(r2|0)){r6=r4;r3=1710;break}r4=r4+1|0}if(r3==1709){return r6}else if(r3==1710){return r6}}function __ZN7toadlet6peeper20BackableVertexFormat11findElementERKNS_3egg6StringE(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17;r3=0;r4=HEAP32[r1+68>>2];r5=r1+76|0;r1=r2|0;r6=r2+4|0;r2=0;L1652:while(1){if((r2|0)>=(r4|0)){r7=-1;r3=1724;break}r8=HEAP32[r5>>2];r9=HEAP32[r8+(r2*12&-1)+4>>2];do{if((r9|0)==0){if((HEAP32[r1>>2]|0)==0){r7=r2;r3=1722;break L1652}}else{r10=HEAP32[r6>>2];if((r10|0)==0){if((HEAP32[r8+(r2*12&-1)>>2]|0)==0){r7=r2;r3=1723;break L1652}else{break}}r11=HEAP32[r9>>2];r12=HEAP32[r10>>2];if((r11|0)!=(r12|0)|(r11|0)==0|(r12|0)==0){r13=r11;r14=r12}else{r12=r9;r11=r10;while(1){r10=r12+4|0;r15=r11+4|0;r16=HEAP32[r10>>2];r17=HEAP32[r15>>2];if((r16|0)!=(r17|0)|(r16|0)==0|(r17|0)==0){r13=r16;r14=r17;break}else{r12=r10;r11=r15}}}if((r13|0)==(r14|0)){r7=r2;r3=1725;break L1652}}}while(0);r2=r2+1|0}if(r3==1724){return r7}else if(r3==1725){return r7}else if(r3==1723){return r7}else if(r3==1722){return r7}}function __ZThn36_N7toadlet6peeper20BackableVertexFormat11findElementERKNS_3egg6StringE(r1,r2){return __ZN7toadlet6peeper20BackableVertexFormat11findElementERKNS_3egg6StringE(r1-188+152|0,r2)}function __ZN7toadlet6peeper20BackableVertexFormat7setBackENS_3egg16IntrusivePointerINS0_12VertexFormatENS2_25DefaultIntrusiveSemanticsEEE(r1,r2){var r3,r4,r5,r6,r7;r3=r2|0;r4=r1+184|0;r5=HEAP32[r4>>2];if(!((HEAP32[r3>>2]|0)==(r5|0)|(r5|0)==0)){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+16>>2]](r5)}r5=r1+184|0;do{if((r5|0)!=(r2|0)){r6=r5|0;r7=HEAP32[r6>>2];if((r7|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+12>>2]](r7|0);HEAP32[r6>>2]=0}r7=HEAP32[r3>>2];HEAP32[r6>>2]=r7;if((r7|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+8>>2]](r7|0)}}while(0);r3=HEAP32[r4>>2];if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+44>>2]](r3);r3=r1+40|0;r5=r1+48|0;r2=r1+104|0;r7=r1+132|0;r1=0;while(1){if((r1|0)>=(HEAP32[r3>>2]|0)){break}r6=HEAP32[r4>>2];FUNCTION_TABLE[HEAP32[HEAP32[r6>>2]+48>>2]](r6,HEAP32[HEAP32[r5>>2]+(r1<<2)>>2],HEAP32[HEAP32[r2>>2]+(r1<<2)>>2],HEAP32[HEAP32[r7>>2]+(r1<<2)>>2]);r1=r1+1|0}return}function __ZN7toadlet6peeper20BackableVertexFormatD1Ev(r1){var r2,r3,r4,r5;r2=r1|0;HEAP32[r2>>2]=29448;r3=r1+12|0;HEAP32[r3>>2]=29568;HEAP32[r1+36>>2]=29616;r4=r1+184|0;r5=HEAP32[r4>>2];if((r5|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+12>>2]](r5|0);HEAP32[r4>>2]=0}r4=HEAP32[r1+160>>2];if((r4|0)!=0){_free(r4)}r4=HEAP32[r1+132>>2];if((r4|0)!=0){_free(r4)}r4=HEAP32[r1+104>>2];if((r4|0)!=0){_free(r4)}__ZN7toadlet3egg10CollectionINS0_6StringEED1Ev(HEAP32[r1+76>>2]);r4=HEAP32[r1+48>>2];if((r4|0)!=0){_free(r4)}HEAP32[r2>>2]=35320;HEAP32[r3>>2]=35376;r3=HEAP32[r1+24>>2];if(!((r3|0)==52432|(r3|0)==0)){_free(r3)}r3=HEAP32[r1+28>>2];if((r3|0)==52432|(r3|0)==0){return}_free(r3);return}function __ZN7toadlet6peeper20BackableVertexFormatD0Ev(r1){__ZN7toadlet6peeper20BackableVertexFormatD1Ev(r1);_free(r1);return}function __ZN7toadlet6peeper20BackableVertexFormat6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet6peeper20BackableVertexFormat7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet6peeper20BackableVertexFormat12resourceThisEv(r1){return r1+36|0}function __ZN7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1+16>>2]=r2;return}function __ZN7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1+20|0,r2);return}function __ZNK7toadlet6peeper20BackableVertexFormat7getNameEv(r1){return r1+20|0}function __ZN7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi(r1,r2){HEAP32[r1+32>>2]=r2;return}function __ZNK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv(r1){return HEAP32[r1+32>>2]}function __ZN7toadlet6peeper20BackableVertexFormat19getRootVertexFormatEv(r1){return HEAP32[r1+184>>2]}function __ZN7toadlet6peeper20BackableVertexFormat10addElementEiii(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10;r5=STACKTOP;STACKTOP=STACKTOP+16|0;r6=r5;r7=HEAP32[HEAP32[r1>>2]+64>>2];HEAP32[r6>>2]=0;r8=r6+4|0;HEAP32[r8>>2]=52432;r9=r6+8|0;HEAP32[r9>>2]=52432;r10=FUNCTION_TABLE[r7](r1,r2,r6,r3,r4);r4=HEAP32[r8>>2];if(!((r4|0)==52432|(r4|0)==0)){_free(r4)}r4=HEAP32[r9>>2];if((r4|0)==52432|(r4|0)==0){STACKTOP=r5;return r10}_free(r4);STACKTOP=r5;return r10}function __ZN7toadlet6peeper20BackableVertexFormat10addElementERKNS_3egg6StringEii(r1,r2,r3,r4){return FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+64>>2]](r1,-1,r2,r3,r4)}function __ZNK7toadlet6peeper20BackableVertexFormat14getNumElementsEv(r1){return HEAP32[r1+40>>2]}function __ZNK7toadlet6peeper20BackableVertexFormat18getElementSemanticEi(r1,r2){return HEAP32[HEAP32[r1+48>>2]+(r2<<2)>>2]}function __ZNK7toadlet6peeper20BackableVertexFormat14getElementNameEi(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16;r4=0;r5=HEAP32[r2+76>>2];r2=HEAP32[r5+(r3*12&-1)>>2];r6=r1|0;HEAP32[r6>>2]=r2;r7=_llvm_umul_with_overflow_i32(r2+1|0,4);r2=tempRet0?-1:r7;r7=(r2|0)==0?1:r2;while(1){r8=_malloc(r7);if((r8|0)!=0){break}r2=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r2|0)==0){r4=1815;break}FUNCTION_TABLE[r2]()}if(r4==1815){r7=___cxa_allocate_exception(4);HEAP32[r7>>2]=19680;___cxa_throw(r7,46792,4624)}r7=r8;r8=r1+4|0;HEAP32[r8>>2]=r7;r2=HEAP32[r6>>2]+1|0;L1753:do{if((r2|0)!=0){r9=HEAP32[r5+(r3*12&-1)+4>>2];r10=r2;r11=r7;while(1){r12=HEAP32[r9>>2];if((r12|0)==0){break}r13=r10-1|0;HEAP32[r11>>2]=r12;if((r13|0)==0){break L1753}else{r9=r9+4|0;r10=r13;r11=r11+4|0}}if((r10|0)==0){break}else{r14=r10;r15=r11}while(1){r9=r14-1|0;HEAP32[r15>>2]=0;if((r9|0)==0){break}else{r14=r9;r15=r15+4|0}}}}while(0);r15=r1+8|0;HEAP32[r15>>2]=0;r1=HEAP32[r6>>2]+1|0;r6=(r1|0)>-1?r1:-1;r1=(r6|0)==0?1:r6;while(1){r16=_malloc(r1);if((r16|0)!=0){break}r6=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r6|0)==0){r4=1836;break}FUNCTION_TABLE[r6]()}if(r4==1836){r4=___cxa_allocate_exception(4);HEAP32[r4>>2]=19680;___cxa_throw(r4,46792,4624)}HEAP32[r15>>2]=r16;r15=HEAP32[r8>>2];r8=r16;while(1){r16=HEAP32[r15>>2]&255;HEAP8[r8]=r16;if(r16<<24>>24==0){break}else{r15=r15+4|0;r8=r8+1|0}}return}function __ZNK7toadlet6peeper20BackableVertexFormat15getElementIndexEi(r1,r2){return HEAP32[HEAP32[r1+104>>2]+(r2<<2)>>2]}function __ZNK7toadlet6peeper20BackableVertexFormat16getElementFormatEi(r1,r2){return HEAP32[HEAP32[r1+132>>2]+(r2<<2)>>2]}function __ZNK7toadlet6peeper20BackableVertexFormat16getElementOffsetEi(r1,r2){return HEAP32[HEAP32[r1+160>>2]+(r2<<2)>>2]}function __ZNK7toadlet6peeper20BackableVertexFormat13getVertexSizeEv(r1){return HEAP32[r1+180>>2]}function __ZN7toadlet6peeper20BackableVertexFormat7getBackEv(r1,r2){var r3;r3=HEAP32[r2+184>>2];HEAP32[r1>>2]=r3;if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+8>>2]](r3|0);return}function __ZThn12_N7toadlet6peeper20BackableVertexFormatD1Ev(r1){__ZN7toadlet6peeper20BackableVertexFormatD1Ev(r1-188+176|0);return}function __ZThn12_N7toadlet6peeper20BackableVertexFormatD0Ev(r1){var r2;r2=r1-188+176|0;__ZN7toadlet6peeper20BackableVertexFormatD1Ev(r2);_free(r2);return}function __ZThn12_N7toadlet6peeper20BackableVertexFormat6retainEv(r1){var r2;r2=r1-188+180|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet6peeper20BackableVertexFormat7releaseEv(r1){var r2,r3,r4;r2=r1-188+176|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-188+192>>2]=r2;return}function __ZThn12_N7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-188+196|0,r2);return}function __ZThn12_NK7toadlet6peeper20BackableVertexFormat7getNameEv(r1){return r1-188+196|0}function __ZThn12_N7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-188+208>>2]=r2;return}function __ZThn12_NK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv(r1){return HEAP32[r1-188+208>>2]}function __ZThn36_N7toadlet6peeper20BackableVertexFormatD1Ev(r1){__ZN7toadlet6peeper20BackableVertexFormatD1Ev(r1-188+152|0);return}function __ZThn36_N7toadlet6peeper20BackableVertexFormatD0Ev(r1){var r2;r2=r1-188+152|0;__ZN7toadlet6peeper20BackableVertexFormatD1Ev(r2);_free(r2);return}function __ZThn36_N7toadlet6peeper20BackableVertexFormat6retainEv(r1){var r2;r2=r1-188+156|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn36_N7toadlet6peeper20BackableVertexFormat7releaseEv(r1){var r2,r3,r4;r2=r1-188+152|0;r3=r2;r4=r1-188+156|0;r1=(tempValue=HEAP32[r4>>2],HEAP32[r4>>2]=tempValue-1,tempValue)-1|0;if((r1|0)>=1){return r1}r4=r2;FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+20>>2]](r3);if((r2|0)==0){return r1}FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+4>>2]](r3);return r1}function __ZThn36_N7toadlet6peeper20BackableVertexFormat20setDestroyedListenerEPNS_3egg25ResourceDestroyedListenerE(r1,r2){HEAP32[r1-188+168>>2]=r2;return}function __ZThn36_N7toadlet6peeper20BackableVertexFormat7setNameERKNS_3egg6StringE(r1,r2){__ZN7toadlet3egg6StringaSERKS1_(r1-188+172|0,r2);return}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat7getNameEv(r1){return r1-188+172|0}function __ZThn36_N7toadlet6peeper20BackableVertexFormat24internal_setUniqueHandleEi(r1,r2){HEAP32[r1-188+184>>2]=r2;return}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat15getUniqueHandleEv(r1){return HEAP32[r1-188+184>>2]}function __ZThn36_N7toadlet6peeper20BackableVertexFormat19getRootVertexFormatEv(r1){return HEAP32[r1-188+336>>2]}function __ZThn36_N7toadlet6peeper20BackableVertexFormat10addElementEiii(r1,r2,r3,r4){return __ZN7toadlet6peeper20BackableVertexFormat10addElementEiii(r1-188+152|0,r2,r3,r4)}function __ZThn36_N7toadlet6peeper20BackableVertexFormat10addElementERKNS_3egg6StringEii(r1,r2,r3,r4){var r5;r5=r1-188+152|0;return FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+64>>2]](r5,-1,r2,r3,r4)}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat14getNumElementsEv(r1){return HEAP32[r1-188+192>>2]}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat18getElementSemanticEi(r1,r2){return HEAP32[HEAP32[r1-188+200>>2]+(r2<<2)>>2]}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat14getElementNameEi(r1,r2,r3){__ZNK7toadlet6peeper20BackableVertexFormat14getElementNameEi(r1,r2-188+152|0,r3);return}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat15getElementIndexEi(r1,r2){return HEAP32[HEAP32[r1-188+256>>2]+(r2<<2)>>2]}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat16getElementFormatEi(r1,r2){return HEAP32[HEAP32[r1-188+284>>2]+(r2<<2)>>2]}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat16getElementOffsetEi(r1,r2){return HEAP32[HEAP32[r1-188+312>>2]+(r2<<2)>>2]}function __ZThn36_NK7toadlet6peeper20BackableVertexFormat13getVertexSizeEv(r1){return HEAP32[r1-188+332>>2]}function __GLOBAL__I_a13407(){__ZN7toadlet3egg6StringC1EPKci(54072,11096);_atexit(982,54072,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73208,54072,10832);_atexit(982,73208,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61432,73208,10728);_atexit(982,61432,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67320,54072,10688);_atexit(982,67320,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71736,54072,10680);_atexit(982,71736,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68792,54072,10672);_atexit(982,68792,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65848,54072,10664);_atexit(982,65848,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64376,54072,10640);_atexit(982,64376,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62904,54072,10624);_atexit(982,62904,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59960,62904,10616);_atexit(982,59960,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58488,62904,10600);_atexit(982,58488,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(57016,62904,10592);_atexit(982,57016,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55544,62904,10568);_atexit(982,55544,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70264,54072,10560);_atexit(982,70264,___dso_handle);return}function __ZN7toadlet6peeper19IndexBufferAccessorD0Ev(r1){var r2,r3;HEAP32[r1>>2]=29728;r2=r1+4|0;r3=HEAP32[r2>>2];do{if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+60>>2]](r3);HEAP32[r2>>2]=0;HEAP32[r1+12>>2]=0;if((r1|0)!=0){break}return}}while(0);_free(r1);return}function __ZN7toadlet6peeper19IndexBufferAccessorD2Ev(r1){var r2,r3;HEAP32[r1>>2]=29728;r2=r1+4|0;r3=HEAP32[r2>>2];if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+60>>2]](r3);HEAP32[r2>>2]=0;HEAP32[r1+12>>2]=0;return}function __GLOBAL__I_a13573(){__ZN7toadlet3egg6StringC1EPKci(54056,11096);_atexit(982,54056,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73192,54056,10832);_atexit(982,73192,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61416,73192,10728);_atexit(982,61416,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67304,54056,10688);_atexit(982,67304,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71720,54056,10680);_atexit(982,71720,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68776,54056,10672);_atexit(982,68776,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65832,54056,10664);_atexit(982,65832,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64360,54056,10640);_atexit(982,64360,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62888,54056,10624);_atexit(982,62888,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59944,62888,10616);_atexit(982,59944,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58472,62888,10600);_atexit(982,58472,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(57e3,62888,10592);_atexit(982,57e3,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55528,62888,10568);_atexit(982,55528,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70248,54056,10560);_atexit(982,70248,___dso_handle);return}function __ZN7toadlet6peeper9IndexDataD1Ev(r1){var r2;HEAP32[r1>>2]=27864;r2=r1+16|0;r1=HEAP32[r2>>2];if((r1|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+12>>2]](r1|0);HEAP32[r2>>2]=0;return}function __ZN7toadlet6peeper9IndexDataD0Ev(r1){var r2,r3,r4;HEAP32[r1>>2]=27864;r2=r1+16|0;r3=HEAP32[r2>>2];if((r3|0)==0){r4=r1;_free(r4);return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r2>>2]=0;r4=r1;_free(r4);return}function __ZN7toadlet6peeper9IndexData6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet6peeper9IndexData7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __GLOBAL__I_a13737(){__ZN7toadlet3egg6StringC1EPKci(54040,11096);_atexit(982,54040,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73176,54040,10832);_atexit(982,73176,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61400,73176,10728);_atexit(982,61400,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67288,54040,10688);_atexit(982,67288,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71704,54040,10680);_atexit(982,71704,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68760,54040,10672);_atexit(982,68760,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65816,54040,10664);_atexit(982,65816,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64344,54040,10640);_atexit(982,64344,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62872,54040,10624);_atexit(982,62872,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59928,62872,10616);_atexit(982,59928,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58456,62872,10600);_atexit(982,58456,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56984,62872,10592);_atexit(982,56984,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55512,62872,10568);_atexit(982,55512,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70232,54040,10560);_atexit(982,70232,___dso_handle);return}function __ZN7toadlet6peeper23TextureFormatConversion7convertEPhPNS0_13TextureFormatES2_S4_(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41,r42,r43,r44,r45,r46,r47,r48,r49,r50;r5=0;r6=STACKTOP;STACKTOP=STACKTOP+16|0;r7=r6;r8=HEAP32[r2+16>>2];r9=HEAP32[r4+16>>2];r10=HEAP32[r2+32>>2]-HEAP32[r2+20>>2]|0;r11=HEAP32[r2+36>>2]-HEAP32[r2+24>>2]|0;r12=HEAP32[r2+40>>2]-HEAP32[r2+28>>2]|0;r13=HEAP32[r2+44>>2];r14=HEAP32[r2+48>>2];r15=HEAP32[r4+44>>2];r16=HEAP32[r4+48>>2];if((r13|0)<0){r17=r1+(HEAP32[r2+52>>2]+r13)|0}else{r17=r1}if((r15|0)<0){r18=r3+(HEAP32[r4+52>>2]+r15)|0}else{r18=r3}L1868:do{if((r8|0)==257){if((r9|0)==259){r19=0}else{r20=(r8|0)==258;break}while(1){if((r19|0)>=(r12|0)){r5=1960;break L1868}r3=Math_imul(r19,r14)|0;r4=Math_imul(r19,r16)|0;r1=0;while(1){if((r1|0)>=(r11|0)){break}r2=r3+Math_imul(r1,r13)|0;r21=r4+Math_imul(r1,r15)|0;r22=0;while(1){if((r22|0)>=(r10|0)){break}HEAP16[r18+(r21+(r22<<1))>>1]=HEAPU8[r17+(r2+r22)|0]<<8|255;r22=r22+1|0}r1=r1+1|0}r19=r19+1|0}}else{if((r8|0)==259&(r9|0)==257){r23=0}else{r5=1960;break}while(1){if((r23|0)>=(r12|0)){r5=1960;break L1868}r1=Math_imul(r23,r14)|0;r4=Math_imul(r23,r16)|0;r3=0;while(1){if((r3|0)>=(r11|0)){break}r22=r1+Math_imul(r3,r13)|0;r2=r4+Math_imul(r3,r15)|0;r21=0;while(1){if((r21|0)>=(r10|0)){break}HEAP8[r18+(r2+r21)|0]=HEAPU16[r17+(r22+(r21<<1))>>1]>>>8&255;r21=r21+1|0}r3=r3+1|0}r23=r23+1|0}}}while(0);do{if(r5==1960){r23=(r8|0)==258;if(r23){if((r9|0)==259){r24=0}else{r20=r23;break}while(1){if((r24|0)>=(r12|0)){r25=1;break}r19=Math_imul(r24,r14)|0;r3=Math_imul(r24,r16)|0;r4=0;while(1){if((r4|0)>=(r11|0)){break}r1=r19+Math_imul(r4,r13)|0;r21=r3+Math_imul(r4,r15)|0;r22=0;while(1){if((r22|0)>=(r10|0)){break}HEAP16[r18+(r21+(r22<<1))>>1]=HEAPU8[r17+(r1+r22)|0]|-256;r22=r22+1|0}r4=r4+1|0}r24=r24+1|0}STACKTOP=r6;return r25}else{if((r8|0)==259&(r9|0)==258){r26=0}else{r20=r23;break}while(1){if((r26|0)>=(r12|0)){r25=1;break}r4=Math_imul(r26,r14)|0;r3=Math_imul(r26,r16)|0;r19=0;while(1){if((r19|0)>=(r11|0)){break}r22=r4+Math_imul(r19,r13)|0;r1=r3+Math_imul(r19,r15)|0;r21=0;while(1){if((r21|0)>=(r10|0)){break}HEAP8[r18+(r1+r21)|0]=HEAP16[r17+(r22+(r21<<1))>>1]&255;r21=r21+1|0}r19=r19+1|0}r26=r26+1|0}STACKTOP=r6;return r25}}}while(0);do{if((r8|0)==260|(r8|0)==257){if((r9-264|0)>>>0<2){r27=0}else{break}while(1){if((r27|0)>=(r12|0)){r25=1;break}r26=Math_imul(r27,r14)|0;r24=Math_imul(r27,r16)|0;r23=0;while(1){if((r23|0)>=(r11|0)){break}r19=r26+Math_imul(r23,r13)|0;r3=r24+Math_imul(r23,r15)|0;r4=0;while(1){if((r4|0)>=(r10|0)){break}r21=HEAPU8[r17+(r19+r4)|0];HEAP32[r18+(r3+(r4<<2))>>2]=r21|r21<<8|r21<<16|-16777216;r4=r4+1|0}r23=r23+1|0}r27=r27+1|0}STACKTOP=r6;return r25}}while(0);r27=(r8|0)==264;if((r27|(r8|0)==265)&(r9|0)==257){r23=0;while(1){if((r23|0)>=(r12|0)){r25=1;break}r24=Math_imul(r23,r14)|0;r26=Math_imul(r23,r16)|0;r4=0;while(1){if((r4|0)>=(r11|0)){break}r3=r24+Math_imul(r4,r13)|0;r19=r26+Math_imul(r4,r15)|0;r21=0;while(1){if((r21|0)>=(r10|0)){break}HEAP8[r18+(r19+r21)|0]=HEAP32[r17+(r3+(r21<<2))>>2]&255;r21=r21+1|0}r4=r4+1|0}r23=r23+1|0}STACKTOP=r6;return r25}do{if((r8|0)==260|(r8|0)==257){if((r9-262|0)>>>0<2){r28=0}else{break}while(1){if((r28|0)>=(r12|0)){r25=1;break}r23=Math_imul(r28,r14)|0;r4=Math_imul(r28,r16)|0;r26=0;while(1){if((r26|0)>=(r11|0)){break}r24=r23+Math_imul(r26,r13)|0;r21=r4+Math_imul(r26,r15)|0;r3=0;while(1){if((r3|0)>=(r10|0)){break}r19=r17+(r24+r3)|0;r22=r21+(r3*3&-1)|0;HEAP8[r18+r22|0]=HEAP8[r19];HEAP8[r18+(r22+1)|0]=HEAP8[r19];HEAP8[r18+(r22+2)|0]=HEAP8[r19];r3=r3+1|0}r26=r26+1|0}r28=r28+1|0}STACKTOP=r6;return r25}}while(0);r28=(r8|0)==262;do{if((r8-262|0)>>>0<2){if((r9|0)==257|(r9|0)==260){r29=0}else{break}while(1){if((r29|0)>=(r12|0)){r25=1;break}r26=Math_imul(r29,r14)|0;r4=Math_imul(r29,r16)|0;r23=0;while(1){if((r23|0)>=(r11|0)){break}r3=r26+Math_imul(r23,r13)|0;r21=r4+Math_imul(r23,r15)|0;r24=0;while(1){if((r24|0)>=(r10|0)){break}HEAP8[r18+(r21+r24)|0]=HEAP8[r17+(r3+(r24*3&-1))|0];r24=r24+1|0}r23=r23+1|0}r29=r29+1|0}STACKTOP=r6;return r25}}while(0);do{if(r20){if((r9-264|0)>>>0<2){r30=0}else{break}while(1){if((r30|0)>=(r12|0)){r25=1;break}r29=Math_imul(r30,r14)|0;r23=Math_imul(r30,r16)|0;r4=0;while(1){if((r4|0)>=(r11|0)){break}r26=r29+Math_imul(r4,r13)|0;r24=r23+Math_imul(r4,r15)|0;r3=0;while(1){if((r3|0)>=(r10|0)){break}HEAP32[r18+(r24+(r3<<2))>>2]=HEAPU8[r17+(r26+r3)|0]<<24|16777215;r3=r3+1|0}r4=r4+1|0}r30=r30+1|0}STACKTOP=r6;return r25}}while(0);do{if((r8|0)==259){if((r9-264|0)>>>0<2){r31=0}else{r5=2047;break}while(1){if((r31|0)>=(r12|0)){r25=1;break}r30=Math_imul(r31,r14)|0;r20=Math_imul(r31,r16)|0;r4=0;while(1){if((r4|0)>=(r11|0)){break}r23=r30+Math_imul(r4,r13)|0;r29=r20+Math_imul(r4,r15)|0;r3=0;while(1){if((r3|0)>=(r10|0)){break}r26=HEAPU16[r17+(r23+(r3<<1))>>1];r24=r26<<16;HEAP32[r18+(r29+(r3<<2))>>2]=r26&255|r26<<8&65280|r24&16711680|r24&-16777216;r3=r3+1|0}r4=r4+1|0}r31=r31+1|0}STACKTOP=r6;return r25}else if((r8|0)==265|(r8|0)==264){if((r9|0)==258){r32=0}else{r5=2047;break}while(1){if((r32|0)>=(r12|0)){r25=1;break}r4=Math_imul(r32,r14)|0;r20=Math_imul(r32,r16)|0;r30=0;while(1){if((r30|0)>=(r11|0)){break}r3=r4+Math_imul(r30,r13)|0;r29=r20+Math_imul(r30,r15)|0;r23=0;while(1){if((r23|0)>=(r10|0)){break}HEAP8[r18+(r29+r23)|0]=HEAP32[r17+(r3+(r23<<2))>>2]>>>24&255;r23=r23+1|0}r30=r30+1|0}r32=r32+1|0}STACKTOP=r6;return r25}else if((r8|0)==262){r5=2057}else{r5=2059}}while(0);do{if(r5==2047){if((r27|(r8|0)==265)&(r9|0)==259){r33=0}else{if(r28){r5=2057;break}else{r5=2059;break}}while(1){if((r33|0)>=(r12|0)){r25=1;break}r32=Math_imul(r33,r14)|0;r31=Math_imul(r33,r16)|0;r30=0;while(1){if((r30|0)>=(r11|0)){break}r20=r32+Math_imul(r30,r13)|0;r4=r31+Math_imul(r30,r15)|0;r23=0;while(1){if((r23|0)>=(r10|0)){break}r3=HEAP32[r17+(r20+(r23<<2))>>2];HEAP16[r18+(r4+(r23<<1))>>1]=(r3&255|r3>>>16&65280)&65535;r23=r23+1|0}r30=r30+1|0}r33=r33+1|0}STACKTOP=r6;return r25}}while(0);do{if(r5==2057){if((r9|0)==264){r34=0;r5=2061;break}r35=(r8|0)==263;r5=2070}else if(r5==2059){r33=(r8|0)==263;if(r33){if((r9|0)==265){r34=0;r5=2061;break}else{r35=r33;r5=2070;break}}else{r36=r8;r37=r33}if((r36|0)==264&(r9|0)==262){r38=0}else{r33=(r8|0)==265;if(!r33){r39=r8;r40=r37;r41=r33;r5=2081;break}if((r9|0)==263){r38=0}else{r42=r37;r43=r33;r5=2093;break}}while(1){if((r38|0)>=(r12|0)){r25=1;break}r33=Math_imul(r38,r14)|0;r28=Math_imul(r38,r16)|0;r30=0;while(1){if((r30|0)>=(r11|0)){break}r31=r33+Math_imul(r30,r13)|0;r32=r28+Math_imul(r30,r15)|0;r23=0;while(1){if((r23|0)>=(r10|0)){break}r4=r17+(r31+(r23<<2))|0;r20=r32+(r23*3&-1)|0;HEAP8[r18+r20|0]=HEAP32[r4>>2]&255;HEAP8[r18+(r20+1)|0]=HEAP32[r4>>2]>>>8&255;HEAP8[r18+(r20+2)|0]=HEAP32[r4>>2]>>>16&255;r23=r23+1|0}r30=r30+1|0}r38=r38+1|0}STACKTOP=r6;return r25}}while(0);if(r5==2061){while(1){r5=0;if((r34|0)>=(r12|0)){r25=1;break}r38=Math_imul(r34,r14)|0;r37=Math_imul(r34,r16)|0;r36=0;while(1){if((r36|0)>=(r11|0)){break}r30=r38+Math_imul(r36,r13)|0;r28=r37+Math_imul(r36,r15)|0;r33=0;while(1){if((r33|0)>=(r10|0)){break}r23=r30+(r33*3&-1)|0;HEAP32[r18+(r28+(r33<<2))>>2]=HEAPU8[r17+(r23+2)|0]<<16|HEAPU8[r17+(r23+1)|0]<<8|HEAPU8[r17+r23|0]|-16777216;r33=r33+1|0}r36=r36+1|0}r34=r34+1|0;r5=2061}STACKTOP=r6;return r25}else if(r5==2070){r39=r8;r40=r35;r41=(r8|0)==265;r5=2081}L2078:do{if(r5==2081){do{if((r39|0)==262&(r9|0)==265){r44=0}else{if(r40){if((r9|0)==264){r44=0;break}else{r45=r41;r46=r40;r5=2094;break L2078}}else{r35=r41;r34=r40;if((r8|0)==265){r42=r34;r43=r35;r5=2093;break L2078}else{r45=r35;r46=r34;r5=2094;break L2078}}}}while(0);while(1){if((r44|0)>=(r12|0)){r25=1;break}r34=Math_imul(r44,r14)|0;r35=Math_imul(r44,r16)|0;r36=0;while(1){if((r36|0)>=(r11|0)){break}r37=r34+Math_imul(r36,r13)|0;r38=r35+Math_imul(r36,r15)|0;r33=0;while(1){if((r33|0)>=(r10|0)){break}r28=r37+(r33*3&-1)|0;HEAP32[r18+(r38+(r33<<2))>>2]=HEAPU8[r17+r28|0]<<16|HEAPU8[r17+(r28+1)|0]<<8|HEAPU8[r17+(r28+2)|0]|-16777216;r33=r33+1|0}r36=r36+1|0}r44=r44+1|0}STACKTOP=r6;return r25}}while(0);if(r5==2093){if((r9|0)==262){r47=0}else{r45=r43;r46=r42;r5=2094}}do{if(r5==2094){if(r27){if((r9|0)==263){r47=0;break}else{r5=2105}}else{if((r8|0)==262&(r9|0)==263){r48=0}else{r5=2105}}do{if(r5==2105){if(r46){if((r9|0)==262){r48=0;break}else{r5=2116}}else{if((r8|0)==264&(r9|0)==265){r49=0}else{r5=2116}}L2109:do{if(r5==2116){do{if(r45){if((r9|0)==264){r49=0;break L2109}}else{if((r8|0)==4108&(r9|0)==264){r42=0;while(1){if((r42|0)>=(r12|0)){r25=1;break}r43=Math_imul(r42,r14)|0;r44=Math_imul(r42,r16)|0;r40=0;while(1){if((r40|0)>=(r11|0)){break}r41=r17+(r43+Math_imul(r40,r13))|0;r39=r18+(r44+Math_imul(r40,r15))|0;r36=0;while(1){if((r36|0)>=(r10|0)){break}r35=HEAPU16[r41+(r36<<1)>>1];r34=r36|1;r33=HEAPU16[r41+(r34<<1)>>1];r38=(r35>>>8)-128|0;r37=(r33>>>8)-128|0;r28=((r35&255)*298&-1)-4768|0;r35=r37*409&-1;r30=r28+r35+128>>8;r23=(r30|0)<0?0:r30;r30=r38*-100&-1;r32=r37*-208&-1;r37=r28+r30+r32+128>>8;r31=(r37|0)<0?0:r37;r37=r38*516&-1;r38=r28+r37+128>>8;r28=(r38|0)<0?0:r38;HEAP32[r39+(r36<<2)>>2]=((r23|0)>255?255:r23)|((r31|0)>255?65280:r31<<8)|((r28|0)>255?16711680:r28<<16);r28=((r33&255)*298&-1)-4768|0;r33=r28+r35+128>>8;r35=(r33|0)<0?0:r33;r33=r28+r30+r32+128>>8;r32=(r33|0)<0?0:r33;r33=r28+r37+128>>8;r37=(r33|0)<0?0:r33;HEAP32[r39+(r34<<2)>>2]=((r35|0)>255?255:r35)|((r32|0)>255?65280:r32<<8)|((r37|0)>255?16711680:r37<<16);r36=r36+2|0}r40=r40+1|0}r42=r42+1|0}STACKTOP=r6;return r25}if((r8|0)==4108&(r9|0)==265){r50=0}else{break}while(1){if((r50|0)>=(r12|0)){r25=1;break}r42=Math_imul(r50,r14)|0;r40=Math_imul(r50,r16)|0;r44=0;while(1){if((r44|0)>=(r11|0)){break}r43=r17+(r42+Math_imul(r44,r13))|0;r36=r18+(r40+Math_imul(r44,r15))|0;r39=0;while(1){if((r39|0)>=(r10|0)){break}r41=HEAPU16[r43+(r39<<1)>>1];r37=r39|1;r32=HEAPU16[r43+(r37<<1)>>1];r35=(r41>>>8)-128|0;r34=(r32>>>8)-128|0;r33=((r41&255)*298&-1)-4768|0;r41=r34*409&-1;r28=r33+r41+128>>8;r30=(r28|0)<0?0:r28;r28=r35*-100&-1;r31=r34*-208&-1;r34=r33+r28+r31+128>>8;r23=(r34|0)<0?0:r34;r34=r35*516&-1;r35=r33+r34+128>>8;r33=(r35|0)<0?0:r35;HEAP32[r36+(r39<<2)>>2]=((r30|0)>255?16711680:r30<<16)|((r23|0)>255?65280:r23<<8)|((r33|0)>255?255:r33);r33=((r32&255)*298&-1)-4768|0;r32=r33+r41+128>>8;r41=(r32|0)<0?0:r32;r32=r33+r28+r31+128>>8;r31=(r32|0)<0?0:r32;r32=r33+r34+128>>8;r34=(r32|0)<0?0:r32;HEAP32[r36+(r37<<2)>>2]=((r41|0)>255?16711680:r41<<16)|((r31|0)>255?65280:r31<<8)|((r34|0)>255?255:r34);r39=r39+2|0}r44=r44+1|0}r50=r50+1|0}STACKTOP=r6;return r25}}while(0);if((r8|0)==(r9|0)){r44=0;while(1){if((r44|0)>=(r12|0)){r25=1;break}r40=Math_imul(r44,r16)|0;r42=Math_imul(r44,r14)|0;r39=0;while(1){if((r39|0)>=(r11|0)){break}r36=r18+(r40+Math_imul(r39,r15))|0;r43=r17+(r42+Math_imul(r39,r13))|0;r34=Math_abs(r15);_memcpy(r36,r43,r34)|0;r39=r39+1|0}r44=r44+1|0}STACKTOP=r6;return r25}__ZN7toadlet3egg6StringC1EPKci(r7,9944);__ZN7toadlet3egg5Error7unknownERKNS0_6StringEbS4_(73144,r7);r44=HEAP32[r7+4>>2];if(!((r44|0)==52432|(r44|0)==0)){_free(r44)}r44=HEAP32[r7+8>>2];if((r44|0)==52432|(r44|0)==0){r25=0;STACKTOP=r6;return r25}_free(r44);r25=0;STACKTOP=r6;return r25}}while(0);while(1){if((r49|0)>=(r12|0)){r25=1;break}r44=Math_imul(r49,r14)|0;r39=Math_imul(r49,r16)|0;r42=0;while(1){if((r42|0)>=(r11|0)){break}r40=r44+Math_imul(r42,r13)|0;r34=r39+Math_imul(r42,r15)|0;r43=0;while(1){if((r43|0)>=(r10|0)){break}r36=r43<<2;r31=r40+r36|0;r41=r34+r36|0;HEAP8[r18+r41|0]=HEAP8[r17+(r31+2)|0];HEAP8[r18+(r41+1)|0]=HEAP8[r17+(r31+1)|0];HEAP8[r18+(r41+2)|0]=HEAP8[r17+r31|0];HEAP8[r18+(r41+3)|0]=HEAP8[r17+(r31+3)|0];r43=r43+1|0}r42=r42+1|0}r49=r49+1|0}STACKTOP=r6;return r25}}while(0);while(1){if((r48|0)>=(r12|0)){r25=1;break}r42=Math_imul(r48,r14)|0;r39=Math_imul(r48,r16)|0;r44=0;while(1){if((r44|0)>=(r11|0)){break}r43=r42+Math_imul(r44,r13)|0;r34=r39+Math_imul(r44,r15)|0;r40=0;while(1){if((r40|0)>=(r10|0)){break}r31=r40*3&-1;r41=r43+r31|0;r36=r34+r31|0;HEAP8[r18+r36|0]=HEAP8[r17+(r41+2)|0];HEAP8[r18+(r36+1)|0]=HEAP8[r17+(r41+1)|0];HEAP8[r18+(r36+2)|0]=HEAP8[r17+r41|0];r40=r40+1|0}r44=r44+1|0}r48=r48+1|0}STACKTOP=r6;return r25}}while(0);while(1){if((r47|0)>=(r12|0)){r25=1;break}r48=Math_imul(r47,r14)|0;r49=Math_imul(r47,r16)|0;r7=0;while(1){if((r7|0)>=(r11|0)){break}r9=r48+Math_imul(r7,r13)|0;r8=r49+Math_imul(r7,r15)|0;r50=0;while(1){if((r50|0)>=(r10|0)){break}r45=r17+(r9+(r50<<2))|0;r5=r8+(r50*3&-1)|0;HEAP8[r18+r5|0]=HEAP32[r45>>2]>>>16&255;HEAP8[r18+(r5+1)|0]=HEAP32[r45>>2]>>>8&255;HEAP8[r18+(r5+2)|0]=HEAP32[r45>>2]&255;r50=r50+1|0}r7=r7+1|0}r47=r47+1|0}STACKTOP=r6;return r25}function __GLOBAL__I_a13905(){__ZN7toadlet3egg6StringC1EPKci(54008,11096);_atexit(982,54008,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73144,54008,10832);_atexit(982,73144,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61368,73144,10728);_atexit(982,61368,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67256,54008,10688);_atexit(982,67256,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71672,54008,10680);_atexit(982,71672,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68728,54008,10672);_atexit(982,68728,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65784,54008,10664);_atexit(982,65784,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64328,54008,10640);_atexit(982,64328,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62856,54008,10624);_atexit(982,62856,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59912,62856,10616);_atexit(982,59912,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58440,62856,10600);_atexit(982,58440,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56968,62856,10592);_atexit(982,56968,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55496,62856,10568);_atexit(982,55496,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70216,54008,10560);_atexit(982,70216,___dso_handle);return}function __ZN7toadlet6peeper20VariableBufferFormat7compileENS_3egg13SharedPointerINS1_8VariableENS2_22DefaultSharedSemanticsEEEPS4_(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13;r3=STACKTOP;STACKTOP=STACKTOP+8|0;r4=r3;r5=r1+40|0;r6=r5|0;r7=HEAP32[r6>>2];if((r7+1|0)>(HEAP32[r1+44>>2]|0)){__ZN7toadlet3egg10CollectionINS0_13SharedPointerINS_6peeper20VariableBufferFormat8VariableENS0_22DefaultSharedSemanticsEEEE7reserveEi(r5,(r7<<1)+2|0);r8=HEAP32[r6>>2]}else{r8=r7}r7=HEAP32[r1+48>>2];r9=r7+(r8<<3)|0;do{if((r9|0)==(r2|0)){r10=r2|0}else{__ZN7toadlet3egg13SharedPointerINS_6peeper20VariableBufferFormat8VariableENS0_22DefaultSharedSemanticsEE7cleanupEv(r9);r11=r2|0;HEAP32[r9>>2]=HEAP32[r11>>2];r12=HEAP32[r2+4>>2];HEAP32[r7+(r8<<3)+4>>2]=r12;if((r12|0)==0){r10=r11;break}r13=r12+4|0;HEAP32[r13>>2]=HEAP32[r13>>2]+1;r13=r12+8|0;HEAP32[r13>>2]=HEAP32[r13>>2]+1;r10=r11}}while(0);r8=HEAP32[r6>>2]+1|0;HEAP32[r6>>2]=r8;r6=r1+60|0;HEAP32[r6>>2]=r5;HEAP32[r6+4>>2]=r8;r8=HEAP32[r10>>2];__ZN7toadlet3egg6StringaSERKS1_(r8+12|0,r8|0);r8=r4|0;r6=r4+4|0;r5=0;while(1){r7=HEAP32[r10>>2];if((r5|0)>=(HEAP32[r7+48>>2]|0)){break}r2=HEAP32[r7+56>>2];HEAP32[r8>>2]=HEAP32[r2+(r5<<3)>>2];r7=HEAP32[r2+(r5<<3)+4>>2];HEAP32[r6>>2]=r7;if((r7|0)!=0){r2=r7+4|0;HEAP32[r2>>2]=HEAP32[r2>>2]+1;r2=r7+8|0;HEAP32[r2>>2]=HEAP32[r2>>2]+1}__ZN7toadlet6peeper20VariableBufferFormat7compileENS_3egg13SharedPointerINS1_8VariableENS2_22DefaultSharedSemanticsEEEPS4_(r1,r4);__ZN7toadlet3egg13SharedPointerINS_6peeper20VariableBufferFormat8VariableENS0_22DefaultSharedSemanticsEE7cleanupEv(r4);r5=r5+1|0}STACKTOP=r3;return}function __GLOBAL__I_a14076(){__ZN7toadlet3egg6StringC1EPKci(53992,11096);_atexit(982,53992,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73128,53992,10832);_atexit(982,73128,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61352,73128,10728);_atexit(982,61352,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67240,53992,10688);_atexit(982,67240,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71656,53992,10680);_atexit(982,71656,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68712,53992,10672);_atexit(982,68712,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65768,53992,10664);_atexit(982,65768,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64296,53992,10640);_atexit(982,64296,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62824,53992,10624);_atexit(982,62824,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59880,62824,10616);_atexit(982,59880,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58408,62824,10600);_atexit(982,58408,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56936,62824,10592);_atexit(982,56936,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55464,62824,10568);_atexit(982,55464,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70184,53992,10560);_atexit(982,70184,___dso_handle);return}function __ZN7toadlet6peeper20VertexBufferAccessor4lockEPNS0_6BufferEPNS0_12VertexFormatEi(r1,r2,r3,r4){var r5,r6,r7,r8;r5=r1+4|0;r6=HEAP32[r5>>2];if((r6|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r6>>2]+60>>2]](r6);HEAP32[r5>>2]=0;r6=r1+80|0;HEAP32[r6>>2]=0;HEAP32[r6+4>>2]=0;HEAP32[r6+8>>2]=0;HEAP32[r6+12>>2]=0}HEAP32[r5>>2]=r2;r2=r3;HEAP32[r1+8>>2]=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+80>>2]](r3)>>>2;r6=FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+56>>2]](r3);r2=r3;r7=0;while(1){r8=HEAP32[r2>>2];if((r7|0)>=(r6|0)){break}HEAP32[r1+12+(r7<<2)>>2]=FUNCTION_TABLE[HEAP32[r8+76>>2]](r3,r7)>>>2;r7=r7+1|0}r7=r1+76|0;if((FUNCTION_TABLE[HEAP32[r8+72>>2]](r3,0)&255|0)==5){HEAP8[r7]=1}else{HEAP8[r7]=0}r7=HEAP32[r5>>2];r5=FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+56>>2]](r7,r4);r4=r1+80|0;HEAP32[r4>>2]=r5;if((HEAP8[r1+76|0]&1)==0){HEAP32[r1+88>>2]=r5}else{HEAP32[r1+84>>2]=r5}if((FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+84>>2]](r3,4)|0)<=0){return}HEAP32[r1+92>>2]=HEAP32[r4>>2];return}function __ZN7toadlet6peeper20VertexBufferAccessorD0Ev(r1){var r2,r3,r4;HEAP32[r1>>2]=29032;r2=r1+4|0;r3=HEAP32[r2>>2];do{if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+60>>2]](r3);HEAP32[r2>>2]=0;r4=r1+80|0;HEAP32[r4>>2]=0;HEAP32[r4+4>>2]=0;HEAP32[r4+8>>2]=0;HEAP32[r4+12>>2]=0;if((r1|0)!=0){break}return}}while(0);_free(r1);return}function __ZN7toadlet6peeper20VertexBufferAccessorD2Ev(r1){var r2,r3;HEAP32[r1>>2]=29032;r2=r1+4|0;r3=HEAP32[r2>>2];if((r3|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+60>>2]](r3);HEAP32[r2>>2]=0;r2=r1+80|0;HEAP32[r2>>2]=0;HEAP32[r2+4>>2]=0;HEAP32[r2+8>>2]=0;HEAP32[r2+12>>2]=0;return}function __GLOBAL__I_a14240(){__ZN7toadlet3egg6StringC1EPKci(53976,11096);_atexit(982,53976,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73112,53976,10832);_atexit(982,73112,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61336,73112,10728);_atexit(982,61336,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67224,53976,10688);_atexit(982,67224,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71640,53976,10680);_atexit(982,71640,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68696,53976,10672);_atexit(982,68696,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65752,53976,10664);_atexit(982,65752,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64280,53976,10640);_atexit(982,64280,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62808,53976,10624);_atexit(982,62808,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59864,62808,10616);_atexit(982,59864,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58392,62808,10600);_atexit(982,58392,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56920,62808,10592);_atexit(982,56920,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55448,62808,10568);_atexit(982,55448,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70168,53976,10560);_atexit(982,70168,___dso_handle);return}function __ZN7toadlet6peeper10VertexDataC2ENS_3egg16IntrusivePointerINS0_12VertexBufferENS2_25DefaultIntrusiveSemanticsEEE(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14;r3=0;r4=STACKTOP;STACKTOP=STACKTOP+40|0;r5=r4;r6=r4+8;r7=r4+24;r8=r4+32;HEAP32[r1+4>>2]=0;HEAP32[r1+8>>2]=0;HEAP32[r1>>2]=34160;r9=r1+12|0;r10=r1+20|0;r11=r1+24|0;r12=r9;r13=0;r14=r9;HEAP32[r14>>2]=0;HEAP32[r14+4>>2]=0;HEAP32[r14+8>>2]=0;HEAP32[r14+12>>2]=0;HEAP32[r14+16>>2]=0;HEAP32[r11>>2]=r12;HEAP32[r11+4>>2]=r13;r11=r1+32|0;HEAP32[r11>>2]=r12;HEAP32[r11+4>>2]=r13;r13=r1+40|0;HEAP32[r13>>2]=0;r11=r2|0;r2=HEAP32[r11>>2];FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+84>>2]](r7,r2);r2=HEAP32[r13>>2];if((r2|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+12>>2]](r2|0);HEAP32[r13>>2]=0}r2=r7|0;r7=HEAP32[r2>>2];HEAP32[r13>>2]=r7;do{if((r7|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+8>>2]](r7|0);r13=HEAP32[r2>>2];if((r13|0)==0){break}FUNCTION_TABLE[HEAP32[HEAP32[r13>>2]+12>>2]](r13|0);HEAP32[r2>>2]=0}}while(0);r2=HEAP32[r11>>2];r11=r8|0;HEAP32[r11>>2]=r2;if((r2|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+8>>2]](r2|0)}do{if((HEAP32[r1+12>>2]|0)>0){r2=HEAP32[HEAP32[r10>>2]>>2];FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+84>>2]](r5,r2);r2=r5|0;r7=HEAP32[r2>>2];r13=(HEAP32[r1+40>>2]|0)==(r7|0);if((r7|0)==0){if(!r13){r3=2259;break}}else{FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+12>>2]](r7|0);HEAP32[r2>>2]=0;if(!r13){r3=2259;break}}__ZN7toadlet3egg6StringC1EPKci(r6,9864);__ZN7toadlet3egg5Error7unknownERKNS0_6StringEbS4_(65736,r6);r13=HEAP32[r6+4>>2];if(!((r13|0)==52432|(r13|0)==0)){_free(r13)}r13=HEAP32[r6+8>>2];if((r13|0)==52432|(r13|0)==0){break}_free(r13)}else{r3=2259}}while(0);if(r3==2259){__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper12VertexBufferENS0_25DefaultIntrusiveSemanticsEEEE3addERKS6_(r9,r8)}r8=HEAP32[r11>>2];if((r8|0)==0){STACKTOP=r4;return}FUNCTION_TABLE[HEAP32[HEAP32[r8>>2]+12>>2]](r8|0);HEAP32[r11>>2]=0;STACKTOP=r4;return}function __ZN7toadlet6peeper10VertexDataD1Ev(r1){var r2,r3;HEAP32[r1>>2]=34160;r2=r1+40|0;r3=HEAP32[r2>>2];if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r2>>2]=0}__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper12VertexBufferENS0_25DefaultIntrusiveSemanticsEEEED1Ev(HEAP32[r1+20>>2]);return}function __ZN7toadlet6peeper10VertexDataD0Ev(r1){var r2,r3;HEAP32[r1>>2]=34160;r2=r1+40|0;r3=HEAP32[r2>>2];if((r3|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r2>>2]=0}__ZN7toadlet3egg10CollectionINS0_16IntrusivePointerINS_6peeper12VertexBufferENS0_25DefaultIntrusiveSemanticsEEEED1Ev(HEAP32[r1+20>>2]);_free(r1);return}function __ZN7toadlet6peeper10VertexData6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet6peeper10VertexData7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __GLOBAL__I_a14405(){__ZN7toadlet3egg6StringC1EPKci(53960,11096);_atexit(982,53960,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73096,53960,10832);_atexit(982,73096,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61320,73096,10728);_atexit(982,61320,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67208,53960,10688);_atexit(982,67208,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71624,53960,10680);_atexit(982,71624,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68680,53960,10672);_atexit(982,68680,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65736,53960,10664);_atexit(982,65736,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64264,53960,10640);_atexit(982,64264,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62792,53960,10624);_atexit(982,62792,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59848,62792,10616);_atexit(982,59848,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58376,62792,10600);_atexit(982,58376,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56904,62792,10592);_atexit(982,56904,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55432,62792,10568);_atexit(982,55432,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70152,53960,10560);_atexit(982,70152,___dso_handle);return}function __ZN7toadlet3egg2io10DataStreamC2EPNS1_6StreamE(r1,r2){var r3,r4;HEAP32[r1+4>>2]=0;HEAP32[r1+8>>2]=0;HEAP32[r1>>2]=34736;HEAP32[r1+12>>2]=34812;r3=r1+16|0;HEAP32[r3>>2]=0;r1=(r2|0)==0;if(r1){HEAP32[r3>>2]=r2;return}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+8>>2]](r2|0);r4=HEAP32[r3>>2];if((r4|0)!=0){FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+12>>2]](r4|0);HEAP32[r3>>2]=0}HEAP32[r3>>2]=r2;if(r1){return}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+8>>2]](r2|0);FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+12>>2]](r2|0);return}function __ZN7toadlet3egg2io10DataStreamD0Ev(r1){var r2,r3,r4;HEAP32[r1>>2]=34736;HEAP32[r1+12>>2]=34812;r2=r1+16|0;r3=HEAP32[r2>>2];if((r3|0)==0){r4=r1;_free(r4);return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3|0);HEAP32[r2>>2]=0;r4=r1;_free(r4);return}function __ZThn12_N7toadlet3egg2io10DataStreamD0Ev(r1){var r2,r3,r4;r2=r1-20+8|0;HEAP32[r2>>2]=34736;HEAP32[r2+12>>2]=34812;r1=r2+16|0;r3=HEAP32[r1>>2];if((r3|0)==0){r4=r2;_free(r4);return}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+12>>2]](r3);HEAP32[r1>>2]=0;r4=r2;_free(r4);return}function __ZN7toadlet3egg2io10DataStreamD2Ev(r1){var r2;HEAP32[r1>>2]=34736;HEAP32[r1+12>>2]=34812;r2=r1+16|0;r1=HEAP32[r2>>2];if((r1|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+12>>2]](r1|0);HEAP32[r2>>2]=0;return}function __ZThn12_N7toadlet3egg2io10DataStreamD1Ev(r1){var r2;r2=r1-20+8|0;HEAP32[r2>>2]=34736;HEAP32[r2+12>>2]=34812;r1=r2+16|0;r2=HEAP32[r1>>2];if((r2|0)==0){return}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+12>>2]](r2);HEAP32[r1>>2]=0;return}function __ZN7toadlet3egg2io10DataStream4readEPhi(r1,r2,r3){var r4,r5,r6,r7,r8;r4=0;r5=r1+16|0;r1=0;r6=r3;while(1){if((r6|0)<=0){r7=r1;r4=2356;break}r3=HEAP32[r5>>2];r8=FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+28>>2]](r3,r2+r1|0,r6);if((r8|0)==0){r7=r1;r4=2355;break}if((r8|0)<0){r7=r8;r4=2357;break}r1=r1+r8|0;r6=r6-r8|0}if(r4==2355){return r7}else if(r4==2356){return r7}else if(r4==2357){return r7}}function __ZThn12_N7toadlet3egg2io10DataStream4readEPhi(r1,r2,r3){return __ZN7toadlet3egg2io10DataStream4readEPhi(r1-20+8|0,r2,r3)}function __ZN7toadlet3egg2io10DataStream24readNullTerminatedStringERNS0_6StringE(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13;r3=0;r4=STACKTOP;STACKTOP=STACKTOP+1040|0;r5=r4;r6=r4+16;HEAP32[r5>>2]=0;r7=r5+4|0;HEAP32[r7>>2]=52432;r8=r5+8|0;HEAP32[r8>>2]=52432;__ZN7toadlet3egg6StringaSERKS1_(r2,r5);r5=HEAP32[r7>>2];if(!((r5|0)==52432|(r5|0)==0)){_free(r5)}r5=HEAP32[r8>>2];if(!((r5|0)==52432|(r5|0)==0)){_free(r5)}r5=r1;r8=r6|0;r7=0;r9=0;while(1){r10=r6+r7|0;r11=FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+36>>2]](r1,r10,1);if((r11|0)<0){r12=-1;r3=2378;break}if((r11|0)==0){break}if((HEAP8[r10]|0)==0){break}r13=r9+r11|0;r11=r7+1|0;if((r11|0)!=1023){r7=r11;r9=r13;continue}HEAP8[r6+1023|0]=0;__ZN7toadlet3egg6StringpLEPKc(r2,r8);r7=0;r9=r13}if(r3==2378){STACKTOP=r4;return r12}HEAP8[r10]=0;__ZN7toadlet3egg6StringpLEPKc(r2,r8);r12=r9;STACKTOP=r4;return r12}function __ZN7toadlet3egg2io10DataStream24readNullTerminatedStringEv(r1,r2){HEAP32[r1>>2]=0;HEAP32[r1+4>>2]=52432;HEAP32[r1+8>>2]=52432;__ZN7toadlet3egg2io10DataStream24readNullTerminatedStringERNS0_6StringE(r2,r1);return}function __ZN7toadlet3egg2io10DataStream11writeLAABoxERKNS0_4math5AABoxE(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11;r3=STACKTOP;STACKTOP=STACKTOP+48|0;r4=r3;r5=r3+8;r6=r3+16;r7=r3+24;r8=r3+32;r9=r3+40;HEAPF32[r9>>2]=HEAPF32[r2>>2];r10=r1;r11=FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r9,4);HEAPF32[r8>>2]=HEAPF32[r2+4>>2];r9=r11+FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r8,4)|0;HEAPF32[r7>>2]=HEAPF32[r2+8>>2];r8=r9+FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r7,4)|0;HEAPF32[r6>>2]=HEAPF32[r2+12>>2];r7=FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r6,4);HEAPF32[r5>>2]=HEAPF32[r2+16>>2];r6=r7+FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r5,4)|0;HEAPF32[r4>>2]=HEAPF32[r2+20>>2];r2=r8+(r6+FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+44>>2]](r1,r4,4))|0;STACKTOP=r3;return r2}function __ZN7toadlet3egg2io10DataStream6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet3egg2io10DataStream7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet3egg2io10DataStream5closeEv(r1){var r2;r2=HEAP32[r1+16>>2];FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+16>>2]](r2);return}function __ZN7toadlet3egg2io10DataStream6closedEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+20>>2]](r2)}function __ZN7toadlet3egg2io10DataStream8readableEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+24>>2]](r2)}function __ZN7toadlet3egg2io10DataStream9writeableEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+32>>2]](r2)}function __ZN7toadlet3egg2io10DataStream5writeEPKhi(r1,r2,r3){var r4;r4=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+36>>2]](r4,r2,r3)}function __ZN7toadlet3egg2io10DataStream5resetEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+40>>2]](r2)}function __ZN7toadlet3egg2io10DataStream6lengthEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+44>>2]](r2)}function __ZN7toadlet3egg2io10DataStream8positionEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+48>>2]](r2)}function __ZN7toadlet3egg2io10DataStream4seekEi(r1,r2){var r3;r3=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+52>>2]](r3,r2)}function __ZN7toadlet3egg2io10DataStream5flushEv(r1){var r2;r2=HEAP32[r1+16>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+56>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream6retainEv(r1){var r2;r2=r1-20+12|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet3egg2io10DataStream7releaseEv(r1){var r2,r3,r4;r2=r1-20+8|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet3egg2io10DataStream5closeEv(r1){var r2;r2=HEAP32[r1-20+24>>2];FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+16>>2]](r2);return}function __ZThn12_N7toadlet3egg2io10DataStream6closedEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+20>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream8readableEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+24>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream9writeableEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+32>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream5writeEPKhi(r1,r2,r3){var r4;r4=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r4>>2]+36>>2]](r4,r2,r3)}function __ZThn12_N7toadlet3egg2io10DataStream5resetEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+40>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream6lengthEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+44>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream8positionEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+48>>2]](r2)}function __ZThn12_N7toadlet3egg2io10DataStream4seekEi(r1,r2){var r3;r3=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+52>>2]](r3,r2)}function __ZThn12_N7toadlet3egg2io10DataStream5flushEv(r1){var r2;r2=HEAP32[r1-20+24>>2];return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+56>>2]](r2)}function __GLOBAL__I_a14612(){__ZN7toadlet3egg6StringC1EPKci(53944,11096);_atexit(982,53944,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73080,53944,10832);_atexit(982,73080,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61304,73080,10728);_atexit(982,61304,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67192,53944,10688);_atexit(982,67192,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71608,53944,10680);_atexit(982,71608,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68664,53944,10672);_atexit(982,68664,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65720,53944,10664);_atexit(982,65720,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64248,53944,10640);_atexit(982,64248,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62776,53944,10624);_atexit(982,62776,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59832,62776,10616);_atexit(982,59832,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58360,62776,10600);_atexit(982,58360,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56888,62776,10592);_atexit(982,56888,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55416,62776,10568);_atexit(982,55416,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70136,53944,10560);_atexit(982,70136,___dso_handle);return}function __ZN7toadlet3egg2io10FileStreamC2ERKNS0_6StringEi(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12;r4=STACKTOP;STACKTOP=STACKTOP+32|0;r5=r4;r6=r4+16;HEAP32[r1+4>>2]=0;HEAP32[r1+8>>2]=0;HEAP32[r1>>2]=34584;HEAP32[r1+12>>2]=34660;r7=r1+16|0;HEAP32[r7>>2]=0;r8=r1+20|0;HEAP32[r8>>2]=0;r9=r1+24|0;HEAP8[r9]=0;__ZN7toadlet3eggplEPKcRKNS0_6StringE(r5,11e3,r2);r1=HEAP32[52920>>2];if((r1|0)==0){__ZN7toadlet3egg3Log10initializeEb(0);r10=HEAP32[52920>>2]}else{r10=r1}do{if((HEAP32[r10+36>>2]|0)>4){r1=r10;r11=__ZN7toadlet3egg6Logger11getCategoryERKNS0_6StringE(r1,73064);if((r11|0)==0){r12=6}else{r12=HEAP32[r11+12>>2]}if((r12|0)<=4){break}__ZN7toadlet3egg6Logger11addLogEntryERKNS0_6StringENS1_5LevelES4_(r1,73064,5,r5)}}while(0);r12=HEAP32[r5+4>>2];if(!((r12|0)==52432|(r12|0)==0)){_free(r12)}r12=HEAP32[r5+8>>2];if(!((r12|0)==52432|(r12|0)==0)){_free(r12)}HEAP32[r6>>2]=0;r12=r6+4|0;HEAP32[r12>>2]=52432;r5=r6+8|0;HEAP32[r5>>2]=52432;do{if((r3&8|0)==0){if((r3&1|0)!=0){__ZN7toadlet3egg6StringpLEPKc(r6,10872);break}if((r3&2|0)==0){break}__ZN7toadlet3egg6StringpLEPKc(r6,10848)}else{__ZN7toadlet3egg6StringpLEPKc(r6,10904)}}while(0);if((r3&4|0)!=0){__ZN7toadlet3egg6StringpLEPKc(r6,10840)}if((r3&16|0)!=0){__ZN7toadlet3egg6StringpLEPKc(r6,10760)}HEAP32[r7>>2]=r3;r3=HEAP32[r5>>2];HEAP32[r8>>2]=_fopen(HEAP32[r2+8>>2],r3);HEAP8[r9]=1;r9=HEAP32[r12>>2];if(!((r9|0)==52432|(r9|0)==0)){_free(r9)}if((r3|0)==52432|(r3|0)==0){STACKTOP=r4;return}_free(r3);STACKTOP=r4;return}function __ZN7toadlet3egg2io10FileStreamD0Ev(r1){HEAP32[r1>>2]=34584;HEAP32[r1+12>>2]=34660;__ZN7toadlet3egg2io10FileStream5closeEv(r1);_free(r1);return}function __ZThn12_N7toadlet3egg2io10FileStreamD0Ev(r1){var r2;r2=r1-28+16|0;HEAP32[r2>>2]=34584;HEAP32[r2+12>>2]=34660;__ZN7toadlet3egg2io10FileStream5closeEv(r2);_free(r2);return}function __ZN7toadlet3egg2io10FileStreamD2Ev(r1){HEAP32[r1>>2]=34584;HEAP32[r1+12>>2]=34660;__ZN7toadlet3egg2io10FileStream5closeEv(r1);return}function __ZThn12_N7toadlet3egg2io10FileStreamD1Ev(r1){var r2;r2=r1-28+16|0;HEAP32[r2>>2]=34584;HEAP32[r2+12>>2]=34660;__ZN7toadlet3egg2io10FileStream5closeEv(r2);return}function __ZN7toadlet3egg2io10FileStream5closeEv(r1){var r2,r3;r2=r1+20|0;r3=HEAP32[r2>>2];if((r3|0)==0){return}if((HEAP8[r1+24|0]&1)==0){return}_fclose(r3);HEAP32[r2>>2]=0;return}function __ZThn12_N7toadlet3egg2io10FileStream5closeEv(r1){var r2,r3;r2=r1-28+16|0;r1=r2+20|0;r3=HEAP32[r1>>2];if((r3|0)==0){return}if((HEAP8[r2+24|0]&1)==0){return}_fclose(r3);HEAP32[r1>>2]=0;return}function __ZN7toadlet3egg2io10FileStream4readEPhi(r1,r2,r3){return _fread(r2,1,r3,HEAP32[r1+20>>2])}function __ZThn12_N7toadlet3egg2io10FileStream4readEPhi(r1,r2,r3){return _fread(r2,1,r3,HEAP32[r1-28+36>>2])}function __ZN7toadlet3egg2io10FileStream5writeEPKhi(r1,r2,r3){return _fwrite(r2,1,r3,HEAP32[r1+20>>2])}function __ZThn12_N7toadlet3egg2io10FileStream5writeEPKhi(r1,r2,r3){return _fwrite(r2,1,r3,HEAP32[r1-28+36>>2])}function __ZN7toadlet3egg2io10FileStream5resetEv(r1){return FUNCTION_TABLE[HEAP32[HEAP32[r1>>2]+60>>2]](r1,0)}function __ZThn12_N7toadlet3egg2io10FileStream5resetEv(r1){var r2;r2=r1-28+16|0;return FUNCTION_TABLE[HEAP32[HEAP32[r2>>2]+60>>2]](r2,0)}function __ZN7toadlet3egg2io10FileStream6lengthEv(r1){var r2,r3;r2=r1+20|0;r1=_ftell(HEAP32[r2>>2]);_fseek(HEAP32[r2>>2],0,2);r3=_ftell(HEAP32[r2>>2]);_fseek(HEAP32[r2>>2],r1,0);return r3}function __ZThn12_N7toadlet3egg2io10FileStream6lengthEv(r1){var r2,r3;r2=r1-28+36|0;r1=_ftell(HEAP32[r2>>2]);_fseek(HEAP32[r2>>2],0,2);r3=_ftell(HEAP32[r2>>2]);_fseek(HEAP32[r2>>2],r1,0);return r3}function __ZN7toadlet3egg2io10FileStream8positionEv(r1){return _ftell(HEAP32[r1+20>>2])}function __ZThn12_N7toadlet3egg2io10FileStream8positionEv(r1){return _ftell(HEAP32[r1-28+36>>2])}function __ZN7toadlet3egg2io10FileStream4seekEi(r1,r2){return(_fseek(HEAP32[r1+20>>2],r2,0)|0)==0}function __ZThn12_N7toadlet3egg2io10FileStream4seekEi(r1,r2){return(_fseek(HEAP32[r1-28+36>>2],r2,0)|0)==0}function __ZN7toadlet3egg2io10FileStream5flushEv(r1){return(_fflush(HEAP32[r1+20>>2])|0)==0}function __ZThn12_N7toadlet3egg2io10FileStream5flushEv(r1){return(_fflush(HEAP32[r1-28+36>>2])|0)==0}function __ZN7toadlet3egg2io10FileStream6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet3egg2io10FileStream7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet3egg2io10FileStream6closedEv(r1){return(HEAP32[r1+20>>2]|0)==0}function __ZN7toadlet3egg2io10FileStream8readableEv(r1){var r2;if((HEAP32[r1+20>>2]|0)==0){r2=0;return r2}r2=(HEAP32[r1+16>>2]&1|0)!=0;return r2}function __ZN7toadlet3egg2io10FileStream9writeableEv(r1){var r2;if((HEAP32[r1+20>>2]|0)==0){r2=0;return r2}r2=(HEAP32[r1+16>>2]&2|0)!=0;return r2}function __ZThn12_N7toadlet3egg2io10FileStream6retainEv(r1){var r2;r2=r1-28+20|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet3egg2io10FileStream7releaseEv(r1){var r2,r3,r4;r2=r1-28+16|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet3egg2io10FileStream6closedEv(r1){return(HEAP32[r1-28+36>>2]|0)==0}function __ZThn12_N7toadlet3egg2io10FileStream8readableEv(r1){var r2,r3;r2=r1-28+16|0;if((HEAP32[r2+20>>2]|0)==0){r3=0;return r3}r3=(HEAP32[r2+16>>2]&1|0)!=0;return r3}function __ZThn12_N7toadlet3egg2io10FileStream9writeableEv(r1){var r2,r3;r2=r1-28+16|0;if((HEAP32[r2+20>>2]|0)==0){r3=0;return r3}r3=(HEAP32[r2+16>>2]&2|0)!=0;return r3}function __GLOBAL__I_a14664(){__ZN7toadlet3egg6StringC1EPKci(53928,11096);_atexit(982,53928,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73064,53928,10832);_atexit(982,73064,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61288,73064,10728);_atexit(982,61288,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67176,53928,10688);_atexit(982,67176,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71592,53928,10680);_atexit(982,71592,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68648,53928,10672);_atexit(982,68648,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65704,53928,10664);_atexit(982,65704,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64232,53928,10640);_atexit(982,64232,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62760,53928,10624);_atexit(982,62760,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59816,62760,10616);_atexit(982,59816,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58344,62760,10600);_atexit(982,58344,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56872,62760,10592);_atexit(982,56872,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55400,62760,10568);_atexit(982,55400,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70120,53928,10560);_atexit(982,70120,___dso_handle);return}function __ZN7toadlet3egg2io12MemoryStreamC2Ev(r1){var r2,r3,r4,r5;r2=0;HEAP32[r1+4>>2]=0;HEAP32[r1+8>>2]=0;HEAP32[r1>>2]=34432;HEAP32[r1+12>>2]=34508;r3=r1+24|0;r4=r1+16|0;HEAP32[r4>>2]=0;HEAP32[r4+4>>2]=0;HEAP32[r4+8>>2]=0;HEAP32[r4+12>>2]=0;HEAP32[r4+16>>2]=0;HEAP32[r4+20>>2]=0;HEAP8[r4+24|0]=0;while(1){r5=_malloc(28);if((r5|0)!=0){r2=2550;break}r4=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r4|0)==0){break}FUNCTION_TABLE[r4]()}if(r2==2550){r2=r5+12|0;r4=r5;r1=0;HEAP32[r5>>2]=0;HEAP32[r5+4>>2]=0;HEAP32[r5+8>>2]=0;HEAP32[r5+12>>2]=0;HEAP32[r5+16>>2]=0;HEAP32[r2>>2]=r4;HEAP32[r2+4>>2]=r1;r2=r5+20|0;HEAP32[r2>>2]=r4;HEAP32[r2+4>>2]=r1;HEAP32[r3>>2]=r5;return}r5=___cxa_allocate_exception(4);HEAP32[r5>>2]=19680;___cxa_throw(r5,46792,4624)}function __ZN7toadlet3egg2io12MemoryStreamD0Ev(r1){__ZN7toadlet3egg2io12MemoryStreamD2Ev(r1);_free(r1);return}function __ZThn12_N7toadlet3egg2io12MemoryStreamD0Ev(r1){var r2;r2=r1-44+32|0;__ZN7toadlet3egg2io12MemoryStreamD2Ev(r2);_free(r2);return}function __ZN7toadlet3egg2io12MemoryStreamD2Ev(r1){var r2,r3,r4;HEAP32[r1>>2]=34432;HEAP32[r1+12>>2]=34508;r2=r1+16|0;do{if((HEAP8[r1+40|0]&1)==0){r3=r2}else{r4=HEAP32[r2>>2];if((r4|0)==0){r3=r2;break}_free(r4);r3=r2}}while(0);HEAP32[r3>>2]=0;r3=r1+24|0;r1=HEAP32[r3>>2];if((r1|0)==0){return}r2=HEAP32[r1+8>>2];if((r2|0)!=0){_free(r2)}_free(r1);HEAP32[r3>>2]=0;return}function __ZThn12_N7toadlet3egg2io12MemoryStreamD1Ev(r1){__ZN7toadlet3egg2io12MemoryStreamD2Ev(r1-44+32|0);return}function __ZN7toadlet3egg2io12MemoryStream4readEPhi(r1,r2,r3){var r4,r5,r6,r7;r4=r1+16|0;if((HEAP32[r4>>2]|0)==0){r5=HEAP32[r1+24>>2]+8|0}else{r5=r4}r4=r1+36|0;r6=HEAP32[r4>>2];r7=HEAP32[r1+28>>2]-r6|0;r1=(r7|0)>(r3|0)?r3:r7;r7=HEAP32[r5>>2]+r6|0;_memcpy(r2,r7,r1)|0;HEAP32[r4>>2]=HEAP32[r4>>2]+r1;return r1}function __ZThn12_N7toadlet3egg2io12MemoryStream4readEPhi(r1,r2,r3){var r4,r5,r6,r7;r4=r1-44+32|0;r1=r4+16|0;if((HEAP32[r1>>2]|0)==0){r5=HEAP32[r4+24>>2]+8|0}else{r5=r1}r1=r4+36|0;r6=HEAP32[r1>>2];r7=HEAP32[r4+28>>2]-r6|0;r4=(r7|0)>(r3|0)?r3:r7;r7=HEAP32[r5>>2]+r6|0;_memcpy(r2,r7,r4)|0;HEAP32[r1>>2]=HEAP32[r1>>2]+r4;return r4}function __ZN7toadlet3egg2io12MemoryStream5writeEPKhi(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18;r4=HEAP32[r1+16>>2];if((r4|0)!=0){r5=HEAP32[r1+36>>2];r6=HEAP32[r1+20>>2]-r5|0;r7=(r6|0)<(r3|0)?r6:r3;r6=r4+r5|0;_memcpy(r6,r2,r7)|0;r8=r7;r9=r1+36|0;r10=HEAP32[r9>>2];r11=r10+r8|0;HEAP32[r9>>2]=r11;r12=r1+28|0;r13=HEAP32[r12>>2];r14=(r11|0)>(r13|0);r15=r14?r11:r13;HEAP32[r12>>2]=r15;return r8}r7=r1+24|0;r6=HEAP32[r7>>2];r5=HEAP32[r6+4>>2];if((r5|0)>(r3|0)){r16=r6}else{r4=r5<<1;__ZN7toadlet3egg10CollectionIhE7reserveEi(r6,(r4|0)<(r3|0)?r3:r4);r16=HEAP32[r7>>2]}r4=r1+36|0;r6=HEAP32[r4>>2];r5=r6+r3|0;if((r5|0)>(HEAP32[r16>>2]|0)){__ZN7toadlet3egg10CollectionIhE6resizeEi(r16,r5);r17=HEAP32[r7>>2];r18=HEAP32[r4>>2]}else{r17=r16;r18=r6}r6=HEAP32[r17+8>>2]+r18|0;_memcpy(r6,r2,r3)|0;r8=r3;r9=r1+36|0;r10=HEAP32[r9>>2];r11=r10+r8|0;HEAP32[r9>>2]=r11;r12=r1+28|0;r13=HEAP32[r12>>2];r14=(r11|0)>(r13|0);r15=r14?r11:r13;HEAP32[r12>>2]=r15;return r8}function __ZThn12_N7toadlet3egg2io12MemoryStream5writeEPKhi(r1,r2,r3){return __ZN7toadlet3egg2io12MemoryStream5writeEPKhi(r1-44+32|0,r2,r3)}function __ZN7toadlet3egg2io12MemoryStream5resetEv(r1){var r2,r3,r4;HEAP32[r1+28>>2]=HEAP32[r1+32>>2];HEAP32[r1+36>>2]=0;r2=HEAP32[r1+24>>2];if((r2|0)==0){return 1}r1=r2|0;r3=r2+8|0;r4=0;while(1){if((r4|0)>=(HEAP32[r1>>2]|0)){break}HEAP8[HEAP32[r3>>2]+r4|0]=0;r4=r4+1|0}HEAP32[r1>>2]=0;r1=r2+20|0;HEAP32[r1>>2]=r2;HEAP32[r1+4>>2]=0;return 1}function __ZThn12_N7toadlet3egg2io12MemoryStream5resetEv(r1){var r2,r3,r4;r2=r1-44+32|0;HEAP32[r2+28>>2]=HEAP32[r2+32>>2];HEAP32[r2+36>>2]=0;r1=HEAP32[r2+24>>2];if((r1|0)==0){return 1}r2=r1|0;r3=r1+8|0;r4=0;while(1){if((r4|0)>=(HEAP32[r2>>2]|0)){break}HEAP8[HEAP32[r3>>2]+r4|0]=0;r4=r4+1|0}HEAP32[r2>>2]=0;r2=r1+20|0;HEAP32[r2>>2]=r1;HEAP32[r2+4>>2]=0;return 1}function __ZN7toadlet3egg2io12MemoryStream4seekEi(r1,r2){var r3;if((HEAP32[r1+16>>2]|0)==0){r3=HEAP32[HEAP32[r1+24>>2]>>2];HEAP32[r1+36>>2]=(r3|0)<(r2|0)?r3:r2;return 1}else{r3=HEAP32[r1+28>>2];HEAP32[r1+36>>2]=(r3|0)<(r2|0)?r3:r2;return 1}}function __ZThn12_N7toadlet3egg2io12MemoryStream4seekEi(r1,r2){var r3;r3=r1-44+32|0;if((HEAP32[r3+16>>2]|0)==0){r1=HEAP32[HEAP32[r3+24>>2]>>2];HEAP32[r3+36>>2]=(r1|0)<(r2|0)?r1:r2;return 1}else{r1=HEAP32[r3+28>>2];HEAP32[r3+36>>2]=(r1|0)<(r2|0)?r1:r2;return 1}}function __ZN7toadlet3egg2io12MemoryStream6retainEv(r1){var r2;r2=r1+4|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZN7toadlet3egg2io12MemoryStream7releaseEv(r1){var r2,r3,r4;r2=r1|0;r3=r1+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r1;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r2);if((r1|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r2);return r4}function __ZN7toadlet3egg2io12MemoryStream5closeEv(r1){return}function __ZN7toadlet3egg2io12MemoryStream6closedEv(r1){return 0}function __ZN7toadlet3egg2io12MemoryStream8readableEv(r1){return 1}function __ZN7toadlet3egg2io12MemoryStream9writeableEv(r1){return 1}function __ZN7toadlet3egg2io12MemoryStream6lengthEv(r1){return HEAP32[r1+28>>2]}function __ZN7toadlet3egg2io12MemoryStream8positionEv(r1){return HEAP32[r1+36>>2]}function __ZN7toadlet3egg2io12MemoryStream5flushEv(r1){return 0}function __ZThn12_N7toadlet3egg2io12MemoryStream6retainEv(r1){var r2;r2=r1-44+36|0;return(tempValue=HEAP32[r2>>2],HEAP32[r2>>2]=tempValue+1,tempValue)+1|0}function __ZThn12_N7toadlet3egg2io12MemoryStream7releaseEv(r1){var r2,r3,r4;r2=r1-44+32|0;r1=r2;r3=r2+4|0;r4=(tempValue=HEAP32[r3>>2],HEAP32[r3>>2]=tempValue-1,tempValue)-1|0;if((r4|0)>=1){return r4}r3=r2;FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+20>>2]](r1);if((r2|0)==0){return r4}FUNCTION_TABLE[HEAP32[HEAP32[r3>>2]+4>>2]](r1);return r4}function __ZThn12_N7toadlet3egg2io12MemoryStream5closeEv(r1){return}function __ZThn12_N7toadlet3egg2io12MemoryStream6closedEv(r1){return 0}function __ZThn12_N7toadlet3egg2io12MemoryStream8readableEv(r1){return 1}function __ZThn12_N7toadlet3egg2io12MemoryStream9writeableEv(r1){return 1}function __ZThn12_N7toadlet3egg2io12MemoryStream6lengthEv(r1){return HEAP32[r1-44+60>>2]}function __ZThn12_N7toadlet3egg2io12MemoryStream8positionEv(r1){return HEAP32[r1-44+68>>2]}function __ZThn12_N7toadlet3egg2io12MemoryStream5flushEv(r1){return 0}function __ZN7toadlet3egg4math4Math3mulERNS1_9Matrix4x4ERKS3_S6_(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23;r4=r2|0;r5=r3|0;r6=r2+16|0;r7=r3+4|0;r8=r2+32|0;r9=r3+8|0;r10=r2+48|0;r11=r3+12|0;HEAPF32[r1>>2]=HEAPF32[r4>>2]*HEAPF32[r5>>2]+HEAPF32[r6>>2]*HEAPF32[r7>>2]+HEAPF32[r8>>2]*HEAPF32[r9>>2]+HEAPF32[r10>>2]*HEAPF32[r11>>2];r12=r3+16|0;r13=r3+20|0;r14=r3+24|0;r15=r3+28|0;HEAPF32[r1+16>>2]=HEAPF32[r4>>2]*HEAPF32[r12>>2]+HEAPF32[r6>>2]*HEAPF32[r13>>2]+HEAPF32[r8>>2]*HEAPF32[r14>>2]+HEAPF32[r10>>2]*HEAPF32[r15>>2];r16=r3+32|0;r17=r3+36|0;r18=r3+40|0;r19=r3+44|0;HEAPF32[r1+32>>2]=HEAPF32[r4>>2]*HEAPF32[r16>>2]+HEAPF32[r6>>2]*HEAPF32[r17>>2]+HEAPF32[r8>>2]*HEAPF32[r18>>2]+HEAPF32[r10>>2]*HEAPF32[r19>>2];r20=r3+48|0;r21=r3+52|0;r22=r3+56|0;r23=r3+60|0;HEAPF32[r1+48>>2]=HEAPF32[r4>>2]*HEAPF32[r20>>2]+HEAPF32[r6>>2]*HEAPF32[r21>>2]+HEAPF32[r8>>2]*HEAPF32[r22>>2]+HEAPF32[r10>>2]*HEAPF32[r23>>2];r10=r2+4|0;r8=r2+20|0;r6=r2+36|0;r4=r2+52|0;HEAPF32[r1+4>>2]=HEAPF32[r10>>2]*HEAPF32[r5>>2]+HEAPF32[r8>>2]*HEAPF32[r7>>2]+HEAPF32[r6>>2]*HEAPF32[r9>>2]+HEAPF32[r4>>2]*HEAPF32[r11>>2];HEAPF32[r1+20>>2]=HEAPF32[r10>>2]*HEAPF32[r12>>2]+HEAPF32[r8>>2]*HEAPF32[r13>>2]+HEAPF32[r6>>2]*HEAPF32[r14>>2]+HEAPF32[r4>>2]*HEAPF32[r15>>2];HEAPF32[r1+36>>2]=HEAPF32[r10>>2]*HEAPF32[r16>>2]+HEAPF32[r8>>2]*HEAPF32[r17>>2]+HEAPF32[r6>>2]*HEAPF32[r18>>2]+HEAPF32[r4>>2]*HEAPF32[r19>>2];HEAPF32[r1+52>>2]=HEAPF32[r10>>2]*HEAPF32[r20>>2]+HEAPF32[r8>>2]*HEAPF32[r21>>2]+HEAPF32[r6>>2]*HEAPF32[r22>>2]+HEAPF32[r4>>2]*HEAPF32[r23>>2];r4=r2+8|0;r6=r2+24|0;r8=r2+40|0;r10=r2+56|0;HEAPF32[r1+8>>2]=HEAPF32[r4>>2]*HEAPF32[r5>>2]+HEAPF32[r6>>2]*HEAPF32[r7>>2]+HEAPF32[r8>>2]*HEAPF32[r9>>2]+HEAPF32[r10>>2]*HEAPF32[r11>>2];HEAPF32[r1+24>>2]=HEAPF32[r4>>2]*HEAPF32[r12>>2]+HEAPF32[r6>>2]*HEAPF32[r13>>2]+HEAPF32[r8>>2]*HEAPF32[r14>>2]+HEAPF32[r10>>2]*HEAPF32[r15>>2];HEAPF32[r1+40>>2]=HEAPF32[r4>>2]*HEAPF32[r16>>2]+HEAPF32[r6>>2]*HEAPF32[r17>>2]+HEAPF32[r8>>2]*HEAPF32[r18>>2]+HEAPF32[r10>>2]*HEAPF32[r19>>2];HEAPF32[r1+56>>2]=HEAPF32[r4>>2]*HEAPF32[r20>>2]+HEAPF32[r6>>2]*HEAPF32[r21>>2]+HEAPF32[r8>>2]*HEAPF32[r22>>2]+HEAPF32[r10>>2]*HEAPF32[r23>>2];r10=r2+12|0;r8=r2+28|0;r6=r2+44|0;r4=r2+60|0;HEAPF32[r1+12>>2]=HEAPF32[r10>>2]*HEAPF32[r5>>2]+HEAPF32[r8>>2]*HEAPF32[r7>>2]+HEAPF32[r6>>2]*HEAPF32[r9>>2]+HEAPF32[r4>>2]*HEAPF32[r11>>2];HEAPF32[r1+28>>2]=HEAPF32[r10>>2]*HEAPF32[r12>>2]+HEAPF32[r8>>2]*HEAPF32[r13>>2]+HEAPF32[r6>>2]*HEAPF32[r14>>2]+HEAPF32[r4>>2]*HEAPF32[r15>>2];HEAPF32[r1+44>>2]=HEAPF32[r10>>2]*HEAPF32[r16>>2]+HEAPF32[r8>>2]*HEAPF32[r17>>2]+HEAPF32[r6>>2]*HEAPF32[r18>>2]+HEAPF32[r4>>2]*HEAPF32[r19>>2];HEAPF32[r1+60>>2]=HEAPF32[r10>>2]*HEAPF32[r20>>2]+HEAPF32[r8>>2]*HEAPF32[r21>>2]+HEAPF32[r6>>2]*HEAPF32[r22>>2]+HEAPF32[r4>>2]*HEAPF32[r23>>2];return}function __ZN7toadlet3egg4math4Math3mulERNS1_7Vector4ERKNS1_9Matrix4x4E(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12;r3=r1|0;r4=HEAPF32[r3>>2];r5=r1+4|0;r6=HEAPF32[r5>>2];r7=r1+8|0;r8=HEAPF32[r7>>2];r9=r1+12|0;r1=HEAPF32[r9>>2];r10=HEAPF32[r2+4>>2]*r4+HEAPF32[r2+20>>2]*r6+HEAPF32[r2+36>>2]*r8+HEAPF32[r2+52>>2]*r1;r11=HEAPF32[r2+8>>2]*r4+HEAPF32[r2+24>>2]*r6+HEAPF32[r2+40>>2]*r8+HEAPF32[r2+56>>2]*r1;r12=HEAPF32[r2+12>>2]*r4+HEAPF32[r2+28>>2]*r6+HEAPF32[r2+44>>2]*r8+HEAPF32[r2+60>>2]*r1;HEAPF32[r3>>2]=HEAPF32[r2>>2]*r4+HEAPF32[r2+16>>2]*r6+HEAPF32[r2+32>>2]*r8+HEAPF32[r2+48>>2]*r1;HEAPF32[r5>>2]=r10;HEAPF32[r7>>2]=r11;HEAPF32[r9>>2]=r12;return}function __ZN7toadlet3egg4math4Math6invertERNS1_9Matrix4x4ERKS3_(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37;r3=r2+36|0;r4=HEAPF32[r3>>2];r5=r2+56|0;r6=HEAPF32[r5>>2];r7=r2+40|0;r8=HEAPF32[r7>>2];r9=r2+52|0;r10=HEAPF32[r9>>2];r11=r4*r6-r8*r10;r12=r2+20|0;r13=HEAPF32[r12>>2];r14=r2+24|0;r15=HEAPF32[r14>>2];r16=r13*r6-r15*r10;r17=r13*r8-r15*r4;r18=r2+4|0;r19=HEAPF32[r18>>2];r20=r2+8|0;r21=HEAPF32[r20>>2];r22=r19*r6-r21*r10;r23=r19*r8-r21*r4;r24=r19*r15-r21*r13;r21=r2+12|0;r19=r2+16|0;r25=HEAPF32[r19>>2];r26=r2+32|0;r27=HEAPF32[r26>>2];r28=r2+48|0;r29=HEAPF32[r28>>2];r30=r2+28|0;r31=HEAPF32[r30>>2];r32=r2|0;r33=HEAPF32[r32>>2];r34=r2+44|0;r35=HEAPF32[r34>>2];r36=r2+60|0;r2=HEAPF32[r36>>2];r37=r31*(r33*r11-r27*r22+r29*r23)-HEAPF32[r21>>2]*(r25*r11-r27*r16+r29*r17)-r35*(r33*r16-r25*r22+r29*r24)+r2*(r33*r17-r25*r23+r27*r24);if(Math_abs(r37)==0){return}r24=1/r37;HEAPF32[r1>>2]=r24*(r13*(r8*r2-r6*r35)+r4*(r6*r31-r15*r2)+r10*(r15*r35-r8*r31));r31=HEAPF32[r26>>2];r8=HEAPF32[r36>>2];r35=HEAPF32[r28>>2];r15=HEAPF32[r34>>2];r10=HEAPF32[r30>>2];r2=HEAPF32[r19>>2];HEAPF32[r1+16>>2]=r24*(HEAPF32[r14>>2]*(r31*r8-r35*r15)+HEAPF32[r7>>2]*(r35*r10-r2*r8)+HEAPF32[r5>>2]*(r2*r15-r31*r10));r10=HEAPF32[r26>>2];r31=HEAPF32[r9>>2];r15=HEAPF32[r28>>2];r2=HEAPF32[r3>>2];r8=HEAPF32[r12>>2];r35=HEAPF32[r19>>2];HEAPF32[r1+32>>2]=r24*(HEAPF32[r30>>2]*(r10*r31-r15*r2)+HEAPF32[r34>>2]*(r15*r8-r35*r31)+HEAPF32[r36>>2]*(r35*r2-r10*r8));r8=HEAPF32[r9>>2];r10=HEAPF32[r7>>2];r2=HEAPF32[r3>>2];r35=HEAPF32[r5>>2];r31=HEAPF32[r12>>2];r15=HEAPF32[r14>>2];HEAPF32[r1+48>>2]=r24*(HEAPF32[r19>>2]*(r8*r10-r2*r35)+HEAPF32[r26>>2]*(r31*r35-r8*r15)+HEAPF32[r28>>2]*(r2*r15-r31*r10));r10=HEAPF32[r20>>2];r31=HEAPF32[r36>>2];r15=HEAPF32[r5>>2];r2=HEAPF32[r21>>2];r8=HEAPF32[r7>>2];r35=HEAPF32[r34>>2];HEAPF32[r1+4>>2]=r24*(HEAPF32[r3>>2]*(r10*r31-r15*r2)+HEAPF32[r9>>2]*(r8*r2-r10*r35)+HEAPF32[r18>>2]*(r15*r35-r8*r31));r31=HEAPF32[r32>>2];r8=HEAPF32[r36>>2];r35=HEAPF32[r28>>2];r15=HEAPF32[r21>>2];r10=HEAPF32[r26>>2];r2=HEAPF32[r34>>2];HEAPF32[r1+20>>2]=r24*(HEAPF32[r7>>2]*(r31*r8-r35*r15)+HEAPF32[r5>>2]*(r10*r15-r31*r2)+HEAPF32[r20>>2]*(r35*r2-r10*r8));r8=HEAPF32[r32>>2];r10=HEAPF32[r9>>2];r2=HEAPF32[r28>>2];r35=HEAPF32[r18>>2];r31=HEAPF32[r26>>2];r15=HEAPF32[r3>>2];HEAPF32[r1+36>>2]=r24*(HEAPF32[r34>>2]*(r8*r10-r2*r35)+HEAPF32[r36>>2]*(r31*r35-r8*r15)+HEAPF32[r21>>2]*(r2*r15-r31*r10));r10=HEAPF32[r9>>2];r31=HEAPF32[r20>>2];r15=HEAPF32[r18>>2];r2=HEAPF32[r5>>2];r8=HEAPF32[r7>>2];r35=HEAPF32[r3>>2];HEAPF32[r1+52>>2]=r24*(HEAPF32[r26>>2]*(r10*r31-r15*r2)+HEAPF32[r28>>2]*(r15*r8-r35*r31)+HEAPF32[r32>>2]*(r35*r2-r10*r8));r8=HEAPF32[r20>>2];r10=HEAPF32[r30>>2];r2=HEAPF32[r14>>2];r35=HEAPF32[r21>>2];r31=HEAPF32[r36>>2];r15=HEAPF32[r5>>2];HEAPF32[r1+8>>2]=r24*(HEAPF32[r9>>2]*(r8*r10-r2*r35)+HEAPF32[r18>>2]*(r2*r31-r15*r10)+HEAPF32[r12>>2]*(r15*r35-r8*r31));r31=HEAPF32[r32>>2];r8=HEAPF32[r30>>2];r35=HEAPF32[r19>>2];r15=HEAPF32[r21>>2];r10=HEAPF32[r36>>2];r2=HEAPF32[r28>>2];HEAPF32[r1+24>>2]=r24*(HEAPF32[r5>>2]*(r31*r8-r35*r15)+HEAPF32[r20>>2]*(r35*r10-r2*r8)+HEAPF32[r14>>2]*(r2*r15-r31*r10));r10=HEAPF32[r32>>2];r31=HEAPF32[r12>>2];r15=HEAPF32[r19>>2];r2=HEAPF32[r18>>2];r8=HEAPF32[r9>>2];r35=HEAPF32[r28>>2];HEAPF32[r1+40>>2]=r24*(HEAPF32[r36>>2]*(r10*r31-r15*r2)+HEAPF32[r21>>2]*(r15*r8-r35*r31)+HEAPF32[r30>>2]*(r35*r2-r10*r8));r8=HEAPF32[r12>>2];r10=HEAPF32[r20>>2];r2=HEAPF32[r18>>2];r35=HEAPF32[r14>>2];r31=HEAPF32[r9>>2];r9=HEAPF32[r5>>2];HEAPF32[r1+56>>2]=r24*(HEAPF32[r28>>2]*(r8*r10-r2*r35)+HEAPF32[r32>>2]*(r31*r35-r8*r9)+HEAPF32[r19>>2]*(r2*r9-r31*r10));r10=HEAPF32[r7>>2];r31=HEAPF32[r30>>2];r9=HEAPF32[r14>>2];r2=HEAPF32[r34>>2];r8=HEAPF32[r20>>2];r35=HEAPF32[r21>>2];HEAPF32[r1+12>>2]=r24*(HEAPF32[r18>>2]*(r10*r31-r9*r2)+HEAPF32[r12>>2]*(r8*r2-r10*r35)+HEAPF32[r3>>2]*(r9*r35-r8*r31));r31=HEAPF32[r26>>2];r8=HEAPF32[r30>>2];r35=HEAPF32[r19>>2];r9=HEAPF32[r34>>2];r10=HEAPF32[r32>>2];r2=HEAPF32[r21>>2];HEAPF32[r1+28>>2]=r24*(HEAPF32[r20>>2]*(r31*r8-r35*r9)+HEAPF32[r14>>2]*(r10*r9-r31*r2)+HEAPF32[r7>>2]*(r35*r2-r10*r8));r8=HEAPF32[r26>>2];r10=HEAPF32[r12>>2];r2=HEAPF32[r19>>2];r35=HEAPF32[r3>>2];r31=HEAPF32[r32>>2];r9=HEAPF32[r18>>2];HEAPF32[r1+44>>2]=r24*(HEAPF32[r21>>2]*(r8*r10-r2*r35)+HEAPF32[r30>>2]*(r31*r35-r8*r9)+HEAPF32[r34>>2]*(r2*r9-r31*r10));r10=HEAPF32[r12>>2];r12=HEAPF32[r7>>2];r7=HEAPF32[r3>>2];r3=HEAPF32[r14>>2];r14=HEAPF32[r20>>2];r20=HEAPF32[r18>>2];HEAPF32[r1+60>>2]=r24*(HEAPF32[r32>>2]*(r10*r12-r7*r3)+HEAPF32[r19>>2]*(r7*r14-r20*r12)+HEAPF32[r26>>2]*(r20*r3-r10*r14));return}function __ZN7toadlet3egg4math4Math26setQuaternionFromMatrix3x3ERNS1_10QuaternionERKNS1_9Matrix3x3E(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16;r3=STACKTOP;STACKTOP=STACKTOP+40|0;r4=r3;r5=HEAPF32[r2>>2];r6=HEAPF32[r2+4>>2];r7=HEAPF32[r2+8>>2];r8=Math_sqrt(r5*r5+r6*r6+r7*r7);r9=HEAPF32[r2+12>>2];r10=HEAPF32[r2+16>>2];r11=HEAPF32[r2+20>>2];r12=Math_sqrt(r9*r9+r10*r10+r11*r11);r13=HEAPF32[r2+24>>2];r14=HEAPF32[r2+28>>2];r15=HEAPF32[r2+32>>2];r2=Math_sqrt(r13*r13+r14*r14+r15*r15);r16=r5/r8;HEAPF32[r4>>2]=r16;r5=r6/r8;HEAPF32[r4+4>>2]=r5;r6=r7/r8;HEAPF32[r4+8>>2]=r6;r8=r9/r12;HEAPF32[r4+12>>2]=r8;r9=r10/r12;HEAPF32[r4+16>>2]=r9;r10=r11/r12;HEAPF32[r4+20>>2]=r10;r12=r13/r2;HEAPF32[r4+24>>2]=r12;r13=r14/r2;HEAPF32[r4+28>>2]=r13;r14=r15/r2;HEAPF32[r4+32>>2]=r14;r2=r16+r9+r14;if(r2>0){r15=Math_sqrt(r2+1);HEAPF32[r1+12>>2]=r15*.5;r2=.5/r15;HEAPF32[r1>>2]=(r10-r13)*r2;HEAPF32[r1+4>>2]=(r12-r6)*r2;HEAPF32[r1+8>>2]=(r5-r8)*r2;STACKTOP=r3;return}else{r2=r9>r16?1:0;r16=r14>HEAPF32[r4+(r2<<2<<2)>>2]?2:r2;r2=HEAP32[19656+(r16<<2)>>2];r14=HEAP32[19656+(r2<<2)>>2];r9=Math_sqrt(HEAPF32[r4+(r16<<2<<2)>>2]-HEAPF32[r4+(r2<<2<<2)>>2]-HEAPF32[r4+(r14<<2<<2)>>2]+1);r8=r1|0;HEAPF32[r8+(r16<<2)>>2]=r9*.5;r5=.5/r9;r9=r2*3&-1;r6=r14*3&-1;HEAPF32[r1+12>>2]=(HEAPF32[r4+(r9+r14<<2)>>2]-HEAPF32[r4+(r6+r2<<2)>>2])*r5;r1=r16*3&-1;HEAPF32[r8+(r2<<2)>>2]=(HEAPF32[r4+(r1+r2<<2)>>2]+HEAPF32[r4+(r9+r16<<2)>>2])*r5;HEAPF32[r8+(r14<<2)>>2]=(HEAPF32[r4+(r1+r14<<2)>>2]+HEAPF32[r4+(r6+r16<<2)>>2])*r5;STACKTOP=r3;return}}function __ZN7toadlet3egg4math4Math26setQuaternionFromMatrix4x4ERNS1_10QuaternionERKNS1_9Matrix4x4E(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18;r3=STACKTOP;STACKTOP=STACKTOP+64|0;r4=r3;r5=HEAPF32[r2>>2];r6=HEAPF32[r2+4>>2];r7=HEAPF32[r2+8>>2];r8=Math_sqrt(r5*r5+r6*r6+r7*r7);r9=HEAPF32[r2+16>>2];r10=HEAPF32[r2+20>>2];r11=HEAPF32[r2+24>>2];r12=Math_sqrt(r9*r9+r10*r10+r11*r11);r13=HEAPF32[r2+32>>2];r14=HEAPF32[r2+36>>2];r15=HEAPF32[r2+40>>2];r16=Math_sqrt(r13*r13+r14*r14+r15*r15);r17=r4;r18=r2;HEAP32[r17>>2]=HEAP32[r18>>2];HEAP32[r17+4>>2]=HEAP32[r18+4>>2];HEAP32[r17+8>>2]=HEAP32[r18+8>>2];HEAP32[r17+12>>2]=HEAP32[r18+12>>2];HEAP32[r17+16>>2]=HEAP32[r18+16>>2];HEAP32[r17+20>>2]=HEAP32[r18+20>>2];HEAP32[r17+24>>2]=HEAP32[r18+24>>2];HEAP32[r17+28>>2]=HEAP32[r18+28>>2];HEAP32[r17+32>>2]=HEAP32[r18+32>>2];HEAP32[r17+36>>2]=HEAP32[r18+36>>2];HEAP32[r17+40>>2]=HEAP32[r18+40>>2];HEAP32[r17+44>>2]=HEAP32[r18+44>>2];HEAP32[r17+48>>2]=HEAP32[r18+48>>2];HEAP32[r17+52>>2]=HEAP32[r18+52>>2];HEAP32[r17+56>>2]=HEAP32[r18+56>>2];HEAP32[r17+60>>2]=HEAP32[r18+60>>2];r18=r5/r8;HEAPF32[r4>>2]=r18;r5=r6/r8;HEAPF32[r4+4>>2]=r5;r6=r7/r8;HEAPF32[r4+8>>2]=r6;r8=r9/r12;HEAPF32[r4+16>>2]=r8;r9=r10/r12;HEAPF32[r4+20>>2]=r9;r10=r11/r12;HEAPF32[r4+24>>2]=r10;r12=r13/r16;HEAPF32[r4+32>>2]=r12;r13=r14/r16;HEAPF32[r4+36>>2]=r13;r14=r15/r16;HEAPF32[r4+40>>2]=r14;r16=r18+r9+r14;if(r16>0){r15=Math_sqrt(r16+1);HEAPF32[r1+12>>2]=r15*.5;r16=.5/r15;HEAPF32[r1>>2]=(r10-r13)*r16;HEAPF32[r1+4>>2]=(r12-r6)*r16;HEAPF32[r1+8>>2]=(r5-r8)*r16;STACKTOP=r3;return}else{r16=r9>r18?1:0;r18=r14>HEAPF32[r4+((-r16&5)<<2)>>2]?2:r16;r16=HEAP32[19656+(r18<<2)>>2];r14=HEAP32[19656+(r16<<2)>>2];r9=Math_sqrt(HEAPF32[r4+((r18*5&-1)<<2)>>2]-HEAPF32[r4+((r16*5&-1)<<2)>>2]-HEAPF32[r4+((r14*5&-1)<<2)>>2]+1);r8=r1|0;HEAPF32[r8+(r18<<2)>>2]=r9*.5;r5=.5/r9;r9=r16<<2;r6=r14<<2;HEAPF32[r1+12>>2]=(HEAPF32[r4+(r9+r14<<2)>>2]-HEAPF32[r4+(r6+r16<<2)>>2])*r5;r1=r18<<2;HEAPF32[r8+(r16<<2)>>2]=(HEAPF32[r4+(r1+r16<<2)>>2]+HEAPF32[r4+(r9+r18<<2)>>2])*r5;HEAPF32[r8+(r14<<2)>>2]=(HEAPF32[r4+(r1+r14<<2)>>2]+HEAPF32[r4+(r6+r18<<2)>>2])*r5;STACKTOP=r3;return}}function __ZN7toadlet3egg4math4Math7projectERNS1_7Vector3ERKNS1_7SegmentERKS3_b(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10,r11,r12;r4=HEAPF32[r2+12>>2];r5=HEAPF32[r2+16>>2];do{if(r4==0&r5==0){if(HEAPF32[r2+20>>2]!=0){break}r6=r1;r7=r2;HEAP32[r6>>2]=HEAP32[r7>>2];HEAP32[r6+4>>2]=HEAP32[r7+4>>2];HEAP32[r6+8>>2]=HEAP32[r7+8>>2];return}}while(0);r7=HEAPF32[r2>>2];r6=r2+16|0;r8=r2+4|0;r9=r2+20|0;r10=HEAPF32[r9>>2];r11=r2+8|0;r2=(r4*(HEAPF32[r3>>2]-r7)+r5*(HEAPF32[r3+4>>2]-HEAPF32[r8>>2])+r10*(HEAPF32[r3+8>>2]-HEAPF32[r11>>2]))/(r4*r4+r5*r5+r10*r10);do{if(r2<0){r12=0}else{if(r2<=1){r12=r2;break}r12=1}}while(0);HEAPF32[r1>>2]=r7+r12*r4;HEAPF32[r1+4>>2]=HEAPF32[r8>>2]+r12*HEAPF32[r6>>2];HEAPF32[r1+8>>2]=HEAPF32[r11>>2]+r12*HEAPF32[r9>>2];return}function __ZN7toadlet3egg4math15MathInitializer9referenceEv(r1){return}function __GLOBAL__I_a14826(){__ZN7toadlet3egg6StringC1EPKci(53912,11096);_atexit(982,53912,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(73048,53912,10832);_atexit(982,73048,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(61272,73048,10728);_atexit(982,61272,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(67160,53912,10688);_atexit(982,67160,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(71576,53912,10680);_atexit(982,71576,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(68632,53912,10672);_atexit(982,68632,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(65688,53912,10664);_atexit(982,65688,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(64216,53912,10640);_atexit(982,64216,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(62744,53912,10624);_atexit(982,62744,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(59800,62744,10616);_atexit(982,59800,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(58328,62744,10600);_atexit(982,58328,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(56856,62744,10592);_atexit(982,56856,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(55384,62744,10568);_atexit(982,55384,___dso_handle);__ZNK7toadlet3egg6StringplEPKc(70104,53912,10560);_atexit(982,70104,___dso_handle);HEAP32[52448>>2]=34408;return}function ___getTypeName(r1){return _strdup(HEAP32[r1+4>>2])}function __GLOBAL__I_a14965(){__embind_register_void(46760,11088);__embind_register_bool(46768,18248,1,0);__embind_register_integer(__ZTIc,11040,-128,127);__embind_register_integer(__ZTIa,10816,-128,127);__embind_register_integer(__ZTIh,10648,0,255);__embind_register_integer(__ZTIs,10584,-32768,32767);__embind_register_integer(__ZTIt,10416,0,65535);__embind_register_integer(__ZTIi,10328,-2147483648,2147483647);__embind_register_integer(__ZTIj,10216,0,-1);__embind_register_integer(__ZTIl,3384,-2147483648,2147483647);__embind_register_integer(__ZTIm,19024,0,-1);__embind_register_float(__ZTIf,18488);__embind_register_float(__ZTId,16848);__embind_register_std_string(47496,11824);__embind_register_std_wstring(47472,4,11192);__embind_register_emval(51320,11072);__embind_register_memory_view(51328,11048);return}function __ZN10__cxxabiv116__shim_type_infoD2Ev(r1){return}function __ZNK10__cxxabiv116__shim_type_info5noop1Ev(r1){return}function __ZNK10__cxxabiv116__shim_type_info5noop2Ev(r1){return}function __ZN10__cxxabiv123__fundamental_type_infoD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZN10__cxxabiv117__class_type_infoD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZN10__cxxabiv120__si_class_type_infoD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZN10__cxxabiv121__vmi_class_type_infoD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZN10__cxxabiv119__pointer_type_infoD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZNK10__cxxabiv123__fundamental_type_info9can_catchEPKNS_16__shim_type_infoERPv(r1,r2,r3){return(r1|0)==(r2|0)}function __ZNK10__cxxabiv117__class_type_info9can_catchEPKNS_16__shim_type_infoERPv(r1,r2,r3){var r4,r5,r6,r7;r4=STACKTOP;STACKTOP=STACKTOP+56|0;r5=r4;if((r1|0)==(r2|0)){r6=1;STACKTOP=r4;return r6}if((r2|0)==0){r6=0;STACKTOP=r4;return r6}r7=___dynamic_cast(r2,51416);r2=r7;if((r7|0)==0){r6=0;STACKTOP=r4;return r6}_memset(r5,0,56);HEAP32[r5>>2]=r2;HEAP32[r5+8>>2]=r1;HEAP32[r5+12>>2]=-1;HEAP32[r5+48>>2]=1;FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+28>>2]](r2,r5,HEAP32[r3>>2],1);if((HEAP32[r5+24>>2]|0)!=1){r6=0;STACKTOP=r4;return r6}HEAP32[r3>>2]=HEAP32[r5+16>>2];r6=1;STACKTOP=r4;return r6}function __ZNK10__cxxabiv117__class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi(r1,r2,r3,r4){var r5;if((HEAP32[r2+8>>2]|0)!=(r1|0)){return}r1=r2+16|0;r5=HEAP32[r1>>2];if((r5|0)==0){HEAP32[r1>>2]=r3;HEAP32[r2+24>>2]=r4;HEAP32[r2+36>>2]=1;return}if((r5|0)!=(r3|0)){r3=r2+36|0;HEAP32[r3>>2]=HEAP32[r3>>2]+1;HEAP32[r2+24>>2]=2;HEAP8[r2+54|0]=1;return}r3=r2+24|0;if((HEAP32[r3>>2]|0)!=2){return}HEAP32[r3>>2]=r4;return}function __ZNK10__cxxabiv120__si_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi(r1,r2,r3,r4){var r5;if((r1|0)!=(HEAP32[r2+8>>2]|0)){r5=HEAP32[r1+8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r5>>2]+28>>2]](r5,r2,r3,r4);return}r5=r2+16|0;r1=HEAP32[r5>>2];if((r1|0)==0){HEAP32[r5>>2]=r3;HEAP32[r2+24>>2]=r4;HEAP32[r2+36>>2]=1;return}if((r1|0)!=(r3|0)){r3=r2+36|0;HEAP32[r3>>2]=HEAP32[r3>>2]+1;HEAP32[r2+24>>2]=2;HEAP8[r2+54|0]=1;return}r3=r2+24|0;if((HEAP32[r3>>2]|0)!=2){return}HEAP32[r3>>2]=r4;return}function __ZNK10__cxxabiv121__vmi_class_type_info27has_unambiguous_public_baseEPNS_19__dynamic_cast_infoEPvi(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10,r11;r5=0;if((r1|0)==(HEAP32[r2+8>>2]|0)){r6=r2+16|0;r7=HEAP32[r6>>2];if((r7|0)==0){HEAP32[r6>>2]=r3;HEAP32[r2+24>>2]=r4;HEAP32[r2+36>>2]=1;return}if((r7|0)!=(r3|0)){r7=r2+36|0;HEAP32[r7>>2]=HEAP32[r7>>2]+1;HEAP32[r2+24>>2]=2;HEAP8[r2+54|0]=1;return}r7=r2+24|0;if((HEAP32[r7>>2]|0)!=2){return}HEAP32[r7>>2]=r4;return}r7=HEAP32[r1+12>>2];r6=r1+16+(r7<<3)|0;r8=HEAP32[r1+20>>2];r9=r8>>8;if((r8&1|0)==0){r10=r9}else{r10=HEAP32[HEAP32[r3>>2]+r9>>2]}r9=HEAP32[r1+16>>2];FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+28>>2]](r9,r2,r3+r10|0,(r8&2|0)!=0?r4:2);if((r7|0)<=1){return}r7=r2+54|0;r8=r3;r10=r1+24|0;while(1){r1=HEAP32[r10+4>>2];r9=r1>>8;if((r1&1|0)==0){r11=r9}else{r11=HEAP32[HEAP32[r8>>2]+r9>>2]}r9=HEAP32[r10>>2];FUNCTION_TABLE[HEAP32[HEAP32[r9>>2]+28>>2]](r9,r2,r3+r11|0,(r1&2|0)!=0?r4:2);if((HEAP8[r7]&1)!=0){r5=2762;break}r1=r10+8|0;if(r1>>>0<r6>>>0){r10=r1}else{r5=2765;break}}if(r5==2762){return}else if(r5==2765){return}}function __ZNK10__cxxabiv119__pointer_type_info9can_catchEPKNS_16__shim_type_infoERPv(r1,r2,r3){var r4,r5,r6,r7,r8,r9,r10;r4=STACKTOP;STACKTOP=STACKTOP+56|0;r5=r4;HEAP32[r3>>2]=HEAP32[HEAP32[r3>>2]>>2];r6=r2|0;do{if((r1|0)==(r6|0)|(r6|0)==51448){r7=1}else{if((r2|0)==0){r7=0;break}r8=___dynamic_cast(r2,51384);if((r8|0)==0){r7=0;break}if((HEAP32[r8+8>>2]&~HEAP32[r1+8>>2]|0)!=0){r7=0;break}r9=HEAP32[r1+12>>2];r10=r8+12|0;if((r9|0)==(HEAP32[r10>>2]|0)|(r9|0)==46760){r7=1;break}if((r9|0)==0){r7=0;break}r8=___dynamic_cast(r9,51416);if((r8|0)==0){r7=0;break}r9=HEAP32[r10>>2];if((r9|0)==0){r7=0;break}r10=___dynamic_cast(r9,51416);r9=r10;if((r10|0)==0){r7=0;break}_memset(r5,0,56);HEAP32[r5>>2]=r9;HEAP32[r5+8>>2]=r8;HEAP32[r5+12>>2]=-1;HEAP32[r5+48>>2]=1;FUNCTION_TABLE[HEAP32[HEAP32[r10>>2]+28>>2]](r9,r5,HEAP32[r3>>2],1);if((HEAP32[r5+24>>2]|0)!=1){r7=0;break}HEAP32[r3>>2]=HEAP32[r5+16>>2];r7=1}}while(0);STACKTOP=r4;return r7}function ___dynamic_cast(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13;r3=STACKTOP;STACKTOP=STACKTOP+56|0;r4=r3;r5=HEAP32[r1>>2];r6=r1+HEAP32[r5-8>>2]|0;r7=HEAP32[r5-4>>2];r5=r7;HEAP32[r4>>2]=r2;HEAP32[r4+4>>2]=r1;HEAP32[r4+8>>2]=51432;HEAP32[r4+12>>2]=-1;r1=r4+16|0;r8=r4+20|0;r9=r4+24|0;r10=r4+28|0;r11=r4+32|0;r12=r4+40|0;_memset(r1,0,39);if((r7|0)==(r2|0)){HEAP32[r4+48>>2]=1;FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+20>>2]](r5,r4,r6,r6,1,0);STACKTOP=r3;return(HEAP32[r9>>2]|0)==1?r6:0}FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+24>>2]](r5,r4,r6,1,0);r6=HEAP32[r4+36>>2];do{if((r6|0)==0){if((HEAP32[r12>>2]|0)!=1){r13=0;break}if((HEAP32[r10>>2]|0)!=1){r13=0;break}r13=(HEAP32[r11>>2]|0)==1?HEAP32[r8>>2]:0}else if((r6|0)==1){if((HEAP32[r9>>2]|0)!=1){if((HEAP32[r12>>2]|0)!=0){r13=0;break}if((HEAP32[r10>>2]|0)!=1){r13=0;break}if((HEAP32[r11>>2]|0)!=1){r13=0;break}}r13=HEAP32[r1>>2]}else{r13=0}}while(0);STACKTOP=r3;return r13}function __ZNK10__cxxabiv121__vmi_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib(r1,r2,r3,r4,r5){var r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32;r6=0;r7=r1|0;if((r7|0)==(HEAP32[r2+8>>2]|0)){if((HEAP32[r2+4>>2]|0)!=(r3|0)){return}r8=r2+28|0;if((HEAP32[r8>>2]|0)==1){return}HEAP32[r8>>2]=r4;return}if((r7|0)==(HEAP32[r2>>2]|0)){do{if((HEAP32[r2+16>>2]|0)!=(r3|0)){r7=r2+20|0;if((HEAP32[r7>>2]|0)==(r3|0)){break}HEAP32[r2+32>>2]=r4;r8=r2+44|0;if((HEAP32[r8>>2]|0)==4){return}r9=HEAP32[r1+12>>2];r10=r1+16+(r9<<3)|0;L2828:do{if((r9|0)>0){r11=r2+52|0;r12=r2+53|0;r13=r2+54|0;r14=r1+8|0;r15=r2+24|0;r16=r3;r17=0;r18=r1+16|0;r19=0;L2830:while(1){HEAP8[r11]=0;HEAP8[r12]=0;r20=HEAP32[r18+4>>2];r21=r20>>8;if((r20&1|0)==0){r22=r21}else{r22=HEAP32[HEAP32[r16>>2]+r21>>2]}r21=HEAP32[r18>>2];FUNCTION_TABLE[HEAP32[HEAP32[r21>>2]+20>>2]](r21,r2,r3,r3+r22|0,2-(r20>>>1&1)|0,r5);if((HEAP8[r13]&1)!=0){r23=r19;r24=r17;break}do{if((HEAP8[r12]&1)==0){r25=r19;r26=r17}else{if((HEAP8[r11]&1)==0){if((HEAP32[r14>>2]&1|0)==0){r23=1;r24=r17;break L2830}else{r25=1;r26=r17;break}}if((HEAP32[r15>>2]|0)==1){r6=2818;break L2828}if((HEAP32[r14>>2]&2|0)==0){r6=2818;break L2828}else{r25=1;r26=1}}}while(0);r20=r18+8|0;if(r20>>>0<r10>>>0){r17=r26;r18=r20;r19=r25}else{r23=r25;r24=r26;break}}if(r24){r27=r23;r6=2817}else{r28=r23;r6=2814}}else{r28=0;r6=2814}}while(0);do{if(r6==2814){HEAP32[r7>>2]=r3;r10=r2+40|0;HEAP32[r10>>2]=HEAP32[r10>>2]+1;if((HEAP32[r2+36>>2]|0)!=1){r27=r28;r6=2817;break}if((HEAP32[r2+24>>2]|0)!=2){r27=r28;r6=2817;break}HEAP8[r2+54|0]=1;if(r28){r6=2818}else{r6=2819}}}while(0);if(r6==2817){if(r27){r6=2818}else{r6=2819}}if(r6==2818){HEAP32[r8>>2]=3;return}else if(r6==2819){HEAP32[r8>>2]=4;return}}}while(0);if((r4|0)!=1){return}HEAP32[r2+32>>2]=1;return}r27=HEAP32[r1+12>>2];r28=r1+16+(r27<<3)|0;r23=HEAP32[r1+20>>2];r24=r23>>8;if((r23&1|0)==0){r29=r24}else{r29=HEAP32[HEAP32[r3>>2]+r24>>2]}r24=HEAP32[r1+16>>2];FUNCTION_TABLE[HEAP32[HEAP32[r24>>2]+24>>2]](r24,r2,r3+r29|0,(r23&2|0)!=0?r4:2,r5);r23=r1+24|0;if((r27|0)<=1){return}r27=HEAP32[r1+8>>2];do{if((r27&2|0)==0){r1=r2+36|0;if((HEAP32[r1>>2]|0)==1){break}if((r27&1|0)==0){r29=r2+54|0;r24=r3;r26=r23;while(1){if((HEAP8[r29]&1)!=0){r6=2847;break}if((HEAP32[r1>>2]|0)==1){r6=2861;break}r25=HEAP32[r26+4>>2];r22=r25>>8;if((r25&1|0)==0){r30=r22}else{r30=HEAP32[HEAP32[r24>>2]+r22>>2]}r22=HEAP32[r26>>2];FUNCTION_TABLE[HEAP32[HEAP32[r22>>2]+24>>2]](r22,r2,r3+r30|0,(r25&2|0)!=0?r4:2,r5);r25=r26+8|0;if(r25>>>0<r28>>>0){r26=r25}else{r6=2850;break}}if(r6==2847){return}else if(r6==2850){return}else if(r6==2861){return}}r26=r2+24|0;r24=r2+54|0;r29=r3;r8=r23;while(1){if((HEAP8[r24]&1)!=0){r6=2858;break}if((HEAP32[r1>>2]|0)==1){if((HEAP32[r26>>2]|0)==1){r6=2859;break}}r25=HEAP32[r8+4>>2];r22=r25>>8;if((r25&1|0)==0){r31=r22}else{r31=HEAP32[HEAP32[r29>>2]+r22>>2]}r22=HEAP32[r8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r22>>2]+24>>2]](r22,r2,r3+r31|0,(r25&2|0)!=0?r4:2,r5);r25=r8+8|0;if(r25>>>0<r28>>>0){r8=r25}else{r6=2846;break}}if(r6==2846){return}else if(r6==2858){return}else if(r6==2859){return}}}while(0);r31=r2+54|0;r30=r3;r27=r23;while(1){if((HEAP8[r31]&1)!=0){r6=2856;break}r23=HEAP32[r27+4>>2];r8=r23>>8;if((r23&1|0)==0){r32=r8}else{r32=HEAP32[HEAP32[r30>>2]+r8>>2]}r8=HEAP32[r27>>2];FUNCTION_TABLE[HEAP32[HEAP32[r8>>2]+24>>2]](r8,r2,r3+r32|0,(r23&2|0)!=0?r4:2,r5);r23=r27+8|0;if(r23>>>0<r28>>>0){r27=r23}else{r6=2857;break}}if(r6==2856){return}else if(r6==2857){return}}function __ZNK10__cxxabiv120__si_class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib(r1,r2,r3,r4,r5){var r6,r7,r8,r9,r10,r11,r12;r6=0;r7=r1|0;if((r7|0)==(HEAP32[r2+8>>2]|0)){if((HEAP32[r2+4>>2]|0)!=(r3|0)){return}r8=r2+28|0;if((HEAP32[r8>>2]|0)==1){return}HEAP32[r8>>2]=r4;return}if((r7|0)!=(HEAP32[r2>>2]|0)){r7=HEAP32[r1+8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+24>>2]](r7,r2,r3,r4,r5);return}do{if((HEAP32[r2+16>>2]|0)!=(r3|0)){r7=r2+20|0;if((HEAP32[r7>>2]|0)==(r3|0)){break}HEAP32[r2+32>>2]=r4;r8=r2+44|0;if((HEAP32[r8>>2]|0)==4){return}r9=r2+52|0;HEAP8[r9]=0;r10=r2+53|0;HEAP8[r10]=0;r11=HEAP32[r1+8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r11>>2]+20>>2]](r11,r2,r3,r3,1,r5);if((HEAP8[r10]&1)==0){r12=0;r6=2874}else{if((HEAP8[r9]&1)==0){r12=1;r6=2874}}L2930:do{if(r6==2874){HEAP32[r7>>2]=r3;r9=r2+40|0;HEAP32[r9>>2]=HEAP32[r9>>2]+1;do{if((HEAP32[r2+36>>2]|0)==1){if((HEAP32[r2+24>>2]|0)!=2){r6=2877;break}HEAP8[r2+54|0]=1;if(r12){break L2930}}else{r6=2877}}while(0);if(r6==2877){if(r12){break}}HEAP32[r8>>2]=4;return}}while(0);HEAP32[r8>>2]=3;return}}while(0);if((r4|0)!=1){return}HEAP32[r2+32>>2]=1;return}function __ZNK10__cxxabiv117__class_type_info16search_below_dstEPNS_19__dynamic_cast_infoEPKvib(r1,r2,r3,r4,r5){if((HEAP32[r2+8>>2]|0)==(r1|0)){if((HEAP32[r2+4>>2]|0)!=(r3|0)){return}r5=r2+28|0;if((HEAP32[r5>>2]|0)==1){return}HEAP32[r5>>2]=r4;return}if((HEAP32[r2>>2]|0)!=(r1|0)){return}do{if((HEAP32[r2+16>>2]|0)!=(r3|0)){r1=r2+20|0;if((HEAP32[r1>>2]|0)==(r3|0)){break}HEAP32[r2+32>>2]=r4;HEAP32[r1>>2]=r3;r1=r2+40|0;HEAP32[r1>>2]=HEAP32[r1>>2]+1;do{if((HEAP32[r2+36>>2]|0)==1){if((HEAP32[r2+24>>2]|0)!=2){break}HEAP8[r2+54|0]=1}}while(0);HEAP32[r2+44>>2]=4;return}}while(0);if((r4|0)!=1){return}HEAP32[r2+32>>2]=1;return}function __ZNK10__cxxabiv121__vmi_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib(r1,r2,r3,r4,r5,r6){var r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21;if((r1|0)!=(HEAP32[r2+8>>2]|0)){r7=r2+52|0;r8=HEAP8[r7]&1;r9=r2+53|0;r10=HEAP8[r9]&1;r11=HEAP32[r1+12>>2];r12=r1+16+(r11<<3)|0;HEAP8[r7]=0;HEAP8[r9]=0;r13=HEAP32[r1+20>>2];r14=r13>>8;if((r13&1|0)==0){r15=r14}else{r15=HEAP32[HEAP32[r4>>2]+r14>>2]}r14=HEAP32[r1+16>>2];FUNCTION_TABLE[HEAP32[HEAP32[r14>>2]+20>>2]](r14,r2,r3,r4+r15|0,(r13&2|0)!=0?r5:2,r6);L2979:do{if((r11|0)>1){r13=r2+24|0;r15=r1+8|0;r14=r2+54|0;r16=r4;r17=r1+24|0;while(1){if((HEAP8[r14]&1)!=0){break L2979}do{if((HEAP8[r7]&1)==0){if((HEAP8[r9]&1)==0){break}if((HEAP32[r15>>2]&1|0)==0){break L2979}}else{if((HEAP32[r13>>2]|0)==1){break L2979}if((HEAP32[r15>>2]&2|0)==0){break L2979}}}while(0);HEAP8[r7]=0;HEAP8[r9]=0;r18=HEAP32[r17+4>>2];r19=r18>>8;if((r18&1|0)==0){r20=r19}else{r20=HEAP32[HEAP32[r16>>2]+r19>>2]}r19=HEAP32[r17>>2];FUNCTION_TABLE[HEAP32[HEAP32[r19>>2]+20>>2]](r19,r2,r3,r4+r20|0,(r18&2|0)!=0?r5:2,r6);r18=r17+8|0;if(r18>>>0<r12>>>0){r17=r18}else{break}}}}while(0);HEAP8[r7]=r8;HEAP8[r9]=r10;return}HEAP8[r2+53|0]=1;if((HEAP32[r2+4>>2]|0)!=(r4|0)){return}HEAP8[r2+52|0]=1;r4=r2+16|0;r10=HEAP32[r4>>2];if((r10|0)==0){HEAP32[r4>>2]=r3;HEAP32[r2+24>>2]=r5;HEAP32[r2+36>>2]=1;if(!((HEAP32[r2+48>>2]|0)==1&(r5|0)==1)){return}HEAP8[r2+54|0]=1;return}if((r10|0)!=(r3|0)){r3=r2+36|0;HEAP32[r3>>2]=HEAP32[r3>>2]+1;HEAP8[r2+54|0]=1;return}r3=r2+24|0;r10=HEAP32[r3>>2];if((r10|0)==2){HEAP32[r3>>2]=r5;r21=r5}else{r21=r10}if(!((HEAP32[r2+48>>2]|0)==1&(r21|0)==1)){return}HEAP8[r2+54|0]=1;return}function __ZNK10__cxxabiv120__si_class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib(r1,r2,r3,r4,r5,r6){var r7,r8;if((r1|0)!=(HEAP32[r2+8>>2]|0)){r7=HEAP32[r1+8>>2];FUNCTION_TABLE[HEAP32[HEAP32[r7>>2]+20>>2]](r7,r2,r3,r4,r5,r6);return}HEAP8[r2+53|0]=1;if((HEAP32[r2+4>>2]|0)!=(r4|0)){return}HEAP8[r2+52|0]=1;r4=r2+16|0;r6=HEAP32[r4>>2];if((r6|0)==0){HEAP32[r4>>2]=r3;HEAP32[r2+24>>2]=r5;HEAP32[r2+36>>2]=1;if(!((HEAP32[r2+48>>2]|0)==1&(r5|0)==1)){return}HEAP8[r2+54|0]=1;return}if((r6|0)!=(r3|0)){r3=r2+36|0;HEAP32[r3>>2]=HEAP32[r3>>2]+1;HEAP8[r2+54|0]=1;return}r3=r2+24|0;r6=HEAP32[r3>>2];if((r6|0)==2){HEAP32[r3>>2]=r5;r8=r5}else{r8=r6}if(!((HEAP32[r2+48>>2]|0)==1&(r8|0)==1)){return}HEAP8[r2+54|0]=1;return}function __ZNK10__cxxabiv117__class_type_info16search_above_dstEPNS_19__dynamic_cast_infoEPKvS4_ib(r1,r2,r3,r4,r5,r6){var r7;if((HEAP32[r2+8>>2]|0)!=(r1|0)){return}HEAP8[r2+53|0]=1;if((HEAP32[r2+4>>2]|0)!=(r4|0)){return}HEAP8[r2+52|0]=1;r4=r2+16|0;r1=HEAP32[r4>>2];if((r1|0)==0){HEAP32[r4>>2]=r3;HEAP32[r2+24>>2]=r5;HEAP32[r2+36>>2]=1;if(!((HEAP32[r2+48>>2]|0)==1&(r5|0)==1)){return}HEAP8[r2+54|0]=1;return}if((r1|0)!=(r3|0)){r3=r2+36|0;HEAP32[r3>>2]=HEAP32[r3>>2]+1;HEAP8[r2+54|0]=1;return}r3=r2+24|0;r1=HEAP32[r3>>2];if((r1|0)==2){HEAP32[r3>>2]=r5;r7=r5}else{r7=r1}if(!((HEAP32[r2+48>>2]|0)==1&(r7|0)==1)){return}HEAP8[r2+54|0]=1;return}function _malloc(r1){var r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41,r42,r43,r44,r45,r46,r47,r48,r49,r50,r51,r52,r53,r54,r55,r56,r57,r58,r59,r60,r61,r62,r63,r64,r65,r66,r67,r68,r69,r70,r71,r72,r73,r74,r75,r76,r77,r78,r79,r80,r81,r82,r83,r84,r85,r86;r2=0;do{if(r1>>>0<245){if(r1>>>0<11){r3=16}else{r3=r1+11&-8}r4=r3>>>3;r5=HEAP32[51776>>2];r6=r5>>>(r4>>>0);if((r6&3|0)!=0){r7=(r6&1^1)+r4|0;r8=r7<<1;r9=51816+(r8<<2)|0;r10=51816+(r8+2<<2)|0;r8=HEAP32[r10>>2];r11=r8+8|0;r12=HEAP32[r11>>2];do{if((r9|0)==(r12|0)){HEAP32[51776>>2]=r5&~(1<<r7)}else{if(r12>>>0<HEAP32[51792>>2]>>>0){_abort()}r13=r12+12|0;if((HEAP32[r13>>2]|0)==(r8|0)){HEAP32[r13>>2]=r9;HEAP32[r10>>2]=r12;break}else{_abort()}}}while(0);r12=r7<<3;HEAP32[r8+4>>2]=r12|3;r10=r8+(r12|4)|0;HEAP32[r10>>2]=HEAP32[r10>>2]|1;r14=r11;return r14}if(r3>>>0<=HEAP32[51784>>2]>>>0){r15=r3;break}if((r6|0)!=0){r10=2<<r4;r12=r6<<r4&(r10|-r10);r10=(r12&-r12)-1|0;r12=r10>>>12&16;r9=r10>>>(r12>>>0);r10=r9>>>5&8;r13=r9>>>(r10>>>0);r9=r13>>>2&4;r16=r13>>>(r9>>>0);r13=r16>>>1&2;r17=r16>>>(r13>>>0);r16=r17>>>1&1;r18=(r10|r12|r9|r13|r16)+(r17>>>(r16>>>0))|0;r16=r18<<1;r17=51816+(r16<<2)|0;r13=51816+(r16+2<<2)|0;r16=HEAP32[r13>>2];r9=r16+8|0;r12=HEAP32[r9>>2];do{if((r17|0)==(r12|0)){HEAP32[51776>>2]=r5&~(1<<r18)}else{if(r12>>>0<HEAP32[51792>>2]>>>0){_abort()}r10=r12+12|0;if((HEAP32[r10>>2]|0)==(r16|0)){HEAP32[r10>>2]=r17;HEAP32[r13>>2]=r12;break}else{_abort()}}}while(0);r12=r18<<3;r13=r12-r3|0;HEAP32[r16+4>>2]=r3|3;r17=r16;r5=r17+r3|0;HEAP32[r17+(r3|4)>>2]=r13|1;HEAP32[r17+r12>>2]=r13;r12=HEAP32[51784>>2];if((r12|0)!=0){r17=HEAP32[51796>>2];r4=r12>>>3;r12=r4<<1;r6=51816+(r12<<2)|0;r11=HEAP32[51776>>2];r8=1<<r4;do{if((r11&r8|0)==0){HEAP32[51776>>2]=r11|r8;r19=r6;r20=51816+(r12+2<<2)|0}else{r4=51816+(r12+2<<2)|0;r7=HEAP32[r4>>2];if(r7>>>0>=HEAP32[51792>>2]>>>0){r19=r7;r20=r4;break}_abort()}}while(0);HEAP32[r20>>2]=r17;HEAP32[r19+12>>2]=r17;HEAP32[r17+8>>2]=r19;HEAP32[r17+12>>2]=r6}HEAP32[51784>>2]=r13;HEAP32[51796>>2]=r5;r14=r9;return r14}r12=HEAP32[51780>>2];if((r12|0)==0){r15=r3;break}r8=(r12&-r12)-1|0;r12=r8>>>12&16;r11=r8>>>(r12>>>0);r8=r11>>>5&8;r16=r11>>>(r8>>>0);r11=r16>>>2&4;r18=r16>>>(r11>>>0);r16=r18>>>1&2;r4=r18>>>(r16>>>0);r18=r4>>>1&1;r7=HEAP32[52080+((r8|r12|r11|r16|r18)+(r4>>>(r18>>>0))<<2)>>2];r18=r7;r4=r7;r16=(HEAP32[r7+4>>2]&-8)-r3|0;while(1){r7=HEAP32[r18+16>>2];if((r7|0)==0){r11=HEAP32[r18+20>>2];if((r11|0)==0){break}else{r21=r11}}else{r21=r7}r7=(HEAP32[r21+4>>2]&-8)-r3|0;r11=r7>>>0<r16>>>0;r18=r21;r4=r11?r21:r4;r16=r11?r7:r16}r18=r4;r9=HEAP32[51792>>2];if(r18>>>0<r9>>>0){_abort()}r5=r18+r3|0;r13=r5;if(r18>>>0>=r5>>>0){_abort()}r5=HEAP32[r4+24>>2];r6=HEAP32[r4+12>>2];do{if((r6|0)==(r4|0)){r17=r4+20|0;r7=HEAP32[r17>>2];if((r7|0)==0){r11=r4+16|0;r12=HEAP32[r11>>2];if((r12|0)==0){r22=0;break}else{r23=r12;r24=r11}}else{r23=r7;r24=r17}while(1){r17=r23+20|0;r7=HEAP32[r17>>2];if((r7|0)!=0){r23=r7;r24=r17;continue}r17=r23+16|0;r7=HEAP32[r17>>2];if((r7|0)==0){break}else{r23=r7;r24=r17}}if(r24>>>0<r9>>>0){_abort()}else{HEAP32[r24>>2]=0;r22=r23;break}}else{r17=HEAP32[r4+8>>2];if(r17>>>0<r9>>>0){_abort()}r7=r17+12|0;if((HEAP32[r7>>2]|0)!=(r4|0)){_abort()}r11=r6+8|0;if((HEAP32[r11>>2]|0)==(r4|0)){HEAP32[r7>>2]=r6;HEAP32[r11>>2]=r17;r22=r6;break}else{_abort()}}}while(0);L3269:do{if((r5|0)!=0){r6=r4+28|0;r9=52080+(HEAP32[r6>>2]<<2)|0;do{if((r4|0)==(HEAP32[r9>>2]|0)){HEAP32[r9>>2]=r22;if((r22|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r6>>2]);break L3269}else{if(r5>>>0<HEAP32[51792>>2]>>>0){_abort()}r17=r5+16|0;if((HEAP32[r17>>2]|0)==(r4|0)){HEAP32[r17>>2]=r22}else{HEAP32[r5+20>>2]=r22}if((r22|0)==0){break L3269}}}while(0);if(r22>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r22+24>>2]=r5;r6=HEAP32[r4+16>>2];do{if((r6|0)!=0){if(r6>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r22+16>>2]=r6;HEAP32[r6+24>>2]=r22;break}}}while(0);r6=HEAP32[r4+20>>2];if((r6|0)==0){break}if(r6>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r22+20>>2]=r6;HEAP32[r6+24>>2]=r22;break}}}while(0);if(r16>>>0<16){r5=r16+r3|0;HEAP32[r4+4>>2]=r5|3;r6=r18+(r5+4)|0;HEAP32[r6>>2]=HEAP32[r6>>2]|1}else{HEAP32[r4+4>>2]=r3|3;HEAP32[r18+(r3|4)>>2]=r16|1;HEAP32[r18+(r16+r3)>>2]=r16;r6=HEAP32[51784>>2];if((r6|0)!=0){r5=HEAP32[51796>>2];r9=r6>>>3;r6=r9<<1;r17=51816+(r6<<2)|0;r11=HEAP32[51776>>2];r7=1<<r9;do{if((r11&r7|0)==0){HEAP32[51776>>2]=r11|r7;r25=r17;r26=51816+(r6+2<<2)|0}else{r9=51816+(r6+2<<2)|0;r12=HEAP32[r9>>2];if(r12>>>0>=HEAP32[51792>>2]>>>0){r25=r12;r26=r9;break}_abort()}}while(0);HEAP32[r26>>2]=r5;HEAP32[r25+12>>2]=r5;HEAP32[r5+8>>2]=r25;HEAP32[r5+12>>2]=r17}HEAP32[51784>>2]=r16;HEAP32[51796>>2]=r13}r6=r4+8|0;if((r6|0)==0){r15=r3;break}else{r14=r6}return r14}else{if(r1>>>0>4294967231){r15=-1;break}r6=r1+11|0;r7=r6&-8;r11=HEAP32[51780>>2];if((r11|0)==0){r15=r7;break}r18=-r7|0;r9=r6>>>8;do{if((r9|0)==0){r27=0}else{if(r7>>>0>16777215){r27=31;break}r6=(r9+1048320|0)>>>16&8;r12=r9<<r6;r8=(r12+520192|0)>>>16&4;r10=r12<<r8;r12=(r10+245760|0)>>>16&2;r28=14-(r8|r6|r12)+(r10<<r12>>>15)|0;r27=r7>>>((r28+7|0)>>>0)&1|r28<<1}}while(0);r9=HEAP32[52080+(r27<<2)>>2];L3077:do{if((r9|0)==0){r29=0;r30=r18;r31=0}else{if((r27|0)==31){r32=0}else{r32=25-(r27>>>1)|0}r4=0;r13=r18;r16=r9;r17=r7<<r32;r5=0;while(1){r28=HEAP32[r16+4>>2]&-8;r12=r28-r7|0;if(r12>>>0<r13>>>0){if((r28|0)==(r7|0)){r29=r16;r30=r12;r31=r16;break L3077}else{r33=r16;r34=r12}}else{r33=r4;r34=r13}r12=HEAP32[r16+20>>2];r28=HEAP32[r16+16+(r17>>>31<<2)>>2];r10=(r12|0)==0|(r12|0)==(r28|0)?r5:r12;if((r28|0)==0){r29=r33;r30=r34;r31=r10;break}else{r4=r33;r13=r34;r16=r28;r17=r17<<1;r5=r10}}}}while(0);if((r31|0)==0&(r29|0)==0){r9=2<<r27;r18=r11&(r9|-r9);if((r18|0)==0){r15=r7;break}r9=(r18&-r18)-1|0;r18=r9>>>12&16;r5=r9>>>(r18>>>0);r9=r5>>>5&8;r17=r5>>>(r9>>>0);r5=r17>>>2&4;r16=r17>>>(r5>>>0);r17=r16>>>1&2;r13=r16>>>(r17>>>0);r16=r13>>>1&1;r35=HEAP32[52080+((r9|r18|r5|r17|r16)+(r13>>>(r16>>>0))<<2)>>2]}else{r35=r31}if((r35|0)==0){r36=r30;r37=r29}else{r16=r35;r13=r30;r17=r29;while(1){r5=(HEAP32[r16+4>>2]&-8)-r7|0;r18=r5>>>0<r13>>>0;r9=r18?r5:r13;r5=r18?r16:r17;r18=HEAP32[r16+16>>2];if((r18|0)!=0){r16=r18;r13=r9;r17=r5;continue}r18=HEAP32[r16+20>>2];if((r18|0)==0){r36=r9;r37=r5;break}else{r16=r18;r13=r9;r17=r5}}}if((r37|0)==0){r15=r7;break}if(r36>>>0>=(HEAP32[51784>>2]-r7|0)>>>0){r15=r7;break}r17=r37;r13=HEAP32[51792>>2];if(r17>>>0<r13>>>0){_abort()}r16=r17+r7|0;r11=r16;if(r17>>>0>=r16>>>0){_abort()}r5=HEAP32[r37+24>>2];r9=HEAP32[r37+12>>2];do{if((r9|0)==(r37|0)){r18=r37+20|0;r4=HEAP32[r18>>2];if((r4|0)==0){r10=r37+16|0;r28=HEAP32[r10>>2];if((r28|0)==0){r38=0;break}else{r39=r28;r40=r10}}else{r39=r4;r40=r18}while(1){r18=r39+20|0;r4=HEAP32[r18>>2];if((r4|0)!=0){r39=r4;r40=r18;continue}r18=r39+16|0;r4=HEAP32[r18>>2];if((r4|0)==0){break}else{r39=r4;r40=r18}}if(r40>>>0<r13>>>0){_abort()}else{HEAP32[r40>>2]=0;r38=r39;break}}else{r18=HEAP32[r37+8>>2];if(r18>>>0<r13>>>0){_abort()}r4=r18+12|0;if((HEAP32[r4>>2]|0)!=(r37|0)){_abort()}r10=r9+8|0;if((HEAP32[r10>>2]|0)==(r37|0)){HEAP32[r4>>2]=r9;HEAP32[r10>>2]=r18;r38=r9;break}else{_abort()}}}while(0);L3127:do{if((r5|0)!=0){r9=r37+28|0;r13=52080+(HEAP32[r9>>2]<<2)|0;do{if((r37|0)==(HEAP32[r13>>2]|0)){HEAP32[r13>>2]=r38;if((r38|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r9>>2]);break L3127}else{if(r5>>>0<HEAP32[51792>>2]>>>0){_abort()}r18=r5+16|0;if((HEAP32[r18>>2]|0)==(r37|0)){HEAP32[r18>>2]=r38}else{HEAP32[r5+20>>2]=r38}if((r38|0)==0){break L3127}}}while(0);if(r38>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r38+24>>2]=r5;r9=HEAP32[r37+16>>2];do{if((r9|0)!=0){if(r9>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r38+16>>2]=r9;HEAP32[r9+24>>2]=r38;break}}}while(0);r9=HEAP32[r37+20>>2];if((r9|0)==0){break}if(r9>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r38+20>>2]=r9;HEAP32[r9+24>>2]=r38;break}}}while(0);do{if(r36>>>0<16){r5=r36+r7|0;HEAP32[r37+4>>2]=r5|3;r9=r17+(r5+4)|0;HEAP32[r9>>2]=HEAP32[r9>>2]|1}else{HEAP32[r37+4>>2]=r7|3;HEAP32[r17+(r7|4)>>2]=r36|1;HEAP32[r17+(r36+r7)>>2]=r36;r9=r36>>>3;if(r36>>>0<256){r5=r9<<1;r13=51816+(r5<<2)|0;r18=HEAP32[51776>>2];r10=1<<r9;do{if((r18&r10|0)==0){HEAP32[51776>>2]=r18|r10;r41=r13;r42=51816+(r5+2<<2)|0}else{r9=51816+(r5+2<<2)|0;r4=HEAP32[r9>>2];if(r4>>>0>=HEAP32[51792>>2]>>>0){r41=r4;r42=r9;break}_abort()}}while(0);HEAP32[r42>>2]=r11;HEAP32[r41+12>>2]=r11;HEAP32[r17+(r7+8)>>2]=r41;HEAP32[r17+(r7+12)>>2]=r13;break}r5=r16;r10=r36>>>8;do{if((r10|0)==0){r43=0}else{if(r36>>>0>16777215){r43=31;break}r18=(r10+1048320|0)>>>16&8;r9=r10<<r18;r4=(r9+520192|0)>>>16&4;r28=r9<<r4;r9=(r28+245760|0)>>>16&2;r12=14-(r4|r18|r9)+(r28<<r9>>>15)|0;r43=r36>>>((r12+7|0)>>>0)&1|r12<<1}}while(0);r10=52080+(r43<<2)|0;HEAP32[r17+(r7+28)>>2]=r43;HEAP32[r17+(r7+20)>>2]=0;HEAP32[r17+(r7+16)>>2]=0;r13=HEAP32[51780>>2];r12=1<<r43;if((r13&r12|0)==0){HEAP32[51780>>2]=r13|r12;HEAP32[r10>>2]=r5;HEAP32[r17+(r7+24)>>2]=r10;HEAP32[r17+(r7+12)>>2]=r5;HEAP32[r17+(r7+8)>>2]=r5;break}if((r43|0)==31){r44=0}else{r44=25-(r43>>>1)|0}r12=r36<<r44;r13=HEAP32[r10>>2];while(1){if((HEAP32[r13+4>>2]&-8|0)==(r36|0)){break}r45=r13+16+(r12>>>31<<2)|0;r10=HEAP32[r45>>2];if((r10|0)==0){r2=3134;break}else{r12=r12<<1;r13=r10}}if(r2==3134){if(r45>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r45>>2]=r5;HEAP32[r17+(r7+24)>>2]=r13;HEAP32[r17+(r7+12)>>2]=r5;HEAP32[r17+(r7+8)>>2]=r5;break}}r12=r13+8|0;r10=HEAP32[r12>>2];r9=HEAP32[51792>>2];if(r13>>>0<r9>>>0){_abort()}if(r10>>>0<r9>>>0){_abort()}else{HEAP32[r10+12>>2]=r5;HEAP32[r12>>2]=r5;HEAP32[r17+(r7+8)>>2]=r10;HEAP32[r17+(r7+12)>>2]=r13;HEAP32[r17+(r7+24)>>2]=0;break}}}while(0);r17=r37+8|0;if((r17|0)==0){r15=r7;break}else{r14=r17}return r14}}while(0);r37=HEAP32[51784>>2];if(r15>>>0<=r37>>>0){r45=r37-r15|0;r36=HEAP32[51796>>2];if(r45>>>0>15){r44=r36;HEAP32[51796>>2]=r44+r15;HEAP32[51784>>2]=r45;HEAP32[r44+(r15+4)>>2]=r45|1;HEAP32[r44+r37>>2]=r45;HEAP32[r36+4>>2]=r15|3}else{HEAP32[51784>>2]=0;HEAP32[51796>>2]=0;HEAP32[r36+4>>2]=r37|3;r45=r36+(r37+4)|0;HEAP32[r45>>2]=HEAP32[r45>>2]|1}r14=r36+8|0;return r14}r36=HEAP32[51788>>2];if(r15>>>0<r36>>>0){r45=r36-r15|0;HEAP32[51788>>2]=r45;r36=HEAP32[51800>>2];r37=r36;HEAP32[51800>>2]=r37+r15;HEAP32[r37+(r15+4)>>2]=r45|1;HEAP32[r36+4>>2]=r15|3;r14=r36+8|0;return r14}do{if((HEAP32[51744>>2]|0)==0){r36=_sysconf(30);if((r36-1&r36|0)==0){HEAP32[51752>>2]=r36;HEAP32[51748>>2]=r36;HEAP32[51756>>2]=-1;HEAP32[51760>>2]=-1;HEAP32[51764>>2]=0;HEAP32[52220>>2]=0;HEAP32[51744>>2]=_time(0)&-16^1431655768;break}else{_abort()}}}while(0);r36=r15+48|0;r45=HEAP32[51752>>2];r37=r15+47|0;r44=r45+r37|0;r43=-r45|0;r45=r44&r43;if(r45>>>0<=r15>>>0){r14=0;return r14}r41=HEAP32[52216>>2];do{if((r41|0)!=0){r42=HEAP32[52208>>2];r38=r42+r45|0;if(r38>>>0<=r42>>>0|r38>>>0>r41>>>0){r14=0}else{break}return r14}}while(0);L3336:do{if((HEAP32[52220>>2]&4|0)==0){r41=HEAP32[51800>>2];L3338:do{if((r41|0)==0){r2=3164}else{r38=r41;r42=52224;while(1){r46=r42|0;r39=HEAP32[r46>>2];if(r39>>>0<=r38>>>0){r47=r42+4|0;if((r39+HEAP32[r47>>2]|0)>>>0>r38>>>0){break}}r39=HEAP32[r42+8>>2];if((r39|0)==0){r2=3164;break L3338}else{r42=r39}}if((r42|0)==0){r2=3164;break}r38=r44-HEAP32[51788>>2]&r43;if(r38>>>0>=2147483647){r48=0;break}r13=_sbrk(r38);r5=(r13|0)==(HEAP32[r46>>2]+HEAP32[r47>>2]|0);r49=r5?r13:-1;r50=r5?r38:0;r51=r13;r52=r38;r2=3173}}while(0);do{if(r2==3164){r41=_sbrk(0);if((r41|0)==-1){r48=0;break}r7=r41;r38=HEAP32[51748>>2];r13=r38-1|0;if((r13&r7|0)==0){r53=r45}else{r53=r45-r7+(r13+r7&-r38)|0}r38=HEAP32[52208>>2];r7=r38+r53|0;if(!(r53>>>0>r15>>>0&r53>>>0<2147483647)){r48=0;break}r13=HEAP32[52216>>2];if((r13|0)!=0){if(r7>>>0<=r38>>>0|r7>>>0>r13>>>0){r48=0;break}}r13=_sbrk(r53);r7=(r13|0)==(r41|0);r49=r7?r41:-1;r50=r7?r53:0;r51=r13;r52=r53;r2=3173}}while(0);L3358:do{if(r2==3173){r13=-r52|0;if((r49|0)!=-1){r54=r50;r55=r49;r2=3184;break L3336}do{if((r51|0)!=-1&r52>>>0<2147483647&r52>>>0<r36>>>0){r7=HEAP32[51752>>2];r41=r37-r52+r7&-r7;if(r41>>>0>=2147483647){r56=r52;break}if((_sbrk(r41)|0)==-1){_sbrk(r13);r48=r50;break L3358}else{r56=r41+r52|0;break}}else{r56=r52}}while(0);if((r51|0)==-1){r48=r50}else{r54=r56;r55=r51;r2=3184;break L3336}}}while(0);HEAP32[52220>>2]=HEAP32[52220>>2]|4;r57=r48;r2=3181}else{r57=0;r2=3181}}while(0);do{if(r2==3181){if(r45>>>0>=2147483647){break}r48=_sbrk(r45);r51=_sbrk(0);if(!((r51|0)!=-1&(r48|0)!=-1&r48>>>0<r51>>>0)){break}r56=r51-r48|0;r51=r56>>>0>(r15+40|0)>>>0;r50=r51?r48:-1;if((r50|0)!=-1){r54=r51?r56:r57;r55=r50;r2=3184}}}while(0);do{if(r2==3184){r57=HEAP32[52208>>2]+r54|0;HEAP32[52208>>2]=r57;if(r57>>>0>HEAP32[52212>>2]>>>0){HEAP32[52212>>2]=r57}r57=HEAP32[51800>>2];L3378:do{if((r57|0)==0){r45=HEAP32[51792>>2];if((r45|0)==0|r55>>>0<r45>>>0){HEAP32[51792>>2]=r55}HEAP32[52224>>2]=r55;HEAP32[52228>>2]=r54;HEAP32[52236>>2]=0;HEAP32[51812>>2]=HEAP32[51744>>2];HEAP32[51808>>2]=-1;r45=0;while(1){r50=r45<<1;r56=51816+(r50<<2)|0;HEAP32[51816+(r50+3<<2)>>2]=r56;HEAP32[51816+(r50+2<<2)>>2]=r56;r56=r45+1|0;if(r56>>>0<32){r45=r56}else{break}}r45=r55+8|0;if((r45&7|0)==0){r58=0}else{r58=-r45&7}r45=r54-40-r58|0;HEAP32[51800>>2]=r55+r58;HEAP32[51788>>2]=r45;HEAP32[r55+(r58+4)>>2]=r45|1;HEAP32[r55+(r54-36)>>2]=40;HEAP32[51804>>2]=HEAP32[51760>>2]}else{r45=52224;while(1){r59=HEAP32[r45>>2];r60=r45+4|0;r61=HEAP32[r60>>2];if((r55|0)==(r59+r61|0)){r2=3196;break}r56=HEAP32[r45+8>>2];if((r56|0)==0){break}else{r45=r56}}do{if(r2==3196){if((HEAP32[r45+12>>2]&8|0)!=0){break}r56=r57;if(!(r56>>>0>=r59>>>0&r56>>>0<r55>>>0)){break}HEAP32[r60>>2]=r61+r54;r56=HEAP32[51800>>2];r50=HEAP32[51788>>2]+r54|0;r51=r56;r48=r56+8|0;if((r48&7|0)==0){r62=0}else{r62=-r48&7}r48=r50-r62|0;HEAP32[51800>>2]=r51+r62;HEAP32[51788>>2]=r48;HEAP32[r51+(r62+4)>>2]=r48|1;HEAP32[r51+(r50+4)>>2]=40;HEAP32[51804>>2]=HEAP32[51760>>2];break L3378}}while(0);if(r55>>>0<HEAP32[51792>>2]>>>0){HEAP32[51792>>2]=r55}r45=r55+r54|0;r50=52224;while(1){r63=r50|0;if((HEAP32[r63>>2]|0)==(r45|0)){r2=3206;break}r51=HEAP32[r50+8>>2];if((r51|0)==0){break}else{r50=r51}}do{if(r2==3206){if((HEAP32[r50+12>>2]&8|0)!=0){break}HEAP32[r63>>2]=r55;r45=r50+4|0;HEAP32[r45>>2]=HEAP32[r45>>2]+r54;r45=r55+8|0;if((r45&7|0)==0){r64=0}else{r64=-r45&7}r45=r55+(r54+8)|0;if((r45&7|0)==0){r65=0}else{r65=-r45&7}r45=r55+(r65+r54)|0;r51=r45;r48=r64+r15|0;r56=r55+r48|0;r52=r56;r37=r45-(r55+r64)-r15|0;HEAP32[r55+(r64+4)>>2]=r15|3;do{if((r51|0)==(HEAP32[51800>>2]|0)){r36=HEAP32[51788>>2]+r37|0;HEAP32[51788>>2]=r36;HEAP32[51800>>2]=r52;HEAP32[r55+(r48+4)>>2]=r36|1}else{if((r51|0)==(HEAP32[51796>>2]|0)){r36=HEAP32[51784>>2]+r37|0;HEAP32[51784>>2]=r36;HEAP32[51796>>2]=r52;HEAP32[r55+(r48+4)>>2]=r36|1;HEAP32[r55+(r36+r48)>>2]=r36;break}r36=r54+4|0;r49=HEAP32[r55+(r36+r65)>>2];if((r49&3|0)==1){r53=r49&-8;r47=r49>>>3;L3423:do{if(r49>>>0<256){r46=HEAP32[r55+((r65|8)+r54)>>2];r43=HEAP32[r55+(r54+12+r65)>>2];r44=51816+(r47<<1<<2)|0;do{if((r46|0)!=(r44|0)){if(r46>>>0<HEAP32[51792>>2]>>>0){_abort()}if((HEAP32[r46+12>>2]|0)==(r51|0)){break}_abort()}}while(0);if((r43|0)==(r46|0)){HEAP32[51776>>2]=HEAP32[51776>>2]&~(1<<r47);break}do{if((r43|0)==(r44|0)){r66=r43+8|0}else{if(r43>>>0<HEAP32[51792>>2]>>>0){_abort()}r13=r43+8|0;if((HEAP32[r13>>2]|0)==(r51|0)){r66=r13;break}_abort()}}while(0);HEAP32[r46+12>>2]=r43;HEAP32[r66>>2]=r46}else{r44=r45;r13=HEAP32[r55+((r65|24)+r54)>>2];r42=HEAP32[r55+(r54+12+r65)>>2];do{if((r42|0)==(r44|0)){r41=r65|16;r7=r55+(r36+r41)|0;r38=HEAP32[r7>>2];if((r38|0)==0){r5=r55+(r41+r54)|0;r41=HEAP32[r5>>2];if((r41|0)==0){r67=0;break}else{r68=r41;r69=r5}}else{r68=r38;r69=r7}while(1){r7=r68+20|0;r38=HEAP32[r7>>2];if((r38|0)!=0){r68=r38;r69=r7;continue}r7=r68+16|0;r38=HEAP32[r7>>2];if((r38|0)==0){break}else{r68=r38;r69=r7}}if(r69>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r69>>2]=0;r67=r68;break}}else{r7=HEAP32[r55+((r65|8)+r54)>>2];if(r7>>>0<HEAP32[51792>>2]>>>0){_abort()}r38=r7+12|0;if((HEAP32[r38>>2]|0)!=(r44|0)){_abort()}r5=r42+8|0;if((HEAP32[r5>>2]|0)==(r44|0)){HEAP32[r38>>2]=r42;HEAP32[r5>>2]=r7;r67=r42;break}else{_abort()}}}while(0);if((r13|0)==0){break}r42=r55+(r54+28+r65)|0;r46=52080+(HEAP32[r42>>2]<<2)|0;do{if((r44|0)==(HEAP32[r46>>2]|0)){HEAP32[r46>>2]=r67;if((r67|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r42>>2]);break L3423}else{if(r13>>>0<HEAP32[51792>>2]>>>0){_abort()}r43=r13+16|0;if((HEAP32[r43>>2]|0)==(r44|0)){HEAP32[r43>>2]=r67}else{HEAP32[r13+20>>2]=r67}if((r67|0)==0){break L3423}}}while(0);if(r67>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r67+24>>2]=r13;r44=r65|16;r42=HEAP32[r55+(r44+r54)>>2];do{if((r42|0)!=0){if(r42>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r67+16>>2]=r42;HEAP32[r42+24>>2]=r67;break}}}while(0);r42=HEAP32[r55+(r36+r44)>>2];if((r42|0)==0){break}if(r42>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r67+20>>2]=r42;HEAP32[r42+24>>2]=r67;break}}}while(0);r70=r55+((r53|r65)+r54)|0;r71=r53+r37|0}else{r70=r51;r71=r37}r36=r70+4|0;HEAP32[r36>>2]=HEAP32[r36>>2]&-2;HEAP32[r55+(r48+4)>>2]=r71|1;HEAP32[r55+(r71+r48)>>2]=r71;r36=r71>>>3;if(r71>>>0<256){r47=r36<<1;r49=51816+(r47<<2)|0;r42=HEAP32[51776>>2];r13=1<<r36;do{if((r42&r13|0)==0){HEAP32[51776>>2]=r42|r13;r72=r49;r73=51816+(r47+2<<2)|0}else{r36=51816+(r47+2<<2)|0;r46=HEAP32[r36>>2];if(r46>>>0>=HEAP32[51792>>2]>>>0){r72=r46;r73=r36;break}_abort()}}while(0);HEAP32[r73>>2]=r52;HEAP32[r72+12>>2]=r52;HEAP32[r55+(r48+8)>>2]=r72;HEAP32[r55+(r48+12)>>2]=r49;break}r47=r56;r13=r71>>>8;do{if((r13|0)==0){r74=0}else{if(r71>>>0>16777215){r74=31;break}r42=(r13+1048320|0)>>>16&8;r53=r13<<r42;r36=(r53+520192|0)>>>16&4;r46=r53<<r36;r53=(r46+245760|0)>>>16&2;r43=14-(r36|r42|r53)+(r46<<r53>>>15)|0;r74=r71>>>((r43+7|0)>>>0)&1|r43<<1}}while(0);r13=52080+(r74<<2)|0;HEAP32[r55+(r48+28)>>2]=r74;HEAP32[r55+(r48+20)>>2]=0;HEAP32[r55+(r48+16)>>2]=0;r49=HEAP32[51780>>2];r43=1<<r74;if((r49&r43|0)==0){HEAP32[51780>>2]=r49|r43;HEAP32[r13>>2]=r47;HEAP32[r55+(r48+24)>>2]=r13;HEAP32[r55+(r48+12)>>2]=r47;HEAP32[r55+(r48+8)>>2]=r47;break}if((r74|0)==31){r75=0}else{r75=25-(r74>>>1)|0}r43=r71<<r75;r49=HEAP32[r13>>2];while(1){if((HEAP32[r49+4>>2]&-8|0)==(r71|0)){break}r76=r49+16+(r43>>>31<<2)|0;r13=HEAP32[r76>>2];if((r13|0)==0){r2=3279;break}else{r43=r43<<1;r49=r13}}if(r2==3279){if(r76>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r76>>2]=r47;HEAP32[r55+(r48+24)>>2]=r49;HEAP32[r55+(r48+12)>>2]=r47;HEAP32[r55+(r48+8)>>2]=r47;break}}r43=r49+8|0;r13=HEAP32[r43>>2];r53=HEAP32[51792>>2];if(r49>>>0<r53>>>0){_abort()}if(r13>>>0<r53>>>0){_abort()}else{HEAP32[r13+12>>2]=r47;HEAP32[r43>>2]=r47;HEAP32[r55+(r48+8)>>2]=r13;HEAP32[r55+(r48+12)>>2]=r49;HEAP32[r55+(r48+24)>>2]=0;break}}}while(0);r14=r55+(r64|8)|0;return r14}}while(0);r50=r57;r48=52224;while(1){r77=HEAP32[r48>>2];if(r77>>>0<=r50>>>0){r78=HEAP32[r48+4>>2];r79=r77+r78|0;if(r79>>>0>r50>>>0){break}}r48=HEAP32[r48+8>>2]}r48=r77+(r78-39)|0;if((r48&7|0)==0){r80=0}else{r80=-r48&7}r48=r77+(r78-47+r80)|0;r56=r48>>>0<(r57+16|0)>>>0?r50:r48;r48=r56+8|0;r52=r55+8|0;if((r52&7|0)==0){r81=0}else{r81=-r52&7}r52=r54-40-r81|0;HEAP32[51800>>2]=r55+r81;HEAP32[51788>>2]=r52;HEAP32[r55+(r81+4)>>2]=r52|1;HEAP32[r55+(r54-36)>>2]=40;HEAP32[51804>>2]=HEAP32[51760>>2];HEAP32[r56+4>>2]=27;HEAP32[r48>>2]=HEAP32[52224>>2];HEAP32[r48+4>>2]=HEAP32[52228>>2];HEAP32[r48+8>>2]=HEAP32[52232>>2];HEAP32[r48+12>>2]=HEAP32[52236>>2];HEAP32[52224>>2]=r55;HEAP32[52228>>2]=r54;HEAP32[52236>>2]=0;HEAP32[52232>>2]=r48;r48=r56+28|0;HEAP32[r48>>2]=7;if((r56+32|0)>>>0<r79>>>0){r52=r48;while(1){r48=r52+4|0;HEAP32[r48>>2]=7;if((r52+8|0)>>>0<r79>>>0){r52=r48}else{break}}}if((r56|0)==(r50|0)){break}r52=r56-r57|0;r48=r50+(r52+4)|0;HEAP32[r48>>2]=HEAP32[r48>>2]&-2;HEAP32[r57+4>>2]=r52|1;HEAP32[r50+r52>>2]=r52;r48=r52>>>3;if(r52>>>0<256){r37=r48<<1;r51=51816+(r37<<2)|0;r45=HEAP32[51776>>2];r13=1<<r48;do{if((r45&r13|0)==0){HEAP32[51776>>2]=r45|r13;r82=r51;r83=51816+(r37+2<<2)|0}else{r48=51816+(r37+2<<2)|0;r43=HEAP32[r48>>2];if(r43>>>0>=HEAP32[51792>>2]>>>0){r82=r43;r83=r48;break}_abort()}}while(0);HEAP32[r83>>2]=r57;HEAP32[r82+12>>2]=r57;HEAP32[r57+8>>2]=r82;HEAP32[r57+12>>2]=r51;break}r37=r57;r13=r52>>>8;do{if((r13|0)==0){r84=0}else{if(r52>>>0>16777215){r84=31;break}r45=(r13+1048320|0)>>>16&8;r50=r13<<r45;r56=(r50+520192|0)>>>16&4;r48=r50<<r56;r50=(r48+245760|0)>>>16&2;r43=14-(r56|r45|r50)+(r48<<r50>>>15)|0;r84=r52>>>((r43+7|0)>>>0)&1|r43<<1}}while(0);r13=52080+(r84<<2)|0;HEAP32[r57+28>>2]=r84;HEAP32[r57+20>>2]=0;HEAP32[r57+16>>2]=0;r51=HEAP32[51780>>2];r43=1<<r84;if((r51&r43|0)==0){HEAP32[51780>>2]=r51|r43;HEAP32[r13>>2]=r37;HEAP32[r57+24>>2]=r13;HEAP32[r57+12>>2]=r57;HEAP32[r57+8>>2]=r57;break}if((r84|0)==31){r85=0}else{r85=25-(r84>>>1)|0}r43=r52<<r85;r51=HEAP32[r13>>2];while(1){if((HEAP32[r51+4>>2]&-8|0)==(r52|0)){break}r86=r51+16+(r43>>>31<<2)|0;r13=HEAP32[r86>>2];if((r13|0)==0){r2=3314;break}else{r43=r43<<1;r51=r13}}if(r2==3314){if(r86>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r86>>2]=r37;HEAP32[r57+24>>2]=r51;HEAP32[r57+12>>2]=r57;HEAP32[r57+8>>2]=r57;break}}r43=r51+8|0;r52=HEAP32[r43>>2];r13=HEAP32[51792>>2];if(r51>>>0<r13>>>0){_abort()}if(r52>>>0<r13>>>0){_abort()}else{HEAP32[r52+12>>2]=r37;HEAP32[r43>>2]=r37;HEAP32[r57+8>>2]=r52;HEAP32[r57+12>>2]=r51;HEAP32[r57+24>>2]=0;break}}}while(0);r57=HEAP32[51788>>2];if(r57>>>0<=r15>>>0){break}r52=r57-r15|0;HEAP32[51788>>2]=r52;r57=HEAP32[51800>>2];r43=r57;HEAP32[51800>>2]=r43+r15;HEAP32[r43+(r15+4)>>2]=r52|1;HEAP32[r57+4>>2]=r15|3;r14=r57+8|0;return r14}}while(0);HEAP32[___errno_location()>>2]=12;r14=0;return r14}function _free(r1){var r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40;r2=0;if((r1|0)==0){return}r3=r1-8|0;r4=r3;r5=HEAP32[51792>>2];if(r3>>>0<r5>>>0){_abort()}r6=HEAP32[r1-4>>2];r7=r6&3;if((r7|0)==1){_abort()}r8=r6&-8;r9=r1+(r8-8)|0;r10=r9;L3595:do{if((r6&1|0)==0){r11=HEAP32[r3>>2];if((r7|0)==0){return}r12=-8-r11|0;r13=r1+r12|0;r14=r13;r15=r11+r8|0;if(r13>>>0<r5>>>0){_abort()}if((r14|0)==(HEAP32[51796>>2]|0)){r16=r1+(r8-4)|0;if((HEAP32[r16>>2]&3|0)!=3){r17=r14;r18=r15;break}HEAP32[51784>>2]=r15;HEAP32[r16>>2]=HEAP32[r16>>2]&-2;HEAP32[r1+(r12+4)>>2]=r15|1;HEAP32[r9>>2]=r15;return}r16=r11>>>3;if(r11>>>0<256){r11=HEAP32[r1+(r12+8)>>2];r19=HEAP32[r1+(r12+12)>>2];r20=51816+(r16<<1<<2)|0;do{if((r11|0)!=(r20|0)){if(r11>>>0<r5>>>0){_abort()}if((HEAP32[r11+12>>2]|0)==(r14|0)){break}_abort()}}while(0);if((r19|0)==(r11|0)){HEAP32[51776>>2]=HEAP32[51776>>2]&~(1<<r16);r17=r14;r18=r15;break}do{if((r19|0)==(r20|0)){r21=r19+8|0}else{if(r19>>>0<r5>>>0){_abort()}r22=r19+8|0;if((HEAP32[r22>>2]|0)==(r14|0)){r21=r22;break}_abort()}}while(0);HEAP32[r11+12>>2]=r19;HEAP32[r21>>2]=r11;r17=r14;r18=r15;break}r20=r13;r16=HEAP32[r1+(r12+24)>>2];r22=HEAP32[r1+(r12+12)>>2];do{if((r22|0)==(r20|0)){r23=r1+(r12+20)|0;r24=HEAP32[r23>>2];if((r24|0)==0){r25=r1+(r12+16)|0;r26=HEAP32[r25>>2];if((r26|0)==0){r27=0;break}else{r28=r26;r29=r25}}else{r28=r24;r29=r23}while(1){r23=r28+20|0;r24=HEAP32[r23>>2];if((r24|0)!=0){r28=r24;r29=r23;continue}r23=r28+16|0;r24=HEAP32[r23>>2];if((r24|0)==0){break}else{r28=r24;r29=r23}}if(r29>>>0<r5>>>0){_abort()}else{HEAP32[r29>>2]=0;r27=r28;break}}else{r23=HEAP32[r1+(r12+8)>>2];if(r23>>>0<r5>>>0){_abort()}r24=r23+12|0;if((HEAP32[r24>>2]|0)!=(r20|0)){_abort()}r25=r22+8|0;if((HEAP32[r25>>2]|0)==(r20|0)){HEAP32[r24>>2]=r22;HEAP32[r25>>2]=r23;r27=r22;break}else{_abort()}}}while(0);if((r16|0)==0){r17=r14;r18=r15;break}r22=r1+(r12+28)|0;r13=52080+(HEAP32[r22>>2]<<2)|0;do{if((r20|0)==(HEAP32[r13>>2]|0)){HEAP32[r13>>2]=r27;if((r27|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r22>>2]);r17=r14;r18=r15;break L3595}else{if(r16>>>0<HEAP32[51792>>2]>>>0){_abort()}r11=r16+16|0;if((HEAP32[r11>>2]|0)==(r20|0)){HEAP32[r11>>2]=r27}else{HEAP32[r16+20>>2]=r27}if((r27|0)==0){r17=r14;r18=r15;break L3595}}}while(0);if(r27>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r27+24>>2]=r16;r20=HEAP32[r1+(r12+16)>>2];do{if((r20|0)!=0){if(r20>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r27+16>>2]=r20;HEAP32[r20+24>>2]=r27;break}}}while(0);r20=HEAP32[r1+(r12+20)>>2];if((r20|0)==0){r17=r14;r18=r15;break}if(r20>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r27+20>>2]=r20;HEAP32[r20+24>>2]=r27;r17=r14;r18=r15;break}}else{r17=r4;r18=r8}}while(0);r4=r17;if(r4>>>0>=r9>>>0){_abort()}r27=r1+(r8-4)|0;r5=HEAP32[r27>>2];if((r5&1|0)==0){_abort()}do{if((r5&2|0)==0){if((r10|0)==(HEAP32[51800>>2]|0)){r28=HEAP32[51788>>2]+r18|0;HEAP32[51788>>2]=r28;HEAP32[51800>>2]=r17;HEAP32[r17+4>>2]=r28|1;if((r17|0)!=(HEAP32[51796>>2]|0)){return}HEAP32[51796>>2]=0;HEAP32[51784>>2]=0;return}if((r10|0)==(HEAP32[51796>>2]|0)){r28=HEAP32[51784>>2]+r18|0;HEAP32[51784>>2]=r28;HEAP32[51796>>2]=r17;HEAP32[r17+4>>2]=r28|1;HEAP32[r4+r28>>2]=r28;return}r28=(r5&-8)+r18|0;r29=r5>>>3;L3698:do{if(r5>>>0<256){r21=HEAP32[r1+r8>>2];r7=HEAP32[r1+(r8|4)>>2];r3=51816+(r29<<1<<2)|0;do{if((r21|0)!=(r3|0)){if(r21>>>0<HEAP32[51792>>2]>>>0){_abort()}if((HEAP32[r21+12>>2]|0)==(r10|0)){break}_abort()}}while(0);if((r7|0)==(r21|0)){HEAP32[51776>>2]=HEAP32[51776>>2]&~(1<<r29);break}do{if((r7|0)==(r3|0)){r30=r7+8|0}else{if(r7>>>0<HEAP32[51792>>2]>>>0){_abort()}r6=r7+8|0;if((HEAP32[r6>>2]|0)==(r10|0)){r30=r6;break}_abort()}}while(0);HEAP32[r21+12>>2]=r7;HEAP32[r30>>2]=r21}else{r3=r9;r6=HEAP32[r1+(r8+16)>>2];r20=HEAP32[r1+(r8|4)>>2];do{if((r20|0)==(r3|0)){r16=r1+(r8+12)|0;r22=HEAP32[r16>>2];if((r22|0)==0){r13=r1+(r8+8)|0;r11=HEAP32[r13>>2];if((r11|0)==0){r31=0;break}else{r32=r11;r33=r13}}else{r32=r22;r33=r16}while(1){r16=r32+20|0;r22=HEAP32[r16>>2];if((r22|0)!=0){r32=r22;r33=r16;continue}r16=r32+16|0;r22=HEAP32[r16>>2];if((r22|0)==0){break}else{r32=r22;r33=r16}}if(r33>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r33>>2]=0;r31=r32;break}}else{r16=HEAP32[r1+r8>>2];if(r16>>>0<HEAP32[51792>>2]>>>0){_abort()}r22=r16+12|0;if((HEAP32[r22>>2]|0)!=(r3|0)){_abort()}r13=r20+8|0;if((HEAP32[r13>>2]|0)==(r3|0)){HEAP32[r22>>2]=r20;HEAP32[r13>>2]=r16;r31=r20;break}else{_abort()}}}while(0);if((r6|0)==0){break}r20=r1+(r8+20)|0;r21=52080+(HEAP32[r20>>2]<<2)|0;do{if((r3|0)==(HEAP32[r21>>2]|0)){HEAP32[r21>>2]=r31;if((r31|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r20>>2]);break L3698}else{if(r6>>>0<HEAP32[51792>>2]>>>0){_abort()}r7=r6+16|0;if((HEAP32[r7>>2]|0)==(r3|0)){HEAP32[r7>>2]=r31}else{HEAP32[r6+20>>2]=r31}if((r31|0)==0){break L3698}}}while(0);if(r31>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r31+24>>2]=r6;r3=HEAP32[r1+(r8+8)>>2];do{if((r3|0)!=0){if(r3>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r31+16>>2]=r3;HEAP32[r3+24>>2]=r31;break}}}while(0);r3=HEAP32[r1+(r8+12)>>2];if((r3|0)==0){break}if(r3>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r31+20>>2]=r3;HEAP32[r3+24>>2]=r31;break}}}while(0);HEAP32[r17+4>>2]=r28|1;HEAP32[r4+r28>>2]=r28;if((r17|0)!=(HEAP32[51796>>2]|0)){r34=r28;break}HEAP32[51784>>2]=r28;return}else{HEAP32[r27>>2]=r5&-2;HEAP32[r17+4>>2]=r18|1;HEAP32[r4+r18>>2]=r18;r34=r18}}while(0);r18=r34>>>3;if(r34>>>0<256){r4=r18<<1;r5=51816+(r4<<2)|0;r27=HEAP32[51776>>2];r31=1<<r18;do{if((r27&r31|0)==0){HEAP32[51776>>2]=r27|r31;r35=r5;r36=51816+(r4+2<<2)|0}else{r18=51816+(r4+2<<2)|0;r8=HEAP32[r18>>2];if(r8>>>0>=HEAP32[51792>>2]>>>0){r35=r8;r36=r18;break}_abort()}}while(0);HEAP32[r36>>2]=r17;HEAP32[r35+12>>2]=r17;HEAP32[r17+8>>2]=r35;HEAP32[r17+12>>2]=r5;return}r5=r17;r35=r34>>>8;do{if((r35|0)==0){r37=0}else{if(r34>>>0>16777215){r37=31;break}r36=(r35+1048320|0)>>>16&8;r4=r35<<r36;r31=(r4+520192|0)>>>16&4;r27=r4<<r31;r4=(r27+245760|0)>>>16&2;r18=14-(r31|r36|r4)+(r27<<r4>>>15)|0;r37=r34>>>((r18+7|0)>>>0)&1|r18<<1}}while(0);r35=52080+(r37<<2)|0;HEAP32[r17+28>>2]=r37;HEAP32[r17+20>>2]=0;HEAP32[r17+16>>2]=0;r18=HEAP32[51780>>2];r4=1<<r37;do{if((r18&r4|0)==0){HEAP32[51780>>2]=r18|r4;HEAP32[r35>>2]=r5;HEAP32[r17+24>>2]=r35;HEAP32[r17+12>>2]=r17;HEAP32[r17+8>>2]=r17}else{if((r37|0)==31){r38=0}else{r38=25-(r37>>>1)|0}r27=r34<<r38;r36=HEAP32[r35>>2];while(1){if((HEAP32[r36+4>>2]&-8|0)==(r34|0)){break}r39=r36+16+(r27>>>31<<2)|0;r31=HEAP32[r39>>2];if((r31|0)==0){r2=3491;break}else{r27=r27<<1;r36=r31}}if(r2==3491){if(r39>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r39>>2]=r5;HEAP32[r17+24>>2]=r36;HEAP32[r17+12>>2]=r17;HEAP32[r17+8>>2]=r17;break}}r27=r36+8|0;r28=HEAP32[r27>>2];r31=HEAP32[51792>>2];if(r36>>>0<r31>>>0){_abort()}if(r28>>>0<r31>>>0){_abort()}else{HEAP32[r28+12>>2]=r5;HEAP32[r27>>2]=r5;HEAP32[r17+8>>2]=r28;HEAP32[r17+12>>2]=r36;HEAP32[r17+24>>2]=0;break}}}while(0);r17=HEAP32[51808>>2]-1|0;HEAP32[51808>>2]=r17;if((r17|0)==0){r40=52232}else{return}while(1){r17=HEAP32[r40>>2];if((r17|0)==0){break}else{r40=r17+8|0}}HEAP32[51808>>2]=-1;return}function _dispose_chunk(r1,r2){var r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37;r3=0;r4=r1;r5=r4+r2|0;r6=r5;r7=HEAP32[r1+4>>2];L3813:do{if((r7&1|0)==0){r8=HEAP32[r1>>2];if((r7&3|0)==0){return}r9=r4+ -r8|0;r10=r9;r11=r8+r2|0;r12=HEAP32[51792>>2];if(r9>>>0<r12>>>0){_abort()}if((r10|0)==(HEAP32[51796>>2]|0)){r13=r4+(r2+4)|0;if((HEAP32[r13>>2]&3|0)!=3){r14=r10;r15=r11;break}HEAP32[51784>>2]=r11;HEAP32[r13>>2]=HEAP32[r13>>2]&-2;HEAP32[r4+(4-r8)>>2]=r11|1;HEAP32[r5>>2]=r11;return}r13=r8>>>3;if(r8>>>0<256){r16=HEAP32[r4+(8-r8)>>2];r17=HEAP32[r4+(12-r8)>>2];r18=51816+(r13<<1<<2)|0;do{if((r16|0)!=(r18|0)){if(r16>>>0<r12>>>0){_abort()}if((HEAP32[r16+12>>2]|0)==(r10|0)){break}_abort()}}while(0);if((r17|0)==(r16|0)){HEAP32[51776>>2]=HEAP32[51776>>2]&~(1<<r13);r14=r10;r15=r11;break}do{if((r17|0)==(r18|0)){r19=r17+8|0}else{if(r17>>>0<r12>>>0){_abort()}r20=r17+8|0;if((HEAP32[r20>>2]|0)==(r10|0)){r19=r20;break}_abort()}}while(0);HEAP32[r16+12>>2]=r17;HEAP32[r19>>2]=r16;r14=r10;r15=r11;break}r18=r9;r13=HEAP32[r4+(24-r8)>>2];r20=HEAP32[r4+(12-r8)>>2];do{if((r20|0)==(r18|0)){r21=16-r8|0;r22=r4+(r21+4)|0;r23=HEAP32[r22>>2];if((r23|0)==0){r24=r4+r21|0;r21=HEAP32[r24>>2];if((r21|0)==0){r25=0;break}else{r26=r21;r27=r24}}else{r26=r23;r27=r22}while(1){r22=r26+20|0;r23=HEAP32[r22>>2];if((r23|0)!=0){r26=r23;r27=r22;continue}r22=r26+16|0;r23=HEAP32[r22>>2];if((r23|0)==0){break}else{r26=r23;r27=r22}}if(r27>>>0<r12>>>0){_abort()}else{HEAP32[r27>>2]=0;r25=r26;break}}else{r22=HEAP32[r4+(8-r8)>>2];if(r22>>>0<r12>>>0){_abort()}r23=r22+12|0;if((HEAP32[r23>>2]|0)!=(r18|0)){_abort()}r24=r20+8|0;if((HEAP32[r24>>2]|0)==(r18|0)){HEAP32[r23>>2]=r20;HEAP32[r24>>2]=r22;r25=r20;break}else{_abort()}}}while(0);if((r13|0)==0){r14=r10;r15=r11;break}r20=r4+(28-r8)|0;r12=52080+(HEAP32[r20>>2]<<2)|0;do{if((r18|0)==(HEAP32[r12>>2]|0)){HEAP32[r12>>2]=r25;if((r25|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r20>>2]);r14=r10;r15=r11;break L3813}else{if(r13>>>0<HEAP32[51792>>2]>>>0){_abort()}r9=r13+16|0;if((HEAP32[r9>>2]|0)==(r18|0)){HEAP32[r9>>2]=r25}else{HEAP32[r13+20>>2]=r25}if((r25|0)==0){r14=r10;r15=r11;break L3813}}}while(0);if(r25>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r25+24>>2]=r13;r18=16-r8|0;r20=HEAP32[r4+r18>>2];do{if((r20|0)!=0){if(r20>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r25+16>>2]=r20;HEAP32[r20+24>>2]=r25;break}}}while(0);r20=HEAP32[r4+(r18+4)>>2];if((r20|0)==0){r14=r10;r15=r11;break}if(r20>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r25+20>>2]=r20;HEAP32[r20+24>>2]=r25;r14=r10;r15=r11;break}}else{r14=r1;r15=r2}}while(0);r1=HEAP32[51792>>2];if(r5>>>0<r1>>>0){_abort()}r25=r4+(r2+4)|0;r26=HEAP32[r25>>2];do{if((r26&2|0)==0){if((r6|0)==(HEAP32[51800>>2]|0)){r27=HEAP32[51788>>2]+r15|0;HEAP32[51788>>2]=r27;HEAP32[51800>>2]=r14;HEAP32[r14+4>>2]=r27|1;if((r14|0)!=(HEAP32[51796>>2]|0)){return}HEAP32[51796>>2]=0;HEAP32[51784>>2]=0;return}if((r6|0)==(HEAP32[51796>>2]|0)){r27=HEAP32[51784>>2]+r15|0;HEAP32[51784>>2]=r27;HEAP32[51796>>2]=r14;HEAP32[r14+4>>2]=r27|1;HEAP32[r14+r27>>2]=r27;return}r27=(r26&-8)+r15|0;r19=r26>>>3;L3913:do{if(r26>>>0<256){r7=HEAP32[r4+(r2+8)>>2];r20=HEAP32[r4+(r2+12)>>2];r8=51816+(r19<<1<<2)|0;do{if((r7|0)!=(r8|0)){if(r7>>>0<r1>>>0){_abort()}if((HEAP32[r7+12>>2]|0)==(r6|0)){break}_abort()}}while(0);if((r20|0)==(r7|0)){HEAP32[51776>>2]=HEAP32[51776>>2]&~(1<<r19);break}do{if((r20|0)==(r8|0)){r28=r20+8|0}else{if(r20>>>0<r1>>>0){_abort()}r13=r20+8|0;if((HEAP32[r13>>2]|0)==(r6|0)){r28=r13;break}_abort()}}while(0);HEAP32[r7+12>>2]=r20;HEAP32[r28>>2]=r7}else{r8=r5;r13=HEAP32[r4+(r2+24)>>2];r12=HEAP32[r4+(r2+12)>>2];do{if((r12|0)==(r8|0)){r9=r4+(r2+20)|0;r16=HEAP32[r9>>2];if((r16|0)==0){r17=r4+(r2+16)|0;r22=HEAP32[r17>>2];if((r22|0)==0){r29=0;break}else{r30=r22;r31=r17}}else{r30=r16;r31=r9}while(1){r9=r30+20|0;r16=HEAP32[r9>>2];if((r16|0)!=0){r30=r16;r31=r9;continue}r9=r30+16|0;r16=HEAP32[r9>>2];if((r16|0)==0){break}else{r30=r16;r31=r9}}if(r31>>>0<r1>>>0){_abort()}else{HEAP32[r31>>2]=0;r29=r30;break}}else{r9=HEAP32[r4+(r2+8)>>2];if(r9>>>0<r1>>>0){_abort()}r16=r9+12|0;if((HEAP32[r16>>2]|0)!=(r8|0)){_abort()}r17=r12+8|0;if((HEAP32[r17>>2]|0)==(r8|0)){HEAP32[r16>>2]=r12;HEAP32[r17>>2]=r9;r29=r12;break}else{_abort()}}}while(0);if((r13|0)==0){break}r12=r4+(r2+28)|0;r7=52080+(HEAP32[r12>>2]<<2)|0;do{if((r8|0)==(HEAP32[r7>>2]|0)){HEAP32[r7>>2]=r29;if((r29|0)!=0){break}HEAP32[51780>>2]=HEAP32[51780>>2]&~(1<<HEAP32[r12>>2]);break L3913}else{if(r13>>>0<HEAP32[51792>>2]>>>0){_abort()}r20=r13+16|0;if((HEAP32[r20>>2]|0)==(r8|0)){HEAP32[r20>>2]=r29}else{HEAP32[r13+20>>2]=r29}if((r29|0)==0){break L3913}}}while(0);if(r29>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r29+24>>2]=r13;r8=HEAP32[r4+(r2+16)>>2];do{if((r8|0)!=0){if(r8>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r29+16>>2]=r8;HEAP32[r8+24>>2]=r29;break}}}while(0);r8=HEAP32[r4+(r2+20)>>2];if((r8|0)==0){break}if(r8>>>0<HEAP32[51792>>2]>>>0){_abort()}else{HEAP32[r29+20>>2]=r8;HEAP32[r8+24>>2]=r29;break}}}while(0);HEAP32[r14+4>>2]=r27|1;HEAP32[r14+r27>>2]=r27;if((r14|0)!=(HEAP32[51796>>2]|0)){r32=r27;break}HEAP32[51784>>2]=r27;return}else{HEAP32[r25>>2]=r26&-2;HEAP32[r14+4>>2]=r15|1;HEAP32[r14+r15>>2]=r15;r32=r15}}while(0);r15=r32>>>3;if(r32>>>0<256){r26=r15<<1;r25=51816+(r26<<2)|0;r29=HEAP32[51776>>2];r2=1<<r15;do{if((r29&r2|0)==0){HEAP32[51776>>2]=r29|r2;r33=r25;r34=51816+(r26+2<<2)|0}else{r15=51816+(r26+2<<2)|0;r4=HEAP32[r15>>2];if(r4>>>0>=HEAP32[51792>>2]>>>0){r33=r4;r34=r15;break}_abort()}}while(0);HEAP32[r34>>2]=r14;HEAP32[r33+12>>2]=r14;HEAP32[r14+8>>2]=r33;HEAP32[r14+12>>2]=r25;return}r25=r14;r33=r32>>>8;do{if((r33|0)==0){r35=0}else{if(r32>>>0>16777215){r35=31;break}r34=(r33+1048320|0)>>>16&8;r26=r33<<r34;r2=(r26+520192|0)>>>16&4;r29=r26<<r2;r26=(r29+245760|0)>>>16&2;r15=14-(r2|r34|r26)+(r29<<r26>>>15)|0;r35=r32>>>((r15+7|0)>>>0)&1|r15<<1}}while(0);r33=52080+(r35<<2)|0;HEAP32[r14+28>>2]=r35;HEAP32[r14+20>>2]=0;HEAP32[r14+16>>2]=0;r15=HEAP32[51780>>2];r26=1<<r35;if((r15&r26|0)==0){HEAP32[51780>>2]=r15|r26;HEAP32[r33>>2]=r25;HEAP32[r14+24>>2]=r33;HEAP32[r14+12>>2]=r14;HEAP32[r14+8>>2]=r14;return}if((r35|0)==31){r36=0}else{r36=25-(r35>>>1)|0}r35=r32<<r36;r36=HEAP32[r33>>2];while(1){if((HEAP32[r36+4>>2]&-8|0)==(r32|0)){break}r37=r36+16+(r35>>>31<<2)|0;r33=HEAP32[r37>>2];if((r33|0)==0){r3=3659;break}else{r35=r35<<1;r36=r33}}if(r3==3659){if(r37>>>0<HEAP32[51792>>2]>>>0){_abort()}HEAP32[r37>>2]=r25;HEAP32[r14+24>>2]=r36;HEAP32[r14+12>>2]=r14;HEAP32[r14+8>>2]=r14;return}r37=r36+8|0;r3=HEAP32[r37>>2];r35=HEAP32[51792>>2];if(r36>>>0<r35>>>0){_abort()}if(r3>>>0<r35>>>0){_abort()}HEAP32[r3+12>>2]=r25;HEAP32[r37>>2]=r25;HEAP32[r14+8>>2]=r3;HEAP32[r14+12>>2]=r36;HEAP32[r14+24>>2]=0;return}function __Znwj(r1){var r2,r3,r4;r2=0;r3=(r1|0)==0?1:r1;while(1){r4=_malloc(r3);if((r4|0)!=0){r2=3703;break}r1=(tempValue=HEAP32[73576>>2],HEAP32[73576>>2]=tempValue+0,tempValue);if((r1|0)==0){break}FUNCTION_TABLE[r1]()}if(r2==3703){return r4}r4=___cxa_allocate_exception(4);HEAP32[r4>>2]=19680;___cxa_throw(r4,46792,4624)}function __ZNSt9bad_allocD0Ev(r1){if((r1|0)==0){return}_free(r1);return}function __ZNSt9bad_allocD2Ev(r1){return}function __ZNKSt9bad_alloc4whatEv(r1){return 10800}function _i64Add(r1,r2,r3,r4){var r5,r6;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0;r5=r1+r3>>>0;r6=r2+r4+(r5>>>0<r1>>>0|0)>>>0;return tempRet0=r6,r5|0}function _i64Subtract(r1,r2,r3,r4){var r5,r6;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0;r5=r1-r3>>>0;r6=r2-r4>>>0;r6=r2-r4-(r3>>>0>r1>>>0|0)>>>0;return tempRet0=r6,r5|0}function _bitshift64Shl(r1,r2,r3){var r4;r1=r1|0;r2=r2|0;r3=r3|0;r4=0;if((r3|0)<32){r4=(1<<r3)-1|0;tempRet0=r2<<r3|(r1&r4<<32-r3)>>>32-r3;return r1<<r3}tempRet0=r1<<r3-32;return 0}function _bitshift64Lshr(r1,r2,r3){var r4;r1=r1|0;r2=r2|0;r3=r3|0;r4=0;if((r3|0)<32){r4=(1<<r3)-1|0;tempRet0=r2>>>r3;return r1>>>r3|(r2&r4)<<32-r3}tempRet0=0;return r2>>>r3-32|0}function _bitshift64Ashr(r1,r2,r3){var r4;r1=r1|0;r2=r2|0;r3=r3|0;r4=0;if((r3|0)<32){r4=(1<<r3)-1|0;tempRet0=r2>>r3;return r1>>>r3|(r2&r4)<<32-r3}tempRet0=(r2|0)<0?-1:0;return r2>>r3-32|0}function _llvm_ctlz_i32(r1){var r2;r1=r1|0;r2=0;r2=HEAP8[ctlz_i8+(r1>>>24)|0];if((r2|0)<8)return r2|0;r2=HEAP8[ctlz_i8+(r1>>16&255)|0];if((r2|0)<8)return r2+8|0;r2=HEAP8[ctlz_i8+(r1>>8&255)|0];if((r2|0)<8)return r2+16|0;return HEAP8[ctlz_i8+(r1&255)|0]+24|0}var ctlz_i8=allocate([8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"i8",ALLOC_DYNAMIC);function _llvm_cttz_i32(r1){var r2;r1=r1|0;r2=0;r2=HEAP8[cttz_i8+(r1&255)|0];if((r2|0)<8)return r2|0;r2=HEAP8[cttz_i8+(r1>>8&255)|0];if((r2|0)<8)return r2+8|0;r2=HEAP8[cttz_i8+(r1>>16&255)|0];if((r2|0)<8)return r2+16|0;return HEAP8[cttz_i8+(r1>>>24)|0]+24|0}var cttz_i8=allocate([8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0],"i8",ALLOC_DYNAMIC);function ___muldsi3(r1,r2){var r3,r4,r5,r6,r7,r8,r9;r1=r1|0;r2=r2|0;r3=0,r4=0,r5=0,r6=0,r7=0,r8=0,r9=0;r3=r1&65535;r4=r2&65535;r5=Math_imul(r4,r3)|0;r6=r1>>>16;r7=(r5>>>16)+Math_imul(r4,r6)|0;r8=r2>>>16;r9=Math_imul(r8,r3)|0;return(tempRet0=(r7>>>16)+Math_imul(r8,r6)+(((r7&65535)+r9|0)>>>16)|0,r7+r9<<16|r5&65535|0)|0}function ___divdi3(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0,r7=0,r8=0,r9=0,r10=0,r11=0,r12=0,r13=0,r14=0,r15=0;r5=r2>>31|((r2|0)<0?-1:0)<<1;r6=((r2|0)<0?-1:0)>>31|((r2|0)<0?-1:0)<<1;r7=r4>>31|((r4|0)<0?-1:0)<<1;r8=((r4|0)<0?-1:0)>>31|((r4|0)<0?-1:0)<<1;r9=_i64Subtract(r5^r1,r6^r2,r5,r6)|0;r10=tempRet0;r11=_i64Subtract(r7^r3,r8^r4,r7,r8)|0;r12=r7^r5;r13=r8^r6;r14=___udivmoddi4(r9,r10,r11,tempRet0,0)|0;r15=_i64Subtract(r14^r12,tempRet0^r13,r12,r13)|0;return(tempRet0=tempRet0,r15)|0}function ___remdi3(r1,r2,r3,r4){var r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0,r7=0,r8=0,r9=0,r10=0,r11=0,r12=0,r13=0,r14=0,r15=0;r15=STACKTOP;STACKTOP=STACKTOP+8|0;r5=r15|0;r6=r2>>31|((r2|0)<0?-1:0)<<1;r7=((r2|0)<0?-1:0)>>31|((r2|0)<0?-1:0)<<1;r8=r4>>31|((r4|0)<0?-1:0)<<1;r9=((r4|0)<0?-1:0)>>31|((r4|0)<0?-1:0)<<1;r10=_i64Subtract(r6^r1,r7^r2,r6,r7)|0;r11=tempRet0;r12=_i64Subtract(r8^r3,r9^r4,r8,r9)|0;___udivmoddi4(r10,r11,r12,tempRet0,r5)|0;r13=_i64Subtract(HEAP32[r5>>2]^r6,HEAP32[r5+4>>2]^r7,r6,r7)|0;r14=tempRet0;STACKTOP=r15;return(tempRet0=r14,r13)|0}function ___muldi3(r1,r2,r3,r4){var r5,r6,r7,r8,r9;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0,r7=0,r8=0,r9=0;r5=r1;r6=r3;r7=___muldsi3(r5,r6)|0;r8=tempRet0;r9=Math_imul(r2,r6)|0;return(tempRet0=Math_imul(r4,r5)+r9+r8|r8&0,r7&-1|0)|0}function ___udivdi3(r1,r2,r3,r4){var r5;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0;r5=___udivmoddi4(r1,r2,r3,r4,0)|0;return(tempRet0=tempRet0,r5)|0}function ___uremdi3(r1,r2,r3,r4){var r5,r6;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=0,r6=0;r6=STACKTOP;STACKTOP=STACKTOP+8|0;r5=r6|0;___udivmoddi4(r1,r2,r3,r4,r5)|0;STACKTOP=r6;return(tempRet0=HEAP32[r5+4>>2]|0,HEAP32[r5>>2]|0)|0}function ___udivmoddi4(r1,r2,r3,r4,r5){var r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,r32,r33,r34,r35,r36,r37,r38,r39,r40,r41,r42,r43,r44,r45,r46,r47,r48,r49,r50,r51,r52,r53,r54,r55,r56,r57,r58,r59,r60,r61,r62,r63,r64,r65,r66,r67,r68,r69;r1=r1|0;r2=r2|0;r3=r3|0;r4=r4|0;r5=r5|0;r6=0,r7=0,r8=0,r9=0,r10=0,r11=0,r12=0,r13=0,r14=0,r15=0,r16=0,r17=0,r18=0,r19=0,r20=0,r21=0,r22=0,r23=0,r24=0,r25=0,r26=0,r27=0,r28=0,r29=0,r30=0,r31=0,r32=0,r33=0,r34=0,r35=0,r36=0,r37=0,r38=0,r39=0,r40=0,r41=0,r42=0,r43=0,r44=0,r45=0,r46=0,r47=0,r48=0,r49=0,r50=0,r51=0,r52=0,r53=0,r54=0,r55=0,r56=0,r57=0,r58=0,r59=0,r60=0,r61=0,r62=0,r63=0,r64=0,r65=0,r66=0,r67=0,r68=0,r69=0;r6=r1;r7=r2;r8=r7;r9=r3;r10=r4;r11=r10;if((r8|0)==0){r12=(r5|0)!=0;if((r11|0)==0){if(r12){HEAP32[r5>>2]=(r6>>>0)%(r9>>>0);HEAP32[r5+4>>2]=0}r69=0;r68=(r6>>>0)/(r9>>>0)>>>0;return(tempRet0=r69,r68)|0}else{if(!r12){r69=0;r68=0;return(tempRet0=r69,r68)|0}HEAP32[r5>>2]=r1&-1;HEAP32[r5+4>>2]=r2&0;r69=0;r68=0;return(tempRet0=r69,r68)|0}}r13=(r11|0)==0;do{if((r9|0)==0){if(r13){if((r5|0)!=0){HEAP32[r5>>2]=(r8>>>0)%(r9>>>0);HEAP32[r5+4>>2]=0}r69=0;r68=(r8>>>0)/(r9>>>0)>>>0;return(tempRet0=r69,r68)|0}if((r6|0)==0){if((r5|0)!=0){HEAP32[r5>>2]=0;HEAP32[r5+4>>2]=(r8>>>0)%(r11>>>0)}r69=0;r68=(r8>>>0)/(r11>>>0)>>>0;return(tempRet0=r69,r68)|0}r14=r11-1|0;if((r14&r11|0)==0){if((r5|0)!=0){HEAP32[r5>>2]=r1&-1;HEAP32[r5+4>>2]=r14&r8|r2&0}r69=0;r68=r8>>>((_llvm_cttz_i32(r11|0)|0)>>>0);return(tempRet0=r69,r68)|0}r15=_llvm_ctlz_i32(r11|0)|0;r16=r15-_llvm_ctlz_i32(r8|0)|0;if(r16>>>0<=30){r17=r16+1|0;r18=31-r16|0;r37=r17;r36=r8<<r18|r6>>>(r17>>>0);r35=r8>>>(r17>>>0);r34=0;r33=r6<<r18;break}if((r5|0)==0){r69=0;r68=0;return(tempRet0=r69,r68)|0}HEAP32[r5>>2]=r1&-1;HEAP32[r5+4>>2]=r7|r2&0;r69=0;r68=0;return(tempRet0=r69,r68)|0}else{if(!r13){r28=_llvm_ctlz_i32(r11|0)|0;r29=r28-_llvm_ctlz_i32(r8|0)|0;if(r29>>>0<=31){r30=r29+1|0;r31=31-r29|0;r32=r29-31>>31;r37=r30;r36=r6>>>(r30>>>0)&r32|r8<<r31;r35=r8>>>(r30>>>0)&r32;r34=0;r33=r6<<r31;break}if((r5|0)==0){r69=0;r68=0;return(tempRet0=r69,r68)|0}HEAP32[r5>>2]=r1&-1;HEAP32[r5+4>>2]=r7|r2&0;r69=0;r68=0;return(tempRet0=r69,r68)|0}r19=r9-1|0;if((r19&r9|0)!=0){r21=_llvm_ctlz_i32(r9|0)+33|0;r22=r21-_llvm_ctlz_i32(r8|0)|0;r23=64-r22|0;r24=32-r22|0;r25=r24>>31;r26=r22-32|0;r27=r26>>31;r37=r22;r36=r24-1>>31&r8>>>(r26>>>0)|(r8<<r24|r6>>>(r22>>>0))&r27;r35=r27&r8>>>(r22>>>0);r34=r6<<r23&r25;r33=(r8<<r23|r6>>>(r26>>>0))&r25|r6<<r24&r22-33>>31;break}if((r5|0)!=0){HEAP32[r5>>2]=r19&r6;HEAP32[r5+4>>2]=0}if((r9|0)==1){r69=r7|r2&0;r68=r1&-1|0;return(tempRet0=r69,r68)|0}else{r20=_llvm_cttz_i32(r9|0)|0;r69=r8>>>(r20>>>0)|0;r68=r8<<32-r20|r6>>>(r20>>>0)|0;return(tempRet0=r69,r68)|0}}}while(0);if((r37|0)==0){r64=r33;r63=r34;r62=r35;r61=r36;r60=0;r59=0}else{r38=r3&-1|0;r39=r10|r4&0;r40=_i64Add(r38,r39,-1,-1)|0;r41=tempRet0;r47=r33;r46=r34;r45=r35;r44=r36;r43=r37;r42=0;while(1){r48=r46>>>31|r47<<1;r49=r42|r46<<1;r50=r44<<1|r47>>>31|0;r51=r44>>>31|r45<<1|0;_i64Subtract(r40,r41,r50,r51)|0;r52=tempRet0;r53=r52>>31|((r52|0)<0?-1:0)<<1;r54=r53&1;r55=_i64Subtract(r50,r51,r53&r38,(((r52|0)<0?-1:0)>>31|((r52|0)<0?-1:0)<<1)&r39)|0;r56=r55;r57=tempRet0;r58=r43-1|0;if((r58|0)==0){break}else{r47=r48;r46=r49;r45=r57;r44=r56;r43=r58;r42=r54}}r64=r48;r63=r49;r62=r57;r61=r56;r60=0;r59=r54}r65=r63;r66=0;r67=r64|r66;if((r5|0)!=0){HEAP32[r5>>2]=r61;HEAP32[r5+4>>2]=r62}r69=(r65|0)>>>31|r67<<1|(r66<<1|r65>>>31)&0|r60;r68=(r65<<1|0>>>31)&-2|r59;return(tempRet0=r69,r68)|0}
// EMSCRIPTEN_END_FUNCS
Module["_malloc"] = _malloc;
Module["_free"] = _free;
// TODO: strip out parts of this we do not need
//======= begin closure i64 code =======
// Copyright 2009 The Closure Library Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/**
 * @fileoverview Defines a Long class for representing a 64-bit two's-complement
 * integer value, which faithfully simulates the behavior of a Java "long". This
 * implementation is derived from LongLib in GWT.
 *
 */
var i64Math = (function() { // Emscripten wrapper
  var goog = { math: {} };
  /**
   * Constructs a 64-bit two's-complement integer, given its low and high 32-bit
   * values as *signed* integers.  See the from* functions below for more
   * convenient ways of constructing Longs.
   *
   * The internal representation of a long is the two given signed, 32-bit values.
   * We use 32-bit pieces because these are the size of integers on which
   * Javascript performs bit-operations.  For operations like addition and
   * multiplication, we split each number into 16-bit pieces, which can easily be
   * multiplied within Javascript's floating-point representation without overflow
   * or change in sign.
   *
   * In the algorithms below, we frequently reduce the negative case to the
   * positive case by negating the input(s) and then post-processing the result.
   * Note that we must ALWAYS check specially whether those values are MIN_VALUE
   * (-2^63) because -MIN_VALUE == MIN_VALUE (since 2^63 cannot be represented as
   * a positive number, it overflows back into a negative).  Not handling this
   * case would often result in infinite recursion.
   *
   * @param {number} low  The low (signed) 32 bits of the long.
   * @param {number} high  The high (signed) 32 bits of the long.
   * @constructor
   */
  goog.math.Long = function(low, high) {
    /**
     * @type {number}
     * @private
     */
    this.low_ = low | 0;  // force into 32 signed bits.
    /**
     * @type {number}
     * @private
     */
    this.high_ = high | 0;  // force into 32 signed bits.
  };
  // NOTE: Common constant values ZERO, ONE, NEG_ONE, etc. are defined below the
  // from* methods on which they depend.
  /**
   * A cache of the Long representations of small integer values.
   * @type {!Object}
   * @private
   */
  goog.math.Long.IntCache_ = {};
  /**
   * Returns a Long representing the given (32-bit) integer value.
   * @param {number} value The 32-bit integer in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromInt = function(value) {
    if (-128 <= value && value < 128) {
      var cachedObj = goog.math.Long.IntCache_[value];
      if (cachedObj) {
        return cachedObj;
      }
    }
    var obj = new goog.math.Long(value | 0, value < 0 ? -1 : 0);
    if (-128 <= value && value < 128) {
      goog.math.Long.IntCache_[value] = obj;
    }
    return obj;
  };
  /**
   * Returns a Long representing the given value, provided that it is a finite
   * number.  Otherwise, zero is returned.
   * @param {number} value The number in question.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromNumber = function(value) {
    if (isNaN(value) || !isFinite(value)) {
      return goog.math.Long.ZERO;
    } else if (value <= -goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MIN_VALUE;
    } else if (value + 1 >= goog.math.Long.TWO_PWR_63_DBL_) {
      return goog.math.Long.MAX_VALUE;
    } else if (value < 0) {
      return goog.math.Long.fromNumber(-value).negate();
    } else {
      return new goog.math.Long(
          (value % goog.math.Long.TWO_PWR_32_DBL_) | 0,
          (value / goog.math.Long.TWO_PWR_32_DBL_) | 0);
    }
  };
  /**
   * Returns a Long representing the 64-bit integer that comes by concatenating
   * the given high and low bits.  Each is assumed to use 32 bits.
   * @param {number} lowBits The low 32-bits.
   * @param {number} highBits The high 32-bits.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromBits = function(lowBits, highBits) {
    return new goog.math.Long(lowBits, highBits);
  };
  /**
   * Returns a Long representation of the given string, written using the given
   * radix.
   * @param {string} str The textual representation of the Long.
   * @param {number=} opt_radix The radix in which the text is written.
   * @return {!goog.math.Long} The corresponding Long value.
   */
  goog.math.Long.fromString = function(str, opt_radix) {
    if (str.length == 0) {
      throw Error('number format error: empty string');
    }
    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }
    if (str.charAt(0) == '-') {
      return goog.math.Long.fromString(str.substring(1), radix).negate();
    } else if (str.indexOf('-') >= 0) {
      throw Error('number format error: interior "-" character: ' + str);
    }
    // Do several (8) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 8));
    var result = goog.math.Long.ZERO;
    for (var i = 0; i < str.length; i += 8) {
      var size = Math.min(8, str.length - i);
      var value = parseInt(str.substring(i, i + size), radix);
      if (size < 8) {
        var power = goog.math.Long.fromNumber(Math.pow(radix, size));
        result = result.multiply(power).add(goog.math.Long.fromNumber(value));
      } else {
        result = result.multiply(radixToPower);
        result = result.add(goog.math.Long.fromNumber(value));
      }
    }
    return result;
  };
  // NOTE: the compiler should inline these constant values below and then remove
  // these variables, so there should be no runtime penalty for these.
  /**
   * Number used repeated below in calculations.  This must appear before the
   * first call to any from* function below.
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_16_DBL_ = 1 << 16;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_24_DBL_ = 1 << 24;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_32_DBL_ =
      goog.math.Long.TWO_PWR_16_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_31_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ / 2;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_48_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_16_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_64_DBL_ =
      goog.math.Long.TWO_PWR_32_DBL_ * goog.math.Long.TWO_PWR_32_DBL_;
  /**
   * @type {number}
   * @private
   */
  goog.math.Long.TWO_PWR_63_DBL_ =
      goog.math.Long.TWO_PWR_64_DBL_ / 2;
  /** @type {!goog.math.Long} */
  goog.math.Long.ZERO = goog.math.Long.fromInt(0);
  /** @type {!goog.math.Long} */
  goog.math.Long.ONE = goog.math.Long.fromInt(1);
  /** @type {!goog.math.Long} */
  goog.math.Long.NEG_ONE = goog.math.Long.fromInt(-1);
  /** @type {!goog.math.Long} */
  goog.math.Long.MAX_VALUE =
      goog.math.Long.fromBits(0xFFFFFFFF | 0, 0x7FFFFFFF | 0);
  /** @type {!goog.math.Long} */
  goog.math.Long.MIN_VALUE = goog.math.Long.fromBits(0, 0x80000000 | 0);
  /**
   * @type {!goog.math.Long}
   * @private
   */
  goog.math.Long.TWO_PWR_24_ = goog.math.Long.fromInt(1 << 24);
  /** @return {number} The value, assuming it is a 32-bit integer. */
  goog.math.Long.prototype.toInt = function() {
    return this.low_;
  };
  /** @return {number} The closest floating-point representation to this value. */
  goog.math.Long.prototype.toNumber = function() {
    return this.high_ * goog.math.Long.TWO_PWR_32_DBL_ +
           this.getLowBitsUnsigned();
  };
  /**
   * @param {number=} opt_radix The radix in which the text should be written.
   * @return {string} The textual representation of this value.
   */
  goog.math.Long.prototype.toString = function(opt_radix) {
    var radix = opt_radix || 10;
    if (radix < 2 || 36 < radix) {
      throw Error('radix out of range: ' + radix);
    }
    if (this.isZero()) {
      return '0';
    }
    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        // We need to change the Long value before it can be negated, so we remove
        // the bottom-most digit in this base and then recurse to do the rest.
        var radixLong = goog.math.Long.fromNumber(radix);
        var div = this.div(radixLong);
        var rem = div.multiply(radixLong).subtract(this);
        return div.toString(radix) + rem.toInt().toString(radix);
      } else {
        return '-' + this.negate().toString(radix);
      }
    }
    // Do several (6) digits each time through the loop, so as to
    // minimize the calls to the very expensive emulated div.
    var radixToPower = goog.math.Long.fromNumber(Math.pow(radix, 6));
    var rem = this;
    var result = '';
    while (true) {
      var remDiv = rem.div(radixToPower);
      var intval = rem.subtract(remDiv.multiply(radixToPower)).toInt();
      var digits = intval.toString(radix);
      rem = remDiv;
      if (rem.isZero()) {
        return digits + result;
      } else {
        while (digits.length < 6) {
          digits = '0' + digits;
        }
        result = '' + digits + result;
      }
    }
  };
  /** @return {number} The high 32-bits as a signed value. */
  goog.math.Long.prototype.getHighBits = function() {
    return this.high_;
  };
  /** @return {number} The low 32-bits as a signed value. */
  goog.math.Long.prototype.getLowBits = function() {
    return this.low_;
  };
  /** @return {number} The low 32-bits as an unsigned value. */
  goog.math.Long.prototype.getLowBitsUnsigned = function() {
    return (this.low_ >= 0) ?
        this.low_ : goog.math.Long.TWO_PWR_32_DBL_ + this.low_;
  };
  /**
   * @return {number} Returns the number of bits needed to represent the absolute
   *     value of this Long.
   */
  goog.math.Long.prototype.getNumBitsAbs = function() {
    if (this.isNegative()) {
      if (this.equals(goog.math.Long.MIN_VALUE)) {
        return 64;
      } else {
        return this.negate().getNumBitsAbs();
      }
    } else {
      var val = this.high_ != 0 ? this.high_ : this.low_;
      for (var bit = 31; bit > 0; bit--) {
        if ((val & (1 << bit)) != 0) {
          break;
        }
      }
      return this.high_ != 0 ? bit + 33 : bit + 1;
    }
  };
  /** @return {boolean} Whether this value is zero. */
  goog.math.Long.prototype.isZero = function() {
    return this.high_ == 0 && this.low_ == 0;
  };
  /** @return {boolean} Whether this value is negative. */
  goog.math.Long.prototype.isNegative = function() {
    return this.high_ < 0;
  };
  /** @return {boolean} Whether this value is odd. */
  goog.math.Long.prototype.isOdd = function() {
    return (this.low_ & 1) == 1;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long equals the other.
   */
  goog.math.Long.prototype.equals = function(other) {
    return (this.high_ == other.high_) && (this.low_ == other.low_);
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long does not equal the other.
   */
  goog.math.Long.prototype.notEquals = function(other) {
    return (this.high_ != other.high_) || (this.low_ != other.low_);
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than the other.
   */
  goog.math.Long.prototype.lessThan = function(other) {
    return this.compare(other) < 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is less than or equal to the other.
   */
  goog.math.Long.prototype.lessThanOrEqual = function(other) {
    return this.compare(other) <= 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than the other.
   */
  goog.math.Long.prototype.greaterThan = function(other) {
    return this.compare(other) > 0;
  };
  /**
   * @param {goog.math.Long} other Long to compare against.
   * @return {boolean} Whether this Long is greater than or equal to the other.
   */
  goog.math.Long.prototype.greaterThanOrEqual = function(other) {
    return this.compare(other) >= 0;
  };
  /**
   * Compares this Long with the given one.
   * @param {goog.math.Long} other Long to compare against.
   * @return {number} 0 if they are the same, 1 if the this is greater, and -1
   *     if the given one is greater.
   */
  goog.math.Long.prototype.compare = function(other) {
    if (this.equals(other)) {
      return 0;
    }
    var thisNeg = this.isNegative();
    var otherNeg = other.isNegative();
    if (thisNeg && !otherNeg) {
      return -1;
    }
    if (!thisNeg && otherNeg) {
      return 1;
    }
    // at this point, the signs are the same, so subtraction will not overflow
    if (this.subtract(other).isNegative()) {
      return -1;
    } else {
      return 1;
    }
  };
  /** @return {!goog.math.Long} The negation of this value. */
  goog.math.Long.prototype.negate = function() {
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.MIN_VALUE;
    } else {
      return this.not().add(goog.math.Long.ONE);
    }
  };
  /**
   * Returns the sum of this and the given Long.
   * @param {goog.math.Long} other Long to add to this one.
   * @return {!goog.math.Long} The sum of this and the given Long.
   */
  goog.math.Long.prototype.add = function(other) {
    // Divide each number into 4 chunks of 16 bits, and then sum the chunks.
    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;
    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;
    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 + b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 + b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 + b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 + b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };
  /**
   * Returns the difference of this and the given Long.
   * @param {goog.math.Long} other Long to subtract from this.
   * @return {!goog.math.Long} The difference of this and the given Long.
   */
  goog.math.Long.prototype.subtract = function(other) {
    return this.add(other.negate());
  };
  /**
   * Returns the product of this and the given long.
   * @param {goog.math.Long} other Long to multiply with this.
   * @return {!goog.math.Long} The product of this and the other.
   */
  goog.math.Long.prototype.multiply = function(other) {
    if (this.isZero()) {
      return goog.math.Long.ZERO;
    } else if (other.isZero()) {
      return goog.math.Long.ZERO;
    }
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      return other.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return this.isOdd() ? goog.math.Long.MIN_VALUE : goog.math.Long.ZERO;
    }
    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().multiply(other.negate());
      } else {
        return this.negate().multiply(other).negate();
      }
    } else if (other.isNegative()) {
      return this.multiply(other.negate()).negate();
    }
    // If both longs are small, use float multiplication
    if (this.lessThan(goog.math.Long.TWO_PWR_24_) &&
        other.lessThan(goog.math.Long.TWO_PWR_24_)) {
      return goog.math.Long.fromNumber(this.toNumber() * other.toNumber());
    }
    // Divide each long into 4 chunks of 16 bits, and then add up 4x4 products.
    // We can skip products that would overflow.
    var a48 = this.high_ >>> 16;
    var a32 = this.high_ & 0xFFFF;
    var a16 = this.low_ >>> 16;
    var a00 = this.low_ & 0xFFFF;
    var b48 = other.high_ >>> 16;
    var b32 = other.high_ & 0xFFFF;
    var b16 = other.low_ >>> 16;
    var b00 = other.low_ & 0xFFFF;
    var c48 = 0, c32 = 0, c16 = 0, c00 = 0;
    c00 += a00 * b00;
    c16 += c00 >>> 16;
    c00 &= 0xFFFF;
    c16 += a16 * b00;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c16 += a00 * b16;
    c32 += c16 >>> 16;
    c16 &= 0xFFFF;
    c32 += a32 * b00;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a16 * b16;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c32 += a00 * b32;
    c48 += c32 >>> 16;
    c32 &= 0xFFFF;
    c48 += a48 * b00 + a32 * b16 + a16 * b32 + a00 * b48;
    c48 &= 0xFFFF;
    return goog.math.Long.fromBits((c16 << 16) | c00, (c48 << 16) | c32);
  };
  /**
   * Returns this Long divided by the given one.
   * @param {goog.math.Long} other Long by which to divide.
   * @return {!goog.math.Long} This Long divided by the given one.
   */
  goog.math.Long.prototype.div = function(other) {
    if (other.isZero()) {
      throw Error('division by zero');
    } else if (this.isZero()) {
      return goog.math.Long.ZERO;
    }
    if (this.equals(goog.math.Long.MIN_VALUE)) {
      if (other.equals(goog.math.Long.ONE) ||
          other.equals(goog.math.Long.NEG_ONE)) {
        return goog.math.Long.MIN_VALUE;  // recall that -MIN_VALUE == MIN_VALUE
      } else if (other.equals(goog.math.Long.MIN_VALUE)) {
        return goog.math.Long.ONE;
      } else {
        // At this point, we have |other| >= 2, so |this/other| < |MIN_VALUE|.
        var halfThis = this.shiftRight(1);
        var approx = halfThis.div(other).shiftLeft(1);
        if (approx.equals(goog.math.Long.ZERO)) {
          return other.isNegative() ? goog.math.Long.ONE : goog.math.Long.NEG_ONE;
        } else {
          var rem = this.subtract(other.multiply(approx));
          var result = approx.add(rem.div(other));
          return result;
        }
      }
    } else if (other.equals(goog.math.Long.MIN_VALUE)) {
      return goog.math.Long.ZERO;
    }
    if (this.isNegative()) {
      if (other.isNegative()) {
        return this.negate().div(other.negate());
      } else {
        return this.negate().div(other).negate();
      }
    } else if (other.isNegative()) {
      return this.div(other.negate()).negate();
    }
    // Repeat the following until the remainder is less than other:  find a
    // floating-point that approximates remainder / other *from below*, add this
    // into the result, and subtract it from the remainder.  It is critical that
    // the approximate value is less than or equal to the real value so that the
    // remainder never becomes negative.
    var res = goog.math.Long.ZERO;
    var rem = this;
    while (rem.greaterThanOrEqual(other)) {
      // Approximate the result of division. This may be a little greater or
      // smaller than the actual value.
      var approx = Math.max(1, Math.floor(rem.toNumber() / other.toNumber()));
      // We will tweak the approximate result by changing it in the 48-th digit or
      // the smallest non-fractional digit, whichever is larger.
      var log2 = Math.ceil(Math.log(approx) / Math.LN2);
      var delta = (log2 <= 48) ? 1 : Math.pow(2, log2 - 48);
      // Decrease the approximation until it is smaller than the remainder.  Note
      // that if it is too large, the product overflows and is negative.
      var approxRes = goog.math.Long.fromNumber(approx);
      var approxRem = approxRes.multiply(other);
      while (approxRem.isNegative() || approxRem.greaterThan(rem)) {
        approx -= delta;
        approxRes = goog.math.Long.fromNumber(approx);
        approxRem = approxRes.multiply(other);
      }
      // We know the answer can't be zero... and actually, zero would cause
      // infinite recursion since we would make no progress.
      if (approxRes.isZero()) {
        approxRes = goog.math.Long.ONE;
      }
      res = res.add(approxRes);
      rem = rem.subtract(approxRem);
    }
    return res;
  };
  /**
   * Returns this Long modulo the given one.
   * @param {goog.math.Long} other Long by which to mod.
   * @return {!goog.math.Long} This Long modulo the given one.
   */
  goog.math.Long.prototype.modulo = function(other) {
    return this.subtract(this.div(other).multiply(other));
  };
  /** @return {!goog.math.Long} The bitwise-NOT of this value. */
  goog.math.Long.prototype.not = function() {
    return goog.math.Long.fromBits(~this.low_, ~this.high_);
  };
  /**
   * Returns the bitwise-AND of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to AND.
   * @return {!goog.math.Long} The bitwise-AND of this and the other.
   */
  goog.math.Long.prototype.and = function(other) {
    return goog.math.Long.fromBits(this.low_ & other.low_,
                                   this.high_ & other.high_);
  };
  /**
   * Returns the bitwise-OR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to OR.
   * @return {!goog.math.Long} The bitwise-OR of this and the other.
   */
  goog.math.Long.prototype.or = function(other) {
    return goog.math.Long.fromBits(this.low_ | other.low_,
                                   this.high_ | other.high_);
  };
  /**
   * Returns the bitwise-XOR of this Long and the given one.
   * @param {goog.math.Long} other The Long with which to XOR.
   * @return {!goog.math.Long} The bitwise-XOR of this and the other.
   */
  goog.math.Long.prototype.xor = function(other) {
    return goog.math.Long.fromBits(this.low_ ^ other.low_,
                                   this.high_ ^ other.high_);
  };
  /**
   * Returns this Long with bits shifted to the left by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the left by the given amount.
   */
  goog.math.Long.prototype.shiftLeft = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var low = this.low_;
      if (numBits < 32) {
        var high = this.high_;
        return goog.math.Long.fromBits(
            low << numBits,
            (high << numBits) | (low >>> (32 - numBits)));
      } else {
        return goog.math.Long.fromBits(0, low << (numBits - 32));
      }
    }
  };
  /**
   * Returns this Long with bits shifted to the right by the given amount.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount.
   */
  goog.math.Long.prototype.shiftRight = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >> numBits);
      } else {
        return goog.math.Long.fromBits(
            high >> (numBits - 32),
            high >= 0 ? 0 : -1);
      }
    }
  };
  /**
   * Returns this Long with bits shifted to the right by the given amount, with
   * the new top bits matching the current sign bit.
   * @param {number} numBits The number of bits by which to shift.
   * @return {!goog.math.Long} This shifted to the right by the given amount, with
   *     zeros placed into the new leading bits.
   */
  goog.math.Long.prototype.shiftRightUnsigned = function(numBits) {
    numBits &= 63;
    if (numBits == 0) {
      return this;
    } else {
      var high = this.high_;
      if (numBits < 32) {
        var low = this.low_;
        return goog.math.Long.fromBits(
            (low >>> numBits) | (high << (32 - numBits)),
            high >>> numBits);
      } else if (numBits == 32) {
        return goog.math.Long.fromBits(high, 0);
      } else {
        return goog.math.Long.fromBits(high >>> (numBits - 32), 0);
      }
    }
  };
  //======= begin jsbn =======
  var navigator = { appName: 'Modern Browser' }; // polyfill a little
  // Copyright (c) 2005  Tom Wu
  // All Rights Reserved.
  // http://www-cs-students.stanford.edu/~tjw/jsbn/
  /*
   * Copyright (c) 2003-2005  Tom Wu
   * All Rights Reserved.
   *
   * Permission is hereby granted, free of charge, to any person obtaining
   * a copy of this software and associated documentation files (the
   * "Software"), to deal in the Software without restriction, including
   * without limitation the rights to use, copy, modify, merge, publish,
   * distribute, sublicense, and/or sell copies of the Software, and to
   * permit persons to whom the Software is furnished to do so, subject to
   * the following conditions:
   *
   * The above copyright notice and this permission notice shall be
   * included in all copies or substantial portions of the Software.
   *
   * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
   * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
   * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
   *
   * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
   * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
   * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
   * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
   * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
   *
   * In addition, the following condition applies:
   *
   * All redistributions must retain an intact copy of this copyright notice
   * and disclaimer.
   */
  // Basic JavaScript BN library - subset useful for RSA encryption.
  // Bits per digit
  var dbits;
  // JavaScript engine analysis
  var canary = 0xdeadbeefcafe;
  var j_lm = ((canary&0xffffff)==0xefcafe);
  // (public) Constructor
  function BigInteger(a,b,c) {
    if(a != null)
      if("number" == typeof a) this.fromNumber(a,b,c);
      else if(b == null && "string" != typeof a) this.fromString(a,256);
      else this.fromString(a,b);
  }
  // return new, unset BigInteger
  function nbi() { return new BigInteger(null); }
  // am: Compute w_j += (x*this_i), propagate carries,
  // c is initial carry, returns final carry.
  // c < 3*dvalue, x < 2*dvalue, this_i < dvalue
  // We need to select the fastest one that works in this environment.
  // am1: use a single mult and divide to get the high bits,
  // max digit bits should be 26 because
  // max internal value = 2*dvalue^2-2*dvalue (< 2^53)
  function am1(i,x,w,j,c,n) {
    while(--n >= 0) {
      var v = x*this[i++]+w[j]+c;
      c = Math.floor(v/0x4000000);
      w[j++] = v&0x3ffffff;
    }
    return c;
  }
  // am2 avoids a big mult-and-extract completely.
  // Max digit bits should be <= 30 because we do bitwise ops
  // on values up to 2*hdvalue^2-hdvalue-1 (< 2^31)
  function am2(i,x,w,j,c,n) {
    var xl = x&0x7fff, xh = x>>15;
    while(--n >= 0) {
      var l = this[i]&0x7fff;
      var h = this[i++]>>15;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x7fff)<<15)+w[j]+(c&0x3fffffff);
      c = (l>>>30)+(m>>>15)+xh*h+(c>>>30);
      w[j++] = l&0x3fffffff;
    }
    return c;
  }
  // Alternately, set max digit bits to 28 since some
  // browsers slow down when dealing with 32-bit numbers.
  function am3(i,x,w,j,c,n) {
    var xl = x&0x3fff, xh = x>>14;
    while(--n >= 0) {
      var l = this[i]&0x3fff;
      var h = this[i++]>>14;
      var m = xh*l+h*xl;
      l = xl*l+((m&0x3fff)<<14)+w[j]+c;
      c = (l>>28)+(m>>14)+xh*h;
      w[j++] = l&0xfffffff;
    }
    return c;
  }
  if(j_lm && (navigator.appName == "Microsoft Internet Explorer")) {
    BigInteger.prototype.am = am2;
    dbits = 30;
  }
  else if(j_lm && (navigator.appName != "Netscape")) {
    BigInteger.prototype.am = am1;
    dbits = 26;
  }
  else { // Mozilla/Netscape seems to prefer am3
    BigInteger.prototype.am = am3;
    dbits = 28;
  }
  BigInteger.prototype.DB = dbits;
  BigInteger.prototype.DM = ((1<<dbits)-1);
  BigInteger.prototype.DV = (1<<dbits);
  var BI_FP = 52;
  BigInteger.prototype.FV = Math.pow(2,BI_FP);
  BigInteger.prototype.F1 = BI_FP-dbits;
  BigInteger.prototype.F2 = 2*dbits-BI_FP;
  // Digit conversions
  var BI_RM = "0123456789abcdefghijklmnopqrstuvwxyz";
  var BI_RC = new Array();
  var rr,vv;
  rr = "0".charCodeAt(0);
  for(vv = 0; vv <= 9; ++vv) BI_RC[rr++] = vv;
  rr = "a".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
  rr = "A".charCodeAt(0);
  for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
  function int2char(n) { return BI_RM.charAt(n); }
  function intAt(s,i) {
    var c = BI_RC[s.charCodeAt(i)];
    return (c==null)?-1:c;
  }
  // (protected) copy this to r
  function bnpCopyTo(r) {
    for(var i = this.t-1; i >= 0; --i) r[i] = this[i];
    r.t = this.t;
    r.s = this.s;
  }
  // (protected) set from integer value x, -DV <= x < DV
  function bnpFromInt(x) {
    this.t = 1;
    this.s = (x<0)?-1:0;
    if(x > 0) this[0] = x;
    else if(x < -1) this[0] = x+DV;
    else this.t = 0;
  }
  // return bigint initialized to value
  function nbv(i) { var r = nbi(); r.fromInt(i); return r; }
  // (protected) set from string and radix
  function bnpFromString(s,b) {
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 256) k = 8; // byte array
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else { this.fromRadix(s,b); return; }
    this.t = 0;
    this.s = 0;
    var i = s.length, mi = false, sh = 0;
    while(--i >= 0) {
      var x = (k==8)?s[i]&0xff:intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-") mi = true;
        continue;
      }
      mi = false;
      if(sh == 0)
        this[this.t++] = x;
      else if(sh+k > this.DB) {
        this[this.t-1] |= (x&((1<<(this.DB-sh))-1))<<sh;
        this[this.t++] = (x>>(this.DB-sh));
      }
      else
        this[this.t-1] |= x<<sh;
      sh += k;
      if(sh >= this.DB) sh -= this.DB;
    }
    if(k == 8 && (s[0]&0x80) != 0) {
      this.s = -1;
      if(sh > 0) this[this.t-1] |= ((1<<(this.DB-sh))-1)<<sh;
    }
    this.clamp();
    if(mi) BigInteger.ZERO.subTo(this,this);
  }
  // (protected) clamp off excess high words
  function bnpClamp() {
    var c = this.s&this.DM;
    while(this.t > 0 && this[this.t-1] == c) --this.t;
  }
  // (public) return string representation in given radix
  function bnToString(b) {
    if(this.s < 0) return "-"+this.negate().toString(b);
    var k;
    if(b == 16) k = 4;
    else if(b == 8) k = 3;
    else if(b == 2) k = 1;
    else if(b == 32) k = 5;
    else if(b == 4) k = 2;
    else return this.toRadix(b);
    var km = (1<<k)-1, d, m = false, r = "", i = this.t;
    var p = this.DB-(i*this.DB)%k;
    if(i-- > 0) {
      if(p < this.DB && (d = this[i]>>p) > 0) { m = true; r = int2char(d); }
      while(i >= 0) {
        if(p < k) {
          d = (this[i]&((1<<p)-1))<<(k-p);
          d |= this[--i]>>(p+=this.DB-k);
        }
        else {
          d = (this[i]>>(p-=k))&km;
          if(p <= 0) { p += this.DB; --i; }
        }
        if(d > 0) m = true;
        if(m) r += int2char(d);
      }
    }
    return m?r:"0";
  }
  // (public) -this
  function bnNegate() { var r = nbi(); BigInteger.ZERO.subTo(this,r); return r; }
  // (public) |this|
  function bnAbs() { return (this.s<0)?this.negate():this; }
  // (public) return + if this > a, - if this < a, 0 if equal
  function bnCompareTo(a) {
    var r = this.s-a.s;
    if(r != 0) return r;
    var i = this.t;
    r = i-a.t;
    if(r != 0) return (this.s<0)?-r:r;
    while(--i >= 0) if((r=this[i]-a[i]) != 0) return r;
    return 0;
  }
  // returns bit length of the integer x
  function nbits(x) {
    var r = 1, t;
    if((t=x>>>16) != 0) { x = t; r += 16; }
    if((t=x>>8) != 0) { x = t; r += 8; }
    if((t=x>>4) != 0) { x = t; r += 4; }
    if((t=x>>2) != 0) { x = t; r += 2; }
    if((t=x>>1) != 0) { x = t; r += 1; }
    return r;
  }
  // (public) return the number of bits in "this"
  function bnBitLength() {
    if(this.t <= 0) return 0;
    return this.DB*(this.t-1)+nbits(this[this.t-1]^(this.s&this.DM));
  }
  // (protected) r = this << n*DB
  function bnpDLShiftTo(n,r) {
    var i;
    for(i = this.t-1; i >= 0; --i) r[i+n] = this[i];
    for(i = n-1; i >= 0; --i) r[i] = 0;
    r.t = this.t+n;
    r.s = this.s;
  }
  // (protected) r = this >> n*DB
  function bnpDRShiftTo(n,r) {
    for(var i = n; i < this.t; ++i) r[i-n] = this[i];
    r.t = Math.max(this.t-n,0);
    r.s = this.s;
  }
  // (protected) r = this << n
  function bnpLShiftTo(n,r) {
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<cbs)-1;
    var ds = Math.floor(n/this.DB), c = (this.s<<bs)&this.DM, i;
    for(i = this.t-1; i >= 0; --i) {
      r[i+ds+1] = (this[i]>>cbs)|c;
      c = (this[i]&bm)<<bs;
    }
    for(i = ds-1; i >= 0; --i) r[i] = 0;
    r[ds] = c;
    r.t = this.t+ds+1;
    r.s = this.s;
    r.clamp();
  }
  // (protected) r = this >> n
  function bnpRShiftTo(n,r) {
    r.s = this.s;
    var ds = Math.floor(n/this.DB);
    if(ds >= this.t) { r.t = 0; return; }
    var bs = n%this.DB;
    var cbs = this.DB-bs;
    var bm = (1<<bs)-1;
    r[0] = this[ds]>>bs;
    for(var i = ds+1; i < this.t; ++i) {
      r[i-ds-1] |= (this[i]&bm)<<cbs;
      r[i-ds] = this[i]>>bs;
    }
    if(bs > 0) r[this.t-ds-1] |= (this.s&bm)<<cbs;
    r.t = this.t-ds;
    r.clamp();
  }
  // (protected) r = this - a
  function bnpSubTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]-a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c -= a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c -= a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c -= a.s;
    }
    r.s = (c<0)?-1:0;
    if(c < -1) r[i++] = this.DV+c;
    else if(c > 0) r[i++] = c;
    r.t = i;
    r.clamp();
  }
  // (protected) r = this * a, r != this,a (HAC 14.12)
  // "this" should be the larger one if appropriate.
  function bnpMultiplyTo(a,r) {
    var x = this.abs(), y = a.abs();
    var i = x.t;
    r.t = i+y.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < y.t; ++i) r[i+x.t] = x.am(0,y[i],r,i,0,x.t);
    r.s = 0;
    r.clamp();
    if(this.s != a.s) BigInteger.ZERO.subTo(r,r);
  }
  // (protected) r = this^2, r != this (HAC 14.16)
  function bnpSquareTo(r) {
    var x = this.abs();
    var i = r.t = 2*x.t;
    while(--i >= 0) r[i] = 0;
    for(i = 0; i < x.t-1; ++i) {
      var c = x.am(i,x[i],r,2*i,0,1);
      if((r[i+x.t]+=x.am(i+1,2*x[i],r,2*i+1,c,x.t-i-1)) >= x.DV) {
        r[i+x.t] -= x.DV;
        r[i+x.t+1] = 1;
      }
    }
    if(r.t > 0) r[r.t-1] += x.am(i,x[i],r,2*i,0,1);
    r.s = 0;
    r.clamp();
  }
  // (protected) divide this by m, quotient and remainder to q, r (HAC 14.20)
  // r != q, this != m.  q or r may be null.
  function bnpDivRemTo(m,q,r) {
    var pm = m.abs();
    if(pm.t <= 0) return;
    var pt = this.abs();
    if(pt.t < pm.t) {
      if(q != null) q.fromInt(0);
      if(r != null) this.copyTo(r);
      return;
    }
    if(r == null) r = nbi();
    var y = nbi(), ts = this.s, ms = m.s;
    var nsh = this.DB-nbits(pm[pm.t-1]);	// normalize modulus
    if(nsh > 0) { pm.lShiftTo(nsh,y); pt.lShiftTo(nsh,r); }
    else { pm.copyTo(y); pt.copyTo(r); }
    var ys = y.t;
    var y0 = y[ys-1];
    if(y0 == 0) return;
    var yt = y0*(1<<this.F1)+((ys>1)?y[ys-2]>>this.F2:0);
    var d1 = this.FV/yt, d2 = (1<<this.F1)/yt, e = 1<<this.F2;
    var i = r.t, j = i-ys, t = (q==null)?nbi():q;
    y.dlShiftTo(j,t);
    if(r.compareTo(t) >= 0) {
      r[r.t++] = 1;
      r.subTo(t,r);
    }
    BigInteger.ONE.dlShiftTo(ys,t);
    t.subTo(y,y);	// "negative" y so we can replace sub with am later
    while(y.t < ys) y[y.t++] = 0;
    while(--j >= 0) {
      // Estimate quotient digit
      var qd = (r[--i]==y0)?this.DM:Math.floor(r[i]*d1+(r[i-1]+e)*d2);
      if((r[i]+=y.am(0,qd,r,j,0,ys)) < qd) {	// Try it out
        y.dlShiftTo(j,t);
        r.subTo(t,r);
        while(r[i] < --qd) r.subTo(t,r);
      }
    }
    if(q != null) {
      r.drShiftTo(ys,q);
      if(ts != ms) BigInteger.ZERO.subTo(q,q);
    }
    r.t = ys;
    r.clamp();
    if(nsh > 0) r.rShiftTo(nsh,r);	// Denormalize remainder
    if(ts < 0) BigInteger.ZERO.subTo(r,r);
  }
  // (public) this mod a
  function bnMod(a) {
    var r = nbi();
    this.abs().divRemTo(a,null,r);
    if(this.s < 0 && r.compareTo(BigInteger.ZERO) > 0) a.subTo(r,r);
    return r;
  }
  // Modular reduction using "classic" algorithm
  function Classic(m) { this.m = m; }
  function cConvert(x) {
    if(x.s < 0 || x.compareTo(this.m) >= 0) return x.mod(this.m);
    else return x;
  }
  function cRevert(x) { return x; }
  function cReduce(x) { x.divRemTo(this.m,null,x); }
  function cMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
  function cSqrTo(x,r) { x.squareTo(r); this.reduce(r); }
  Classic.prototype.convert = cConvert;
  Classic.prototype.revert = cRevert;
  Classic.prototype.reduce = cReduce;
  Classic.prototype.mulTo = cMulTo;
  Classic.prototype.sqrTo = cSqrTo;
  // (protected) return "-1/this % 2^DB"; useful for Mont. reduction
  // justification:
  //         xy == 1 (mod m)
  //         xy =  1+km
  //   xy(2-xy) = (1+km)(1-km)
  // x[y(2-xy)] = 1-k^2m^2
  // x[y(2-xy)] == 1 (mod m^2)
  // if y is 1/x mod m, then y(2-xy) is 1/x mod m^2
  // should reduce x and y(2-xy) by m^2 at each step to keep size bounded.
  // JS multiply "overflows" differently from C/C++, so care is needed here.
  function bnpInvDigit() {
    if(this.t < 1) return 0;
    var x = this[0];
    if((x&1) == 0) return 0;
    var y = x&3;		// y == 1/x mod 2^2
    y = (y*(2-(x&0xf)*y))&0xf;	// y == 1/x mod 2^4
    y = (y*(2-(x&0xff)*y))&0xff;	// y == 1/x mod 2^8
    y = (y*(2-(((x&0xffff)*y)&0xffff)))&0xffff;	// y == 1/x mod 2^16
    // last step - calculate inverse mod DV directly;
    // assumes 16 < DB <= 32 and assumes ability to handle 48-bit ints
    y = (y*(2-x*y%this.DV))%this.DV;		// y == 1/x mod 2^dbits
    // we really want the negative inverse, and -DV < y < DV
    return (y>0)?this.DV-y:-y;
  }
  // Montgomery reduction
  function Montgomery(m) {
    this.m = m;
    this.mp = m.invDigit();
    this.mpl = this.mp&0x7fff;
    this.mph = this.mp>>15;
    this.um = (1<<(m.DB-15))-1;
    this.mt2 = 2*m.t;
  }
  // xR mod m
  function montConvert(x) {
    var r = nbi();
    x.abs().dlShiftTo(this.m.t,r);
    r.divRemTo(this.m,null,r);
    if(x.s < 0 && r.compareTo(BigInteger.ZERO) > 0) this.m.subTo(r,r);
    return r;
  }
  // x/R mod m
  function montRevert(x) {
    var r = nbi();
    x.copyTo(r);
    this.reduce(r);
    return r;
  }
  // x = x/R mod m (HAC 14.32)
  function montReduce(x) {
    while(x.t <= this.mt2)	// pad x so am has enough room later
      x[x.t++] = 0;
    for(var i = 0; i < this.m.t; ++i) {
      // faster way of calculating u0 = x[i]*mp mod DV
      var j = x[i]&0x7fff;
      var u0 = (j*this.mpl+(((j*this.mph+(x[i]>>15)*this.mpl)&this.um)<<15))&x.DM;
      // use am to combine the multiply-shift-add into one call
      j = i+this.m.t;
      x[j] += this.m.am(0,u0,x,i,0,this.m.t);
      // propagate carry
      while(x[j] >= x.DV) { x[j] -= x.DV; x[++j]++; }
    }
    x.clamp();
    x.drShiftTo(this.m.t,x);
    if(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
  }
  // r = "x^2/R mod m"; x != r
  function montSqrTo(x,r) { x.squareTo(r); this.reduce(r); }
  // r = "xy/R mod m"; x,y != r
  function montMulTo(x,y,r) { x.multiplyTo(y,r); this.reduce(r); }
  Montgomery.prototype.convert = montConvert;
  Montgomery.prototype.revert = montRevert;
  Montgomery.prototype.reduce = montReduce;
  Montgomery.prototype.mulTo = montMulTo;
  Montgomery.prototype.sqrTo = montSqrTo;
  // (protected) true iff this is even
  function bnpIsEven() { return ((this.t>0)?(this[0]&1):this.s) == 0; }
  // (protected) this^e, e < 2^32, doing sqr and mul with "r" (HAC 14.79)
  function bnpExp(e,z) {
    if(e > 0xffffffff || e < 1) return BigInteger.ONE;
    var r = nbi(), r2 = nbi(), g = z.convert(this), i = nbits(e)-1;
    g.copyTo(r);
    while(--i >= 0) {
      z.sqrTo(r,r2);
      if((e&(1<<i)) > 0) z.mulTo(r2,g,r);
      else { var t = r; r = r2; r2 = t; }
    }
    return z.revert(r);
  }
  // (public) this^e % m, 0 <= e < 2^32
  function bnModPowInt(e,m) {
    var z;
    if(e < 256 || m.isEven()) z = new Classic(m); else z = new Montgomery(m);
    return this.exp(e,z);
  }
  // protected
  BigInteger.prototype.copyTo = bnpCopyTo;
  BigInteger.prototype.fromInt = bnpFromInt;
  BigInteger.prototype.fromString = bnpFromString;
  BigInteger.prototype.clamp = bnpClamp;
  BigInteger.prototype.dlShiftTo = bnpDLShiftTo;
  BigInteger.prototype.drShiftTo = bnpDRShiftTo;
  BigInteger.prototype.lShiftTo = bnpLShiftTo;
  BigInteger.prototype.rShiftTo = bnpRShiftTo;
  BigInteger.prototype.subTo = bnpSubTo;
  BigInteger.prototype.multiplyTo = bnpMultiplyTo;
  BigInteger.prototype.squareTo = bnpSquareTo;
  BigInteger.prototype.divRemTo = bnpDivRemTo;
  BigInteger.prototype.invDigit = bnpInvDigit;
  BigInteger.prototype.isEven = bnpIsEven;
  BigInteger.prototype.exp = bnpExp;
  // public
  BigInteger.prototype.toString = bnToString;
  BigInteger.prototype.negate = bnNegate;
  BigInteger.prototype.abs = bnAbs;
  BigInteger.prototype.compareTo = bnCompareTo;
  BigInteger.prototype.bitLength = bnBitLength;
  BigInteger.prototype.mod = bnMod;
  BigInteger.prototype.modPowInt = bnModPowInt;
  // "constants"
  BigInteger.ZERO = nbv(0);
  BigInteger.ONE = nbv(1);
  // jsbn2 stuff
  // (protected) convert from radix string
  function bnpFromRadix(s,b) {
    this.fromInt(0);
    if(b == null) b = 10;
    var cs = this.chunkSize(b);
    var d = Math.pow(b,cs), mi = false, j = 0, w = 0;
    for(var i = 0; i < s.length; ++i) {
      var x = intAt(s,i);
      if(x < 0) {
        if(s.charAt(i) == "-" && this.signum() == 0) mi = true;
        continue;
      }
      w = b*w+x;
      if(++j >= cs) {
        this.dMultiply(d);
        this.dAddOffset(w,0);
        j = 0;
        w = 0;
      }
    }
    if(j > 0) {
      this.dMultiply(Math.pow(b,j));
      this.dAddOffset(w,0);
    }
    if(mi) BigInteger.ZERO.subTo(this,this);
  }
  // (protected) return x s.t. r^x < DV
  function bnpChunkSize(r) { return Math.floor(Math.LN2*this.DB/Math.log(r)); }
  // (public) 0 if this == 0, 1 if this > 0
  function bnSigNum() {
    if(this.s < 0) return -1;
    else if(this.t <= 0 || (this.t == 1 && this[0] <= 0)) return 0;
    else return 1;
  }
  // (protected) this *= n, this >= 0, 1 < n < DV
  function bnpDMultiply(n) {
    this[this.t] = this.am(0,n-1,this,0,0,this.t);
    ++this.t;
    this.clamp();
  }
  // (protected) this += n << w words, this >= 0
  function bnpDAddOffset(n,w) {
    if(n == 0) return;
    while(this.t <= w) this[this.t++] = 0;
    this[w] += n;
    while(this[w] >= this.DV) {
      this[w] -= this.DV;
      if(++w >= this.t) this[this.t++] = 0;
      ++this[w];
    }
  }
  // (protected) convert to radix string
  function bnpToRadix(b) {
    if(b == null) b = 10;
    if(this.signum() == 0 || b < 2 || b > 36) return "0";
    var cs = this.chunkSize(b);
    var a = Math.pow(b,cs);
    var d = nbv(a), y = nbi(), z = nbi(), r = "";
    this.divRemTo(d,y,z);
    while(y.signum() > 0) {
      r = (a+z.intValue()).toString(b).substr(1) + r;
      y.divRemTo(d,y,z);
    }
    return z.intValue().toString(b) + r;
  }
  // (public) return value as integer
  function bnIntValue() {
    if(this.s < 0) {
      if(this.t == 1) return this[0]-this.DV;
      else if(this.t == 0) return -1;
    }
    else if(this.t == 1) return this[0];
    else if(this.t == 0) return 0;
    // assumes 16 < DB < 32
    return ((this[1]&((1<<(32-this.DB))-1))<<this.DB)|this[0];
  }
  // (protected) r = this + a
  function bnpAddTo(a,r) {
    var i = 0, c = 0, m = Math.min(a.t,this.t);
    while(i < m) {
      c += this[i]+a[i];
      r[i++] = c&this.DM;
      c >>= this.DB;
    }
    if(a.t < this.t) {
      c += a.s;
      while(i < this.t) {
        c += this[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += this.s;
    }
    else {
      c += this.s;
      while(i < a.t) {
        c += a[i];
        r[i++] = c&this.DM;
        c >>= this.DB;
      }
      c += a.s;
    }
    r.s = (c<0)?-1:0;
    if(c > 0) r[i++] = c;
    else if(c < -1) r[i++] = this.DV+c;
    r.t = i;
    r.clamp();
  }
  BigInteger.prototype.fromRadix = bnpFromRadix;
  BigInteger.prototype.chunkSize = bnpChunkSize;
  BigInteger.prototype.signum = bnSigNum;
  BigInteger.prototype.dMultiply = bnpDMultiply;
  BigInteger.prototype.dAddOffset = bnpDAddOffset;
  BigInteger.prototype.toRadix = bnpToRadix;
  BigInteger.prototype.intValue = bnIntValue;
  BigInteger.prototype.addTo = bnpAddTo;
  //======= end jsbn =======
  // Emscripten wrapper
  var Wrapper = {
    abs: function(l, h) {
      var x = new goog.math.Long(l, h);
      var ret;
      if (x.isNegative()) {
        ret = x.negate();
      } else {
        ret = x;
      }
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
    },
    ensureTemps: function() {
      if (Wrapper.ensuredTemps) return;
      Wrapper.ensuredTemps = true;
      Wrapper.two32 = new BigInteger();
      Wrapper.two32.fromString('4294967296', 10);
      Wrapper.two64 = new BigInteger();
      Wrapper.two64.fromString('18446744073709551616', 10);
      Wrapper.temp1 = new BigInteger();
      Wrapper.temp2 = new BigInteger();
    },
    lh2bignum: function(l, h) {
      var a = new BigInteger();
      a.fromString(h.toString(), 10);
      var b = new BigInteger();
      a.multiplyTo(Wrapper.two32, b);
      var c = new BigInteger();
      c.fromString(l.toString(), 10);
      var d = new BigInteger();
      c.addTo(b, d);
      return d;
    },
    stringify: function(l, h, unsigned) {
      var ret = new goog.math.Long(l, h).toString();
      if (unsigned && ret[0] == '-') {
        // unsign slowly using jsbn bignums
        Wrapper.ensureTemps();
        var bignum = new BigInteger();
        bignum.fromString(ret, 10);
        ret = new BigInteger();
        Wrapper.two64.addTo(bignum, ret);
        ret = ret.toString(10);
      }
      return ret;
    },
    fromString: function(str, base, min, max, unsigned) {
      Wrapper.ensureTemps();
      var bignum = new BigInteger();
      bignum.fromString(str, base);
      var bigmin = new BigInteger();
      bigmin.fromString(min, 10);
      var bigmax = new BigInteger();
      bigmax.fromString(max, 10);
      if (unsigned && bignum.compareTo(BigInteger.ZERO) < 0) {
        var temp = new BigInteger();
        bignum.addTo(Wrapper.two64, temp);
        bignum = temp;
      }
      var error = false;
      if (bignum.compareTo(bigmin) < 0) {
        bignum = bigmin;
        error = true;
      } else if (bignum.compareTo(bigmax) > 0) {
        bignum = bigmax;
        error = true;
      }
      var ret = goog.math.Long.fromString(bignum.toString()); // min-max checks should have clamped this to a range goog.math.Long can handle well
      HEAP32[tempDoublePtr>>2] = ret.low_;
      HEAP32[tempDoublePtr+4>>2] = ret.high_;
      if (error) throw 'range error';
    }
  };
  return Wrapper;
})();
//======= end closure i64 code =======
// === Auto-generated postamble setup entry stuff ===
if (memoryInitializer) {
  function applyData(data) {
    HEAPU8.set(data, STATIC_BASE);
  }
  if (ENVIRONMENT_IS_NODE || ENVIRONMENT_IS_SHELL) {
    applyData(Module['readBinary'](memoryInitializer));
  } else {
    addRunDependency('memory initializer');
    Browser.asyncLoad(memoryInitializer, function(data) {
      applyData(data);
      removeRunDependency('memory initializer');
    }, function(data) {
      throw 'could not load memory initializer ' + memoryInitializer;
    });
  }
}
function ExitStatus(status) {
  this.name = "ExitStatus";
  this.message = "Program terminated with exit(" + status + ")";
  this.status = status;
};
ExitStatus.prototype = new Error();
ExitStatus.prototype.constructor = ExitStatus;
var initialStackTop;
var preloadStartTime = null;
var calledMain = false;
dependenciesFulfilled = function runCaller() {
  // If run has never been called, and we should call run (INVOKE_RUN is true, and Module.noInitialRun is not false)
  if (!Module['calledRun'] && shouldRunNow) run();
  if (!Module['calledRun']) dependenciesFulfilled = runCaller; // try this again later, after new deps are fulfilled
}
Module['callMain'] = Module.callMain = function callMain(args) {
  assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on __ATMAIN__)');
  assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');
  args = args || [];
  if (ENVIRONMENT_IS_WEB && preloadStartTime !== null) {
    Module.printErr('preload time: ' + (Date.now() - preloadStartTime) + ' ms');
  }
  ensureInitRuntime();
  var argc = args.length+1;
  function pad() {
    for (var i = 0; i < 4-1; i++) {
      argv.push(0);
    }
  }
  var argv = [allocate(intArrayFromString("/bin/this.program"), 'i8', ALLOC_NORMAL) ];
  pad();
  for (var i = 0; i < argc-1; i = i + 1) {
    argv.push(allocate(intArrayFromString(args[i]), 'i8', ALLOC_NORMAL));
    pad();
  }
  argv.push(0);
  argv = allocate(argv, 'i32', ALLOC_NORMAL);
  initialStackTop = STACKTOP;
  try {
    var ret = Module['_main'](argc, argv, 0);
    // if we're not running an evented main loop, it's time to exit
    if (!Module['noExitRuntime']) {
      exit(ret);
    }
  }
  catch(e) {
    if (e instanceof ExitStatus) {
      // exit() throws this once it's done to make sure execution
      // has been stopped completely
      return;
    } else if (e == 'SimulateInfiniteLoop') {
      // running an evented main loop, don't immediately exit
      Module['noExitRuntime'] = true;
      return;
    } else {
      if (e && typeof e === 'object' && e.stack) Module.printErr('exception thrown: ' + [e, e.stack]);
      throw e;
    }
  } finally {
    calledMain = true;
  }
}
function run(args) {
  args = args || Module['arguments'];
  if (preloadStartTime === null) preloadStartTime = Date.now();
  if (runDependencies > 0) {
    Module.printErr('run() called, but dependencies remain, so not running');
    return;
  }
  preRun();
  if (runDependencies > 0) {
    // a preRun added a dependency, run will be called later
    return;
  }
  function doRun() {
    ensureInitRuntime();
    preMain();
    Module['calledRun'] = true;
    if (Module['_main'] && shouldRunNow) {
      Module['callMain'](args);
    }
    postRun();
  }
  if (Module['setStatus']) {
    Module['setStatus']('Running...');
    setTimeout(function() {
      setTimeout(function() {
        Module['setStatus']('');
      }, 1);
      if (!ABORT) doRun();
    }, 1);
  } else {
    doRun();
  }
}
Module['run'] = Module.run = run;
function exit(status) {
  ABORT = true;
  EXITSTATUS = status;
  STACKTOP = initialStackTop;
  // exit the runtime
  exitRuntime();
  // TODO We should handle this differently based on environment.
  // In the browser, the best we can do is throw an exception
  // to halt execution, but in node we could process.exit and
  // I'd imagine SM shell would have something equivalent.
  // This would let us set a proper exit status (which
  // would be great for checking test exit statuses).
  // https://github.com/kripken/emscripten/issues/1371
  // throw an exception to halt the current execution
  throw new ExitStatus(status);
}
Module['exit'] = Module.exit = exit;
function abort(text) {
  if (text) {
    Module.print(text);
    Module.printErr(text);
  }
  ABORT = true;
  EXITSTATUS = 1;
  throw 'abort() at ' + stackTrace();
}
Module['abort'] = Module.abort = abort;
// {{PRE_RUN_ADDITIONS}}
/*global Module*/
/*global _malloc, _free, _memcpy*/
/*global FUNCTION_TABLE, HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32*/
/*global readLatin1String*/
/*global __emval_register, _emval_handle_array, __emval_decref*/
/*global ___getTypeName*/
/*jslint sub:true*/ /* The symbols 'fromWireType' and 'toWireType' must be accessed via array notation to be closure-safe since craftInvokerFunction crafts functions as strings that can't be closured. */
var InternalError = Module['InternalError'] = extendError(Error, 'InternalError');
var BindingError = Module['BindingError'] = extendError(Error, 'BindingError');
var UnboundTypeError = Module['UnboundTypeError'] = extendError(BindingError, 'UnboundTypeError');
function throwInternalError(message) {
    throw new InternalError(message);
}
function throwBindingError(message) {
    throw new BindingError(message);
}
function throwUnboundTypeError(message, types) {
    var unboundTypes = [];
    var seen = {};
    function visit(type) {
        if (seen[type]) {
            return;
        }
        if (registeredTypes[type]) {
            return;
        }
        if (typeDependencies[type]) {
            typeDependencies[type].forEach(visit);
            return;
        }
        unboundTypes.push(type);
        seen[type] = true;
    }
    types.forEach(visit);
    throw new UnboundTypeError(message + ': ' + unboundTypes.map(getTypeName).join([', ']));
}
// Creates a function overload resolution table to the given method 'methodName' in the given prototype,
// if the overload table doesn't yet exist.
function ensureOverloadTable(proto, methodName, humanName) {
    if (undefined === proto[methodName].overloadTable) {
        var prevFunc = proto[methodName];
        // Inject an overload resolver function that routes to the appropriate overload based on the number of arguments.
        proto[methodName] = function() {
            // TODO This check can be removed in -O3 level "unsafe" optimizations.
            if (!proto[methodName].overloadTable.hasOwnProperty(arguments.length)) {
                throwBindingError("Function '" + humanName + "' called with an invalid number of arguments (" + arguments.length + ") - expects one of (" + proto[methodName].overloadTable + ")!");
            }
            return proto[methodName].overloadTable[arguments.length].apply(this, arguments);
        };
        // Move the previous function into the overload table.
        proto[methodName].overloadTable = [];
        proto[methodName].overloadTable[prevFunc.argCount] = prevFunc;
    }            
}
/* Registers a symbol (function, class, enum, ...) as part of the Module JS object so that
   hand-written code is able to access that symbol via 'Module.name'.
   name: The name of the symbol that's being exposed.
   value: The object itself to expose (function, class, ...)
   numArguments: For functions, specifies the number of arguments the function takes in. For other types, unused and undefined.
   To implement support for multiple overloads of a function, an 'overload selector' function is used. That selector function chooses
   the appropriate overload to call from an function overload table. This selector function is only used if multiple overloads are
   actually registered, since it carries a slight performance penalty. */
function exposePublicSymbol(name, value, numArguments) {
    if (Module.hasOwnProperty(name)) {
        if (undefined === numArguments || (undefined !== Module[name].overloadTable && undefined !== Module[name].overloadTable[numArguments])) {
            throwBindingError("Cannot register public name '" + name + "' twice");
        }
        // We are exposing a function with the same name as an existing function. Create an overload table and a function selector
        // that routes between the two.
        ensureOverloadTable(Module, name, name);
        if (Module.hasOwnProperty(numArguments)) {
            throwBindingError("Cannot register multiple overloads of a function with the same number of arguments (" + numArguments + ")!");
        }
        // Add the new function into the overload table.
        Module[name].overloadTable[numArguments] = value;
    }
    else {
        Module[name] = value;
        if (undefined !== numArguments) {
            Module[name].numArguments = numArguments;
        }
    }
}
function replacePublicSymbol(name, value, numArguments) {
    if (!Module.hasOwnProperty(name)) {
        throwInternalError('Replacing nonexistant public symbol');
    }
    // If there's an overload table for this symbol, replace the symbol in the overload table instead.
    if (undefined !== Module[name].overloadTable && undefined !== numArguments) {
        Module[name].overloadTable[numArguments] = value;
    }
    else {
        Module[name] = value;
    }
}
// from https://github.com/imvu/imvujs/blob/master/src/error.js
function extendError(baseErrorType, errorName) {
    var errorClass = createNamedFunction(errorName, function(message) {
        this.name = errorName;
        this.message = message;
        var stack = (new Error(message)).stack;
        if (stack !== undefined) {
            this.stack = this.toString() + '\n' +
                stack.replace(/^Error(:[^\n]*)?\n/, '');
        }
    });
    errorClass.prototype = Object.create(baseErrorType.prototype);
    errorClass.prototype.constructor = errorClass;
    errorClass.prototype.toString = function() {
        if (this.message === undefined) {
            return this.name;
        } else {
            return this.name + ': ' + this.message;
        }
    };
    return errorClass;
}
// from https://github.com/imvu/imvujs/blob/master/src/function.js
function createNamedFunction(name, body) {
    name = makeLegalFunctionName(name);
    /*jshint evil:true*/
    return new Function(
        "body",
        "return function " + name + "() {\n" +
        "    \"use strict\";" +
        "    return body.apply(this, arguments);\n" +
        "};\n"
    )(body);
}
function _embind_repr(v) {
    var t = typeof v;
    if (t === 'object' || t === 'array' || t === 'function') {
        return v.toString();
    } else {
        return '' + v;
    }
}
// typeID -> { toWireType: ..., fromWireType: ... }
var registeredTypes = {};
// typeID -> [callback]
var awaitingDependencies = {};
// typeID -> [dependentTypes]
var typeDependencies = {};
// class typeID -> {pointerType: ..., constPointerType: ...}
var registeredPointers = {};
function registerType(rawType, registeredInstance) {
    var name = registeredInstance.name;
    if (!rawType) {
        throwBindingError('type "' + name + '" must have a positive integer typeid pointer');
    }
    if (registeredTypes.hasOwnProperty(rawType)) {
        throwBindingError("Cannot register type '" + name + "' twice");
    }
    registeredTypes[rawType] = registeredInstance;
    delete typeDependencies[rawType];
    if (awaitingDependencies.hasOwnProperty(rawType)) {
        var callbacks = awaitingDependencies[rawType];
        delete awaitingDependencies[rawType];
        callbacks.forEach(function(cb) {
            cb();
        });
    }
}
function whenDependentTypesAreResolved(myTypes, dependentTypes, getTypeConverters) {
    myTypes.forEach(function(type) {
        typeDependencies[type] = dependentTypes;
    });
    function onComplete(typeConverters) {
        var myTypeConverters = getTypeConverters(typeConverters);
        if (myTypeConverters.length !== myTypes.length) {
            throwInternalError('Mismatched type converter count');
        }
        for (var i = 0; i < myTypes.length; ++i) {
            registerType(myTypes[i], myTypeConverters[i]);
        }
    }
    var typeConverters = new Array(dependentTypes.length);
    var unregisteredTypes = [];
    var registered = 0;
    dependentTypes.forEach(function(dt, i) {
        if (registeredTypes.hasOwnProperty(dt)) {
            typeConverters[i] = registeredTypes[dt];
        } else {
            unregisteredTypes.push(dt);
            if (!awaitingDependencies.hasOwnProperty(dt)) {
                awaitingDependencies[dt] = [];
            }
            awaitingDependencies[dt].push(function() {
                typeConverters[i] = registeredTypes[dt];
                ++registered;
                if (registered === unregisteredTypes.length) {
                    onComplete(typeConverters);
                }
            });
        }
    });
    if (0 === unregisteredTypes.length) {
        onComplete(typeConverters);
    }
}
var __charCodes = (function() {
    var codes = new Array(256);
    for (var i = 0; i < 256; ++i) {
        codes[i] = String.fromCharCode(i);
    }
    return codes;
})();
function readLatin1String(ptr) {
    var ret = "";
    var c = ptr;
    while (HEAPU8[c]) {
        ret += __charCodes[HEAPU8[c++]];
    }
    return ret;
}
function getTypeName(type) {
    var ptr = ___getTypeName(type);
    var rv = readLatin1String(ptr);
    _free(ptr);
    return rv;
}
function heap32VectorToArray(count, firstElement) {
    var array = [];
    for (var i = 0; i < count; i++) {
        array.push(HEAP32[(firstElement >> 2) + i]);
    }
    return array;
}
function requireRegisteredType(rawType, humanName) {
    var impl = registeredTypes[rawType];
    if (undefined === impl) {
        throwBindingError(humanName + " has unknown type " + getTypeName(rawType));
    }
    return impl;
}
function __embind_register_void(rawType, name) {
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function() {
            return undefined;
        },
        'toWireType': function(destructors, o) {
            // TODO: assert if anything else is given?
            return undefined;
        },
    });
}
function __embind_register_bool(rawType, name, trueValue, falseValue) {
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function(wt) {
            // ambiguous emscripten ABI: sometimes return values are
            // true or false, and sometimes integers (0 or 1)
            return !!wt;
        },
        'toWireType': function(destructors, o) {
            return o ? trueValue : falseValue;
        },
        destructorFunction: null, // This type does not need a destructor
    });
}
// When converting a number from JS to C++ side, the valid range of the number is
// [minRange, maxRange], inclusive.
function __embind_register_integer(primitiveType, name, minRange, maxRange) {
    name = readLatin1String(name);
    if (maxRange === -1) { // LLVM doesn't have signed and unsigned 32-bit types, so u32 literals come out as 'i32 -1'. Always treat those as max u32.
        maxRange = 4294967295;
    }
    registerType(primitiveType, {
        name: name,
        minRange: minRange,
        maxRange: maxRange,
        'fromWireType': function(value) {
            return value;
        },
        'toWireType': function(destructors, value) {
            // todo: Here we have an opportunity for -O3 level "unsafe" optimizations: we could
            // avoid the following two if()s and assume value is of proper type.
            if (typeof value !== "number" && typeof value !== "boolean") {
                throw new TypeError('Cannot convert "' + _embind_repr(value) + '" to ' + this.name);
            }
            if (value < minRange || value > maxRange) {
                throw new TypeError('Passing a number "' + _embind_repr(value) + '" from JS side to C/C++ side to an argument of type "' + name + '", which is outside the valid range [' + minRange + ', ' + maxRange + ']!');
            }
            return value | 0;
        },
        destructorFunction: null, // This type does not need a destructor
    });
}
function __embind_register_float(rawType, name) {
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function(value) {
            return value;
        },
        'toWireType': function(destructors, value) {
            // todo: Here we have an opportunity for -O3 level "unsafe" optimizations: we could
            // avoid the following if() and assume value is of proper type.
            if (typeof value !== "number" && typeof value !== "boolean") {
                throw new TypeError('Cannot convert "' + _embind_repr(value) + '" to ' + this.name);
            }
            return value;
        },
        destructorFunction: null, // This type does not need a destructor
    });
}
function __embind_register_std_string(rawType, name) {
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function(value) {
            var length = HEAPU32[value >> 2];
            var a = new Array(length);
            for (var i = 0; i < length; ++i) {
                a[i] = String.fromCharCode(HEAPU8[value + 4 + i]);
            }
            _free(value);
            return a.join('');
        },
        'toWireType': function(destructors, value) {
            if (value instanceof ArrayBuffer) {
                value = new Uint8Array(value);
            }
            function getTAElement(ta, index) {
                return ta[index];
            }
            function getStringElement(string, index) {
                return string.charCodeAt(index);
            }
            var getElement;
            if (value instanceof Uint8Array) {
                getElement = getTAElement;
            } else if (value instanceof Int8Array) {
                getElement = getTAElement;
            } else if (typeof value === 'string') {
                getElement = getStringElement;
            } else {
                throwBindingError('Cannot pass non-string to std::string');
            }
            // assumes 4-byte alignment
            var length = value.length;
            var ptr = _malloc(4 + length);
            HEAPU32[ptr >> 2] = length;
            for (var i = 0; i < length; ++i) {
                var charCode = getElement(value, i);
                if (charCode > 255) {
                    _free(ptr);
                    throwBindingError('String has UTF-16 code units that do not fit in 8 bits');
                }
                HEAPU8[ptr + 4 + i] = charCode;
            }
            if (destructors !== null) {
                destructors.push(_free, ptr);
            }
            return ptr;
        },
        destructorFunction: function(ptr) { _free(ptr); },
    });
}
function __embind_register_std_wstring(rawType, charSize, name) {
    name = readLatin1String(name);
    var HEAP, shift;
    if (charSize === 2) {
        HEAP = HEAPU16;
        shift = 1;
    } else if (charSize === 4) {
        HEAP = HEAPU32;
        shift = 2;
    }
    registerType(rawType, {
        name: name,
        'fromWireType': function(value) {
            var length = HEAPU32[value >> 2];
            var a = new Array(length);
            var start = (value + 4) >> shift;
            for (var i = 0; i < length; ++i) {
                a[i] = String.fromCharCode(HEAP[start + i]);
            }
            _free(value);
            return a.join('');
        },
        'toWireType': function(destructors, value) {
            // assumes 4-byte alignment
            var length = value.length;
            var ptr = _malloc(4 + length * charSize);
            HEAPU32[ptr >> 2] = length;
            var start = (ptr + 4) >> shift;
            for (var i = 0; i < length; ++i) {
                HEAP[start + i] = value.charCodeAt(i);
            }
            if (destructors !== null) {
                destructors.push(_free, ptr);
            }
            return ptr;
        },
        destructorFunction: function(ptr) { _free(ptr); },
    });
}
function __embind_register_emval(rawType, name) {
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function(handle) {
            var rv = _emval_handle_array[handle].value;
            __emval_decref(handle);
            return rv;
        },
        'toWireType': function(destructors, value) {
            return __emval_register(value);
        },
        destructorFunction: null, // This type does not need a destructor
    });
}
function __embind_register_memory_view(rawType, name) {
    var typeMapping = [
        Int8Array,
        Uint8Array,
        Int16Array,
        Uint16Array,
        Int32Array,
        Uint32Array,
        Float32Array,
        Float64Array,        
    ];
    name = readLatin1String(name);
    registerType(rawType, {
        name: name,
        'fromWireType': function(handle) {
            var type = HEAPU32[handle >> 2];
            var size = HEAPU32[(handle >> 2) + 1]; // in elements
            var data = HEAPU32[(handle >> 2) + 2]; // byte offset into emscripten heap
            var TA = typeMapping[type];
            return new TA(HEAP8.buffer, data, size);
        },
    });
}
function runDestructors(destructors) {
    while (destructors.length) {
        var ptr = destructors.pop();
        var del = destructors.pop();
        del(ptr);
    }
}
// Function implementation of operator new, per
// http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf
// 13.2.2
// ES3
function new_(constructor, argumentList) {
    if (!(constructor instanceof Function)) {
        throw new TypeError('new_ called with constructor type ' + typeof(constructor) + " which is not a function");
    }
    /*
     * Previously, the following line was just:
     function dummy() {};
     * Unfortunately, Chrome was preserving 'dummy' as the object's name, even though at creation, the 'dummy' has the
     * correct constructor name.  Thus, objects created with IMVU.new would show up in the debugger as 'dummy', which
     * isn't very helpful.  Using IMVU.createNamedFunction addresses the issue.  Doublely-unfortunately, there's no way
     * to write a test for this behavior.  -NRD 2013.02.22
     */
    var dummy = createNamedFunction(constructor.name, function(){});
    dummy.prototype = constructor.prototype;
    var obj = new dummy;
    var r = constructor.apply(obj, argumentList);
    return (r instanceof Object) ? r : obj;
}
// The path to interop from JS code to C++ code:
// (hand-written JS code) -> (autogenerated JS invoker) -> (template-generated C++ invoker) -> (target C++ function)
// craftInvokerFunction generates the JS invoker function for each function exposed to JS through embind.
function craftInvokerFunction(humanName, argTypes, classType, cppInvokerFunc, cppTargetFunc) {
    // humanName: a human-readable string name for the function to be generated.
    // argTypes: An array that contains the embind type objects for all types in the function signature.
    //    argTypes[0] is the type object for the function return value.
    //    argTypes[1] is the type object for function this object/class type, or null if not crafting an invoker for a class method.
    //    argTypes[2...] are the actual function parameters.
    // classType: The embind type object for the class to be bound, or null if this is not a method of a class.
    // cppInvokerFunc: JS Function object to the C++-side function that interops into C++ code.
    // cppTargetFunc: Function pointer (an integer to FUNCTION_TABLE) to the target C++ function the cppInvokerFunc will end up calling.
    var argCount = argTypes.length;
    if (argCount < 2) {
        throwBindingError("argTypes array size mismatch! Must at least get return value and 'this' types!");
    }
    var isClassMethodFunc = (argTypes[1] !== null && classType !== null);
    if (!isClassMethodFunc && !FUNCTION_TABLE[cppTargetFunc]) {
        throwBindingError('Global function '+humanName+' is not defined!');
    }
    // Free functions with signature "void function()" do not need an invoker that marshalls between wire types.
// TODO: This omits argument count check - enable only at -O3 or similar.
//    if (ENABLE_UNSAFE_OPTS && argCount == 2 && argTypes[0].name == "void" && !isClassMethodFunc) {
//       return FUNCTION_TABLE[fn];
//    }
    var argsList = "";
    var argsListWired = "";
    for(var i = 0; i < argCount-2; ++i) {
        argsList += (i!==0?", ":"")+"arg"+i;
        argsListWired += (i!==0?", ":"")+"arg"+i+"Wired";
    }
    var invokerFnBody =
        "return function "+makeLegalFunctionName(humanName)+"("+argsList+") {\n" +
        "if (arguments.length !== "+(argCount - 2)+") {\n" +
            "throwBindingError('function "+humanName+" called with ' + arguments.length + ' arguments, expected "+(argCount - 2)+" args!');\n" +
        "}\n";
    // Determine if we need to use a dynamic stack to store the destructors for the function parameters.
    // TODO: Remove this completely once all function invokers are being dynamically generated.
    var needsDestructorStack = false;
    for(var i = 1; i < argTypes.length; ++i) { // Skip return value at index 0 - it's not deleted here.
        if (argTypes[i] !== null && argTypes[i].destructorFunction === undefined) { // The type does not define a destructor function - must use dynamic stack
            needsDestructorStack = true;
            break;
        }
    }
    if (needsDestructorStack) {
        invokerFnBody +=
            "var destructors = [];\n";
    }
    var dtorStack = needsDestructorStack ? "destructors" : "null";
    var args1 = ["throwBindingError", "classType", "invoker", "fn", "runDestructors", "retType", "classParam"];
    var args2 = [throwBindingError, classType, cppInvokerFunc, cppTargetFunc, runDestructors, argTypes[0], argTypes[1]];
    if (isClassMethodFunc) {
        invokerFnBody += "var thisWired = classParam.toWireType("+dtorStack+", this);\n";
    }
    for(var i = 0; i < argCount-2; ++i) {
        invokerFnBody += "var arg"+i+"Wired = argType"+i+".toWireType("+dtorStack+", arg"+i+"); // "+argTypes[i+2].name+"\n";
        args1.push("argType"+i);
        args2.push(argTypes[i+2]);
    }
    if (isClassMethodFunc) {
        argsListWired = "thisWired" + (argsListWired.length > 0 ? ", " : "") + argsListWired;
    }
    var returns = (argTypes[0].name !== "void");
    invokerFnBody +=
        (returns?"var rv = ":"") + "invoker(fn"+(argsListWired.length>0?", ":"")+argsListWired+");\n";
    if (needsDestructorStack) {
        invokerFnBody += "runDestructors(destructors);\n";
    } else {
        for(var i = isClassMethodFunc?1:2; i < argTypes.length; ++i) { // Skip return value at index 0 - it's not deleted here. Also skip class type if not a method.
            var paramName = (i === 1 ? "thisWired" : ("arg"+(i-2)+"Wired"));
            if (argTypes[i].destructorFunction !== null) {
                invokerFnBody += paramName+"_dtor("+paramName+"); // "+argTypes[i].name+"\n";
                args1.push(paramName+"_dtor");
                args2.push(argTypes[i].destructorFunction);
            }
        }
    }
    if (returns) {
        invokerFnBody += "return retType.fromWireType(rv);\n";
    }
    invokerFnBody += "}\n";
    args1.push(invokerFnBody);
    var invokerFunction = new_(Function, args1).apply(null, args2);
    return invokerFunction;
}
function __embind_register_function(name, argCount, rawArgTypesAddr, rawInvoker, fn) {
    var argTypes = heap32VectorToArray(argCount, rawArgTypesAddr);
    name = readLatin1String(name);
    rawInvoker = FUNCTION_TABLE[rawInvoker];
    exposePublicSymbol(name, function() {
        throwUnboundTypeError('Cannot call ' + name + ' due to unbound types', argTypes);
    }, argCount - 1);
    whenDependentTypesAreResolved([], argTypes, function(argTypes) {
        var invokerArgsArray = [argTypes[0] /* return value */, null /* no class 'this'*/].concat(argTypes.slice(1) /* actual params */);
        replacePublicSymbol(name, craftInvokerFunction(name, invokerArgsArray, null /* no class 'this'*/, rawInvoker, fn), argCount - 1);
        return [];
    });
}
var tupleRegistrations = {};
function __embind_register_value_array(rawType, name, rawConstructor, rawDestructor) {
    tupleRegistrations[rawType] = {
        name: readLatin1String(name),
        rawConstructor: FUNCTION_TABLE[rawConstructor],
        rawDestructor: FUNCTION_TABLE[rawDestructor],
        elements: [],
    };
}
function __embind_register_value_array_element(
    rawTupleType,
    getterReturnType,
    getter,
    getterContext,
    setterArgumentType,
    setter,
    setterContext
) {
    tupleRegistrations[rawTupleType].elements.push({
        getterReturnType: getterReturnType,
        getter: FUNCTION_TABLE[getter],
        getterContext: getterContext,
        setterArgumentType: setterArgumentType,
        setter: FUNCTION_TABLE[setter],
        setterContext: setterContext,
    });
}
function __embind_finalize_value_array(rawTupleType) {
    var reg = tupleRegistrations[rawTupleType];
    delete tupleRegistrations[rawTupleType];
    var elements = reg.elements;
    var elementsLength = elements.length;
    var elementTypes = elements.map(function(elt) { return elt.getterReturnType; }).
                concat(elements.map(function(elt) { return elt.setterArgumentType; }));
    var rawConstructor = reg.rawConstructor;
    var rawDestructor = reg.rawDestructor;
    whenDependentTypesAreResolved([rawTupleType], elementTypes, function(elementTypes) {
        elements.forEach(function(elt, i) {
            var getterReturnType = elementTypes[i];
            var getter = elt.getter;
            var getterContext = elt.getterContext;
            var setterArgumentType = elementTypes[i + elementsLength];
            var setter = elt.setter;
            var setterContext = elt.setterContext;
            elt.read = function(ptr) {
                return getterReturnType['fromWireType'](getter(getterContext, ptr));
            };
            elt.write = function(ptr, o) {
                var destructors = [];
                setter(setterContext, ptr, setterArgumentType['toWireType'](destructors, o));
                runDestructors(destructors);
            };
        });
        return [{
            name: reg.name,
            'fromWireType': function(ptr) {
                var rv = new Array(elementsLength);
                for (var i = 0; i < elementsLength; ++i) {
                    rv[i] = elements[i].read(ptr);
                }
                rawDestructor(ptr);
                return rv;
            },
            'toWireType': function(destructors, o) {
                if (elementsLength !== o.length) {
                    throw new TypeError("Incorrect number of tuple elements for " + reg.name + ": expected=" + elementsLength + ", actual=" + o.length);
                }
                var ptr = rawConstructor();
                for (var i = 0; i < elementsLength; ++i) {
                    elements[i].write(ptr, o[i]);
                }
                if (destructors !== null) {
                    destructors.push(rawDestructor, ptr);
                }
                return ptr;
            },
            destructorFunction: rawDestructor,
        }];
    });
}
var structRegistrations = {};
function __embind_register_value_object(
    rawType,
    name,
    rawConstructor,
    rawDestructor
) {
    structRegistrations[rawType] = {
        name: readLatin1String(name),
        rawConstructor: FUNCTION_TABLE[rawConstructor],
        rawDestructor: FUNCTION_TABLE[rawDestructor],
        fields: [],
    };
}
function __embind_register_value_object_field(
    structType,
    fieldName,
    getterReturnType,
    getter,
    getterContext,
    setterArgumentType,
    setter,
    setterContext
) {
    structRegistrations[structType].fields.push({
        fieldName: readLatin1String(fieldName),
        getterReturnType: getterReturnType,
        getter: FUNCTION_TABLE[getter],
        getterContext: getterContext,
        setterArgumentType: setterArgumentType,
        setter: FUNCTION_TABLE[setter],
        setterContext: setterContext,
    });
}
function __embind_finalize_value_object(structType) {
    var reg = structRegistrations[structType];
    delete structRegistrations[structType];
    var rawConstructor = reg.rawConstructor;
    var rawDestructor = reg.rawDestructor;
    var fieldRecords = reg.fields;
    var fieldTypes = fieldRecords.map(function(field) { return field.getterReturnType; }).
              concat(fieldRecords.map(function(field) { return field.setterArgumentType; }));
    whenDependentTypesAreResolved([structType], fieldTypes, function(fieldTypes) {
        var fields = {};
        fieldRecords.forEach(function(field, i) {
            var fieldName = field.fieldName;
            var getterReturnType = fieldTypes[i];
            var getter = field.getter;
            var getterContext = field.getterContext;
            var setterArgumentType = fieldTypes[i + fieldRecords.length];
            var setter = field.setter;
            var setterContext = field.setterContext;
            fields[fieldName] = {
                read: function(ptr) {
                    return getterReturnType['fromWireType'](
                        getter(getterContext, ptr));
                },
                write: function(ptr, o) {
                    var destructors = [];
                    setter(setterContext, ptr, setterArgumentType['toWireType'](destructors, o));
                    runDestructors(destructors);
                }
            };
        });
        return [{
            name: reg.name,
            'fromWireType': function(ptr) {
                var rv = {};
                for (var i in fields) {
                    rv[i] = fields[i].read(ptr);
                }
                rawDestructor(ptr);
                return rv;
            },
            'toWireType': function(destructors, o) {
                // todo: Here we have an opportunity for -O3 level "unsafe" optimizations:
                // assume all fields are present without checking.
                for (var fieldName in fields) {
                    if (!(fieldName in o)) {
                        throw new TypeError('Missing field');
                    }
                }
                var ptr = rawConstructor();
                for (fieldName in fields) {
                    fields[fieldName].write(ptr, o[fieldName]);
                }
                if (destructors !== null) {
                    destructors.push(rawDestructor, ptr);
                }
                return ptr;
            },
            destructorFunction: rawDestructor,
        }];
    });
}
var genericPointerToWireType = function(destructors, handle) {
    if (handle === null) {
        if (this.isReference) {
            throwBindingError('null is not a valid ' + this.name);
        }
        if (this.isSmartPointer) {
            var ptr = this.rawConstructor();
            if (destructors !== null) {
                destructors.push(this.rawDestructor, ptr);
            }
            return ptr;
        } else {
            return 0;
        }
    }
    if (!handle.$$) {
        throwBindingError('Cannot pass "' + _embind_repr(handle) + '" as a ' + this.name);
    }
    if (!handle.$$.ptr) {
        throwBindingError('Cannot pass deleted object as a pointer of type ' + this.name);
    }
    if (!this.isConst && handle.$$.ptrType.isConst) {
        throwBindingError('Cannot convert argument of type ' + (handle.$$.smartPtrType ? handle.$$.smartPtrType.name : handle.$$.ptrType.name) + ' to parameter type ' + this.name);
    }
    var handleClass = handle.$$.ptrType.registeredClass;
    var ptr = upcastPointer(handle.$$.ptr, handleClass, this.registeredClass);
    if (this.isSmartPointer) {
        // TODO: this is not strictly true
        // We could support BY_EMVAL conversions from raw pointers to smart pointers
        // because the smart pointer can hold a reference to the handle
        if (undefined === handle.$$.smartPtr) {
            throwBindingError('Passing raw pointer to smart pointer is illegal');
        }
        switch (this.sharingPolicy) {
            case 0: // NONE
                // no upcasting
                if (handle.$$.smartPtrType === this) {
                    ptr = handle.$$.smartPtr;
                } else {
                    throwBindingError('Cannot convert argument of type ' + (handle.$$.smartPtrType ? handle.$$.smartPtrType.name : handle.$$.ptrType.name) + ' to parameter type ' + this.name);
                }
                break;
            case 1: // INTRUSIVE
                ptr = handle.$$.smartPtr;
                break;
            case 2: // BY_EMVAL
                if (handle.$$.smartPtrType === this) {
                    ptr = handle.$$.smartPtr;
                } else {
                    var clonedHandle = handle['clone']();
                    ptr = this.rawShare(
                        ptr,
                        __emval_register(function() {
                            clonedHandle['delete']();
                        })
                    );
                    if (destructors !== null) {
                        destructors.push(this.rawDestructor, ptr);
                    }
                }
                break;
            default:
                throwBindingError('Unsupporting sharing policy');
        }
    }
    return ptr;
};
// If we know a pointer type is not going to have SmartPtr logic in it, we can
// special-case optimize it a bit (compare to genericPointerToWireType)
var constNoSmartPtrRawPointerToWireType = function(destructors, handle) {
    if (handle === null) {
        if (this.isReference) {
            throwBindingError('null is not a valid ' + this.name);
        }
        return 0;
    }
    if (!handle.$$) {
        throwBindingError('Cannot pass "' + _embind_repr(handle) + '" as a ' + this.name);
    }
    if (!handle.$$.ptr) {
        throwBindingError('Cannot pass deleted object as a pointer of type ' + this.name);
    }
    var handleClass = handle.$$.ptrType.registeredClass;
    var ptr = upcastPointer(handle.$$.ptr, handleClass, this.registeredClass);
    return ptr;
};
// An optimized version for non-const method accesses - there we must additionally restrict that
// the pointer is not a const-pointer.
var nonConstNoSmartPtrRawPointerToWireType = function(destructors, handle) {
    if (handle === null) {
        if (this.isReference) {
            throwBindingError('null is not a valid ' + this.name);
        }
        return 0;
    }
    if (!handle.$$) {
        throwBindingError('Cannot pass "' + _embind_repr(handle) + '" as a ' + this.name);
    }
    if (!handle.$$.ptr) {
        throwBindingError('Cannot pass deleted object as a pointer of type ' + this.name);
    }
    if (handle.$$.ptrType.isConst) {
        throwBindingError('Cannot convert argument of type ' + handle.$$.ptrType.name + ' to parameter type ' + this.name);
    }
    var handleClass = handle.$$.ptrType.registeredClass;
    var ptr = upcastPointer(handle.$$.ptr, handleClass, this.registeredClass);
    return ptr;
};
function RegisteredPointer(
    name,
    registeredClass,
    isReference,
    isConst,
    // smart pointer properties
    isSmartPointer,
    pointeeType,
    sharingPolicy,
    rawGetPointee,
    rawConstructor,
    rawShare,
    rawDestructor
) {
    this.name = name;
    this.registeredClass = registeredClass;
    this.isReference = isReference;
    this.isConst = isConst;
    // smart pointer properties
    this.isSmartPointer = isSmartPointer;
    this.pointeeType = pointeeType;
    this.sharingPolicy = sharingPolicy;
    this.rawGetPointee = rawGetPointee;
    this.rawConstructor = rawConstructor;
    this.rawShare = rawShare;
    this.rawDestructor = rawDestructor;
    if (!isSmartPointer && registeredClass.baseClass === undefined) {
        if (isConst) {
            this['toWireType'] = constNoSmartPtrRawPointerToWireType;
            this.destructorFunction = null;
        } else {
            this['toWireType'] = nonConstNoSmartPtrRawPointerToWireType;
            this.destructorFunction = null;
        }
    } else {
        this['toWireType'] = genericPointerToWireType;
        // Here we must leave this.destructorFunction undefined, since whether genericPointerToWireType returns
        // a pointer that needs to be freed up is runtime-dependent, and cannot be evaluated at registration time.
        // TODO: Create an alternative mechanism that allows removing the use of var destructors = []; array in 
        //       craftInvokerFunction altogether.
    }
}
RegisteredPointer.prototype.getPointee = function(ptr) {
    if (this.rawGetPointee) {
        ptr = this.rawGetPointee(ptr);
    }
    return ptr;
};
RegisteredPointer.prototype.destructor = function(ptr) {
    if (this.rawDestructor) {
        this.rawDestructor(ptr);
    }
};
RegisteredPointer.prototype['fromWireType'] = function(ptr) {
    // ptr is a raw pointer (or a raw smartpointer)
    // rawPointer is a maybe-null raw pointer
    var rawPointer = this.getPointee(ptr);
    if (!rawPointer) {
        this.destructor(ptr);
        return null;
    }
    function makeDefaultHandle() {
        if (this.isSmartPointer) {
            return makeClassHandle(this.registeredClass.instancePrototype, {
                ptrType: this.pointeeType,
                ptr: rawPointer,
                smartPtrType: this,
                smartPtr: ptr,
            });
        } else {
            return makeClassHandle(this.registeredClass.instancePrototype, {
                ptrType: this,
                ptr: ptr,
            });
        }
    }
    var actualType = this.registeredClass.getActualType(rawPointer);
    var registeredPointerRecord = registeredPointers[actualType];
    if (!registeredPointerRecord) {
        return makeDefaultHandle.call(this);
    }
    var toType;
    if (this.isConst) {
        toType = registeredPointerRecord.constPointerType;
    } else {
        toType = registeredPointerRecord.pointerType;
    }
    var dp = downcastPointer(
        rawPointer,
        this.registeredClass,
        toType.registeredClass);
    if (dp === null) {
        return makeDefaultHandle.call(this);
    }
    if (this.isSmartPointer) {
        return makeClassHandle(toType.registeredClass.instancePrototype, {
            ptrType: toType,
            ptr: dp,
            smartPtrType: this,
            smartPtr: ptr,
        });
    } else {
        return makeClassHandle(toType.registeredClass.instancePrototype, {
            ptrType: toType,
            ptr: dp,
        });
    }
};
function makeClassHandle(prototype, record) {
    if (!record.ptrType || !record.ptr) {
        throwInternalError('makeClassHandle requires ptr and ptrType');
    }
    var hasSmartPtrType = !!record.smartPtrType;
    var hasSmartPtr = !!record.smartPtr;
    if (hasSmartPtrType !== hasSmartPtr) {
        throwInternalError('Both smartPtrType and smartPtr must be specified');
    }
    record.count = { value: 1 };
    return Object.create(prototype, {
        $$: {
            value: record,
        },
    });
}
// root of all pointer and smart pointer handles in embind
function ClassHandle() {
}
function getInstanceTypeName(handle) {
    return handle.$$.ptrType.registeredClass.name;
}
ClassHandle.prototype['isAliasOf'] = function(other) {
    if (!(this instanceof ClassHandle)) {
        return false;
    }
    if (!(other instanceof ClassHandle)) {
        return false;
    }
    var leftClass = this.$$.ptrType.registeredClass;
    var left = this.$$.ptr;
    var rightClass = other.$$.ptrType.registeredClass;
    var right = other.$$.ptr;
    while (leftClass.baseClass) {
        left = leftClass.upcast(left);
        leftClass = leftClass.baseClass;
    }
    while (rightClass.baseClass) {
        right = rightClass.upcast(right);
        rightClass = rightClass.baseClass;
    }
    return leftClass === rightClass && left === right;
};
function throwInstanceAlreadyDeleted(obj) {
    throwBindingError(getInstanceTypeName(obj) + ' instance already deleted');
}
ClassHandle.prototype['clone'] = function() {
    if (!this.$$.ptr) {
        throwInstanceAlreadyDeleted(this);
    }
    var clone = Object.create(Object.getPrototypeOf(this), {
        $$: {
            value: shallowCopy(this.$$),
        }
    });
    clone.$$.count.value += 1;
    return clone;
};
function runDestructor(handle) {
    var $$ = handle.$$;
    if ($$.smartPtr) {
        $$.smartPtrType.rawDestructor($$.smartPtr);
    } else {
        $$.ptrType.registeredClass.rawDestructor($$.ptr);
    }
}
ClassHandle.prototype['delete'] = function ClassHandle_delete() {
    if (!this.$$.ptr) {
        throwInstanceAlreadyDeleted(this);
    }
    if (this.$$.deleteScheduled) {
        throwBindingError('Object already scheduled for deletion');
    }
    this.$$.count.value -= 1;
    if (0 === this.$$.count.value) {
        runDestructor(this);
    }
    this.$$.smartPtr = undefined;
    this.$$.ptr = undefined;
};
var deletionQueue = [];
ClassHandle.prototype['isDeleted'] = function isDeleted() {
    return !this.$$.ptr;
};
ClassHandle.prototype['deleteLater'] = function deleteLater() {
    if (!this.$$.ptr) {
        throwInstanceAlreadyDeleted(this);
    }
    if (this.$$.deleteScheduled) {
        throwBindingError('Object already scheduled for deletion');
    }
    deletionQueue.push(this);
    if (deletionQueue.length === 1 && delayFunction) {
        delayFunction(flushPendingDeletes);
    }
    this.$$.deleteScheduled = true;
    return this;
};
function flushPendingDeletes() {
    while (deletionQueue.length) {
        var obj = deletionQueue.pop();
        obj.$$.deleteScheduled = false;
        obj['delete']();
    }
}
Module['flushPendingDeletes'] = flushPendingDeletes;
var delayFunction;
Module['setDelayFunction'] = function setDelayFunction(fn) {
    delayFunction = fn;
    if (deletionQueue.length && delayFunction) {
        delayFunction(flushPendingDeletes);
    }
};
function RegisteredClass(
    name,
    constructor,
    instancePrototype,
    rawDestructor,
    baseClass,
    getActualType,
    upcast,
    downcast
) {
    this.name = name;
    this.constructor = constructor;
    this.instancePrototype = instancePrototype;
    this.rawDestructor = rawDestructor;
    this.baseClass = baseClass;
    this.getActualType = getActualType;
    this.upcast = upcast;
    this.downcast = downcast;
}
function shallowCopy(o) {
    var rv = {};
    for (var k in o) {
        rv[k] = o[k];
    }
    return rv;
}
function __embind_register_class(
    rawType,
    rawPointerType,
    rawConstPointerType,
    baseClassRawType,
    getActualType,
    upcast,
    downcast,
    name,
    rawDestructor
) {
    name = readLatin1String(name);
    rawDestructor = FUNCTION_TABLE[rawDestructor];
    getActualType = FUNCTION_TABLE[getActualType];
    upcast = FUNCTION_TABLE[upcast];
    downcast = FUNCTION_TABLE[downcast];
    var legalFunctionName = makeLegalFunctionName(name);
    exposePublicSymbol(legalFunctionName, function() {
        // this code cannot run if baseClassRawType is zero
        throwUnboundTypeError('Cannot construct ' + name + ' due to unbound types', [baseClassRawType]);
    });
    whenDependentTypesAreResolved(
        [rawType, rawPointerType, rawConstPointerType],
        baseClassRawType ? [baseClassRawType] : [],
        function(base) {
            base = base[0];
            var baseClass;
            var basePrototype;
            if (baseClassRawType) {
                baseClass = base.registeredClass;
                basePrototype = baseClass.instancePrototype;
            } else {
                basePrototype = ClassHandle.prototype;
            }
            var constructor = createNamedFunction(legalFunctionName, function() {
                if (Object.getPrototypeOf(this) !== instancePrototype) {
                    throw new BindingError("Use 'new' to construct " + name);
                }
                if (undefined === registeredClass.constructor_body) {
                    throw new BindingError(name + " has no accessible constructor");
                }
                var body = registeredClass.constructor_body[arguments.length];
                if (undefined === body) {
                    throw new BindingError("Tried to invoke ctor of " + name + " with invalid number of parameters (" + arguments.length + ") - expected (" + Object.keys(registeredClass.constructor_body).toString() + ") parameters instead!");
                }
                return body.apply(this, arguments);
            });
            var instancePrototype = Object.create(basePrototype, {
                constructor: { value: constructor },
            });
            constructor.prototype = instancePrototype;
            var registeredClass = new RegisteredClass(
                name,
                constructor,
                instancePrototype,
                rawDestructor,
                baseClass,
                getActualType,
                upcast,
                downcast);
            var referenceConverter = new RegisteredPointer(
                name,
                registeredClass,
                true,
                false,
                false);
            var pointerConverter = new RegisteredPointer(
                name + '*',
                registeredClass,
                false,
                false,
                false);
            var constPointerConverter = new RegisteredPointer(
                name + ' const*',
                registeredClass,
                false,
                true,
                false);
            registeredPointers[rawType] = {
                pointerType: pointerConverter,
                constPointerType: constPointerConverter
            };
            replacePublicSymbol(legalFunctionName, constructor);
            return [referenceConverter, pointerConverter, constPointerConverter];
        }
    );
}
function __embind_register_class_constructor(
    rawClassType,
    argCount,
    rawArgTypesAddr,
    invoker,
    rawConstructor
) {
    var rawArgTypes = heap32VectorToArray(argCount, rawArgTypesAddr);
    invoker = FUNCTION_TABLE[invoker];
    whenDependentTypesAreResolved([], [rawClassType], function(classType) {
        classType = classType[0];
        var humanName = 'constructor ' + classType.name;
        if (undefined === classType.registeredClass.constructor_body) {
            classType.registeredClass.constructor_body = [];
        }
        if (undefined !== classType.registeredClass.constructor_body[argCount - 1]) {
            throw new BindingError("Cannot register multiple constructors with identical number of parameters (" + (argCount-1) + ") for class '" + classType.name + "'! Overload resolution is currently only performed using the parameter count, not actual type info!");
        }
        classType.registeredClass.constructor_body[argCount - 1] = function() {
            throwUnboundTypeError('Cannot construct ' + classType.name + ' due to unbound types', rawArgTypes);
        };
        whenDependentTypesAreResolved([], rawArgTypes, function(argTypes) {
            classType.registeredClass.constructor_body[argCount - 1] = function() {
                if (arguments.length !== argCount - 1) {
                    throwBindingError(humanName + ' called with ' + arguments.length + ' arguments, expected ' + (argCount-1));
                }
                var destructors = [];
                var args = new Array(argCount);
                args[0] = rawConstructor;
                for (var i = 1; i < argCount; ++i) {
                    args[i] = argTypes[i]['toWireType'](destructors, arguments[i - 1]);
                }
                var ptr = invoker.apply(null, args);
                runDestructors(destructors);
                return argTypes[0]['fromWireType'](ptr);
            };
            return [];
        });
        return [];
    });
}
function downcastPointer(ptr, ptrClass, desiredClass) {
    if (ptrClass === desiredClass) {
        return ptr;
    }
    if (undefined === desiredClass.baseClass) {
        return null; // no conversion
    }
    // O(depth) stack space used
    return desiredClass.downcast(
        downcastPointer(ptr, ptrClass, desiredClass.baseClass));
}
function upcastPointer(ptr, ptrClass, desiredClass) {
    while (ptrClass !== desiredClass) {
        if (!ptrClass.upcast) {
            throwBindingError("Expected null or instance of " + desiredClass.name + ", got an instance of " + ptrClass.name);
        }
        ptr = ptrClass.upcast(ptr);
        ptrClass = ptrClass.baseClass;
    }
    return ptr;
}
function validateThis(this_, classType, humanName) {
    if (!(this_ instanceof Object)) {
        throwBindingError(humanName + ' with invalid "this": ' + this_);
    }
    if (!(this_ instanceof classType.registeredClass.constructor)) {
        throwBindingError(humanName + ' incompatible with "this" of type ' + this_.constructor.name);
    }
    if (!this_.$$.ptr) {
        throwBindingError('cannot call emscripten binding method ' + humanName + ' on deleted object');
    }
    // todo: kill this
    return upcastPointer(
        this_.$$.ptr,
        this_.$$.ptrType.registeredClass,
        classType.registeredClass);
}
function __embind_register_class_function(
    rawClassType,
    methodName,
    argCount,
    rawArgTypesAddr, // [ReturnType, ThisType, Args...]
    rawInvoker,
    context
) {
    var rawArgTypes = heap32VectorToArray(argCount, rawArgTypesAddr);
    methodName = readLatin1String(methodName);
    rawInvoker = FUNCTION_TABLE[rawInvoker];
    whenDependentTypesAreResolved([], [rawClassType], function(classType) {
        classType = classType[0];
        var humanName = classType.name + '.' + methodName;
        var unboundTypesHandler = function() {
            throwUnboundTypeError('Cannot call ' + humanName + ' due to unbound types', rawArgTypes);
        };
        var proto = classType.registeredClass.instancePrototype;
        var method = proto[methodName];
        if (undefined === method || (undefined === method.overloadTable && method.className !== classType.name && method.argCount === argCount-2)) {
            // This is the first overload to be registered, OR we are replacing a function in the base class with a function in the derived class.
            unboundTypesHandler.argCount = argCount-2;
            unboundTypesHandler.className = classType.name;
            proto[methodName] = unboundTypesHandler;
        } else {
            // There was an existing function with the same name registered. Set up a function overload routing table.
            ensureOverloadTable(proto, methodName, humanName);
            proto[methodName].overloadTable[argCount-2] = unboundTypesHandler;
        }
        whenDependentTypesAreResolved([], rawArgTypes, function(argTypes) {
            var memberFunction = craftInvokerFunction(humanName, argTypes, classType, rawInvoker, context);
            // Replace the initial unbound-handler-stub function with the appropriate member function, now that all types
            // are resolved. If multiple overloads are registered for this function, the function goes into an overload table.
            if (undefined === proto[methodName].overloadTable) {
                proto[methodName] = memberFunction;
            } else {
                proto[methodName].overloadTable[argCount-2] = memberFunction;
            }
            return [];
        });
        return [];
    });
}
function __embind_register_class_class_function(
    rawClassType,
    methodName,
    argCount,
    rawArgTypesAddr,
    rawInvoker,
    fn
) {
    var rawArgTypes = heap32VectorToArray(argCount, rawArgTypesAddr);
    methodName = readLatin1String(methodName);
    rawInvoker = FUNCTION_TABLE[rawInvoker];
    whenDependentTypesAreResolved([], [rawClassType], function(classType) {
        classType = classType[0];
        var humanName = classType.name + '.' + methodName;
        var unboundTypesHandler = function() {
                throwUnboundTypeError('Cannot call ' + humanName + ' due to unbound types', rawArgTypes);
            };
        var proto = classType.registeredClass.constructor;
        if (undefined === proto[methodName]) {
            // This is the first function to be registered with this name.
            unboundTypesHandler.argCount = argCount-1;
            proto[methodName] = unboundTypesHandler;
        } else {
            // There was an existing function with the same name registered. Set up a function overload routing table.
            ensureOverloadTable(proto, methodName, humanName);
            proto[methodName].overloadTable[argCount-1] = unboundTypesHandler;
        }
        whenDependentTypesAreResolved([], rawArgTypes, function(argTypes) {
            // Replace the initial unbound-types-handler stub with the proper function. If multiple overloads are registered,
            // the function handlers go into an overload table.
            var invokerArgsArray = [argTypes[0] /* return value */, null /* no class 'this'*/].concat(argTypes.slice(1) /* actual params */);
            var func = craftInvokerFunction(humanName, invokerArgsArray, null /* no class 'this'*/, rawInvoker, fn);
            if (undefined === proto[methodName].overloadTable) {
                proto[methodName] = func;
            } else {
                proto[methodName].overloadTable[argCount-1] = func;
            }
            return [];
        });
        return [];
    });
}
function __embind_register_class_property(
    classType,
    fieldName,
    getterReturnType,
    getter,
    getterContext,
    setterArgumentType,
    setter,
    setterContext
) {
    fieldName = readLatin1String(fieldName);
    getter = FUNCTION_TABLE[getter];
    whenDependentTypesAreResolved([], [classType], function(classType) {
        classType = classType[0];
        var humanName = classType.name + '.' + fieldName;
        var desc = {
            get: function() {
                throwUnboundTypeError('Cannot access ' + humanName + ' due to unbound types', [getterReturnType, setterArgumentType]);
            },
            enumerable: true,
            configurable: true
        };
        if (setter) {
            desc.set = function() {
                throwUnboundTypeError('Cannot access ' + humanName + ' due to unbound types', [getterReturnType, setterArgumentType]);
            };
        } else {
            desc.set = function(v) {
                throwBindingError(humanName + ' is a read-only property');
            };
        }
        Object.defineProperty(classType.registeredClass.instancePrototype, fieldName, desc);
        whenDependentTypesAreResolved(
            [],
            (setter ? [getterReturnType, setterArgumentType] : [getterReturnType]),
        function(types) {
            var getterReturnType = types[0];
            var desc = {
                get: function() {
                    var ptr = validateThis(this, classType, humanName + ' getter');
                    return getterReturnType['fromWireType'](getter(getterContext, ptr));
                },
                enumerable: true
            };
            if (setter) {
                setter = FUNCTION_TABLE[setter];
                var setterArgumentType = types[1];
                desc.set = function(v) {
                    var ptr = validateThis(this, classType, humanName + ' setter');
                    var destructors = [];
                    setter(setterContext, ptr, setterArgumentType['toWireType'](destructors, v));
                    runDestructors(destructors);
                };
            }
            Object.defineProperty(classType.registeredClass.instancePrototype, fieldName, desc);
            return [];
        });
        return [];
    });
}
var char_0 = '0'.charCodeAt(0);
var char_9 = '9'.charCodeAt(0);
function makeLegalFunctionName(name) {
    name = name.replace(/[^a-zA-Z0-9_]/g, '$');
    var f = name.charCodeAt(0);
    if (f >= char_0 && f <= char_9) {
        return '_' + name;
    } else {
        return name;
    }
}
function __embind_register_smart_ptr(
    rawType,
    rawPointeeType,
    name,
    sharingPolicy,
    rawGetPointee,
    rawConstructor,
    rawShare,
    rawDestructor
) {
    name = readLatin1String(name);
    rawGetPointee = FUNCTION_TABLE[rawGetPointee];
    rawConstructor = FUNCTION_TABLE[rawConstructor];
    rawShare = FUNCTION_TABLE[rawShare];
    rawDestructor = FUNCTION_TABLE[rawDestructor];
    whenDependentTypesAreResolved([rawType], [rawPointeeType], function(pointeeType) {
        pointeeType = pointeeType[0];
        var registeredPointer = new RegisteredPointer(
            name,
            pointeeType.registeredClass,
            false,
            false,
            // smart pointer properties
            true,
            pointeeType,
            sharingPolicy,
            rawGetPointee,
            rawConstructor,
            rawShare,
            rawDestructor);
        return [registeredPointer];
    });
}
function __embind_register_enum(
    rawType,
    name
) {
    name = readLatin1String(name);
    function constructor() {
    }
    constructor.values = {};
    registerType(rawType, {
        name: name,
        constructor: constructor,
        'fromWireType': function(c) {
            return this.constructor.values[c];
        },
        'toWireType': function(destructors, c) {
            return c.value;
        },
        destructorFunction: null,
    });
    exposePublicSymbol(name, constructor);
}
function __embind_register_enum_value(
    rawEnumType,
    name,
    enumValue
) {
    var enumType = requireRegisteredType(rawEnumType, 'enum');
    name = readLatin1String(name);
    var Enum = enumType.constructor;
    var Value = Object.create(enumType.constructor.prototype, {
        value: {value: enumValue},
        constructor: {value: createNamedFunction(enumType.name + '_' + name, function() {})},
    });
    Enum.values[enumValue] = Value;
    Enum[name] = Value;
}
function __embind_register_constant(name, type, value) {
    name = readLatin1String(name);
    whenDependentTypesAreResolved([], [type], function(type) {
        type = type[0];
        Module[name] = type['fromWireType'](value);
        return [];
    });
}
/*global Module:true, Runtime*/
/*global HEAP32*/
/*global new_*/
/*global createNamedFunction*/
/*global readLatin1String, writeStringToMemory*/
/*global requireRegisteredType, throwBindingError*/
/*jslint sub:true*/ /* The symbols 'fromWireType' and 'toWireType' must be accessed via array notation to be closure-safe since craftInvokerFunction crafts functions as strings that can't be closured. */
var Module = Module || {};
var _emval_handle_array = [{}]; // reserve zero
var _emval_free_list = [];
// Public JS API
/** @expose */
Module.count_emval_handles = function() {
    var count = 0;
    for (var i = 1; i < _emval_handle_array.length; ++i) {
        if (_emval_handle_array[i] !== undefined) {
            ++count;
        }
    }
    return count;
};
/** @expose */
Module.get_first_emval = function() {
    for (var i = 1; i < _emval_handle_array.length; ++i) {
        if (_emval_handle_array[i] !== undefined) {
            return _emval_handle_array[i];
        }
    }
    return null;
};
// Private C++ API
var _emval_symbols = {}; // address -> string
function __emval_register_symbol(address) {
    _emval_symbols[address] = readLatin1String(address);
}
function getStringOrSymbol(address) {
    var symbol = _emval_symbols[address];
    if (symbol === undefined) {
        return readLatin1String(address);
    } else {
        return symbol;
    }
}
function requireHandle(handle) {
    if (!handle) {
        throwBindingError('Cannot use deleted val. handle = ' + handle);
    }
}
function __emval_register(value) {
    var handle = _emval_free_list.length ?
        _emval_free_list.pop() :
        _emval_handle_array.length;
    _emval_handle_array[handle] = {refcount: 1, value: value};
    return handle;
}
function __emval_incref(handle) {
    if (handle) {
        _emval_handle_array[handle].refcount += 1;
    }
}
function __emval_decref(handle) {
    if (handle && 0 === --_emval_handle_array[handle].refcount) {
        _emval_handle_array[handle] = undefined;
        _emval_free_list.push(handle);
    }
}
function __emval_new_array() {
    return __emval_register([]);
}
function __emval_new_object() {
    return __emval_register({});
}
function __emval_undefined() {
    return __emval_register(undefined);
}
function __emval_null() {
    return __emval_register(null);
}
function __emval_new_cstring(v) {
    return __emval_register(getStringOrSymbol(v));
}
function __emval_take_value(type, v) {
    type = requireRegisteredType(type, '_emval_take_value');
    v = type['fromWireType'](v);
    return __emval_register(v);
}
var __newers = {}; // arity -> function
function craftEmvalAllocator(argCount) {
    /*This function returns a new function that looks like this:
    function emval_allocator_3(handle, argTypes, arg0Wired, arg1Wired, arg2Wired) {
        var argType0 = requireRegisteredType(HEAP32[(argTypes >> 2)], "parameter 0");
        var arg0 = argType0.fromWireType(arg0Wired);
        var argType1 = requireRegisteredType(HEAP32[(argTypes >> 2) + 1], "parameter 1");
        var arg1 = argType1.fromWireType(arg1Wired);
        var argType2 = requireRegisteredType(HEAP32[(argTypes >> 2) + 2], "parameter 2");
        var arg2 = argType2.fromWireType(arg2Wired);
        var constructor = _emval_handle_array[handle].value;
        var emval = new constructor(arg0, arg1, arg2);
        return emval;
    } */
    var args1 = ["requireRegisteredType", "HEAP32", "_emval_handle_array", "__emval_register"];
    var args2 = [requireRegisteredType, HEAP32, _emval_handle_array, __emval_register];
    var argsList = "";
    var argsListWired = "";
    for(var i = 0; i < argCount; ++i) {
        argsList += (i!==0?", ":"")+"arg"+i; // 'arg0, arg1, ..., argn'
        argsListWired += ", arg"+i+"Wired"; // ', arg0Wired, arg1Wired, ..., argnWired'
    }
    var invokerFnBody =
        "return function emval_allocator_"+argCount+"(handle, argTypes " + argsListWired + ") {\n";
    for(var i = 0; i < argCount; ++i) {
        invokerFnBody += 
            "var argType"+i+" = requireRegisteredType(HEAP32[(argTypes >> 2) + "+i+"], \"parameter "+i+"\");\n" +
            "var arg"+i+" = argType"+i+".fromWireType(arg"+i+"Wired);\n";
    }
    invokerFnBody +=
        "var constructor = _emval_handle_array[handle].value;\n" +
        "var obj = new constructor("+argsList+");\n" +
        "return __emval_register(obj);\n" +
        "}\n";
    args1.push(invokerFnBody);
    var invokerFunction = new_(Function, args1).apply(null, args2);
    return invokerFunction;
}
function __emval_new(handle, argCount, argTypes) {
    requireHandle(handle);
    var newer = __newers[argCount];
    if (!newer) {
        newer = craftEmvalAllocator(argCount);
        __newers[argCount] = newer;
    }
    if (argCount === 0) {
        return newer(handle, argTypes);
    } else if (argCount === 1) {
        return newer(handle, argTypes, arguments[3]);
    } else if (argCount === 2) {
        return newer(handle, argTypes, arguments[3], arguments[4]);
    } else if (argCount === 3) {
        return newer(handle, argTypes, arguments[3], arguments[4], arguments[5]);
    } else if (argCount === 4) {
        return newer(handle, argTypes, arguments[3], arguments[4], arguments[5], arguments[6]);
    } else {
        // This is a slow path! (.apply and .splice are slow), so a few specializations are present above.
        return newer.apply(null, arguments.splice(1));
    }
}
// appease jshint (technically this code uses eval)
var global = (function(){return Function;})()('return this')();
function __emval_get_global(name) {
    name = getStringOrSymbol(name);
    return __emval_register(global[name]);
}
function __emval_get_module_property(name) {
    name = getStringOrSymbol(name);
    return __emval_register(Module[name]);
}
function __emval_get_property(handle, key) {
    requireHandle(handle);
    return __emval_register(_emval_handle_array[handle].value[_emval_handle_array[key].value]);
}
function __emval_set_property(handle, key, value) {
    requireHandle(handle);
    _emval_handle_array[handle].value[_emval_handle_array[key].value] = _emval_handle_array[value].value;
}
function __emval_as(handle, returnType) {
    requireHandle(handle);
    returnType = requireRegisteredType(returnType, 'emval::as');
    var destructors = [];
    // caller owns destructing
    return returnType['toWireType'](destructors, _emval_handle_array[handle].value);
}
function parseParameters(argCount, argTypes, argWireTypes) {
    var a = new Array(argCount);
    for (var i = 0; i < argCount; ++i) {
        var argType = requireRegisteredType(
            HEAP32[(argTypes >> 2) + i],
            "parameter " + i);
        a[i] = argType['fromWireType'](argWireTypes[i]);
    }
    return a;
}
function __emval_call(handle, argCount, argTypes) {
    requireHandle(handle);
    var types = lookupTypes(argCount, argTypes);
    var args = new Array(argCount);
    for (var i = 0; i < argCount; ++i) {
        args[i] = types[i]['fromWireType'](arguments[3 + i]);
    }
    var fn = _emval_handle_array[handle].value;
    var rv = fn.apply(undefined, args);
    return __emval_register(rv);
}
function lookupTypes(argCount, argTypes, argWireTypes) {
    var a = new Array(argCount);
    for (var i = 0; i < argCount; ++i) {
        a[i] = requireRegisteredType(
            HEAP32[(argTypes >> 2) + i],
            "parameter " + i);
    }
    return a;
}
function __emval_get_method_caller(argCount, argTypes) {
    var types = lookupTypes(argCount, argTypes);
    var retType = types[0];
    var signatureName = retType.name + "_$" + types.slice(1).map(function (t) { return t.name; }).join("_") + "$";
    var args1 = ["addFunction", "createNamedFunction", "requireHandle", "getStringOrSymbol", "_emval_handle_array", "retType"];
    var args2 = [Runtime.addFunction, createNamedFunction, requireHandle, getStringOrSymbol, _emval_handle_array, retType];
    var argsList = ""; // 'arg0, arg1, arg2, ... , argN'
    var argsListWired = ""; // 'arg0Wired, ..., argNWired'
    for (var i = 0; i < argCount - 1; ++i) {
        argsList += (i !== 0 ? ", " : "") + "arg" + i;
        argsListWired += ", arg" + i + "Wired";
        args1.push("argType" + i);
        args2.push(types[1 + i]);
    }
    var invokerFnBody =
        "return addFunction(createNamedFunction('" + signatureName + "', function (handle, name" + argsListWired + ") {\n" +
        "requireHandle(handle);\n" +
        "name = getStringOrSymbol(name);\n";
    for (var i = 0; i < argCount - 1; ++i) {
        invokerFnBody += "var arg" + i + " = argType" + i + ".fromWireType(arg" + i + "Wired);\n";
    }
    invokerFnBody +=
        "var obj = _emval_handle_array[handle].value;\n" +
        "return retType.toWireType(null, obj[name](" + argsList + "));\n" + 
        "}));\n";
    args1.push(invokerFnBody);
    var invokerFunction = new_(Function, args1).apply(null, args2);
    return invokerFunction;
}
function __emval_has_function(handle, name) {
    name = getStringOrSymbol(name);
    return _emval_handle_array[handle].value[name] instanceof Function;
}
if (Module['preInit']) {
  if (typeof Module['preInit'] == 'function') Module['preInit'] = [Module['preInit']];
  while (Module['preInit'].length > 0) {
    Module['preInit'].pop()();
  }
}
// shouldRunNow refers to calling main(), not run().
var shouldRunNow = true;
if (Module['noInitialRun']) {
  shouldRunNow = false;
}
run();
// {{POST_RUN_ADDITIONS}}
// {{MODULE_ADDITIONS}}