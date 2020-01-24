**System configuration**  
Your OS either has to be configured to have classic ptrace permissions or
the program must be run as root.  
This is needed so that the process_vm_readv syscall can access non-child processes memory  

**Enable classic ptrace permissions**  
You can enable classic ptrace permissions by running the following from a root shell  
``
echo 0 > /proc/sys/kernel/yama/ptrace_scope
``  

Yama ptrace permissions are explained here:  
https://www.kernel.org/doc/Documentation/security/Yama.txt
