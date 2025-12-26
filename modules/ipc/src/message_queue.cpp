#include "ipc/message_queue.hpp"

#include <cstring>
#include <sys/ipc.h>
#include <sys/msg.h>

bool MessageQueue::get_msg_queue(int key) {
    _msgid = msgget(key, 0644 | IPC_CREAT); // 如果key不存在则创建消息队列
    if (_msgid == -1) {
        return false;
    }
    return true;
}

bool MessageQueue::del_msg_queue() {
    if (msgctl(_msgid, IPC_RMID, nullptr) == -1) {
        return false;
    }
    return true;
}

bool MessageQueue::send(const MessageQueue::Message &msg, const bool &queue_cache) {
    if (!queue_cache) {
        if (msgsnd(_msgid, &msg, sizeof(msg.text), 0) == -1) {
          return false;
        }
    } else{
        Message tmp;
        // 接收队列中第一个消息，非阻塞
        while (msgrcv(_msgid, &tmp, sizeof(tmp.text), 0, IPC_NOWAIT) != -1) {
            // 消息已被读取并丢弃
        }
        
        // 发送数据
        // 阻塞直到队列有空间
        if (msgsnd(_msgid, &msg, sizeof(msg.text), 0) == -1) {
            return false;
        }
    }
    return true;
}

bool MessageQueue::recv(MessageQueue::Message &msg) {
    // 消息队列标识符，接收消息的结构体指针，接收数据部分的最大大小，接收消息的类型规则，行为标志
    // 接收队列中第一个消息，阻塞
    if (msgrcv(_msgid, &msg, sizeof(msg.text), 0, 0) == -1) {
        return false;
    }
    return true;
}