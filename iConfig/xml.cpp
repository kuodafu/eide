#include "iConfig.h"

class _CTinyXml2 : public ICTinyXml2
{
    CTinyXml2W m_xml;
    tinyxml2::XMLDocument* m_doc;
public:
    _CTinyXml2() : m_doc(0)
    {
    }
public:

    // 将unicode字符串转换成当前对象需要的文本格式
    virtual LPCWSTR ToPointer(const wchar_t* pstr) {
        return m_xml.ToPointer(pstr);
    }
    // 将ansi字符串转换成当前对象需要的文本格式
    virtual LPCWSTR ToPointer(const char* pstr) {
        return m_xml.ToPointer(pstr);
    }
public:
    // 清空当前xml, 析构时会自动触发
    virtual void Clear() {
        m_xml.Clear();
    }
    virtual ICTinyXml2_XMLElement NewElement(LPCSTR name)
    {
        if (!m_doc)
            m_doc = m_xml.GetDocument();
        if (!m_doc)return 0;
        return (ICTinyXml2_XMLElement)m_doc->NewElement(name);
    }
    virtual ICTinyXml2_XMLNode InsertEndChild(ICTinyXml2_XMLNode node, ICTinyXml2_XMLElement child)
    {
        return (ICTinyXml2_XMLNode)((tinyxml2::XMLNode*)node)->InsertEndChild((tinyxml2::XMLNode*)child);
    }
    virtual ICTinyXml2_XMLNode FirstChildElement(ICTinyXml2_XMLNode node, LPCSTR name)
    {
        return (ICTinyXml2_XMLNode)((tinyxml2::XMLNode*)node)->FirstChildElement(name);

    }
    virtual void SetText(ICTinyXml2_XMLElement node, LPCSTR text)
    {
        ((tinyxml2::XMLElement*)node)->SetText(text);

    }
    virtual void SetAttribute(ICTinyXml2_XMLElement node, LPCSTR name, LPCSTR value)
    {
        return ((tinyxml2::XMLElement*)node)->SetAttribute(name, value);

    }
    virtual ICTinyXml2_XMLElement ToElement(ICTinyXml2_XMLNode node)
    {
        return (ICTinyXml2_XMLElement)((tinyxml2::XMLElement*)node)->ToElement();
    }


    // 创建xml, 会清空当前已有xml数据
    // pszRootName = 根节点名, 为0则创建xml空对象
    virtual bool Create(LPCWSTR pszRootName) {
        return m_xml.Create(pszRootName);
    }

    // 设置xml头
    virtual bool SetDeclaration(LPCWSTR pszText) { return m_xml.SetDeclaration(pszText); }

    // 解析xml数据, 会清空当前已有xml数据
    virtual bool Parse(LPCWSTR p) { return m_xml.Parse(p); }

    // 解析xml数据, 会清空当前已有xml数据
    virtual void Print(ICTinyXml2_XMLPrinter streamer = 0) { return m_xml.Print((tinyxml2::XMLPrinter*)streamer); }

    // 从文件加载xml, 会清空当前已有xml数据
    virtual bool LoadFile(LPCWSTR filename) { return m_xml.LoadFile(filename); }

    // 从已经打开的文件加载xml, 会清空当前已有xml数据
    virtual bool LoadFile(FILE* fp) { return m_xml.LoadFile(fp); }

    // 保存当前xml到磁盘, 如果内部记录的文件为空时会保存当前文件路径
    // filename = 保存的路径, 如果为空, 则使用内部记录的路径, 如果内部没有记录路径则调用失败
    // compact = 不格式化文本, 默认为false, 默认会格式化文本
    virtual bool SaveFile(LPCWSTR filename = 0, bool compact = false) { return m_xml.SaveFile(filename, compact); }

    // 保存当前xml数据到打开的文件中
    virtual bool SaveFile(FILE* fp, bool compact = false) { return m_xml.SaveFile(fp, compact); }

    // 设置保存时使用的目录
    virtual void SetSaveFile(LPCWSTR file) { m_xml.SetSaveFile(file); }

    // 获取内部记录的文件路径
    virtual LPCWSTR GetSaveFile() { return m_xml.GetSaveFile(); }

public:

    // 取根节点名, 如果未创建则返回空
    virtual LPCWSTR GetRootName() const { return m_xml.GetRootName(); }

    // 取子节点数, 取得参数节点的所有子节点的个数
    // node = 节点指针, 读取这个节点下的子节点数量
    virtual int GetChildNodeCount(ICTinyXml2_XMLNode node) const { return m_xml.GetChildNodeCount((tinyxml2::XMLNode*)node); }

