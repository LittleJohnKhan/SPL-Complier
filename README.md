# SPL-Complier

## 项目介绍

本项目是ZJU编译原理的大作业，内容是实现一个类Pascal语言的编译器

项目前端使用flex与bison完成，后端拟使用llvm完成

## 编译环境

- llvm 9.0.0
- flex 2.5.35 Apple(flex-32)
- bison (GNU Bison) 2.3

linux 环境下安装：
```bash
    sudo apt install llvm
    sudo apt install flex bison 
```

## OS

- MacOS (recommend)
- Linux

## 编译运行

NOTE: 
1. llvm use c++14 features, so compiling with `-std=gnu++11` won't work, please modify Makefile with `-std=g++14`.
2. use `-fpermissive` to downgrade some diagnostics about nonconformant code from errors to warnings.
3. change `TheModule->dump();` in SPL-Complier/CodeGenerator.cpp:15 to `module->print(llvm::errs(), nullptr);` because llvm5.0+ does not support dump anymore.

```bash
    make
    cat ./test/test2.pas | ./spl
```
## IR执行
```bash
    cat ./test/test.pas | ./spl 2> spl.ll
    lli spl.ll
```
## DAG可视化
需要Graphviz
```bash
    llvm-as < spl.ll | opt -analyze -view-cfg
    dot *.dot -T png -o dag.png
```
## AST可视化

由于使用了D3.js读取本地json，所以需要在服务器上才能读取，建议在VSCode中安装live Sever插件，然后在tree.html里右键，然后打开liveSever，这样就能看到了

如果不想安这个插件的话，就只能自己配置一个html服务器了，可以用Apache做

注意需要先运行程序生成tree.json

## Reference

[LLVM程序员手册](https://llvm.org/docs/ProgrammersManual.html#the-core-llvm-class-hierarchy-reference)
[LLVM教程](https://releases.llvm.org/2.6/docs/tutorial/JITTutorial1.html)
[GNU Pascal文档](https://www.gnu-pascal.de/gpc/Type-Declaration.html)
[lex教程](https://my.oschina.net/zhoukuo/blog/330089/print)
[flex+lex+llvm实例教程](http://www.cppblog.com/woaidongmao/archive/2009/11/11/100693.aspx)
[上个教程的git仓库](https://github.com/lsegal/my_toy_compiler)
[Pascal教程](https://www.kancloud.cn/yacker/pascaless/254516)
[GNU lex教程](https://www.gnu.org/software/bison/manual/html_node/Token-Values.html)
