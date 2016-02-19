S_CREATE_ROLE = {
	:cmdtype=>81, :name=>"S_CREATE_ROLE", :desc=>"create role", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"uuid", 		:type=>"VARCHAR_64", 		:desc=>"new role cookie" },
		{ :name=>"rolename", 	:type=>"VARCHAR_32", 	:desc=>"new role name" }
	]
}

S_LOGIN = {
	:cmdtype=>82, :name=>"S_LOGIN", :desc=>"create role", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",	:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
	]
}

S_REQUEST_BUY_INGOT = {
	:cmdtype=>83, :name=>"S_REQUEST_BUY_INGOT", :desc=>"client charge", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"dwChargeNum", 		:type=>"DWORD", 		:desc=>"charge gold num" }
	]
}


S_PACKAGE_EXTEND = {
	:cmdtype=>84, :name=>"S_PACKAGE_EXTEND", :desc=>"client extend package", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"dwNeedCoin", 		:type=>"DWORD", 		:desc=>"use coin" },
		{ :name=>"dwNeedGold", 		:type=>"DWORD", 		:desc=>"use gold" },
		{ :name=>"dwRoleCoin", 		:type=>"DWORD", 		:desc=>"role coin" },
		{ :name=>"dwRoleGold", 		:type=>"DWORD", 		:desc=>"role gold" },
		{ :name=>"byCurEquipPkgLimit", 		:type=>"BYTE", 		:desc=>"role equip package max size" },
	]
}

S_SELL_ITEMS = {
	:cmdtype=>85, :name=>"S_SELL_ITEMS", :desc=>"client sell item", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"dwGetCoin", 		:type=>"DWORD", 		:desc=>"get coin by sell" },
		{ :name=>"dwRoleCoin", 		:type=>"DWORD", 		:desc=>"role coin" },
		{ :name=>"wItemID", 		:type=>"WORD", 		:desc=>"sell item id" },
		{ :name=>"wItemNum", 		:type=>"WORD", 		:desc=>"sell item number" },
	]
}

S_CONFIRM_STRENGTHEN = {
	:cmdtype=>86, :name=>"S_CONFIRM_STRENGTHEN", :desc=>"client strength euqip", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"protocal", 	:type=>"VARCHAR_32", 	:desc=>"protocal type name" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
		{ :name=>"dwNeedCoin", 		:type=>"DWORD", 		:desc=>"use coin" },
		{ :name=>"dwRoleCoin", 		:type=>"DWORD", 		:desc=>"role coin" },
		{ :name=>"wItemID", 		:type=>"WORD", 		:desc=>" item id" },
		{ :name=>"byStrengthLevel", 		:type=>"BYTE", 		:desc=>"item strength level" },
	]
}

S_CONNECT_PLAYER = {
	:cmdtype=>87, :name=>"S_CONNECT_PLAYER", :desc=>"active", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"ip", 	:type=>"VARCHAR_32", 	:desc=>"connect ip" },
	]
}

S_ONLINE_NUMS = {
	:cmdtype=>88, :name=>"S_ONLINE_NUMS", :desc=>"active", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"dwTime",		:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"num", 	:type=>"DWORD", 	:desc=>"online player nums" },
	]
}

S_DISCONNECT_PLAYER = {
	:cmdtype=>89, :name=>"S_DISCONNECT_PLAYER", :desc=>"disconnect", :comment=>"",
	:member=>
	[
		{ :name=>"eventID", 	:type=>"UINT64", 	:desc=>"event ID" },
		{ :name=>"dwTime",	:type=>"UINT64",	:desc=>"operate happen time" },
		{ :name=>"ip", 	:type=>"VARCHAR_32", 	:desc=>"disconnect ip" },
		{ :name=>"dwRoleID", 	:type=>"DWORD", 		:desc=>"mysql role id" },
	]
}

GenList = [ S_CREATE_ROLE, S_LOGIN, S_REQUEST_BUY_INGOT, S_PACKAGE_EXTEND, S_SELL_ITEMS, S_CONFIRM_STRENGTHEN, S_CONNECT_PLAYER, S_ONLINE_NUMS, S_DISCONNECT_PLAYER ]