#依赖

###日志(zlog)

安装

	- $git clone https://github.com/HardySimpson/zlog.git
	- $cd zlog	
	- $ make	
	- $sudo make install

配置

	- $sudo vi /etc/ld.so.conf
	- $ /usr/local/lib
	- $sudo ldconfig


##使用
* TODO 

###Build

gcc main.c monitor.c log/mylog.c -o daemon -lzlog

###Usage

sudo ./daemon MONITOR_DIR //the dir you want to monitor and sudo is must

###Debug

tail -f /var/log/syslog

tail -f my_mointor.log


