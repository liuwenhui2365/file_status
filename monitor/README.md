##依赖

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


级别

	由高到低
	     FATAL: FATAL;
	    ERROR:FATAL,ERROR;
	     WARN:FATAL,ERROR,WARN;
	   NOTICE:FATAL,ERROR,WARN,NOTICE;
	       INFO:FATAL,ERROR,WARN,NOTICE;
	    DEBUG:FATAL,ERROR,WARN,NOTICE,DEBUG.

##使用
* TODO 
