#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

class __attribute__((visibility("default"))) MessageQueue {
private:
    int _msgid;

public:
    struct Message {
        long type;      // 消息类型
        double text[2]; // 消息内容
    };

public:
    MessageQueue() = default;
    ~MessageQueue() = default;

    /**
     * @description: 获取消息队列
     * @param {int} key
     * @return {*}
     */
    bool get_msg_queue(int key);

    /**
     * @description: 删除消息队列
     * @return {*}
     */
    bool del_msg_queue();

    /**
     * @description: 发送数据
     * @param {Message} &msg
     * @param {bool} &queue_cache 是否缓存数据
     * @return {*}
     */
    bool send(const Message &msg, const bool &queue_cache = false);

    /**
     * @description: 接收数据
     * @param {Message} &msg 
     * @return {*}
     */
    bool recv(Message &msg);
};

#endif // __MESSAGE_QUEUE_H__
