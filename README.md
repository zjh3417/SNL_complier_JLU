### 介绍
本项目为 jlu 2021 级 编译原理课程设计, 实现了如下的功能
1. 词法分析
2. 语法分析
3. 语义分析
4. 没有优化的目标代码生成,并支持编译链接为elf文件

### 环境搭建
1. 准备一台Linux虚拟机 

2. 安装交叉编译工具
```C
sudo apt install gcc-i686-linux-gnu
```
3. 安装32位运行环境(最终snl编译器输出的文件是32位的x86架构的elf文件.)

```
sudo apt-get install libc6:i386
sudo apt-get install lib32z1
sudo apt-get install libncurses5:i386
```

### 编译项目

```
g++ ./src/gencode.cpp ./src/GrammarAN.cpp ./src/main.cpp ./src/scanner.cpp ./src/SemanticAnalysis.cpp ./src/utils.cpp -O2 -o snl_compiler
```
最终会在当前目录下输出snl_compiler,用于将SNL语言转为x86汇编代码


### 编译snl 代码
编译流程:
- 通过snl_compiler 得到x86汇编代码

```
./snl_compiler code >./main.s
```

- 将main.s编译为目标文件
```
i686-linux-gnu-as main.s -o main
```

- 编译snllib.c,用于支持输入输出

```
i686-linux-gnu-gcc ./src/snllib.c -masm=intel -fno-stack-protector -c -o ./lib/snllib.o
```

- 链接
```
i686-linux-gnu-ld main ./lib/snllib.o -o snl
```

- 运行
```
./snl
```


### Example

#### 测试代码
```
program HelloWorld

var
array [1..12] of char prompt;

begin
	prompt[1] := 'H';
	prompt[2] := 'e' ;
	prompt[3] := 'l' ;
	prompt[4] := 'l' ;
	prompt[5] := 'o' ;
	prompt[6] := ' ' ;
	prompt[7] := 'W' ;
	prompt[8] := 'o' ;
	prompt[9] := 'r' ;
	prompt[10] := 'l';
	prompt[11] := 'd';
	prompt[12] := '!';

	write(prompt);
	
end.
```

#### 编译
```
sb@sb-virtual-machine:~/Desktop/SNL_2$ ./snl_compiler ./sample/helloworld.snl >./main.s
sb@sb-virtual-machine:~/Desktop/SNL_2$ i686-linux-gnu-as ./main.s -o ./main.o

sb@sb-virtual-machine:~/Desktop/SNL_2$ i686-linux-gnu-ld ./main.o ./lib/snllib.o -o ./main
```

#### 运行
```
sb@sb-virtual-machine:~/Desktop/SNL_2$ ./main 
Hello World!
```