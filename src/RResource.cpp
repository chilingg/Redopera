#include "rsc/RResource.h"
#include "RDebug.h"

#include <fstream>
#include <sstream>
#include <regex>

using namespace Redopera;

std::string RResource::resourcesPath = "";
std::mutex RResource::mutex;

RResource::RscList RResource::queryResourceList()
{
    std::lock_guard<std::mutex> guard(mutex);
    return *resourcesList();
}

std::string RResource::getTextFileContent(const std::string &path)
{
    std::string newpath = rscpath(path);

    std::string text;
    std::ifstream file;
    //若状态被置为failbit或badbit，则抛出异常
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(newpath);
        //读取文件缓冲到数据流
        std::stringstream sstream;
        sstream << file.rdbuf();

        file.close();
        text = sstream.str();
    }
    catch(...)
    {
        return "";
    }

    return text;
}

std::string RResource::rscpath(const std::string &path)
{
    std::string newpath;

    if(path[0] == ':')
        newpath = resourcesPath + path.substr(2);
    else
        newpath = path;

    return newpath;
}

void RResource::setResourcePath(const std::string &path)
{
    static std::regex r("(/|(../)+)*([-_a-z0-9.][-_a-z0-9. ]*/)*", std::regex::icase|std::regex::optimize);
    if(check(!std::regex_match(path, r), "Invalid resource path: " + path))
        return;
    resourcesPath = path;
}

const std::string &RResource::getResourcePath()
{
    return resourcesPath;
}

RResource::RResource(const std::string &name, Type type):
    resourcesList_(resourcesList()),
    resourceID_(new unsigned(registerResourceID(name, type)), unregisterResourceID),
    type_(type),
    name_(name)
{

}

RResource::RResource(const RResource &rc):
    resourcesList_(resourcesList()),
    resourceID_(rc.resourceID_),
    type_(rc.type_),
    name_(rc.name_)
{

}

RResource::RResource(const RResource &&rc):
    resourcesList_(resourcesList()),
    resourceID_(std::move(rc.resourceID_)),
    type_(rc.type_),
    name_(std::move(rc.name_))
{

}

RResource &RResource::operator=(const RResource &rc)
{
    name_ = rc.name_;
    resourceID_ = rc.resourceID_;
    return *this;
}

RResource &RResource::operator=(RResource &&rc)
{
    name_ = std::move(rc.name_);
    resourceID_ = std::move(rc.resourceID_);
    return *this;
}

void RResource::swap(RResource &rc) noexcept
{
    name_.swap(rc.name_);
    resourceID_.swap(rc.resourceID_);
}

RscID RResource::resourceID() const
{
    return *resourceID_;
}

const std::string &RResource::name() const
{
    return name_;
}

std::string RResource::nameAndID() const
{
    return '(' + std::to_string(*resourceID_) + ')' + name_;
}

void RResource::resetRscID()
{
    resourceID_.reset(new RscID(registerResourceID(name_, type_)), unregisterResourceID);
}

void RResource::rename(const std::string &name)
{
    name_ = name;
    std::string temp = name;

    {
    std::lock_guard<std::mutex> guard(mutex);
    auto it = resourcesList()->find(*resourceID_);
    it->second.name.swap(temp);
    }
}

RscID RResource::registerResourceID(const std::string &name, Type type)
{
    RscID i = 1;
    std::lock_guard<std::mutex> guard(mutex);

    while(resourcesList()->count(i))
        ++i;
    resourcesList()->emplace(i, RscInfo{ type, name });

    return i;
}

std::shared_ptr<RResource::RscList> &RResource::resourcesList()
{
    static std::shared_ptr<RscList> RESOURCE_LIST(new RscList);
    return RESOURCE_LIST;
}

void RResource::unregisterResourceID(RscID *id)
{
    {
    std::lock_guard<std::mutex> guard(mutex);
    resourcesList()->erase(*id);
    }

    delete id;
}

void swap(RResource &rc1, RResource &rc2)
{
    rc1.swap(rc2);
}
