#ifndef SIGN_H
#define SIGN_H

#include "protocal.h"
#include "config.h"

namespace SIGN
{
	enum eSIGN
	{
		SIGN_SUCCESS = 0,     		//�ɹ�
		SIGN_ITEM_NO_EXISTS = 1,    //��Ʒ������
		SIGN_ITEM_ERROR = 2,        //��ȡ��Ʒ����
		SIGN_ALREADY = 3,           //������ǩ���Ѿ���ȡ
		SIGN_TIMES_NOT_ENOUGH = 4,  //����δ��
	};

	enum eSIGNSTATU
	{
		NO_SIGN_TODAY = 0,     		//����δǩ��
		SIGN_TODAY = 1,			//������ǩ��
	};

	int getSignInfo( ROLE* role, unsigned char * data, size_t dataLen );

	int clientSign( ROLE* role, unsigned char * data, size_t dataLen );

	BYTE getMonth();

	BYTE getDay();

	BYTE getDayByTime(time_t now);

	BYTE getMonthBytime(time_t now);
}


#endif //SIGN_H