    // 取子节点数, 取得参数节点的所有子节点的个数
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual int GetChildNodeCount(LPCWSTR pszPath = 0) const { return m_xml.GetChildNodeCount(pszPath); }
    // 取节点名, 取得参数节点的名字, <节点名 属性名="属性值"/>
    // node = 节点指针, 读取这个节点的节点名
    virtual LPCWSTR GetNodeName(ICTinyXml2_XMLNode node) const { return m_xml.GetNodeName((tinyxml2::XMLNode*)node); }
    // 取节点名, 取得参数节点的名字, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual LPCWSTR GetNodeName(LPCWSTR pszPath = 0) const { return m_xml.GetNodeName(pszPath); }
    // 取节点值, 取得参数节点的值, <节点名 属性名="属性值"/>
    // node = 节点指针, 读取这个节点的节点值
    virtual LPCWSTR GetNodeValue(ICTinyXml2_XMLNode node) const { return m_xml.GetNodeValue((tinyxml2::XMLNode*)node); }
    // 取节点值, 取得参数节点的值, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual LPCWSTR GetNodeValue(LPCWSTR pszPath = 0) const { return m_xml.GetNodeValue(pszPath); }

    // 枚举指定路径节点的所有属性, 返回属性数量, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 属性名对应的值, 留空则不获取
    virtual int EnumAttribute(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.EnumAttribute(pszPath, pNames, pValues);
    }
    // 枚举指定路径节点的所有属性, 返回属性数量, <节点名 属性名="属性值"/>
    // node = 节点指针, 属性值从这个节点获取
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 属性名对应的值, 留空则不获取
    virtual int EnumAttribute(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.EnumAttribute((tinyxml2::XMLNode*)node, pNames, pValues);
    }
    // 取所有属性名, 返回属性数量, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 接收属性值数组, 留空则不获取
    virtual int GetAttrName(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.GetAttrName(pszPath, pNames, pValues);
    }

    // 取所有属性名, 返回属性数量, <节点名 属性名="属性值"/>
    // node = 节点指针, 属性值从这个节点获取
    // pNames = 接收属性名数组, 留空则不获取
    // pValues = 接收属性值数组, 留空则不获取
    virtual int GetAttrName(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.GetAttrName((tinyxml2::XMLNode*)node, pNames, pValues);
    }

