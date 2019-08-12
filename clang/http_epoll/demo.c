#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <gmodule.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ucontext.h>

#define IPADDRESS   "127.0.0.1"
#define PORT        8787
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      1000
#define EPOLLEVENTS 100

#define handle_error(msg) \
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

GHashTable *task_all;

//函数声明
//创建套接字并进行绑定
static int socket_bind(const char* ip,int port);
//IO多路复用epoll
static void do_epoll(int listenfd);
//事件处理函数
static void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd);
//处理接收到的连接
static void handle_accpet(int epollfd,int listenfd);
//添加事件
static void add_event(int epollfd,int fd,int state);
//修改事件
static void modify_event(int epollfd,int fd,int state);
//删除事件
static void delete_event(int epollfd,int fd,int state);

typedef struct conn {
    int epoll_fd;
    int conn_fd;
    uint32_t events;
    ucontext_t ucp;
} conn_t;

static void uthread_run(conn_t *c);
static void do_http(conn_t *c);

static int conn_read(conn_t *c, void *buf, size_t count);
static int conn_write(conn_t *c, void *buf, size_t count);

int main(int argc,char *argv[])
{
    int  listenfd;
    listenfd = socket_bind(IPADDRESS,PORT);
    listen(listenfd,LISTENQ);
    do_epoll(listenfd);
    return 0;
}

static int socket_bind(const char* ip,int port)
{
    int  listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1)
    {
        handle_error("socket error");
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
    {
        handle_error("bind error: ");
    }
    return listenfd;
}

int total_request = 0;
int current_request = 0;
int current_read = 0;
int current_write = 0;

ucontext_t task_next;

static void do_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;

    //创建一个描述符
    epollfd = epoll_create(FDSIZE);

    //添加监听描述符事件
    add_event(epollfd,listenfd,EPOLLIN);

    task_all = g_hash_table_new(g_int_hash, g_int_equal);

    for ( ; ; )
    {
        //获取已经准备好的描述符事件
        ret = epoll_wait(epollfd,events,EPOLLEVENTS, 1000);
        if (ret == -1) {
            handle_error("epoll_wait");
        }

        handle_events(epollfd,events, ret,listenfd);

        printf("request status total:%d current:%d read:%d write:%d \n",
                total_request,
                current_request,
                current_read,
                current_write
            );
    }
    puts("all end");
    close(epollfd);
}

static conn_t *old_conn = NULL;

static void handle_events(int epollfd, struct epoll_event *events,int num,int listenfd)
{
    int i;
    int fd;

    for (i=0; i < num; i++)
    {
        fd = events[i].data.fd;

        printf("fd:%d listenfd:%d \n", fd, listenfd);

        if ((fd == listenfd) &&(events[i].events & EPOLLIN)) {
            handle_accpet(epollfd,listenfd);
        } else {
            conn_t *tmp = g_hash_table_lookup(task_all, &fd);
            if (tmp == NULL) {
                handle_error("lookup fd error");
            }

            printf("swapcontext to fd:%d \n", tmp->conn_fd);

            swapcontext(&task_next, &tmp->ucp);
        }

        if (old_conn != NULL) {
            g_hash_table_remove(task_all, &old_conn->conn_fd);

            free(old_conn);
            old_conn = NULL;
        }
    }
}

static void handle_accpet(int epollfd,int listenfd)
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen;
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
    if (clifd == -1) {
        handle_error("accpet error:");
    }

    printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);

    //设置为非阻塞io
    int flags;
    if ((flags = fcntl(clifd, F_GETFL, NULL)) < 0) {
        handle_error("fcntl get error");
    }
    if (fcntl(clifd, F_SETFL, flags | O_NONBLOCK) == -1) {
        handle_error("fcntl set error");
    }

    size_t stack_size = 1024 * 1024;

    //创建一个新协程
    conn_t *c = (conn_t *) malloc(stack_size);
    if (c == NULL) {
        handle_error("malloc error");
    }

    c->epoll_fd = epollfd;
    c->conn_fd = clifd;

    g_hash_table_insert(task_all, &c->conn_fd, c);

    int re = getcontext(&c->ucp);
    if (re == -1) {
        handle_error("getcontext error");
    }

    c->ucp.uc_stack.ss_sp = (char*)c + sizeof(conn_t);
    c->ucp.uc_stack.ss_size = stack_size - sizeof(conn_t);
    c->ucp.uc_stack.ss_flags = 0;
    c->ucp.uc_link = &task_next;

    makecontext(&c->ucp, (void (*)(void))uthread_run, 1, c);

    printf("swapcontext new conn fd:%d \n", c->conn_fd);

    //执行协程
    swapcontext(&task_next, &c->ucp);
}

static void uthread_run(conn_t *c) {
    total_request++;

    current_request++;
    do_http(c);
    current_request--;

    close(c->conn_fd);

    old_conn = c;
}

static void do_http(conn_t *c) {
    char buf[4096];
    int n;
    char *oft;
    int all_n = 0;
    int count = 4096;

    /* memset(buf, '\0', 4096); */

    int again_n = 0;

again:
    n = conn_read(c, buf + all_n, count);
    if (n == -1) {
        handle_error("read error");
    }
    /* if (n == 0 && again_n++ > 3) { */
    if (n == 0) {
        printf("conn fd:%d broken\n", c->conn_fd);
        return;
    }

    oft = memchr(buf+all_n, '\n', n);

    all_n += n;

    if (oft == NULL) {
        count -= n;

        if (count < 1) {
            handle_error("request error");
        }

        puts("read again");

        goto again;
    }

    printf("dohttp read end\n");


    char *rep = "HTTP/1.0 200 ok\r\n\r\n";

    printf("dohttp write\n");
    conn_write(c, rep, strlen(rep));
    printf("dohttp write end1\n");
    if (all_n > 0) {
        conn_write(c, buf, all_n);
    }
    printf("dohttp end\n");
}

static int conn_read(conn_t *c, void *buf, size_t count) {
    int n;

again:
    n = read(c->conn_fd, buf, count);

    if (n == -1) {
        if (errno == EINTR) {
            goto again;
        }

        if (errno == EAGAIN) {
            add_event(c->epoll_fd, c->conn_fd, EPOLLIN|EPOLLET|EPOLLONESHOT | EPOLLERR|EPOLLHUP);

            current_read++;
            swapcontext(&c->ucp, &task_next);
            current_read--;

            /* delete_event(c->epoll_fd, c->conn_fd, EPOLLIN); */

            goto again;
        }
    }

    return n;
}

static int conn_write(conn_t *c, void *buf, size_t count) {
    int n;

again:
    n = write(c->conn_fd, buf, count);

    if (n == -1) {
        if (errno == EINTR) {
            goto again;
        }

        if (errno == EAGAIN) {
            add_event(c->epoll_fd, c->conn_fd, EPOLLOUT|EPOLLET|EPOLLONESHOT);

            current_write++;

            swapcontext(&c->ucp, &task_next);

            current_write--;

            /* delete_event(c->epoll_fd, c->conn_fd, EPOLLOUT); */

            goto again;
        }
    }

    return n;
}


static void add_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    int re = epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
    if (re == -1) {
        handle_error("epoll_ctl");
    }
}

static void delete_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    int re = epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
    if (re == -1) {
        handle_error("epoll_ctl");
    }
}

static void modify_event(int epollfd,int fd,int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}
