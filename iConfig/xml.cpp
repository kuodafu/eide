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

    // ��unicode�ַ���ת���ɵ�ǰ������Ҫ���ı���ʽ
    virtual LPCWSTR ToPointer(const wchar_t* pstr) {
        return m_xml.ToPointer(pstr);
    }
    // ��ansi�ַ���ת���ɵ�ǰ������Ҫ���ı���ʽ
    virtual LPCWSTR ToPointer(const char* pstr) {
        return m_xml.ToPointer(pstr);
    }
public:
    // ��յ�ǰxml, ����ʱ���Զ�����
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


    // ����xml, ����յ�ǰ����xml����
    // pszRootName = ���ڵ���, Ϊ0�򴴽�xml�ն���
    virtual bool Create(LPCWSTR pszRootName) {
        return m_xml.Create(pszRootName);
    }

    // ����xmlͷ
    virtual bool SetDeclaration(LPCWSTR pszText) { return m_xml.SetDeclaration(pszText); }

    // ����xml����, ����յ�ǰ����xml����
    virtual bool Parse(LPCWSTR p) { return m_xml.Parse(p); }

    // ����xml����, ����յ�ǰ����xml����
    virtual void Print(ICTinyXml2_XMLPrinter streamer = 0) { return m_xml.Print((tinyxml2::XMLPrinter*)streamer); }

    // ���ļ�����xml, ����յ�ǰ����xml����
    virtual bool LoadFile(LPCWSTR filename) { return m_xml.LoadFile(filename); }

    // ���Ѿ��򿪵��ļ�����xml, ����յ�ǰ����xml����
    virtual bool LoadFile(FILE* fp) { return m_xml.LoadFile(fp); }

    // ���浱ǰxml������, ����ڲ���¼���ļ�Ϊ��ʱ�ᱣ�浱ǰ�ļ�·��
    // filename = �����·��, ���Ϊ��, ��ʹ���ڲ���¼��·��, ����ڲ�û�м�¼·�������ʧ��
    // compact = ����ʽ���ı�, Ĭ��Ϊfalse, Ĭ�ϻ��ʽ���ı�
    virtual bool SaveFile(LPCWSTR filename = 0, bool compact = false) { return m_xml.SaveFile(filename, compact); }

    // ���浱ǰxml���ݵ��򿪵��ļ���
    virtual bool SaveFile(FILE* fp, bool compact = false) { return m_xml.SaveFile(fp, compact); }

    // ���ñ���ʱʹ�õ�Ŀ¼
    virtual void SetSaveFile(LPCWSTR file) { m_xml.SetSaveFile(file); }

    // ��ȡ�ڲ���¼���ļ�·��
    virtual LPCWSTR GetSaveFile() { return m_xml.GetSaveFile(); }

