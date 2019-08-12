#include <ucontext.h>
#include <stdio.h>
#include <unistd.h>

void func1(void * arg)
{
    puts("1");
    puts("11");
    puts("111");
    puts("1111");
}

void context_test()
{
    char stack[1024*128];
    ucontext_t child,main;

    getcontext(&child); //获取当前上下文
    child.uc_stack.ss_sp = stack;//指定栈空间
    child.uc_stack.ss_size = sizeof(stack);//指定栈空间大小
    child.uc_stack.ss_flags = 0;
    child.uc_link = &main;//设置后继上下文

    makecontext(&child,(void (*)(void))func1,0);//修改上下文指向func1函数

    swapcontext(&main,&child);//切换到child上下文，保存当前上下文到main
    puts("main");//如果设置了后继上下文，func1函数指向完后会返回此处
}


char bb_stack[1024*128];
ucontext_t aa, bb;

void func2_2() {
    printf("uc_link %x\n", bb.uc_link);
    swapcontext(&bb, &aa);
    puts("func2_2");
    swapcontext(&bb, &aa);
    printf("uc_link %x\n", bb.uc_link);
}

void func2()
{
    puts("bb start");
    func2_2();
    puts("bb end");
}

void context_test2()
{
    getcontext(&bb);
    bb.uc_stack.ss_sp = bb_stack;
    bb.uc_stack.ss_size = sizeof(bb_stack);
    bb.uc_stack.ss_flags = 0;
    bb.uc_link = &aa;

    makecontext(&bb,(void (*)(void))func2,0);

    puts("aa start");

    swapcontext(&aa, &bb);

    puts("aa back");

    swapcontext(&aa, &bb);
    puts("aa back2");
    swapcontext(&aa, &bb);

    puts("aa end");
}


int main()
{
    context_test2();

    puts("main end");

    return 0;
}
