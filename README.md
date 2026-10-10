# Lab2: BombLab

## 实验简介

本实验是 CSAPP 第三章配套实验。你需要用反汇编和 GDB 分析一个没有提供源码的程序，理解其中的函数调用、分支、循环、递归和指针操作，再用 C 语言写出对应的解法。

每位同学领取自己的二进制炸弹 `bomb`，完成六个正式关卡，并提交 `defuser.c` 和实验报告。正式六关考点相同，但不同同学的常量、运算和数据布局可能不同。另有一个选做的隐藏关。

> **本实验提交的是解法程序，不是六个固定密码。** 每关会给出一个数值 challenge，正确口令随它变化。你需要在 `defuser.c` 中实现从 challenge 算出口令的函数；助教会用未公开的输入检查解法。

基础分为 100 分，隐藏关最多加 10 分。截止时间、迟交规则和报告批阅安排以 E-Learning 的课程公告及本实验作业页面为准。

## 部署实验环境

### 领取作业并克隆仓库

使用 [BombLab 学生模板](https://github.com/ICS-26Fall-FDU/BombLab) 创建自己的仓库，放在自己的 GitHub 账号下，名称使用 `ics-2026-bomblab`。助教会根据课程登记的 GitHub 账号和这个仓库名收取作业。

获取仓库后克隆到本地：

```sh
git clone <仓库地址>
cd ics-2026-bomblab
```

将 `<仓库地址>` 换成自己的仓库链接，不要直接在模板仓库中提交作业。

### 环境要求

在 Linux x86-64 环境中完成实验，例如课程服务器、x86-64 Ubuntu 虚拟机或 x86-64 WSL。需要 GCC、GNU Make、Python 3.10+、GDB 和 binutils（包含 `objdump`）。

Ubuntu / Debian 环境可以安装：

```sh
sudo apt-get update
sudo apt-get install gcc make python3 gdb binutils
```

本实验的炸弹是 64 位 Linux 程序，不需要 DataLab 使用的 32 位编译环境，也不能直接在 Windows、macOS 或 ARM Linux 上运行。可以用 `uname -sm` 检查环境；本实验需要的输出是 `Linux x86_64`。

### 先完成调试教程

`tutor/` 是独立的 tutor 教学，提供源码和参考解法，不计分，也不需要填写学号或下载个人炸弹。

在仓库根目录运行：

```sh
make -C tutor test
```

按 [tutor 教学](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/tutor/README.md) 练习设置断点、查看寄存器、读取内存和单步执行。熟悉这些操作后，再开始正式关卡。根目录的 `make test-tutor` 也可以运行 tutor 教学的测试。

### 领取个人炸弹并编译

在 `config.txt` 中填写自己的学号，例如：

```text
student_id = 243xxxx0175
```

这里只是示例，请替换成自己的学号。学号为 11 位数字或英文字母；含字母的学号直接填写，保留大小写和前导零，不要转成数字。

在仓库根目录执行：

```sh
make bomb
make
```

`make bomb` 下载个人炸弹，`make` 编译解法程序 `defuser`。两条命令没有报错，且根目录出现 `bomb` 和 `defuser`，就可以开始实验。下载失败的处理方法见下方“常见问题”。

领取成功后，修改 `defuser.c` 只需要重新运行 `make`，不用再次领取炸弹。已有 `bomb` 时，`make bomb` 也不会重复下载。

为使 GitHub Actions 能运行公开测试，首次领取后把个人炸弹加入版本管理：

```sh
git add -f bomb
```

`config.txt` 只用于领取。已有炸弹的编译、运行和测试不再需要学号配置，你可以清空学号或删除配置，不要求提交；`package.json` 也不需要提交。若不希望把配置中的学号提交到 GitHub，请在首次提交前处理，之后删除文件不会清除已有 Git 历史。

请使用自己的炸弹。正式评分根据课程登记的 GitHub 账号选择助教保存的炸弹，不根据提交的配置或二进制决定题目。

## 主要文件

以下路径均相对于学生仓库根目录：

| 文件或目录 | 用途 |
| --- | --- |
| `bomb` | 下载的个人炸弹，正式关卡不提供源码 |
| `defuser.c` | 唯一需要编写的解法代码文件 |
| `defuser.h` | 求解函数的接口声明 |
| `config.txt` | 首次领取时填写学号，领取后可清空或删除 |
| `tutor/` | [tutor 教学](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/tutor/README.md)，包含源码和参考解法 |
| `teaching/` | [配套 C 小例子](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/teaching/README.md)，用于练习六关涉及的知识点 |
| `defuser_main.c`、`challenge.c`、`cli.c` 等 | 公共驱动程序，不需要修改 |
| `public_test.py`、`tests/public.json` | 公开测试程序和 seed，包括正式六关和选做隐藏关 |
| `.github/workflows/public-tests.yml` | GitHub Actions 公开自测工作流 |

## 实验内容

### 正式六关

每关检查一行口令。正确时输出 `Phase N defused.`；错误时输出 `BOOM!!!` 并退出。爆炸不会扣分，也不会被记录，可以反复调试。

| 关卡 | 主要知识点 |
| --- | --- |
| 第 1 关 | 函数调用、参数传递、返回值 |
| 第 2 关 | 数组、循环、递推关系 |
| 第 3 关 | switch 分支与跳转表 |
| 第 4 关 | 递归调用与返回过程 |
| 第 5 关 | 结构体布局、指针、链表遍历 |
| 第 6 关 | 函数指针表与间接调用 |

六关可以分别运行和调试，不必按顺序完成。暂时卡住时，可以先分析另一关。

### 编写解法

只修改 `defuser.c` 中的解法，可以在这个文件中添加辅助函数。不要修改接口、驱动程序、测试程序、工作流或个人炸弹。评分时只取你的 `defuser.c`，与助教保存的驱动程序一起编译。

例如，第 1 关的接口是：

```c
int solve_phase_1(uint64_t challenge, char *out, size_t cap);
```

根据 challenge 算出口令，写入 `out`；字符串连同结尾的 `\0` 不能超过 `cap` 字节。成功时返回 0，尚未实现或发生错误时返回非零。

正式六关的口令是一行用空格分隔的无符号十进制整数。`out` 中不要加换行，驱动程序会自动添加；调试信息写到 stderr，写到 stdout 会混入口令。

模板中的 `defuser.c` 已经写好每关的框架：把算出的整数依次存入 `values`，把 `count` 改为整数个数，`write_answer` 会按上述格式写入 `out` 并检查截断。需要填写的位置都标有 `// TODO`；`count` 为 0 时视为尚未实现。

**示例：从汇编到求解函数。**下面以 tutor 教学为例（不计分，`tutor/` 中有完整源码），演示从汇编到求解函数的过程。在 `phase_00` 中，challenge 先被保存到 `rbx`，读入两个整数后有这样几行：

```text
shr    $0x5,%rbx            # challenge 右移 5 位
movzbl %bl,%edi             # 只保留低 8 位，相当于 & 255
xor    $0x2a,%edi           # 异或 42，得到第一个数
cmp    %edi,-0x20(%rbp)     # 与输入的第一个整数比较
...
call   tutorial_mix         # 以第一个数为参数
cmp    %eax,-0x1c(%rbp)     # 返回值与输入的第二个整数比较
```

`tutorial_mix` 只有一条 `lea 0x7(%rdi,%rdi,2),%eax`，即 `x + 2x + 7`。所以口令的两个数是：

- 第一个数：`((challenge >> 5) & 255) ^ 42`
- 第二个数：`3 * 第一个数 + 7`

写成求解函数，也就是 `tutor/phase0_solution.c`：

```c
#include <inttypes.h>
#include <stdio.h>

int solve_phase_0(uint64_t challenge, char *out, size_t cap) {
    uint32_t first = (uint32_t)((challenge >> 5) & UINT64_C(255)) ^ UINT32_C(42);
    uint32_t second = first * UINT32_C(3) + UINT32_C(7);
    int size = snprintf(out, cap, "%" PRIu32 " %" PRIu32, first, second);
    return size < 0 || (size_t)size >= cap;
}
```

最后一行检查 `snprintf` 是否出错或被截断；正式关卡中这一步由模板里的 `write_answer` 完成。正式关卡也一样：要恢复出对任意 challenge 都成立的计算关系，而不是记下某一次运行时比较的值。

注意汇编中的运算宽度。32 位无符号加、减、乘会按模 2³² 回绕；不要将它们恢复成无限精度计算，也不要依赖 C 的有符号溢出。

解法只应完成计算。可以使用 `snprintf` 等向内存缓冲区写入结果的函数，但不要读写文件、访问网络、创建进程、调用外部程序或使用内联汇编。正式评分会检查禁用调用，并在隔离环境中编译和运行。

## 调试与测试

### 固定输入，观察程序

以第 1 关为例：

```sh
./bomb --seed 42 --phase 1
```

程序显示 challenge 和口令格式，然后等待输入。`--phase 1` 表示只运行第 1 关；省略这个参数则依次运行六关。

seed 用来生成各关的 challenge。同一个 seed 和关卡对应同一个 challenge，适合反复调试；换 seed 可以检查另一组输入。分析时先固定 seed，不要每次运行都换。

### 查看汇编并设置断点

先导出第 1 关的汇编：

```sh
objdump -d --disassemble=phase_01 ./bomb > phase_01.S
```

再用 GDB 运行：

```sh
gdb --args ./bomb --seed 42 --phase 1
```

在 GDB 中执行：

```text
(gdb) break *phase_01
(gdb) run
```

等待口令时，可以输入数量正确的占位整数，例如第 1 关输入 `0`。即使答案不正确，也能在进入关卡函数时停下观察，不必先猜中答案。

先找函数调用、分支、循环和返回的位置，再分析关键指令。如果关卡调用了其他函数，还需要查看被调用函数的行为，不要只读入口函数。

各关入口函数名是 `phase_01` 至 `phase_06`。导出的汇编适合看清调用、分支和循环的整体结构，在线 GDB 适合确认某一时刻寄存器和内存中的值；两种方式在 [tutor 教学](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/tutor/README.md) 中都有完整示范。工具操作不熟练时，先回到 tutor 教学和 C 小例子；常用命令见下一节“GDB 速成”。

### GDB 速成

GDB 命令大多可以缩写，例如 `break` 写作 `b`、`continue` 写作 `c`；直接按回车会重复上一条命令，连续单步时很方便。

**启动与运行**

| 命令 | 作用 |
| --- | --- |
| `gdb --args ./bomb --seed 42 --phase 1` | 带参数启动 GDB，此时程序还没有运行 |
| `run` | 开始运行，遇到断点时停下 |
| `run --seed 42 --phase 1 < input.txt` | 从文件读取口令；带重定向时要把参数写完整 |
| `kill` | 结束这一次运行，断点保留 |
| `quit` | 退出 GDB |

**断点**

| 命令 | 作用 |
| --- | --- |
| `break *phase_01` | 在函数的第一条指令停下 |
| `break *0x401234` | 在指定地址停下，地址从自己炸弹的反汇编中读取 |
| `info breakpoints` | 列出所有断点及编号 |
| `delete 2` | 删除 2 号断点；不带编号则删除全部断点 |
| `continue` | 继续运行到下一个断点 |

**单步执行**

| 命令 | 作用 |
| --- | --- |
| `si` | 执行一条指令，遇到 `call` 时进入被调用函数 |
| `ni` | 执行一条指令，遇到 `call` 时把整个调用执行完 |
| `finish` | 运行到当前函数返回，之后用 `p/u $eax` 查看返回值 |
| `display/i $pc` | 之后每次停下都显示下一条要执行的指令 |

**查看寄存器和内存**

| 命令 | 作用 |
| --- | --- |
| `disassemble` | 查看当前函数的汇编，`=>` 标出下一条要执行的指令 |
| `info registers rdi rsi` | 查看指定寄存器；不带参数则显示全部 |
| `p/x $rsi` | 以十六进制打印；`p/u`、`p/d` 分别按无符号、有符号十进制打印 |
| `x/s $rdi` | 把 `rdi` 指向的内存当作字符串显示 |
| `x/6uw 地址` | 从该地址起读 6 个 4 字节无符号整数，适合查看数组 |
| `x/2gx 地址` | 读 2 个 8 字节值并以十六进制显示，适合查看指针 |
| `x/8gx $rsp` | 查看栈顶的 8 个 8 字节值 |
| `bt` | 查看调用栈，递归时可以看到每一层 |

`x` 命令的格式是 `x/数量格式单位`。格式常用 `x`（十六进制）、`u`（无符号十进制）、`d`（有符号十进制）、`s`（字符串）和 `i`（指令）；单位 `b`、`h`、`w`、`g` 分别表示 1、2、4、8 字节。

在关卡入口处，`rdi` 指向输入的口令字符串，`rsi` 是 challenge；进入其他函数后，要按那次调用重新判断各寄存器的含义。`layout asm` 和 `layout regs` 可以在终端中分栏显示汇编和寄存器，`tui disable` 退出分栏。

### 本地测试

每次修改 `defuser.c` 后先重新编译，再检查同一个 seed 下的口令：

```sh
make
./defuser --seed 42 --phase 1 | ./bomb --seed 42 --phase 1
```

管道左边生成口令，右边检查口令，两边的 seed 和关卡必须一致。通过时会显示 `Phase 1 defused.`。

运行第 1 关的公开测试：

```sh
python3 public_test.py --phase 1
```

模板中的六关尚未实现，初次测试会出现以下结果，中间省略了其他 seed：

```text
Phase 1 seed 0: FAIL (solver_error)
...
Public tests: 0/10
```

这不表示环境安装失败。完成解法后，本关全部公开测试通过的结果应为 `Public tests: 10/10`。

一次测试全部正式六关：

```sh
make test
```

六关全部公开测试通过时显示 `Public tests: 60/60`，随后检查隐藏关：还没有实现 `solve_secret` 时显示 `Secret phase: not attempted`，不算失败。`make test` 不包含 tutor 教学。

公开测试通过后，再选一个没有用于调试、也不在公开列表中的 seed 检查，例如：

```sh
./defuser --seed 43 | ./bomb --seed 43
```

一次运行正确不能证明所有输入都正确。还需要检查分支、循环边界和不同的数据起点。

### GitHub Actions 自测

提交并 push 个人 `bomb` 和解法后，仓库中的 `BombLab public tests` 工作流会自动运行。它分别测试正式六关和选做隐藏关，不测试 tutor 教学，也不读取 `config.txt`，不需要学号、Secrets 或登录令牌。

在 GitHub 仓库的 Actions 页面打开本次运行，查看 `Phase 1 public tests` 至 `Phase 6 public tests`。某关失败不会取消其他关，点开任务日志可以查看失败的 seed 和错误类别。

> **隐藏关是选做题。** 没有实现 `solve_secret` 时，`Secret phase public tests (optional)` 显示 `not attempted` 并通过；实现后才逐个 seed 检查。这一项失败只表示隐藏关有问题，不代表正式六关没通过，也不会让正式六关扣分，请分别查看六个正式关卡的结果。

Actions 只检查公开测试，不是最终成绩。助教会使用保存的个人炸弹重新编译解法，运行隐藏测试；报告另行人工批阅。

### 隐藏关（选做）

一次运行全部六关（不带 `--phase`），并且口令来自管道或文件时，炸弹在六关通过后还会读取第七行；没有第七行时正常结束。在终端或 GDB 中手动输入六行，都不会进入隐藏关。

求解接口是 `defuser.h` 中的：

```c
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap);
```

`challenge` 是隐藏关的 challenge，`context` 由前六行口令得到。`./defuser --secret` 会先调用六个正式关卡的求解函数计算 `context`，六关未完成时会直接报错。`solve_secret` 写出完整的第七行，不受正式六关整数格式的限制，但仍须是一行 ASCII 文本，不包含换行。

可以从 `secret_gate`、`secret_copy` 和 `secret_phase` 开始分析。入口和校验方式需要自己观察，不要求覆盖返回地址，也不需要 shellcode。

> **调试提示：用文件输入进入隐藏关。** 先用已完成的六关生成前六行，再追加一行试探输入：
>
> ```sh
> ./defuser --seed 42 > secret.txt
> echo 'test' >> secret.txt
> gdb ./bomb
> ```
>
> 在 GDB 中重定向输入，参数要完整写出：
>
> ```text
> (gdb) break *secret_gate
> (gdb) run --seed 42 < secret.txt
> ```
>
> 隐藏关的 challenge 不会打印，可以在 `secret_gate` 入口按调用约定查看参数寄存器。

> **可参考的仓库材料。** [`teaching/07_stack_copy.c`](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/teaching/07_stack_copy.c) 演示同一个结构体内的相邻字段覆盖，可以先对照它观察局部对象的布局。

完成后验证：

```sh
make
{ ./defuser --seed 42; ./defuser --secret --seed 42; } | ./bomb --seed 42
python3 public_test.py --secret
```

只有输出 `Secret phase defused.` 才算隐藏关通过；公开测试全部通过时显示 `Secret public tests: 10/10 (optional)`。`make test` 在六关全部通过后也会自动做这项检查。

公开测试和评分都要求同一个 seed 下的前六关正确。`prerequisite_` 开头的错误表示前六关在该 seed 下尚未通过，不是隐藏关答案本身的错误；评分时某个隐藏 seed 下任一正式关卡出错，该 seed 的隐藏关也不得分。

## 常见问题

### `make bomb` 下载失败

下载程序会自动重试，并在普通下载入口连接失败时尝试 GitHub 官方 API。在线下载会校验 SHA-256，通过后才安装炸弹，不需要额外填写令牌。

如果仍然失败，可以在浏览器中打开 [炸弹下载页面](https://github.com/ICS-26Fall-FDU/BombLab/releases/tag/bombs)，下载自己的 `bomb-<学号>.tar.gz`，再导入：

```sh
python3 fetch_bomb.py --file bomb-243xxxx0175.tar.gz
make
```

把文件名换成自己的附件路径，并确保配置中的学号正确。已有 `bomb` 时无需重新下载；只有丢失或删除炸弹后，才需要重新填写学号领取。

### 炸弹无法执行

先检查 `uname -sm` 是否为 `Linux x86_64`。如果出现 `Permission denied`，执行 `chmod +x bomb`；如果出现 `Exec format error` 或 `cannot execute binary file`，检查是否正在 Windows、macOS 或 ARM 环境中运行，换到符合要求的 Linux 环境。

### 公开测试失败

| 错误类别 | 含义及检查方向 |
| --- | --- |
| `solver_error` | 解法程序返回非零或异常退出；先检查是否已实现、是否返回 0，以及 stderr 中的错误提示 |
| `format_error` | 输出不是规定的一行口令；检查换行、编码和是否混入调试信息 |
| `output_limit` | 输出过长；检查是否向 stdout 输出了日志或重复内容 |
| `wrong_answer` | 口令未通过炸弹检查；检查字段数、计算过程、运算宽度及两边的 seed 和关卡 |
| `timeout` | 解法未在规定时间内结束；检查死循环、递归和算法耗时 |

编译失败时先处理 GCC 的错误，不要直接继续运行旧的 `defuser`。单个 seed 失败时，可以用同一个 seed 和关卡复现，再用 GDB 检查。

### Actions 提示 `Personal bomb is missing`

模板默认忽略下载和编译产物，普通的 `git add` 不会添加炸弹。执行 `git add -f bomb`，然后 commit、push。Actions 会恢复炸弹的可执行权限，不需要为此保留学号配置。

### 修改配置能换题吗

不能。配置只用于下载，正式评分使用课程名单中与你的 GitHub 账号绑定的炸弹。误领他人的炸弹时，请先保留自己的解法文件，删除错误的 `bomb`，用自己的学号重新领取。

## 提交

### 内容要求

仓库根目录需要有：

| 文件 | 要求 |
| --- | --- |
| `defuser.c` | 正式关卡解法；隐藏关解法也写在这个文件中 |
| `report.md` 或 `report.pdf` | 实验报告，任选一种格式，不提交 `.doc` 或 `.docx` |
| `bomb` | 个人炸弹，用于 Actions 公开自测 |

姓名、学号不作报告或提交格式要求，可自行选择是否填写。`config.txt` 和 `package.json` 不要求提交。不要提交解法可执行文件 `defuser`、tutor 教学的编译产物、整份反汇编或调试日志。

助教评分不使用上传的 `bomb`，缺少它不扣提交格式分，但 Actions 无法运行。请不要把 `password.txt` 当成解法提交。

报告需要包含：

- 每个已完成关卡的关键汇编、恢复出的计算关系和推理过程；可以配数据流图或控制流图（不计分数，不强制要求，只是为了更好地展现思路）。
- 每个已完成关卡至少一次使用公开列表以外的 seed 验证的记录。
- 已完成关卡的运行结果截图；尚未完成时，如实说明进展和遇到的问题。
- 引用的资料，以及课程允许的辅助工具的用途。

报告简洁清楚即可，不按字数或截图数量评分，也不需要逐行翻译全部汇编。对实验的意见和建议可以附在最后。

Markdown 报告中的图片放在仓库的 `images/` 或 `assets/` 目录，使用相对路径，例如 `![运行结果](images/result.png)`。图片需要一起提交；提交后在 GitHub 中打开报告，确认图片能显示。

### 上传仓库链接和代码

**先在 E-Learning 本实验作业页面提交自己的 GitHub 仓库链接。** 确保仓库公开。

然后在仓库根目录执行。以下使用 PDF 报告，Markdown 报告将 `report.pdf` 换成 `report.md`，并添加引用的图片目录：

```sh
git add -f bomb
git add defuser.c report.pdf
git commit -m "submit lab2"
git tag final
git push
git push origin final
```

> **普通的 `git push` 不会自动推送新标签。** 最后一条命令不能省略。到 GitHub 的 Tags 页面检查 `final` 是否存在、是否指向本次提交；同时检查报告和最新 Actions 结果。

截止前更新作业时，重新提交并移动 `final` 标签：

```sh
git add defuser.c report.pdf
git commit -m "update lab2"
git tag -f final
git push
git push -f origin final
```

使用 Markdown 报告时，同样替换文件名并提交新增图片。助教按截止时刻收集到的 `final` 提交评分，不是按你本地尚未 push 的内容评分。

### 评分规则

| 项目 | 分值 |
| --- | ---: |
| 提交格式：有 `defuser.c` 和报告、已推送 `final` 标签 | 1 |
| 正式六关，每关 14 分 | 84 |
| 实验报告 | 15 |
| 基础分合计 | 100 |
| 隐藏关，附加分 | +10 |

每个正式关卡使用 128 个隐藏 challenge，按通过比例计分，例如通过一半测试得 7 分。隐藏关使用 128 个隐藏 seed，同样按通过比例计分。公开测试不单独计分，只针对公开 seed 写答案表不能完成实验。

报告的 15 分按以下内容评阅，报告批阅及抽查安排遵循课程统一通知：

| 内容 | 分值 |
| --- | ---: |
| 六关分析：关键汇编、计算关系及理由，每关最多 2 分 | 12 |
| 验证记录：seed、运行结果与结论能相互对应 | 2 |
| 资料与工具说明、报告及图片可读性 | 1 |

未完全通关也可以记录已经确认的分析过程，报告不只按是否有通关截图评分。报告分由助教人工评阅，Actions 不计算这一部分。

## 协作与学术诚信

可以讨论 GDB 的使用、汇编指令和分析方法，不要交换各关的具体计算逻辑、解法代码或报告，也不要复制他人的实现。

引用资料需要在报告中列出。AI 工具的允许范围以课程公告为准；使用允许的工具时说明用途，并能解释自己的分析和代码，不要将工具生成的完整解法当作自己的分析提交。

## 参考资料与问题反馈

- CSAPP 第三章、课程讲义中的 x86-64 汇编与过程调用内容。
- [tutor 教学](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/tutor/README.md)。
- [配套 C 小例子](https://github.com/ICS-26Fall-FDU/BombLab/blob/main/teaching/README.md)。
- GDB 中的 `help`，以及 `man gdb`、`man objdump`。

遇到问题时，在课程讨论区提问或联系本实验助教，联系方式见课程公告。请说明运行环境、使用的命令、报错信息，以及已经尝试过的方法；涉及关卡解法的内容不要公开贴出完整代码。
