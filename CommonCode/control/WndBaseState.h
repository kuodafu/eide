#pragma once

//    normal                = 0x00000000,    // ����
//    ban                   = 0x00000001,    // ��ֹ
//    selected              = 0x00000002,    // ѡ��
//    focus                 = 0x00000004,    // ����
//    down                  = 0x00000008,    // ����
//    checked               = 0x00000010,    // ѡ��
//    indeterMinate         = 0x00000020,    // ��ѡ��
//    readonly              = 0x00000040,    // ֻ��
//    hot                   = 0x00000080,    // ��ȼ
//    def                   = 0x00000100,    // Ĭ��
//    subShow               = 0x00000200,    // ����Ŀ����
//    subHide               = 0x00000400,    // ����Ŀ����
//    busying               = 0x00000800,    // ��æ��
//    zhanwei               = 0x00001000,    // ռλ
//    rolling               = 0x00002000,    // ������
//    effecting             = 0x00004000,    // ������
//    hide                  = 0x00008000,    // ����
//    zhanwei1              = 0x00010000,    // ռλ
//
//    allowModifySize       = 0x00020000,    // �����޸ĳߴ�
//    allowDrag             = 0x00040000,    // �����϶�
//    zhanwei1              = 0x00080000,    // ռλ
//    allowFocus            = 0x00100000,    // ������
//    allowSelect           = 0x00200000,    // ����ѡ��
//    allowHrefHot          = 0x00400000,    // �������ӵ�ȼ
//    allowHrefEnd          = 0x00800000,    // ���������ѷ���
//    allowMulti            = 0x01000000,    // �����ѡ
//    zhanwei3              = 0x02000000,    // ռλ
//    zhanwei4              = 0x04000000,    // ռλ
//    zhanwei5              = 0x08000000,    // ռλ
//    zhanwei6              = 0x10000000,    // ռλ
//    password              = 0x20000000,    // ����ģʽ


enum STATE
{
    NORMAL                  = 0X00000000,    // ����
    BAN                     = 0X00000001,    // ��ֹ
    SELECTED                = 0X00000002,    // ѡ��
    FOCUS                   = 0X00000004,    // ����
    DOWN                    = 0X00000008,    // ����
    CHECKED                 = 0X00000010,    // ѡ��
    INDETERMINATE           = 0X00000020,    // ��ѡ��
    READONLY                = 0X00000040,    // ֻ��
    HOT                     = 0X00000080,    // ��ȼ
    DEF                     = 0X00000100,    // Ĭ��
    SUBSHOW                 = 0X00000200,    // ����Ŀ����
    SUBHIDE                 = 0X00000400,    // ����Ŀ����
    BUSYING                 = 0X00000800,    // ��æ��
    HOVER                   = 0X00001000,    // ����
    ROLLING                 = 0X00002000,    // ������
    EFFECTING               = 0X00004000,    // ������
    HIDE                    = 0X00008000,    // ����
    DOWNDROP                = 0X00010000,    // ������ť������

    ALLOWMODIFYSIZE         = 0X00020000,    // �����޸ĳߴ�
    ALLOWDRAG               = 0X00040000,    // �����϶�
    ZHANWEI2                = 0X00080000,    // ռλ
    ALLOWFOCUS              = 0X00100000,    // ������
    ALLOWSELECT             = 0X00200000,    // ����ѡ��
    ALLOWHREFHOT            = 0X00400000,    // �������ӵ�ȼ
    ALLOWHREFEND            = 0X00800000,    // ���������ѷ���
    ALLOWMULTI              = 0X01000000,    // �����ѡ
    ZHANWEI3                = 0X02000000,    // ռλ
    ZHANWEI4                = 0X04000000,    // ռλ
    ZHANWEI5                = 0X08000000,    // ռλ
    ZHANWEI6                = 0X10000000,    // ռλ
    PASSWORD                = 0X20000000,    // ����ģʽ
};
