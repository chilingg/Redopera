#ifndef RRESOURCE_H
#define RRESOURCE_H

#include <string>
#include <map>
#include <memory>
#include <mutex>

namespace Redopera{

using RData = uint8_t;

using RscID = unsigned;

class RResource
{
    friend void swap(RResource &rc1, RResource &rc2);

public:
    enum class Type
    {
        Cursor,
        Font,
        Image,
        Script,
        Mp3,
        Pack,
        Shader,
        ShaderProg,
        Texture
    };

    struct RscInfo
    {
        Type type;
        std::string name;
    };

    using RscList = std::map<RscID, RscInfo>;

    static const std::shared_ptr<RscList> queryResourceList();

    static std::string getTextFileContent(const std::string &path);
    static std::string rscpath(const std::string &path);
    static void setResourcePath(const std::string &path);
    static const std::string& getResourcePath();

    RResource(const RResource &rc);
    RResource(const RResource &&rc);
    RResource& operator=(const RResource &rc);
    RResource& operator=(RResource &&rc);
    void swap(RResource &rc) noexcept;
    ~RResource() = default;

    RscID resourceID() const;
    const std::string& name() const;
    std::string nameAndID() const;

    void resetRscID();
    void rename(const std::string &name);

protected:
    RResource(const std::string &name, Type type);

private:
    static RscID registerResourceID(const std::string &name, Type type);
    static std::shared_ptr<RscList>& resourcesList();
    static void unregisterResourceID(RscID *id);

    static std::string resourcesPath;
    static std::mutex mutex;

    const Type type_;
    std::string name_;
    std::shared_ptr<RscID> resourceID_;
};

} // Redopera

void swap(Redopera::RResource &rc1, Redopera::RResource &rc2);

#endif // RRESOURCE_H
