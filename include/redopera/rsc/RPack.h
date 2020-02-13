#ifndef RPACK_H
#define RPACK_H

#include "RResource.h"
#include <map>

namespace Redopera {

class RPack : public RResource
{
    friend void swap(RPack &pack1, RPack &pack2);

    //Rpack文件头数据
    struct Head
    {
        uint32_t headformat;    //文件头校验码
        uint32_t fileNum;       //包内文件数量
        uint64_t signature;     //pack签名
    };

    //包内各文件数据
    struct PInfo
    {
        uint64_t nameHash;  //文件名的哈希值
        uint64_t size;      //文件大小
        uint64_t check;     //文件校验码
    };

public:
    //加载后储存各文件数据
    struct FInfo
    {
        size_t size = 0;
        size_t check = 0;
        std::shared_ptr<RData[]> data;
    };

    RPack();
    RPack(const std::string &path, const std::string &name = "Pack");
    RPack(const RPack &&pack);
    void swap(RPack &pack);
    bool operator==(const RPack &pack);
    ~RPack() = default;

    RPack(const RPack&) = delete;
    RPack& operator=(RPack&) = delete;

    bool empty() const;
    size_t size() const;
    const FInfo* getFileInfo(const std::string &file);

    bool load(const std::string &path);
    bool packing(const std::shared_ptr<RData[]> &buffer, size_t size, const std::string &name);
    bool packing(const std::string &path);
    bool save(const std::string &path);
    void release();

    // 检查两个Pack文件之间的校验码是否全部相同
    bool equivalent(const RPack &pack);

protected:
    //文件头格式校验码 1110 1100 1000 0000 1000 1100 1110 0000
    static constexpr uint32_t headformat = 0xec808ce0;

    // 可以继承重写这两个函数，决定如何校验文件，或是压缩加密包内文件
    // 包类签名
    virtual uint64_t packSignature() const;
    // 生成文件校验码
    virtual uint64_t generateCheckCode(const RData *buffer, size_t size);
     // 打包算法
    virtual std::shared_ptr<RData[]> packingOperation(const std::shared_ptr<RData[]> &buffer, size_t &size);
     // 解包算法
    virtual std::shared_ptr<RData[]> unpackingOperation(const std::shared_ptr<RData[]> &buffer, size_t &size);

private:
    std::map<uint64_t, FInfo> fileInfo_;
};

} // Redopera

void swap(Redopera::RPack &pack1, Redopera::RPack &pack2);

#endif // RPACKAGE_H
