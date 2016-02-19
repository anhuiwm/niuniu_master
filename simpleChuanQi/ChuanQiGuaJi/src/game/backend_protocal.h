#pragma once


enum class backend_protocal
{
	charge = 10,// 充值
	query_roleinfo,// 查询用户信息
	close_user,// 封停账号
	kick_user,// 踢用户下线
	broadcast,// 广播
	mail,// 发送邮件
	deny_chat,// 阻止聊天
	max_count,
};

