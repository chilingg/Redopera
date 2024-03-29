#include <rsc/RPack.h>
#include <thread/RThread.h>
#include <RFormat.h>

#include <fstream>
#include <vector>
#include <atomic>
#include <cstring>

using namespace Redopera;

RPack::RPack(const RPack &&pack):
    fileInfo_(std::move(pack.fileInfo_))
{

}

void RPack::swap(RPack &pack)
{
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
    std::ifstream file(path, std::ios::binary);
    if(rCheck(!file, "Failed to load pack in {}!\n", path))
        return false;

    Head head;
    // 若状态被置为failbit或badbit，则抛出异常
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.read(reinterpret_cast<char*>(&head), sizeof(head));
        // 检测文件头是否有效
        if(head.headformat == headformat)
        {
            if(rCheck(head.signature != packSignature(), "Unknow pack signature:" + std::to_string(head.signature)))
                return false;

            std::vector<PInfo> info(head.fileNum);
            file.read(reinterpret_cast<char*>(info.data()), sizeof(PInfo) * head.fileNum);
            for(size_t i = 0; i < head.fileNum; ++i)
            {
                std::shared_ptr<RData[]> data(new RData[info[i].size]);
                file.read(reinterpret_cast<char*>(data.get()), info[i].size);
                unpackingOperation(data, info[i].size);
                fileInfo_.emplace(info[i].nameHash, FInfo{ info[i].size, info[i].check, std::move(data) });
            }
            file.close();
        }
        else
        {
            rError("Invali pack file: {}\n", path);
            return false;
        }
    }
    catch(std::ifstream::failure &e)
    {
        rError("Wrong access pack file {}: {}\n", path, e.what());
        return false;
    }

    return true;
}

bool RPack::packing(const RData *buffer, size_t size, const std::string &name)
{
    std::shared_ptr<RData[]> data(new RData[size]);
    std::memcpy(data.get(), buffer, size);

    return packing(data, size, name);
}

bool RPack::packing(std::shared_ptr<RData[]> buffer, size_t size, const std::string &name)
{
    uint64_t hash = std::hash<std::string>()(name);
    size_t check = generateCheckCode(buffer.get(), size);

    if(fileInfo_.count(hash))
    {
        if(fileInfo_[hash].check != check)
        {
            fileInfo_[hash] = FInfo{ size, check, buffer };
#ifndef NDEBUG
            rMessage("RPack: Update file <{}>.\n", name);
#endif
            return true;
        }
    }
    else
    {
        fileInfo_.emplace(hash, FInfo{ size, check, buffer });
#ifndef NDEBUG
        rMessage("RPack: Insert file <{}>.\n", name);
#endif
        return true;
    }

    return false;
}

bool RPack::packing(const std::string &path, const std::string &name)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if(rCheck(!file, "Failed to packing <{}> in {}!\n", name, path))
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
        rError("Wrong access file {}: {}\n", path, f.what());
        return false;
    }

    return packing(data, size, name);
}

bool RPack::save(const std::string &path)
{
    std::ofstream file(path, std::ios::binary);
    if(rCheck(!file, "Failure save pack file as {}\n", path))
        return false;

    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        //写入文件头
        Head head { headformat,
                    static_cast<uint32_t>(fileInfo_.size()),
                    packSignature()};
        file.write(reinterpret_cast<char*>(&head), sizeof(Head));

        for(auto &info : fileInfo_)
            packingOperation(info.second.data, info.second.size);

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
        rError("Failure save pack file as {}!\n", path);
        return false;
    }
#ifndef NDEBUG
    rMessage("RRPack: Save pack as {}.\n", path);
#endif
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

void RPack::packingOperation(std::shared_ptr<RData[]> &, size_t &)
{

}

void RPack::unpackingOperation(std::shared_ptr<RData[]> &, size_t &)
{

}

void swap(RPack &pack1, RPack &pack2)
{
    pack1.swap(pack2);
}
