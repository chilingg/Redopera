#include "rsc/RLuaScript.h"
#include "RDebug.h"

using namespace Redopera;

RLuaScript::RLuaScript():
    RResource("Script", Type::Script)
{

}

RLuaScript::RLuaScript(const std::string &lua, const std::string &name):
    RResource(name, Type::Script)
{
    load(lua);
}

RLuaScript::RLuaScript(const RData *data, size_t size, const std::string &name):
    RResource(name, Type::Script)
{
    load(data, size, name);
}

RLuaScript::RLuaScript(const RLuaScript &scp):
    RResource(scp),
    lua_(scp.lua_)
{

}

RLuaScript::RLuaScript(const RLuaScript &&scp):
    RResource(std::move(scp)),
    lua_(std::move(scp.lua_))
{

}

RLuaScript &RLuaScript::operator=(RLuaScript scp)
{
    swap(scp);
    return *this;
}

void RLuaScript::swap(RLuaScript &scp)
{
    RResource::swap(scp);
    lua_.swap(scp.lua_);
}

bool RLuaScript::isValid() const
{
    return lua_ != nullptr;
}

bool RLuaScript::isValidIndex(int index) const
{
    return !lua_isnone(lua_.get(), index);
}

bool RLuaScript::valueIsBool(int index) const
{
    return lua_isboolean(lua_.get(), index);
}

bool RLuaScript::valueIsFunction(int index) const
{
    return lua_isfunction(lua_.get(), index);
}

bool RLuaScript::valueIsNumber(int index) const
{
    return lua_isinteger(lua_.get(), index);
}

bool RLuaScript::valueIsString(int index) const
{
    return lua_isstring(lua_.get(), index);
}

bool RLuaScript::valueIsTable(int index) const
{
    return lua_istable(lua_.get(), index);
}

LuaType RLuaScript::valueType(int index) const
{
    return static_cast<LuaType>(lua_type(lua_.get(), index));
}

std::string RLuaScript::typeName(LuaType type)
{
    return lua_typename(lua_.get(), static_cast<int>(type));
}

int RLuaScript::stackSize() const
{
    return lua_gettop(lua_.get());
}

bool RLuaScript::getBool(int index)
{
    return lua_toboolean(lua_.get(), index);
}

double RLuaScript::getNumber(int index)
{
    return lua_tonumber(lua_.get(), index);
}

long long RLuaScript::getInteger(int index)
{
    return lua_tointeger(lua_.get(), index);
}

std::string RLuaScript::getString(int index)
{
    return std::string(lua_tostring(lua_.get(), index));
}

long long RLuaScript::getTableValueToInt(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    long long n = lua_tointeger(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return n;
}

double RLuaScript::getTableValueToDouble(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    double n = lua_tonumber(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return n;
}

std::string RLuaScript::getTableValueToString(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    std::string str = lua_tostring(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return str;
}

lua_State *RLuaScript::getLuaState()
{
    return lua_.get();
}

void RLuaScript::pushNil()
{
    lua_pushnil(lua_.get());
}

void RLuaScript::push(bool b)
{
    lua_pushboolean(lua_.get(), b);
}

void RLuaScript::push(double num)
{
    lua_pushnumber(lua_.get(), num);
}

void RLuaScript::push(const std::string &str)
{
    lua_pushstring(lua_.get(), str.c_str());
}

void RLuaScript::remove(int index)
{
    lua_remove(lua_.get(), index);
}

void RLuaScript::pop(int n)
{
    lua_pop(lua_.get(), n);
}

void RLuaScript::popTopToGlobal(const std::string &name)
{
    lua_setglobal(lua_.get(), name.c_str());
}

void RLuaScript::setStackSize(int size)
{
    lua_settop(lua_.get(), size);
}

void RLuaScript::setGlobalAsTop(const std::string &name)
{
    lua_getglobal(lua_.get(), name.c_str());
}

bool RLuaScript::call(const std::string &func, std::initializer_list<double> numList, std::initializer_list<const char *> strList, int resultN)
{
    lua_getglobal(lua_.get(), func.c_str());
    for(auto n : numList)
        lua_pushnumber(lua_.get(), n);
    for(auto s : strList)
        lua_pushstring(lua_.get(), s);

    if(lua_pcall(lua_.get(), numList.size()+strList.size(), resultN, 0) != 0)
    {
        prError("Failure call in function <" + func + ">: " + lua_tostring(lua_.get(), -1));
        lua_pop(lua_.get(), -1);
        return false;
    }

    return true;
}

bool RLuaScript::load(const std::string &lua)
{
    if(!lua_.unique())
        resetRscID();
    lua_ .reset(luaL_newstate(), lua_close);
    luaL_openlibs(lua_.get());

    std::string path = rscpath(lua);
    if(luaL_dofile(lua_.get(), path.c_str()))
    {
        pop();
        if(luaL_dostring(lua_.get(), lua.c_str()))
        {
            prError(lua_tostring(lua_.get(), -1));
            pop();
            lua_.reset();
        }
    }

    return lua_ != nullptr;
}

bool RLuaScript::load(const RData *buff, size_t size, const std::string &name)
{
    if(!lua_.unique())
        resetRscID();
    lua_ .reset(luaL_newstate(), lua_close);
    luaL_openlibs(lua_.get());

    if(luaL_loadbuffer(lua_.get(), reinterpret_cast<const char*>(buff), size, name.c_str()) || lua_pcall(lua_.get(), 0, 0, 0))
    {
        prError(lua_tostring(lua_.get(), -1));
        pop();
        lua_.reset();
    }

    return lua_ != nullptr;
}

bool RLuaScript::import(const std::string &lua)
{
    std::string path = rscpath(lua);
    if(luaL_dofile(lua_.get(), path.c_str()))
    {
        pop();
        if(luaL_dostring(lua_.get(), lua.c_str()))
        {
            prError(lua_tostring(lua_.get(), -1));
            pop();
            lua_.reset();
        }
    }

    return lua_ != nullptr;
}

bool RLuaScript::import(const RData *buff, size_t size, const std::string &name)
{
    if(luaL_loadbuffer(lua_.get(), reinterpret_cast<const char*>(buff), size, name.c_str()) || lua_pcall(lua_.get(), 0, 0, 0))
    {
        prError(lua_tostring(lua_.get(), -1));
        pop();
        lua_.reset();
    }

    return lua_ != nullptr;
}

void RLuaScript::release()
{
    lua_.reset();
}

int RLuaScript::absIndex(int idx)
{
    return lua_absindex (lua_.get(), idx);
}

void swap(RLuaScript &scp1, RLuaScript &scp2)
{
    scp1.swap(scp2);
}
