#include "rsc/RPack.h"
#include "RDebug.h"
#include "RThread.h"

#include <fstream>
#include <atomic>

using namespace Redopera;

RPack::RPack():
    RResource("Pack", Type::Pack)
{

}

RPack::RPack(const std::string &path, const std::string &name):
    RResource(name, Type::Pack)
{
    load(path);
}

RPack::RPack(const RPack &&pack):
    RResource(std::move(pack)),
    fileInfo_(std::move(pack.fileInfo_))
{

}

void RPack::swap(RPack &pack)
{
    RResource::swap(pack);
    fileInfo_.swap(pack.fileInfo_);
}

bool RPack::operator==(const RPack &pack)
{
    return equivalent(pack);
}

bool RPack::empty() const
{
    return fileInfo_.empty();
}

size_t RPack::size() const
{
    return fileInfo_.size();
}

const RPack::FInfo *RPack::getFileInfo(const std::string &file)
{
    uint64_t hash = std::hash<std::string>()(file);
    if(fileInfo_.count(hash))
        return &fileInfo_[hash];

    return nullptr;
}

bool RPack::load(const std::string &path)
{
    std::string newpath = rscpath(path);
    std::ifstream file(newpath, std::ios::binary);
    if(check(!file, "Failed to load pack <" + name() + "> in " + newpath))
        return false;

    Head head;
    // 若状态被置为failbit或badbit，则抛出异常
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.read(reinterpret_cast<char*>(&head), sizeof(head));
        // 检测文件头是否有效
        if(head.headformat == headformat)
        {
            if(check(head.signature != packSignature(), "Unknow pack signature:" + std::to_string(head.signature)))
                return false;

            std::vector<PInfo> info(head.fileNum);
            file.read(reinterpret_cast<char*>(info.data()), sizeof(PInfo) * head.fileNum);
            for(size_t i = 0; i < head.fileNum; ++i)
            {
                std::shared_ptr<RData[]> data(new RData[info[i].size]);
                file.read(reinterpret_cast<char*>(data.get()), info[i].size);
                data = unpackingOperation(data, info[i].size);
                fileInfo_.emplace(info[i].nameHash, FInfo{ info[i].size, info[i].check, data });
            }
            file.close();
        }
        else
        {
            prError("Invali pack file: " + newpath);
            return false;
        }
    }
    catch(std::ifstream::failure &e)
    {
        prError("Wrong access pack file: " + newpath + '\n' + e.what());
        return false;
    }

    return true;
}

bool RPack::packing(const std::shared_ptr<RData[]> &buffer, size_t size, const std::string &name)
{
    uint64_t hash = std::hash<std::string>()(name);
    size_t check = generateCheckCode(buffer.get(), size);

    if(fileInfo_.count(hash))
    {
        if(fileInfo_[hash].check != check)
        {
            fileInfo_[hash] = { size, check, buffer };
            rDebug << printFormat::yellow << "RPack: Update file <" + name + '>' <<  printFormat::white;
            return true;
        }
    }
    else
    {
        fileInfo_.emplace(hash, FInfo{ size, check, buffer });
        rDebug << printFormat::yellow << "RPack: Insert file <" + name + '>' <<  printFormat::white;
        return true;
    }

    return false;
}

bool RPack::packing(const std::string &path)
{
    std::string newpath = rscpath(path);
    std::ifstream file(newpath, std::ios::binary | std::ios::ate);
    if(check(!file, "Failed to packing <" + name() + "> in " + newpath))
        return false;

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::shared_ptr<RData[]> data(new RData[size]);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.read(reinterpret_cast<char*>(data.get()), size);
        file.close();
    }
    catch(std::ifstream::failure &f)
    {
        prError("Wrong access file: " + newpath + f.what());
        return false;
    }

    return packing(data, size, path);
}

bool RPack::save(const std::string &p)
{
    std::string path = rscpath(p);

    std::ofstream file(path, std::ios::binary);
    if(check(!file, "Failure save pack file as <" + path + '>'))
        return false;

    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        //写入文件头
        Head head { headformat,
                    static_cast<uint32_t>(fileInfo_.size()),
                    packSignature()};
        file.write(reinterpret_cast<char*>(&head), sizeof(Head));

        for(auto &info : fileInfo_)
            info.second.data = packingOperation(info.second.data, info.second.size);

        for(auto &info : fileInfo_)
        {
            PInfo pkgInfo { info.first, info.second.size, info.second.check };
            file.write(reinterpret_cast<char*>(&pkgInfo), sizeof(PInfo));
        }
        //写入数据部分
        for(auto &info : fileInfo_)
        {
            char *buf = reinterpret_cast<char*>(info.second.data.get());
            size_t size = info.second.size;
            file.write(buf, size);
        }

        file.close();
    }
    catch(std::ofstream::failure &f)
    {
        prError("Failure save pack file as <" + path + '>');
        return false;
    }

    rDebug << printFormat::yellow << "RPack: Save pack as <" + path + '>' <<  printFormat::white;
    return true;
}

void RPack::release()
{
    std::map<uint64_t, FInfo> temp;
    fileInfo_.swap(temp);
}

bool RPack::equivalent(const RPack &pack)
{
    if(fileInfo_.size() != pack.fileInfo_.size())
        return false;

    for(auto &info : fileInfo_)
    {
        auto it = pack.fileInfo_.find(info.first);
        if(it != pack.fileInfo_.end())
        {
            if(it->second.check != info.second.check)
                return false;
        }
        else
            return false;
    }

    return true;
}

uint64_t RPack::packSignature() const
{
    return 0x0;
}

uint64_t RPack::generateCheckCode(const RData *buffer, size_t size)
{
    static const int concurrency = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;

    const uint64_t *p = reinterpret_cast<const uint64_t*>(buffer);
    int num = size > 100000 ? concurrency : 1;

    std::atomic_uint64_t result;
    size_t range = size / num;

    { //线程开始
    std::vector<RThread> threads;
    threads.reserve(num);
    for(int i = 1; i < num; ++i)
    {
        threads.emplace_back([&result, range](const uint64_t *p)
        {
            for(size_t j = 0; j < range; ++j)
                result += *p++;
        },
        p+range*i);
    }
    } //线程结束

    return result;
}

std::shared_ptr<RData[]> RPack::packingOperation(const std::shared_ptr<RData[]> &buffer, size_t &)
{
    return buffer;
}

std::shared_ptr<RData[]> RPack::unpackingOperation(const std::shared_ptr<RData[]> &buffer, size_t &)
{
    return buffer;
}

void swap(RPack &pack1, RPack &pack2)
{
    pack1.swap(pack2);
}
