Step1: 解码(2025年束流测试数据，需要加上`2025`)  
执行 ./ECALdig2root(2025) 二进制文件名 root文件名（root文件名不填则默认生成为re.root）
或者执行 ./ECALdig2root(2025)  filenamelist.txt root文件名（root文件名不填则自动生成txt文件中二进制文件对应的root文件）（1）
!注意：接线错误（1024run26以后）需要给第三个参数为1

Step2: "数字化"
执行 ./ECALDigi Step1.root Step2.root （1）
!注意：设置延迟（1024run10以后）需要给第三个参数为1

Step3: 重建
执行 ./Reconstruction Step2.root Step3.root
