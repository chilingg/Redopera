#include <rsc/RScript.h>
#include <RFormat.h>

using namespace Redopera;

RScript::RScript(const std::string &lua)
{
    load(lua);
}

RScript::RScript(const RData *data, size_t size, const std::string &name)
{
    load(data, size, name);
}

RScript::RScript(const RScript &scp):
    lua_(scp.lua_)
{

}

RScript::RScript(const RScript &&scp):
    lua_(std::move(scp.lua_))
{

}

RScript &RScript::operator=(RScript scp)
{
    swap(scp);
    return *this;
}

void RScript::swap(RScript &scp)
{
    lua_.swap(scp.lua_);
}

bool RScript::isValid() const
{
    return lua_ != nullptr;
}

bool RScript::isValidIndex(int index) const
{
    return !lua_isnone(lua_.get(), index);
}

bool RScript::valueIsBool(int index) const
{
    return lua_isboolean(lua_.get(), index);
}

bool RScript::valueIsFunction(int index) const
{
    return lua_isfunction(lua_.get(), index);
}

bool RScript::valueIsNumber(int index) const
{
    return lua_isnumber(lua_.get(), index);
}

bool RScript::valueIsString(int index) const
{
    return lua_isstring(lua_.get(), index);
}

bool RScript::valueIsTable(int index) const
{
    return lua_istable(lua_.get(), index);
}

Type RScript::valueType(int index) const
{
    return static_cast<Type>(lua_type(lua_.get(), index));
}

std::string RScript::typeName(Type type)
{
    return lua_typename(lua_.get(), static_cast<int>(type));
}

int RScript::stackSize() const
{
    return lua_gettop(lua_.get());
}

bool RScript::getBool(int index)
{
    return lua_toboolean(lua_.get(), index);
}

double RScript::getNumber(int index)
{
    return lua_tonumber(lua_.get(), index);
}

long long RScript::getInteger(int index)
{
    return lua_tointeger(lua_.get(), index);
}

std::string RScript::getString(int index)
{
    return std::string(lua_tostring(lua_.get(), index));
}

long long RScript::getTableValueToInt(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    long long n = lua_tointeger(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return n;
}

double RScript::getTableValueToDouble(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    double n = lua_tonumber(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return n;
}

std::string RScript::getTableValueToString(const std::string &key, int index)
{
    lua_getfield(lua_.get(), index, key.c_str());
    std::string str = lua_tostring(lua_.get(), -1);
    lua_pop(lua_.get(), 1);

    return str;
}

lua_State *RScript::getLuaState()
{
    return lua_.get();
}

void RScript::pushNil()
{
    lua_pushnil(lua_.get());
}

void RScript::push(bool b)
{
    lua_pushboolean(lua_.get(), b);
}

void RScript::push(double num)
{
    lua_pushnumber(lua_.get(), num);
}

void RScript::push(const std::string &str)
{
    lua_pushstring(lua_.get(), str.c_str());
}

void RScript::remove(int index)
{
    lua_remove(lua_.get(), index);
}

void RScript::pop(int n)
{
    lua_pop(lua_.get(), n);
}

void RScript::popTopToGlobal(const std::string &name)
{
    lua_setglobal(lua_.get(), name.c_str());
}

void RScript::setStackSize(int size)
{
    lua_settop(lua_.get(), size);
}

void RScript::setGlobalAsTop(const std::string &name)
{
    lua_getglobal(lua_.get(), name.c_str());
}

void RScript::cleanStack()
{
    lua_settop(lua_.get(), 0);
}

bool RScript::call(const std::string &func, std::initializer_list<double> numList, std::initializer_list<const char *> strList, int resultN)
{
    lua_getglobal(lua_.get(), func.c_str());
    for(auto n : numList)
        lua_pushnumber(lua_.get(), n);
    for(auto s : strList)
        lua_pushstring(lua_.get(), s);

    if(lua_pcall(lua_.get(), numList.size()+strList.size(), resultN, 0) != 0)
    {
        rError("Failure call in function <{}>: {}\n", func, lua_tostring(lua_.get(), -1));
        lua_pop(lua_.get(), -1);
        return false;
    }

    return true;
}

bool RScript::load(const std::string &lua)
{
    std::shared_ptr<lua_State> temp(luaL_newstate(), lua_close);
    luaL_openlibs(temp.get());

    if(luaL_dofile(temp.get(), lua.c_str()))
    {
        if(luaL_dostring(temp.get(), lua.c_str()))
        {
            rError(lua_tostring(temp.get(), -2));
            rError(lua_tostring(temp.get(), -1));
            return false;
        }
        else {
            lua_pop(temp.get(), 1);
        }
    }

    lua_.swap(temp);
    return true;
}

bool RScript::load(const RData *buff, size_t size, const std::string &name)
{
    std::shared_ptr<lua_State> temp(luaL_newstate(), lua_close);
    luaL_openlibs(temp.get());

    if(luaL_loadbuffer(temp.get(), reinterpret_cast<const char*>(buff), size, name.c_str()) || lua_pcall(temp.get(), 0, 0, 0))
    {
        rError(lua_tostring(temp.get(), -1));
        return false;
    }

    lua_.swap(temp);
    return true;
}

bool RScript::import(const std::string &lua)
{
    if(luaL_dofile(lua_.get(), lua.c_str()))
    {
        if(luaL_dostring(lua_.get(), lua.c_str()))
        {
            rError(lua_tostring(lua_.get(), -2));
            rError(lua_tostring(lua_.get(), -1));
            pop(2);

            return false;
        }
        else {
            pop();
        }
    }

    return true;
}

bool RScript::import(const RData *buff, size_t size, const std::string &name)
{
    if(luaL_loadbuffer(lua_.get(), reinterpret_cast<const char*>(buff), size, name.c_str()) || lua_pcall(lua_.get(), 0, 0, 0))
    {
        rError(lua_tostring(lua_.get(), -1));
        pop();
        return false;
    }

    return true;
}

void RScript::release()
{
    lua_.reset();
}

int RScript::absIndex(int idx)
{
    return lua_absindex (lua_.get(), idx);
}

void swap(RScript &scp1, RScript &scp2)
{
    scp1.swap(scp2);
}
