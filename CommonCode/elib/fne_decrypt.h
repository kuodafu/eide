#include <tstr.h>
#include <vector>


namespace fne
{
    // ȡ������֧�ֿ�, ֻ��д֧�ֿ���, ���� ietb, wke, ����·����Ϣ������׺
    bool deleteFne(const char* fneName);

    // ��������֧�ֿ�, ֻ��д֧�ֿ���, ���� ietb, wke, ����·����Ϣ������׺
    bool addFne(const char* fneName);

    // �Ƿ�������֧�ֿ�
    bool isFne(const char* fneName, std::vector<_str>* names = 0);

    size_t fneList(std::vector<_str>& names);
}

