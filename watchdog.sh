#!/bin/bash

### BEGIN INIT INFO
# Provides:          watchDog
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: start watchDog
# Description:       start watchDog
### END INIT INFO

sec=1
cnt=0
PROC_NAME=YueLu_Vision #进程名字，取决于你自己make后的名称，也可以在终端输入top来查找
Thread=`ps -ef | grep ${PROC_NAME} | grep -v "grep"` #判断用到，具体用法百度

#init openvino
echo "123456" | sudo -S sudo chmod +777 /opt/intel/openvino_2021/bin/setupvars.sh /opt/intel/openvino_2021/opencv/setupvars.sh #自动输入密码并开启ttyusb权限
cd /opt/intel/openvino_2021/opencv/
./setupvars.sh
cd /opt/intel/openvino_2021/bin/
./setupvars.sh


cd /home/cyril/HNURM/YueLu_Vision_test/cmake-build-release/  #进入文件里面
#make clean && make -j8        #清除并重新make一下，防止文件损坏（-j提高效率）
echo "123456" | sudo -S sudo chmod +777 ./YueLu_Vision #自动输入密码并开启ttyusb权限
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
	####add remake
	cnt=`expr ${cnt} + 1`
	echo "**************************************************"
	echo "Failing counts: ${cnt} "
	if [ ${cnt} -gt 30 ];then # 30 times failed: try to remake the project
		echo "++++++++++++ trying to remake...   "
		cd /home/cyril/HNURM/YueLu_Vision_test/cmake-build-release/  #移动到你要编译运行的文件夹里面
		make clean && make -j8  #清除并重新make一下
		cnt=$[ 0 ]
		echo "++++++++++++ Remake successfully!"
	fi
	####end add remake
	echo "## ${PROC_NAME} has been killed!!!!!!"
	echo "## Trying to restart ${PROC_NAME}......"
	cd ~
	echo "123456" | sudo -S sudo chmod +777 /home/cyril/HNURM/YueLu_Vision_test/cmake-build-release/YueLu_Vision 
	cd /home/cyril/HNURM/YueLu_Vision_test/cmake-build-release/  #移动到你要编译运行的文件夹里面
	./YueLu_Vision	
	echo "## ${PROC_NAME} restart successfully!"
	echo "**************************************************"
	sleep ${sec}
fi  
done

