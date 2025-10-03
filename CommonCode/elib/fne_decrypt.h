#include <tstr.h>
#include <vector>


namespace fne
{
    // 取消启用支持库, 只填写支持库名, 比如 ietb, wke, 不带路径信息不带后缀
    bool deleteFne(const char* fneName);

    // 新增启用支持库, 只填写支持库名, 比如 ietb, wke, 不带路径信息不带后缀
    bool addFne(const char* fneName);

    // 是否已启用支持库
    bool isFne(const char* fneName, std::vector<_str>* names = 0);

    size_t fneList(std::vector<_str>& names);
}

