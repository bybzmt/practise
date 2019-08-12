ucontext-人人都可以实现的简单协程库
=========

2.ucontext初接触
----------
利用ucontext提供的四个函数getcontext(),setcontext(),makecontext(),swapcontext()可以在一个进程中实现用户级的线程切换。

 本节我们先来看ucontext实现的一个简单的例子：
 ```
#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

int main(int argc, const char *argv[]){
    ucontext_t context;

    getcontext(&context);
    puts("Hello world");
    sleep(1);
    setcontext(&context);
    return 0;
}
 ```

想想程序运行的结果会是什么样？

3.ucontext组件到底是什么
-----------
在类System V环境中,在头文件< ucontext.h > 中定义了两个结构类型，mcontext_t和ucontext_t和四个函数getcontext(),setcontext(),makecontext(),swapcontext().利用它们可以在一个进程中实现用户级的线程切换。

mcontext_t类型与机器相关，并且不透明.ucontext_t结构体则至少拥有以下几个域:

```
typedef struct ucontext {
   struct ucontext *uc_link;
   sigset_t         uc_sigmask;
   stack_t          uc_stack;
   mcontext_t       uc_mcontext;
   ...
} ucontext_t;
```

当当前上下文(如使用makecontext创建的上下文）运行终止时系统会恢复uc_link指向的上下文；uc_sigmask为该上下文中的阻塞信号集合；uc_stack为该上下文中使用的栈；uc_mcontext保存的上下文的特定机器表示，包括调用线程的特定寄存器等。

 下面详细介绍四个函数：

```
int getcontext(ucontext_t *ucp);
```

初始化ucp结构体，将当前的上下文保存到ucp中

```
int setcontext(const ucontext_t *ucp);
```

设置当前的上下文为ucp，setcontext的上下文ucp应该通过getcontext或者makecontext取得，如果调用成功则不返回。如果上下文是通过调用getcontext()取得,程序会继续执行这个调用。如果上下文是通过调用makecontext取得,程序会调用makecontext函数的第二个参数指向的函数，如果func函数返回,则恢复makecontext第一个参数指向的上下文第一个参数指向的上下文context_t中指向的uc_link.如果uc_link为NULL,则线程退出。

```
void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
```

makecontext修改通过getcontext取得的上下文ucp(这意味着调用makecontext前必须先调用getcontext)。然后给该上下文指定一个栈空间ucp->stack，设置后继的上下文ucp->uc_link.

当上下文通过setcontext或者swapcontext激活后，执行func函数，argc为func的参数个数，后面是func的参数序列。当func执行返回后，继承的上下文被激活，如果继承上下文为NULL时，线程退出。

```
int swapcontext(ucontext_t *oucp, ucontext_t *ucp);
```

保存当前上下文到oucp结构体中，然后激活upc上下文。

如果执行成功，getcontext返回0，setcontext和swapcontext不返回；如果执行失败，getcontext,setcontext,swapcontext返回-1，并设置对于的errno.

简单说来， getcontext获取当前上下文，setcontext设置当前上下文，swapcontext切换上下文，makecontext创建一个新的上下文。

原文链接：https://blog.csdn.net/qq910894904/article/details/41911175
