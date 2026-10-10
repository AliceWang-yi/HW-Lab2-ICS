# 六关调试线索

先完成 Phase 0 的完整示范。以下只列出观察方向，不给出正式表达式。所有地址均从本人包的反汇编中读取；同一个 seed 在单关模式和六关模式中产生相同的对应 challenge。

## 通用起点

```bash
gdb --args ./bomb --seed 42 --phase 5
```

```gdb
set pagination off
break *phase_05
run
x/s $rdi
p/x $rsi
disassemble phase_05
display/i $pc
```

程序请求输入时，提供数量正确的占位整数。`run` 带重定向时要完整写出参数，例如 `run --seed 42 --phase 5 < input.txt`。

在 phase 入口，`rdi` 为输入字符串，`rsi` 为 challenge。随后每遇到一次 `call`，都应重新根据该次调用设置的寄存器理解参数。

## P1：函数调用

从 `phase_01` 的比较向前追踪计算过程。用 `disassemble` 找出被调用的 helper，在 `call` 前查看 `edi`、`esi` 等寄存器，用 `si` 进入，或在确认其行为后用 `ni` 跳过。

```gdb
info registers rdi rsi rax
si
finish
p/u $eax
```

区分某个 helper 的返回值与最终期望值：前者可能成为下一个函数的参数。不要只记录最后一次调用在一个 challenge 下的结果。

## P2：数组与循环

在 `sscanf` 前记录各输出槽的地址，调用后使用 `x/6uw` 读取连续的六个 32 位无符号整数。前四个转换目的地址可在寄存器中看到，后续参数还可能位于栈上。

观察初值比较、循环的索引寄存器、数组寻址比例和结束条件。写出的递推应覆盖全部六项，注意 unsigned 减法和乘法按 32 位回绕。

## P3：跳转表

先找到间接 `jmp` 和此前的范围检查。表项可能是绝对地址，也可能是相对偏移，应根据真正的取表与加地址指令选择 `x/gx` 或 `x/wx`。

对当前包中的表，先查看每个表项，再反汇编目标地址。表项可能先指向一个直接 `jmp`，再到计算函数；要沿控制流继续跟踪。

challenge 会影响实际分支。完成一个 case 后仍需检查其他 case，公开测试出现的分支不一定是全部分支。

## P4：递归栈

在递归函数入口停下，对照不同层的参数、区间变化和返回值。

```gdb
info registers rdi rsi rdx rcx
bt
info frame
```

先确认基例，再分别跟踪左右分支。子调用返回后还有哪些指令，是恢复递归关系的关键；不要将“进入了哪条路径”直接等同于函数返回值。

## P5：指针与节点

找出取得初始节点指针的位置。先用 `x/gx` 读取指针，再用 `x/4wx` 或 `x/2gx` 查看节点内存；根据后续加载指令的宽度和偏移，判断哪一部分是整数，哪一部分是地址。

将取出的 `next` 地址继续用于 `x`，画出节点连接关系，并记录何时读到 `NULL`。本实验使用有限链，不能在 `NULL` 之后继续解引用；全局指针表的排列也未必与节点在内存中的排列一致。字段布局以本人的加载指令为准。

## P6：间接调用

找到形如 `call *...` 的指令，在调用前确认函数表基址、索引和实际参数。用 `x/gx` 查看选中的函数地址，再用 `si` 验证实际跳向哪个函数。

一次调用返回后，继续观察返回值如何被变换，以及下一个函数指针如何选取。只恢复一张索引表，或者只恢复单个 helper，通常不足以完成整关。

## 验证自己的求解程序

```bash
make
python3 public_test.py --phase 5
./defuser --seed 2026 --phase 5 | ./bomb --seed 2026 --phase 5
./defuser --seed 987654321 | ./bomb --seed 987654321
```

## 可选秘密关

完成六关后，若继续分析 binary，可以检查 `secret_gate` 和 `secret_phase`。秘密入口通过六关后的额外一行 payload 触发；先用 GDB 观察 `secret_gate` 的局部对象和 `secret_copy` 的写入范围，再把填充、解锁标志和 `secret_phase` 的三个输出字段写入 `solve_secret()`。三个字段由第七关 challenge 和六行答案派生的 transcript context 共同决定。秘密 payload 通过 `./defuser --secret --seed 42` 由代码生成，不能代替六关的六行输出。

单关通过后换 seed 验证。所有关完成后，最后一条命令应依次显示六次 `defused` 和最终成功提示。
