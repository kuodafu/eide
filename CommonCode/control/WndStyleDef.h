#pragma once


#define EWS_BUTTON_CLOSE            (LONG_PTR)0x00000001    // ������_�رհ�ť
#define EWS_BUTTON_MAX              (LONG_PTR)0x00000002    // ������_��󻯰�ť
#define EWS_BUTTON_MIN              (LONG_PTR)0x00000004    // ������_��С����ť
#define EWS_BUTTON_MENU             (LONG_PTR)0x00000008    // ������_�˵���ť
#define EWS_BUTTON_SKIN             (LONG_PTR)0x00000010    // ������_Ƥ����ť
#define EWS_BUTTON_SETTING          (LONG_PTR)0x00000020    // ������_���ð�ť
#define EWS_BUTTON_HELP             (LONG_PTR)0x00000040    // ������_������ť
#define EWS_HASICON                 (LONG_PTR)0x00000080    // ������_ͼ��
#define EWS_TITLE                   (LONG_PTR)0x00000100    // ������_����
#define EWS_FULLSCREEN              (LONG_PTR)0x00000200    // ������_ȫ��ģʽ.���øñ�Ǵ������ʱ, �����������ڵ�.
#define EWS_SIZEABLE                (LONG_PTR)0x00000400    // ������_��������ߴ�
#define EWS_MOVEABLE                (LONG_PTR)0x00000800    // ������_���������ƶ�
#define EWS_NOSHADOW                (LONG_PTR)0x00001000    // ������_����ʾ������Ӱ
#define EWS_NOINHERITBKG            (LONG_PTR)0x00002000    // ������_���̳и����ڱ�������
#define EWS_NOTABBORDER             (LONG_PTR)0x00004000    // ������_����ʾTAB����߿�
#define EWS_ESCEXIT                 (LONG_PTR)0x00008000    // ������_ESC�رմ���
#define EWS_MAINWINDOW              (LONG_PTR)0x00010000    // ������_������(ӵ�и÷��ʱ, �����ڱ��ر�, �����PostQuitMessage()�˳���Ϣѭ��)
#define EWS_CENTERWINDOW            (LONG_PTR)0x00020000    // ������_���ھ���(����и�����, ���ڸ������м�, ����Ϊ��Ļ�м�)
#define EWS_NOCAPTIONTOPMOST        (LONG_PTR)0x00040000    // ������_������ȡ���ö�
#define EWS_BLUR                    (LONG_PTR)0x00080000    // ������_ģ������ͼƬ

#define EWS_MENU                    (LONG_PTR)0x40000000
#define EWS_MESSAGEBOX              (LONG_PTR)0x80000000



#define EOS_DISABLENOSCROLL         (LONG_PTR)0x02000000    // ������_������������ʱ��ʾ��ֹ״̬
#define EOS_SIZEBOX                 (LONG_PTR)0x04000000    // ������_�ɵ����ߴ�
#define EOS_DISABLED                (LONG_PTR)0x08000000    // ������_��ֹ
#define EOS_VISIBLE                 (LONG_PTR)0x10000000    // ������_����
#define EOS_BORDER                  (LONG_PTR)0x20000000    // ������_�߿�
#define EOS_VSCROLL                 (LONG_PTR)0x40000000    // ������_ˮƽ������
#define EOS_HSCROLL                 (LONG_PTR)0x80000000    // ������_��ֱ������

#define EOS_EX_DLGMODE              (LONG_PTR)0x00200000    // ������_��չ_�Ի���ģʽ
#define EOS_EX_AUTOSIZE             (LONG_PTR)0x00400000    // ������_��չ_����Ӧ�ߴ�
#define EOS_EX_TRANSPARENT          (LONG_PTR)0x00800000    // ������_��չ_��괩͸
#define EOS_EX_BLUR                 (LONG_PTR)0x01000000    // ������_��չ_����ģ��
#define EOS_EX_DRAGDROP             (LONG_PTR)0x02000000    // ������_��չ_������ק
#define EOS_EX_ACCEPTFILES          (LONG_PTR)0x04000000    // ������_��չ_�����ļ��Ϸ�
#define EOS_EX_FOCUSABLE            (LONG_PTR)0x08000000    // ������_��չ_������
#define EOS_EX_TABSTOP              (LONG_PTR)0x10000000    // ������_��չ_����TAB����
#define EOS_EX_TOPMOST              (LONG_PTR)0x20000000    // ������_��չ_������ǰ
#define EOS_EX_COMPOSITED           (LONG_PTR)0x40000000    // ������_��չ_�������
#define EOS_EX_CUSTOMDRAW           (LONG_PTR)0x80000000    // ������_��չ_�Զ������
