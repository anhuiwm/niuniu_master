#pragma once


enum class backend_protocal
{
	charge = 10,// ��ֵ
	query_roleinfo = 11,// ��ѯ�û���Ϣ
	close_user = 12,// ��ͣ�˺�
	kick_user = 13,// ���û�����
	broadcast = 14,// �㲥
	mail = 15,// �����ʼ�
	deny_chat = 16,// ��ֹ����
	full_scale_mail = 17,//ȫ���ʼ�
	get_platform_reward = 18,//get complete platform task reward 
	max_count,
};

