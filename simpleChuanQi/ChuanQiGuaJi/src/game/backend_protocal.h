#pragma once


enum class backend_protocal
{
	charge = 10,// ��ֵ
	query_roleinfo,// ��ѯ�û���Ϣ
	close_user,// ��ͣ�˺�
	kick_user,// ���û�����
	broadcast,// �㲥
	mail,// �����ʼ�
	deny_chat,// ��ֹ����
	max_count,
};

