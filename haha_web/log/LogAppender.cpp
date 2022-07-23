#include "log/LogAppender.h"
#include "base/TimeStamp.h"
#include <iostream>
#include <assert.h>

namespace haha{

namespace log{

void LogAppender::setFormatter(LogFormatter::ptr val){
    MutexType::RAIILock lock(m_mutex);
    m_formatter = val;
}

LogFormatter::ptr LogAppender::getFormatter(){
    MutexType::RAIILock lock(m_mutex);
    return m_formatter;
}


// ################################################### 同步 ###################################################


void StdoutSyncLogAppender::append(const LogInfo &info){
    auto level = info.getLevel();
    if(level >= m_level){
        MutexType::RAIILock lock(m_mutex);
        LogStream stream;
        m_formatter->format(stream, info);
        std::cout << stream.buffer().data();
    }
}


FileSyncLogAppender::FileSyncLogAppender(const std::string &filepath, off_t rollSize)
    :filepath_(filepath),
    rollSize_(rollSize)
{
    file_ = std::make_unique<LogFile>(filepath_, rollSize_, false);
}

void FileSyncLogAppender::append(const LogInfo &info){
    auto level = info.getLevel();
    if(level >= m_level){
        MutexType::RAIILock lock(m_mutex);
        LogStream stream;
        m_formatter->format(stream, info);
        auto &buffer = stream.buffer();
        file_->append(buffer.data(), buffer.length());
    }
}


// ################################################### 异步 ###################################################


AsyncLogAppender::AsyncLogAppender()
    :currentBuffer_(std::make_unique<Buffer>()),
    nextBuffer_(std::make_unique<Buffer>()),
    newBuffer1(std::make_unique<Buffer>()),
    newBuffer2(std::make_unique<Buffer>())
{
    newBuffer1->bzero();
    newBuffer2->bzero();
    buffersToWrite.reserve(16);
}


// StdoutAsyncLogAppender::StdoutAsyncLogAppender(off_t rollSize,
//                                                 int flushInterval)
//     :AsyncLogAppender(rollSize, flushInterval)
// {

// }


FileAsyncLogAppender::FileAsyncLogAppender(const std::string &filepath,
                                            off_t rollSize)
    :filepath_(filepath),
    rollSize_(rollSize)
{
    file_ = std::make_unique<LogFile>(filepath_, rollSize_, false);
}



void FileAsyncLogAppender::append(const LogInfo &info){
    auto level = info.getLevel();
    if(level >= m_level){
        LogStream stream;
        const char *logline = nullptr;
        int len = 0;

        m_formatter->format(stream, info);
        auto &buffer = stream.buffer();
        logline = buffer.data();
        len = buffer.length();

        // /* 以下两行仅用于测试哦 */
        // logline = "";
        // len = 0;

        MutexType::RAIILock lock(mutex_);
        if (currentBuffer_->avail() > len)
        {
            currentBuffer_->append(logline, len);
        }
        else
        {
            buffers_.push_back(std::move(currentBuffer_));

            if (nextBuffer_)
            {
                currentBuffer_ = std::move(nextBuffer_);
            }
            else
            {
                currentBuffer_.reset(new Buffer); // Rarely happens
            }
            currentBuffer_->append(logline, len);

            // 唤醒日志线程
            if(notify_func_){
                notify_func_();
            }
        }
    }
}


void FileAsyncLogAppender::flush(){
    // std::cout << "fuck you flush" << std::endl;

    assert(newBuffer1 && newBuffer1->length() == 0);
    assert(newBuffer2 && newBuffer2->length() == 0);
    assert(buffersToWrite.empty());
    
    {
        MutexType::RAIILock lock(mutex_);
        buffers_.push_back(std::move(currentBuffer_));
        currentBuffer_ = std::move(newBuffer1);
        buffersToWrite.swap(buffers_);
        if (!nextBuffer_)
        {
            nextBuffer_ = std::move(newBuffer2);
        }
    }


    assert(!buffersToWrite.empty());

    // 如果突然写入太多数据
    if (buffersToWrite.size() > kbuffersMaxSize)
    {
        int leftSize = kbuffersMaxSize;
        char buf[256];
        snprintf(buf, sizeof(buf), "Dropped log messages at %s, %zd larger buffers\n",
                TimeStamp::now().toFormattedString().c_str(),
                buffersToWrite.size()-leftSize);
        fputs(buf, stderr);
        file_->append(buf, static_cast<int>(strlen(buf)));
        // 保留一部分数据，丢弃多的，防止磁盘被冲垮
        buffersToWrite.erase(buffersToWrite.begin()+leftSize, buffersToWrite.end());
    }

    for (const auto& buffer : buffersToWrite)
    {
        // FIXME: use unbuffered stdio FILE ? or use ::writev ?
        file_->append(buffer->data(), buffer->length());
    }

    if (buffersToWrite.size() > 2)
    {
        // drop non-bzero-ed buffers, avoid trashing
        buffersToWrite.resize(2);
    }

    if (!newBuffer1)
    {
        assert(!buffersToWrite.empty());
        newBuffer1 = std::move(buffersToWrite.back());
        buffersToWrite.pop_back();
        newBuffer1->reset();
    }

    if (!newBuffer2)
    {
        assert(!buffersToWrite.empty());
        newBuffer2 = std::move(buffersToWrite.back());
        buffersToWrite.pop_back();
        newBuffer2->reset();
    }

    buffersToWrite.clear();
    file_->flush();
}

}

}