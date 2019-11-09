Writer: Ghufran Latif
University of San Francisco
CS 326: Operating Systems

Summary
This Program is used to display various information about the system. Including Hardware, System and Task Information. You can toggle between which information you want shown through the menu.


Each portion of the display can be toggled with command line options. We’ll let the program do the talking by printing usage information (-h option):

[magical-unicorn:~/P1-malensek]$ ./inspector -h
Usage: ./inspector [-ahrst] [-l] [-p procfs_dir]

Options:
    * -a              Display all (equivalent to -rst, default)
    * -h              Help/usage information
    * -l              Live view. Cannot be used with other view options.
    * -p procfs_dir   Change the expected procfs mount point (default: /proc)
    * -r              Hardware Information
    * -s              System Information
    * -t              Task Information

So the task list, hardware information, system information, and task information can all be turned on/off with the command line options. By default, all of them are displayed.


In order to run the program in the Terminal:
> "make"
(*Hit enter)
> "./inspector -a"

Program Output Format:

System Information
------------------
Hostname: lunafreya
Kernel Version: 4.20.3-arch1-1-ARCH
Uptime: 1 days, 11 hours, 49 minutes, 56 seconds

Hardware Information
------------------
CPU Model: AMD EPYC Processor (with IBPB)
Processing Units: 2
Load Average (1/5/15 min): 0.00 0.00 0.00
CPU Usage:    [--------------------] 0.0%
Memory Usage: [#-------------------] 9.5% (0.1 GB / 1.0 GB)

Task Information
----------------
Tasks running: 88
Since boot:
Interrupts: 2153905
Context Switches: 3678668
Forks: 38849

PID |        State |                 Task Name |            User | Tasks 
------+--------------+---------------------------+-----------------+-------
1 |     sleeping |                   systemd |               0 | 1 
2 |     sleeping |                  kthreadd |               0 | 1 
3 |         idle |                    rcu_gp |               0 | 1 
4 |         idle |                rcu_par_gp |               0 | 1 
6 |         idle |      kworker/0:0H-kblockd |               0 | 1 
