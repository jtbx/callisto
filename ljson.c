/***
 * Manipulation of JavaScript Object Notation
 * (JSON) values.
 *
 * Implementation from
 * [lua-cjson](https://github.com/openresty/lua-cjson),
 * with some additional modifications
 *
 * @module json
 */

#include <lua.h>
#include <lauxlib.h>

#include "lcallisto.h"


/***
 * Returns the given Lua table encoded
 * as a JSON object.
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
 * Returns the given JSON object decoded
 * into a Lua table.
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
 * The *setting* paramater is a string
 * containing either "encode:" or "decode:"
 * followed by the name of the setting.
 * Settings can be found in the
 * [Settings](#Settings) section.
 *
 * @function config
 * @tparam string setting The setting to get/set.
 * @param ...
 */

/***
 * Creates and returns a new independent copy of the
 * module, with default settings and a separate
 * persistent encoding buffer.
 *
 * @function new
 */

/***
 * The name of the module, provided for
 * compatibility with lua-cjson.
 * Normally this will just be *"json"*.
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
 * Configures handling of extremely sparse arrays.
 *
 * **Parameters:**
 *
 * @setting encode:sparse-array
 * @tparam integer safe Always use an array when
 *   the max index is larger than this value.
 * @tparam boolean convert Whether or not to convert
 *   extremely sparse arrays into objects.
 * @tparam integer ration *0*: always allow sparse;
 *   *1*: never allow sparse; *>1*: use ratio
 */
/***
 * Configures the maximum number of nested
 * arrays/objects allowed when encoding.
 *
 * **Parameters:**
 *
 * @setting encode:max-depth
 * @tparam integer depth Max depth allowed.
 */
/***
 * Configures the maximum number of nested
 * arrays/objects allowed when decoding.
 *
 * **Parameters:**
 *
 * @setting decode:max-depth
 * @tparam integer depth Max depth allowed.
 */
/***
 * Configures the amount of significant
 * digits returned when encoding numbers.
 * This can be used to balance accuracy
 * versus performance.
 *
 * **Parameters:**
 *
 * @setting encode:number-precision
 * @tparam integer precision Amount of significant
 * digits to return in floating-point numbers (must
 * be between 1 and 14, default 14)
 */


int luaopen_cjson(lua_State *L);

int
callistoopen_json(lua_State *L)
{
	luaopen_cjson(L);
	return 1;
}
