# SPL-Complier

## 项目介绍

本项目是ZJU编译原理的大作业，内容是实现一个类Pascal语言的编译器

项目前端使用flex与bison完成，后端拟使用llvm完成

## 编译环境

- llvm 9.0.0
- flex 2.5.35 Apple(flex-32)
- bison (GNU Bison) 2.3

## OS

- MacOS (recommend)
- Linux

## 编译运行

```bash
    make
    cat ./test/test2.pas | spl
```