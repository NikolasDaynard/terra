
#include <cairo/cairo-xcb.h>

#include <lua.h>
#include <lauxlib.h>
#include "lhelp.h"


// typedef cairo_atomic_intptr_t cairo_atomic_int_t;

// _cairo_atomic_int_get (cairo_atomic_intptr_t *x)
// {
//     cairo_atomic_intptr_t ret;

//     CAIRO_MUTEX_LOCK (_cairo_atomic_mutex);
//     ret = *x;
//     CAIRO_MUTEX_UNLOCK (_cairo_atomic_mutex);

//     return ret;
// }

// #define CAIRO_REFERENCE_COUNT_GET_VALUE(RC) _cairo_atomic_int_get (&(RC)->ref_count)
// #define CAIRO_REFERENCE_COUNT_IS_INVALID(RC) (CAIRO_REFERENCE_COUNT_GET_VALUE (RC) == CAIRO_REFERENCE_COUNT_INVALID_VALUE)

int luaH_scairo_surface_create_from_pixmap(lua_State *L)
{
    struct Application *ap = lhelp_check_app(L, 1);
    xcb_pixmap_t pixmap_id = (xcb_pixmap_t)lhelp_check_id(L, 2);
    u16 width = luaL_checkinteger(L, 3);
    u16 height = luaL_checkinteger(L, 4);

    cairo_surface_t *cairo_surface = cairo_xcb_surface_create(
        ap->connection,
        pixmap_id,
        ap->visual,
        width,
        height
    );

    lua_pushinteger(L, (u64)cairo_surface); // This HAS to be an integer, NOT a lightuserdata
    return 1;
}

int luaH_scairo_surface_destroy(lua_State *L)
{
    cairo_surface_t *cairo_surface = (cairo_surface_t *) lua_tointeger(L, 1);

    cairo_surface_finish(cairo_surface);
    cairo_surface_destroy(cairo_surface);
    
    return 0;
}

int get_cairo_refs(lua_State *L) {
    cairo_surface_t *cairo_surface = (cairo_surface_t *) lua_tointeger(L, 1);

    int ref_count = cairo_surface_get_reference_count(cairo_surface);

    lua_pushinteger(L, ref_count);

    return 1;
}

static const struct luaL_Reg lib_scairo[] = {
    { "create_from_pixmap", luaH_scairo_surface_create_from_pixmap },
    { "destroy", luaH_scairo_surface_destroy },
    { "getRefs", get_cairo_refs },
    { NULL, NULL }
};

int luaopen_terra_internal_scairo(lua_State *L)
{
    luaL_newlib(L, lib_scairo);
    return 1;
}

