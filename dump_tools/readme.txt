1. �ѱ�Ŀ¼copy��linux
�����磺/root/dump_tools
2. ��objĿ¼copy����Ŀ¼��
�����磺/root/dump_tools/obj
3. ִ��create_symbols.sh
�����磺# ./create_symbols.sh
4. copy dump�ļ�����Ŀ¼��
�����磺/root/dump_tools/xxxxxxxx.dmp
5. ִ��minidump_stackwalk��������
�����磺# ./minidump_stackwalk ./xxxxxxxx.dmp ./symbols/ > xxxxxxxx.txt
5. ִ��dump_stack�������
    �磺# ./dump_stack.sh
6. �鿴xxxxxxxx.txt�������ж�crashλ��