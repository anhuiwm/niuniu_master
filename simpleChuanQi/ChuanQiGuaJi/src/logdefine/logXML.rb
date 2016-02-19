S_CREATE_ROLE = {
	:cmdtype=>81, :name=>"S_CREATE_ROLE", :desc=>"create role", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"uuid", 		:type=>"VARCHAR_64", 		:desc=>"new role cookie" },
		{ :name=>"rolename", 	:type=>"VARCHAR_32", 	:desc=>"new role name" }
	]
}

S_LOGIN = {
	:cmdtype=>82, :name=>"S_LOGIN", :desc=>"create role", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",	:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
	]
}

S_CONNECT_PLAYER = {
	:cmdtype=>83, :name=>"S_CONNECT_PLAYER", :desc=>"active", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"dwTime",		:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"ip", 	:type=>"VARCHAR_32", 	:desc=>"connect ip" },
	]
}

S_ONLINE_NUMS = {
	:cmdtype=>84, :name=>"S_ONLINE_NUMS", :desc=>"active", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"dwTime",		:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"num", 	:type=>"DWORD", 	:desc=>"online player nums" },
	]
}

S_DISCONNECT_PLAYER = {
	:cmdtype=>85, :name=>"S_DISCONNECT_PLAYER", :desc=>"disconnect", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"dwTime",	:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"ip", 	:type=>"VARCHAR_32", 	:desc=>"disconnect ip" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
	]
}

S_TICK_PLAYER_INFO = {
	:cmdtype=>86, :name=>"S_TICK_PLAYER_INFO", :desc=>"tick get playerinfo", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"DWORD", 	:desc=>"event ID" },
		{ :name=>"dwTime",	:type=>"DWORD",	:desc=>"operate happen time" },
		{ :name=>"id", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"rolename", 	:type=>"VARCHAR_32", 	:desc=>"role name" },
		{ :name=>"job", 		:type=>"BYTE", 		:desc=>"job" },
		{ :name=>"sex", 		:type=>"BYTE", 		:desc=>"sex" },
		{ :name=>"level", 		:type=>"WORD", 		:desc=>"level" },
		{ :name=>"viplevel", 		:type=>"DWORD", 		:desc=>"viplevel" },
		{ :name=>"exp", 	:type=>"DWORD", 		:desc=>"exp" },
		{ :name=>"money", 	:type=>"DWORD", 		:desc=>"money" },
		{ :name=>"gold", 	:type=>"DWORD", 		:desc=>"now have gold" },
		{ :name=>"charge", 	:type=>"DWORD", 		:desc=>"all charge gold" },		
	]
}

GenList = [ S_CREATE_ROLE, S_LOGIN, S_CONNECT_PLAYER, S_ONLINE_NUMS, S_DISCONNECT_PLAYER, S_TICK_PLAYER_INFO ]