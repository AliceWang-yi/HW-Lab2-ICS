# Phase 0：从寄存器到通用 solver

Phase 0 不计分，全部教材和练习都在本目录中。这里的 `bomb` 和 `defuser` 是独立的教学程序，不需要填写学号、下载个人炸弹或完成正式关卡。正式关卡仍在仓库根目录完成，不提供源码和参考解法。

从学生包根目录开始：

```bash
cd tutor
make
make test
```

`make` 会编译教学炸弹和已有的 Phase 0 参考求解程序，生成反汇编、占位输入与示例答案。根目录的 `make tutor` 和 `make test-phase0` 也是兼容入口。以下命令均在 `tutor/` 中执行。

## 1. 固定一次运行

在本目录执行：

```bash
./bomb --seed 42 --phase 0
```

记录屏幕上的 challenge。先输入 `0 0`，本次失败只用于认识输入格式。

## 2. 在第一条指令停下

```bash
gdb --args ./bomb --seed 42 --phase 0
```

```gdb
set pagination off
break *phase_00
run --seed 42 --phase 0 < phase0-input.txt
x/i $pc
x/s $rdi
p/x $rsi
disassemble /r phase_00
```

`phase0-input.txt` 包含 `0 0`。重定向命令中显式保留参数：`run` 后出现新参数或重定向时，不应假定此前的 `--args` 仍被沿用。也可以首次直接用 `run` 后手工输入。入口原型是：

```c
bool phase_00(const char *input, uint64_t challenge);
```

因此此刻 `rdi` 指向输入，`rsi` 保存 challenge。`break *phase_00` 指定机器指令入口；`break phase_00` 在有调试信息的程序中可能跳过序言。观察参数时不要混淆这两种断点。

## 3. 追踪读取到栈上的整数

在 `disassemble` 输出中找到 `sscanf` 的调用。此前的 `valid_u32_fields` 只检查十进制格式、数量和范围，使用 `ni` 跳过即可。

```gdb
display/i $pc
ni
info registers rdi rsi rdx rcx rbp rsp
```

重复 `ni`，在调用 `sscanf` 的指令执行前停下。这是一个新的函数调用，寄存器含义已经变化：`rdi` 是字符串，`rsi` 是格式串，`rdx` 和 `rcx` 分别是两个输出整数的地址。

```gdb
x/s $rdi
x/s $rsi
set $first_slot = $rdx
set $second_slot = $rcx
ni
p/d $eax
x/1uw $first_slot
x/1uw $second_slot
```

`eax` 应为 2，两个内存位置中都是占位值 0。`x/1uw` 表示把一个 4 字节单元按无符号十进制读取。保存的是 GDB 的便利变量，不要求被调试程序保留任何变量名。

也可以在反汇编中找到 `sscanf` 后第一条指令，将其实际地址用于 `break *地址`。地址必须从本人包中读取，不要照搬其他发行包的偏移。

## 4. 恢复第一个数

继续逐条执行，观察 challenge 如何被保存、右移、截取和异或。寄存器分配由编译器决定，不预设 challenge 一定在 `rbx`。

本练习可恢复为：

```c
uint32_t first = (uint32_t)((challenge >> 5) & 255) ^ 42;
```

把这条规律与本目录生成的反汇编 `phase0.disasm.txt` 对照。比较指令最终检查输入的第一个整数是否等于这个值。占位输入会在这里失败，不需要强行跳过判断。

## 5. 观察 helper 的参数和返回值

重新运行，这次读取教程提供的正确示例：

```gdb
delete breakpoints
break *tutorial_mix
run --seed 42 --phase 0 < phase0-answer.txt
p/u $edi
disassemble tutorial_mix
si
```

函数参数是一个 `uint32_t`，观察 `edi` 即可。它计算 `3 * first + 7`，编译器可能用 `lea` 表达乘法和加法。用 `si` 观察指令，或执行 `finish` 返回调用者，再用 `p/u $eax` 检查返回值。

```c
uint32_t second = 3 * first + 7;
```

两项条件都成立时，继续运行会打印 `Phase 0 defused.`。

## 6. 将规律写成可执行程序

示例实现在 `phase0_solution.c`，使用 `snprintf` 写入调用者的缓冲区，并检查是否截断。

```bash
make
./defuser --seed 42 --phase 0
./defuser --seed 42 --phase 0 | ./bomb --seed 42 --phase 0
make test
./defuser --seed 987654321 --phase 0 | ./bomb --seed 987654321 --phase 0
```

换 seed 后仍能通过，说明程序实现的不是某一组示例口令。完成教程后执行 `cd ..`，填写学号并运行 `make bomb` 领取个人炸弹。接下来对正式关卡重复同样的观察过程，例如在 `phase_02` 中额外恢复六元素数组的循环和递推关系。
