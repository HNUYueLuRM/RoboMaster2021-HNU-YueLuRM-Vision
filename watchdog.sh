#!/bin/bash

### INFO
# 
# short description: watchdog for YueLu_Vision
# step1:修改watchdog中【setupvars.sh】路径、【YueLu_Vision】路径
# step2:修改"~/.bashrc"文件，在末尾加上： eval "$RUN_AFTER_BASHRC"
# step3:将watchdog.sh加入启动项，输入命令:gnome-session-properties，"add"一项，"Name"随便填，“Command”填：【gnome-terminal -x bash -c 'export RUN_AFTER_BASHRC="/home/rm/Desktop/watchdog.sh"; exec bash' 】(注意：改成自己watchdog.sh路径)
### END INFO

sec=1
cnt=0
PROC_NAME=YueLu_Vision #进程名字，取决于你自己make后的名称
Thread=`ps -ef | grep ${PROC_NAME} | grep -v "grep"` #判断用到，具体用法百度

#init openvino
echo "123456" | sudo -S sudo chmod +777 /opt/intel/openvino_2021/bin/setupvars.sh /opt/intel/openvino_2021/opencv/setupvars.sh #自动输入密码并开启权限,需要改到自己setupvars.sh的路径
cd /opt/intel/openvino_2021/opencv/  #需要改到自己setupvars.sh的路径
./setupvars.sh
cd /opt/intel/openvino_2021/bin/   #需要改到自己setupvars.sh的路径
./setupvars.sh


cd /home/rm/Desktop/YueLu_Vision_test/cmake-build-release/  #进入文件里面，需要改到自己YueLu_Vision的路径
echo "123456" | sudo -S sudo chmod +777 ./YueLu_Vision #自动输入密码并开启权限
./YueLu_Vision #运行

while [ 1 ] #循环，记得大括号里面的1两边都要空格
do
count=`ps -ef | grep ${PROC_NAME} | grep -v "grep" | wc -l` #判定进程运行否，是则继续，否则重新启动
echo "Thread count: ${count}"
if [ ${count} -gt 1 ];then  # 大于1情况下 进程没被杀害
	echo "##################################################"
	echo "## ${PROC_NAME} is running..."
	sleep ${sec} 
else  #进程被杀害
	echo "**************************************************"
	echo "## ${PROC_NAME} has been killed!!!!!!"
	echo "## Trying to restart ${PROC_NAME}......"
	cd ~
	echo "123456" | sudo -S sudo chmod +777 /home/rm/Desktop/YueLu_Vision_test/cmake-build-release/YueLu_Vision #需要改到自己YueLu_Vision的路径
	cd /home/rm/Desktop/YueLu_Vision_test/cmake-build-release/  #需要改到自己YueLu_Vision的路径
	./YueLu_Vision	
	echo "## ${PROC_NAME} restart successfully!"
	echo "**************************************************"
	sleep ${sec}
fi  
done