public:

    // ȡ���ڵ���, ���δ�����򷵻ؿ�
    virtual LPCWSTR GetRootName() const { return m_xml.GetRootName(); }

    // ȡ�ӽڵ���, ȡ�ò����ڵ�������ӽڵ�ĸ���
    // node = �ڵ�ָ��, ��ȡ����ڵ��µ��ӽڵ�����
    virtual int GetChildNodeCount(ICTinyXml2_XMLNode node) const { return m_xml.GetChildNodeCount((tinyxml2::XMLNode*)node); }

    // ȡ�ӽڵ���, ȡ�ò����ڵ�������ӽڵ�ĸ���
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual int GetChildNodeCount(LPCWSTR pszPath = 0) const { return m_xml.GetChildNodeCount(pszPath); }
    // ȡ�ڵ���, ȡ�ò����ڵ������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ��ȡ����ڵ�Ľڵ���
    virtual LPCWSTR GetNodeName(ICTinyXml2_XMLNode node) const { return m_xml.GetNodeName((tinyxml2::XMLNode*)node); }
    // ȡ�ڵ���, ȡ�ò����ڵ������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual LPCWSTR GetNodeName(LPCWSTR pszPath = 0) const { return m_xml.GetNodeName(pszPath); }
    // ȡ�ڵ�ֵ, ȡ�ò����ڵ��ֵ, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ��ȡ����ڵ�Ľڵ�ֵ
    virtual LPCWSTR GetNodeValue(ICTinyXml2_XMLNode node) const { return m_xml.GetNodeValue((tinyxml2::XMLNode*)node); }
    // ȡ�ڵ�ֵ, ȡ�ò����ڵ��ֵ, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual LPCWSTR GetNodeValue(LPCWSTR pszPath = 0) const { return m_xml.GetNodeValue(pszPath); }

    // ö��ָ��·���ڵ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������Ӧ��ֵ, �����򲻻�ȡ
    virtual int EnumAttribute(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.EnumAttribute(pszPath, pNames, pValues);
    }
    // ö��ָ��·���ڵ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ����ֵ������ڵ��ȡ
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������Ӧ��ֵ, �����򲻻�ȡ
    virtual int EnumAttribute(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.EnumAttribute((tinyxml2::XMLNode*)node, pNames, pValues);
    }
    // ȡ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������ֵ����, �����򲻻�ȡ
    virtual int GetAttrName(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.GetAttrName(pszPath, pNames, pValues);
    }

    // ȡ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ����ֵ������ڵ��ȡ
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������ֵ����, �����򲻻�ȡ
    virtual int GetAttrName(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const {
        return m_xml.GetAttrName((tinyxml2::XMLNode*)node, pNames, pValues);
    }

    // ȡ����ֵ, ȡ�ò����ڵ���ض����Ե�ֵ, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ������ڵ��ȡ����ֵ
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual LPCWSTR GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const {
        return m_xml.GetAttrValue((tinyxml2::XMLNode*)node, pszAttrName, 0, isAttribute);
    }
    // ȡ����ֵ, ȡ�ò����ڵ���ض����Ե�ֵ, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual LPCWSTR GetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const {
        return m_xml.GetAttrValue(pszPath, pszAttrName, 0, isAttribute);
    }
    //// ���뱸ע�ı�, ֵΪ0��ɾ���ñ�ע�ı�
    //// pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    //virtual bool InsertComment(ICTinyXml2_XMLNode node, LPCWSTR pszCommentText = 0) = 0;

    //// ���뱸ע�ı�, ֵΪ0��ɾ���ñ�ע�ı�
    //// pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    //virtual bool InsertComment(LPCWSTR pszPath, LPCWSTR pszCommentText = 0) = 0;

    // �����ӽڵ�, �½�һ���ڵ㣬���Ѹò����ڵ���Ϊ���½ڵ�ĸ��ڵ㣬Ȼ����뵽XML����
    // pszParentPath = ���ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�, �¼���Ľڵ�·�� = @1/@1/@2/@�ڵ�����
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    virtual bool InsertChildNode(LPCWSTR pszParentPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertChildNode(pszParentPath, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);
    }

    // �����ӽڵ�, �½�һ���ڵ㣬���Ѹò����ڵ���Ϊ���½ڵ�ĸ��ڵ㣬Ȼ����뵽XML����
    // parentNode = ���ڵ�
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    inline bool InsertChildNode(ICTinyXml2_XMLNode parentnode, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertChildNode((tinyxml2::XMLNode*)parentnode, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);

    }
    // ����ڵ�, ���뵽·���ڵ����
    // pszPath ����·��Ϊ@0 ����뵽��һ���ڵ�, @-1 ����뵽���һ���ڵ�, ������뵽 ָ��·������һ���ڵ�
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    virtual bool InsertNode(LPCWSTR pszPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) {
        return m_xml.InsertNode(pszPath, pszNodeName, pszNodeValue, isCDATA, (tinyxml2::XMLNode**)ppNode);
    }

    // ��������, �ڲ����ڵ��в���һ���µ����ԣ��������Ը�ֵ
    // node = �ڵ�ָ��, ������ڵ��������
    // pszAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // pszAttrValue = ����ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual bool InsertAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.InsertAttr((tinyxml2::XMLNode*)node, pszAttrName, pszAttrValue, isAttribute);
    }
    // ��������, �ڲ����ڵ��в���һ���µ����ԣ��������Ը�ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // pszAttrValue = ����ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual bool InsertAttr(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.InsertAttr(pszPath, pszAttrName, pszAttrValue, isAttribute);
    }

    // ɾ���ڵ�, ɾ�������ڵ�����������ӽڵ�
    // node = �ڵ�ָ��, ɾ������ڵ�
    virtual bool DelNode(ICTinyXml2_XMLNode node) {
        return m_xml.DelNode((tinyxml2::XMLNode*)node);
    }
    // ɾ���ڵ�, ɾ�������ڵ�����������ӽڵ�
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual bool DelNode(LPCWSTR pszPath) {
        return m_xml.DelNode(pszPath);
    }

    // ɾ������
    // node = �ڵ�ָ��, ������ڵ�ɾ������
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool DelAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName) {
        return m_xml.DelAttr((tinyxml2::XMLNode*)node, pszAttrName);
    }
    // ɾ������
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool DelAttr(LPCWSTR pszPath, LPCWSTR pszAttrName) {
        return m_xml.DelAttr(pszPath, pszAttrName);
    }

    // �޸Ľڵ���, �޸�ָ���ڵ������
    // node = �ڵ�ָ��, ������ڵ����ýڵ���
    // pszNodeName = �½ڵ���, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool SetNodeName(ICTinyXml2_XMLNode node, LPCWSTR pszNodeName) {
        return m_xml.SetNodeName((tinyxml2::XMLNode*)node, pszNodeName);
    }
    // �޸Ľڵ���, �޸�ָ���ڵ������
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszNodeName = �½ڵ���, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool SetNodeName(LPCWSTR pszPath, LPCWSTR pszNodeName) {
        return m_xml.SetNodeName(pszPath, pszNodeName);
    }

    // �޸Ľڵ�ֵ, �޸�ָ���ڵ��ֵ
    // node = �ڵ�ָ��, ������ڵ����ýڵ�ֵ
    // pszNodeValue = �½ڵ�ֵ   <�ڵ��� ������="����ֵ">�ڵ�ֵ</�ڵ���>
    virtual bool SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR pszNodeValue) {
        return m_xml.SetNodeValue((tinyxml2::XMLNode*)node, pszNodeValue);
    }
    // �޸Ľڵ�ֵ, �޸�ָ���ڵ��ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszNodeValue = �½ڵ�ֵ   <�ڵ��� ������="����ֵ">�ڵ�ֵ</�ڵ���>
    virtual bool SetNodeValue(LPCWSTR pszPath, LPCWSTR pszNodeValue) {
        return m_xml.SetNodeValue(pszPath, pszNodeValue);
    }

    //// �޸�������, �޸�ָ���ڵ����������
    //// pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszOldAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    //// pszNewAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
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


    // �޸�����ֵ, �޸�ָ���ڵ������ֵ
    // node = �ڵ�ָ��, ������ڵ���������ֵ
    // pszAttrName = ������ֵ, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    // pszAttrValue = ������ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = ������������Ƿ����
    virtual bool SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszNewAttrValue, BOOL* isAttribute = 0) {
        return m_xml.SetAttrValue((tinyxml2::XMLNode*)node, pszAttrName, pszNewAttrValue, isAttribute);
    }
    // �޸�����ֵ, �޸�ָ���ڵ������ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������ֵ, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    // pszAttrValue = ������ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = ������������Ƿ����
    virtual bool SetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) {
        return m_xml.SetAttrValue(pszPath, pszAttrName, pszAttrValue, isAttribute);
    }

    // ��ȡxml����, ����xml�ı�
    virtual LPCWSTR GetXmlData() {
        return m_xml.GetXmlData();
    }

public:
    // ���������ڵ��Ƿ���ͬ, ���������ӽڵ�, 2���ڵ㲻��λ��ͬһDocument��
    // ע�⣺�����XMLDocument�ϵ��ã��򽫷���false��.
    virtual bool Compare(const ICTinyXml2_XMLNode compare) const {
        return m_xml.Compare((tinyxml2::XMLNode*)compare);
    }
public:

    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual bool GetNode(LPCWSTR pszPath, ICTinyXml2_XMLNode& node) const {
        return m_xml.GetNode(pszPath, (tinyxml2::XMLNode*&)node);
    }

};