    // 取属性值, 取得参数节点的特定属性的值, <节点名 属性名="属性值"/>
    // node = 节点指针, 从这个节点获取属性值
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual LPCWSTR GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const {
        return m_xml.GetAttrValue((tinyxml2::XMLNode*)node, pszAttrName, 0, isAttribute);
    }
    // 取属性值, 取得参数节点的特定属性的值, <节点名 属性名="属性值"/>
    // pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual LPCWSTR GetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const {
        return m_xml.GetAttrValue(pszPath, pszAttrName, 0, isAttribute);
    }
    //// 插入备注文本, 值为0则删除该备注文本
    //// pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszNodeValue = 节点值  <节点名>节点值</节点名>
    //virtual bool InsertComment(ICTinyXml2_XMLNode node, LPCWSTR pszCommentText = 0) = 0;

    //// 插入备注文本, 值为0则删除该备注文本
    //// pszPath = 路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszNodeValue = 节点值  <节点名>节点值</节点名>
    //virtual bool InsertComment(LPCWSTR pszPath, LPCWSTR pszCommentText = 0) = 0;

    // 插入子节点, 新建一个节点，并把该参数节点作为该新节点的父节点，然后插入到XML树中
    // pszParentPath = 父节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点, 新加入的节点路径 = @1/@1/@2/@节点数量
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    virtual bool InsertChildNode(LPCWSTR pszParentPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertChildNode(pszParentPath, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);
    }

    // 插入子节点, 新建一个节点，并把该参数节点作为该新节点的父节点，然后插入到XML树中
    // parentNode = 父节点
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    inline bool InsertChildNode(ICTinyXml2_XMLNode parentnode, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertChildNode((tinyxml2::XMLNode*)parentnode, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);

    }
    // 插入节点, 插入到路径节点后面
    // pszPath 最后的路径为@0 则加入到第一个节点, @-1 则加入到最后一个节点, 否则加入到 指定路径的下一个节点
    // pszNodeName = 新的节点名, 为0则调用失败  <节点名></节点名>
    // pszNodeValue = 节点值  <节点名>节点值</节点名>
    // isCDATA = 如果本参数为真，则节点值文本将被 "<![CDATA[" 和 "]]>" 包围，这意味着该文本可以包含任意字符(以不影响CDATA解析为限}
    virtual bool InsertNode(LPCWSTR pszPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertNode(pszPath, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);
    }

    // 插入属性, 在参数节点中插入一个新的属性，并给属性赋值
    // node = 节点指针, 从这个节点加入属性
    // pszAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // pszAttrValue = 属性值  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual bool InsertAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.InsertAttr((tinyxml2::XMLNode*)node, pszAttrName, pszAttrValue, isAttribute);
    }
    // 插入属性, 在参数节点中插入一个新的属性，并给属性赋值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    // pszAttrValue = 属性值  <节点名 属性名="属性值"/>
    // isAttribute = 要获取的这个属性是否存在, 可以为0, 如果属性不存在, 这个值为0, 存在则为1
    virtual bool InsertAttr(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.InsertAttr(pszPath, pszAttrName, pszAttrValue, isAttribute);
    }

    // 删除节点, 删除给定节点和他的所有子节点
    // node = 节点指针, 删除这个节点
    virtual bool DelNode(ICTinyXml2_XMLNode node) {
        return m_xml.DelNode((tinyxml2::XMLNode*)node);
    }
    // 删除节点, 删除给定节点和他的所有子节点
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual bool DelNode(LPCWSTR pszPath) {
        return m_xml.DelNode(pszPath);
    }

    // 删除属性
    // node = 节点指针, 从这个节点删除属性
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool DelAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName) {
        return m_xml.DelAttr((tinyxml2::XMLNode*)node, pszAttrName);
    }
    // 删除属性
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool DelAttr(LPCWSTR pszPath, LPCWSTR pszAttrName) {
        return m_xml.DelAttr(pszPath, pszAttrName);
    }

    // 修改节点名, 修改指定节点的名字
    // node = 节点指针, 从这个节点设置节点名
    // pszNodeName = 新节点名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool SetNodeName(ICTinyXml2_XMLNode node, LPCWSTR pszNodeName) {
        return m_xml.SetNodeName((tinyxml2::XMLNode*)node, pszNodeName);
    }
    // 修改节点名, 修改指定节点的名字
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszNodeName = 新节点名, 为0则调用失败  <节点名 属性名="属性值"/>
    virtual bool SetNodeName(LPCWSTR pszPath, LPCWSTR pszNodeName) {
        return m_xml.SetNodeName(pszPath, pszNodeName);
    }

    // 修改节点值, 修改指定节点的值
    // node = 节点指针, 从这个节点设置节点值
    // pszNodeValue = 新节点值   <节点名 属性名="属性值">节点值</节点名>
    virtual bool SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR pszNodeValue) {
        return m_xml.SetNodeValue((tinyxml2::XMLNode*)node, pszNodeValue);
    }
    // 修改节点值, 修改指定节点的值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszNodeValue = 新节点值   <节点名 属性名="属性值">节点值</节点名>
    virtual bool SetNodeValue(LPCWSTR pszPath, LPCWSTR pszNodeValue) {
        return m_xml.SetNodeValue(pszPath, pszNodeValue);
    }

    //// 修改属性名, 修改指定节点的属性名称
    //// pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    //// pszOldAttrName = 旧属性名, 为0则调用失败  <节点名 属性名="属性值"/> 
    //// pszNewAttrName = 新属性名, 为0则调用失败  <节点名 属性名="属性值"/>
    //virtual bool SetAttrName(LPCWSTR pszPath, LPCWSTR pszOldAttrName, LPCWSTR pszNewAttrName)
    //{
    //    ICTinyXml2_XMLNode node;
    //    if (!GetNode(pszPath, node)) return false;
    //    const tinyxml2::XMLAttribute* attr;
    //    tinyxml2::XMLElement* ele;
    //    if (!GetAttr(node, ele, pszOldAttrName, attr))return false;
    //    //attr->SetName(pszNewAttrName);
    //    return true;
    //}


    // 修改属性值, 修改指定节点的属性值
    // node = 节点指针, 从这个节点设置属性值
    // pszAttrName = 旧属性值, 为0则调用失败  <节点名 属性名="属性值"/> 
    // pszAttrValue = 新属性值  <节点名 属性名="属性值"/>
    // isAttribute = 接收这个属性是否存在
    virtual bool SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszNewAttrValue, BOOL* isAttribute = 0) {
        return m_xml.SetAttrValue((tinyxml2::XMLNode*)node, pszAttrName, pszNewAttrValue, isAttribute);
    }
    // 修改属性值, 修改指定节点的属性值
    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    // pszAttrName = 旧属性值, 为0则调用失败  <节点名 属性名="属性值"/> 
    // pszAttrValue = 新属性值  <节点名 属性名="属性值"/>
    // isAttribute = 接收这个属性是否存在
    virtual bool SetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.SetAttrValue(pszPath, pszAttrName, pszAttrValue, isAttribute);
    }

    // 获取xml数据, 返回xml文本
    virtual LPCWSTR GetXmlData() {
        return m_xml.GetXmlData();
    }

public:
    // 测试两个节点是否相同, 单不测试子节点, 2个节点不必位于同一Document中
    // 注意：如果在XMLDocument上调用，则将返回false。.
    virtual bool Compare(const ICTinyXml2_XMLNode compare) const {
        return m_xml.Compare((tinyxml2::XMLNode*)compare);
    }
public:

    // pszPath = 节点全路径, 根节点/子节点/叶子节点, 或者 @1/@1/@2, 为0则为根节点
    virtual bool GetNode(LPCWSTR pszPath, ICTinyXml2_XMLNode& node) const {
        return m_xml.GetNode(pszPath, (tinyxml2::XMLNode*&)node);
    }

};


