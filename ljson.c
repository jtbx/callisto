/***
 * Manipulation of JavaScript Object Notation (JSON) values.
 *
 * Implementation from [lua-cjson](https://github.com/openresty/lua-cjson),
 * with some additional modifications.
 *
 * **Features**
 *
 *  - Fast, standards compliant encoding/parsing routines
 *
 *  - Full support for JSON with UTF-8, including decoding surrogate pairs
 *
 *  - Optional run-time support for common exceptions to the JSON specification
 *  (infinity, NaN, etc.) 
 *
 *  - No dependencies on other libraries
 *
 * **Caveats**
 *
 *  - UTF-16 and UTF-32 are not supported
 *
 * @module json
 */

#include <lua.h>
#include <lauxlib.h>

#include "callisto.h"


/* -=[ Functions ]=- */

/***
 * Returns the given Lua table encoded as a JSON object.
 *
 * @function encode
 * @usage
local t = {
	key = "value",
	x = 4,
	y = 16.789,
	t = {
		hello = "world"
	}
}
local j = json.encode(t)
 * @tparam table t The table to encode.
 */

/***
 * Returns the given JSON object decoded into a Lua table.
 *
 * @function decode
 * @usage
local j = [[
{
	"key": "value",
	"x": 4,
	"y": 16.789,
	"obj": {
		"hello": "world"
	}
}
]]
local t = json.decode(j)
 * @tparam string j The JSON object to decode.
 */

/***
 * Gets/sets configuration values used when
 * encoding or decoding JSON objects.
 *
 * The *setting* paramater is a string containing either *"encode:"* or
 * *"decode:"*, followed by the name of the setting. Available settings
 * can be found in the [Settings](#Settings) section.
 *
 * The current setting is always returned, and is only updated
 * when an argument is provided.
 *
 * @function config
 * @usage json.config("encode:max-depth", 3)
 * @tparam string setting The setting to get/set.
 * @param ...
 */

/***
 * Creates and returns a new independent copy of the module,
 * with default settings and a separate persistent encoding buffer.
 *
 * @function new
 */

/* -=[ Fields ]=- */

/***
 * The name of the module, provided for compatibility with
 * lua-cjson. Normally this will just be *"json"*.
 *
 * @field _NAME
 */

/***
 * The version of lua-cjson
 * used in the library.
 *
 * @field _VERSION
 */

/***
 * null value used when decoding JSON objects.
 *
 * JSON *null* is decoded as a Lua lightuserdata `NULL` pointer.
 * *json.null* is provided for comparison.
 *
 * @field null
 */

/* -=[ Configuration options ]=- */

/* Decoding */

/***
 * Configures handling of invalid numbers while decoding;
 * numbers not supported by the JSON specification.
 * Invalid numbers are defined as:
 *
 *  - infinity
 *  - NaN
 *  - hexadecimals
 *
 * **Parameters:**
 *
 * @setting decode:invalid-numbers
 * @usage json.config("decode:invalid-numbers", false)
 * @tparam boolean convert Whether or not to accept and decode invalid numbers.
 */
/***
 * Configures the maximum number of nested
 * arrays/objects allowed when decoding.
 *
 * **Parameters:**
 *
 * @setting decode:max-depth
 * @usage json.config("decode:max-depth", 4)
 * @tparam integer depth Max depth allowed when decoding.
 */

/* Encoding */

/***
 * Configures handling of invalid numbers while encoding;
 * numbers not supported by the JSON specification.
 * Invalid numbers are defined as:
 *
 *  - infinity
 *  - NaN
 *  - hexadecimals
 *
 * **Parameters:**
 *
 * @setting encode:invalid-numbers
 * @usage json.config("encode:invalid-numbers", false)
 * @tparam boolean convert Whether or not to accept and encode invalid numbers.
 */
/***
 * Determine whether or not the JSON encoding buffer
 * should be reused after each call to *encode*.
 *
 * If *true*, the buffer will grow to the largest size
 * required and is not freed until the module is garbage
 * collected. This is the default setting.
 *
 * If *false*, the encode buffer is freed after each call.
 *
 * **Parameters:**
 *
 * @setting encode:keep-buffer
 * @usage json.config("encode:keep-buffer", false)
 * @tparam integer keep Whether or not the JSON encoding buffer should be reused.
 */
/***
 * Configures the maximum number of nested
 * arrays/objects allowed when encoding.
 *
 * **Parameters:**
 *
 * @setting encode:max-depth
 * @usage json.config("encode:max-depth", 4)
 * @tparam integer depth Max depth allowed when encoding.
 */
/***
 * Configures the amount of significant digits returned when encoding
 * numbers. This can be used to balance accuracy versus performance.
 *
 * **Parameters:**
 *
 * @setting encode:number-precision
 * @usage json.config("encode:number-precision", 2)
 * @tparam integer precision Amount of significant digits to return in
 *   floating-point numbers (must be between 1 and 14, default 14)
 */
/***
 * Configures handling of extremely sparse arrays; lists with holes.
 *
 * **Parameters:**
 *
 * @setting encode:sparse-array
 * @tparam[opt] integer safe Always use an array when the max index is
 *   larger than this value.
 * @tparam[opt] boolean convert Whether or not to convert extremely sparse
 *   arrays into objects.
 * @tparam[opt] integer ration *0*: always allow sparse; *1*: never allow
 *   sparse; *>1*: use ratio
 */


int luaopen_cjson(lua_State *L);

int
luaopen_json(lua_State *L)
{
	luaopen_cjson(L);
	return 1;
}
