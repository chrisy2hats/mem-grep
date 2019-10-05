mem-grep

Requires that processes can PTRACE other processes owned by the same user
Or to be run as root

echo 0 > /proc/sys/kernel/yama/ptrace_scope


Coding style:
Functions are named PascalCase i.e. "FindName"
Global constants / #defines are uppercase and underscored i.e. "#define FOO_BAR 7"

Parts of the code base that would ideally be improved are marked with "//UGLY"
TODO parts are marked with //TODO
