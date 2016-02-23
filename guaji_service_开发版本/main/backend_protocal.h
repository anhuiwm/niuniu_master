#pragma once


enum class backend_protocal
{
	charge = 10,// 充值
	query_roleinfo = 11,// 查询用户信息
	close_user = 12,// 封停账号
	kick_user = 13,// 踢用户下线
	broadcast = 14,// 广播
	mail = 15,// 发送邮件
	deny_chat = 16,// 阻止聊天
	full_scale_mail = 17,//全服邮件
	get_platform_reward = 18,//get complete platform task reward 
	max_count,
};

