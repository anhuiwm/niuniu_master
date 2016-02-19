#/bin/sh
passstr=passwd
curdate=`date +%Y%m%d-%H%M%S` 
mkdir $curdate
cp miniSvrGame $curdate
svn export ../config $curdate/config

tar cvjf  ${curdate}.tar.bz2 $curdate/*
filename=${curdate}.tar.bz2

scpcmd="scp ${curdate}.tar.bz2 root@192.168.6.175:/home/simplechuanqi/${curdate}.tar.bz2"

expect -c "
	set timeout 60;
	spawn $scpcmd;
	expect {
		\"*password:\" {send \"$passstr\r\";interact}
	}
	exit 0;"

rm -rf $curdate
rm -rf $filename


#连到175上
sshcmd="ssh 192.168.6.175 \"cd /home/simplechuanqi; ./remote_start.sh $curdate;\""
expect -c "
	set timeout 60;
	spawn $sshcmd;
	expect {
	\"*password:\" {send \"$passstr\r\";interact}
						}
	exit 0;"



