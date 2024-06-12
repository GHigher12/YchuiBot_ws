// #include "../include/sparkchain.h"
#include <iostream>
#include <string>
#include <atomic>
#include <unistd.h>
#include <regex>
#include <xunfei_gpt/sparkchain.h>
#include <ros/ros.h>
#include <std_msgs/String.h>

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"

using namespace SparkChain;
using namespace std;

// async status tag
static atomic_bool finish(false);
// result cache
string final_result = "";

std_msgs::String string_msg;
ros::Publisher pub_res;

class SparkCallbacks : public LLMCallbacks
{
    void onLLMResult(LLMResult *result, void *usrContext)
    {
        int status = result->getStatus();
        printf(GREEN "%d:%s:%s:%s \n" RESET, status, result->getRole(), result->getContent(), usrContext);
        final_result += string(result->getContent());
        if (status == 2)
        {
            printf(GREEN "tokens:%d + %d = %d\n" RESET, result->getCompletionTokens(), result->getPromptTokens(), result->getTotalTokens());
            finish = true;
        }
    }

    void onLLMEvent(LLMEvent *event, void *usrContext)
    {
        printf(YELLOW "onLLMEventCB\n  eventID:%d eventMsg:%s\n" RESET, event->getEventID(), event->getEventMsg());
    }

    void onLLMError(LLMError *error, void *usrContext)
    {
        printf(RED "onLLMErrorCB\n errCode:%d errMsg:%s \n" RESET, error->getErrCode(), error->getErrMsg());
        finish = true;
    }
};

void uninitSDK()
{
    // 全局逆初始化
    SparkChain::unInit();
}
int initSDK()
{
    // 全局初始化
    SparkChainConfig *config = SparkChainConfig::builder();
    config->appID("822263b7")        // 你的appid
        ->apiKey("1ea480948b0f46528ba5d649b27b2b08")        // 你的apikey
        ->apiSecret("ZTNmM2JjM2ZkZmU3NTQ4NzI2MTE1YTkw"); // 你的apisecret
        // ->logLevel(0)
        // ->logPath("./aikit.log");
    int ret = SparkChain::init(config);
    printf(RED "\ninit SparkChain result:%d" RESET,ret);
    std::cout<<std::endl;
    return ret;
}
void syncLLMTest(char* input)
{
    cout << "\n######### 同步调用 #########" << endl;
    // 配置大模型参数
    LLMConfig *llmConfig = LLMConfig::builder();
    llmConfig->domain("generalv3.5");
    llmConfig->url("ws(s)://spark-api.xf-yun.com/v3.5/chat");
    Memory* window_memory = Memory::WindowMemory(5);
    LLM *syncllm = LLM::create(llmConfig,window_memory);

    // Memory* token_memory = Memory::TokenMemory(500);
    // LLM *syncllm = LLM::create(llmConfig,token_memory);

    // char* input = "你好用英语怎么说？";
        if(strcmp(input,"q") == 0){
            uninitSDK();
            // break;
        }
        // 同步请求
        LLMSyncOutput *result = syncllm->run(input);
        if (result->getErrCode() != 0)
        {
            printf(RED "\nsyncOutput: %d:%s\n\n" RESET, result->getErrCode(), result->getErrMsg());
            // continue;
        }
        else
        {
            printf(GREEN "\nsyncOutput: %s:%s\n" RESET, result->getRole(), result->getContent());
            string_msg.data = result->getContent();
            pub_res.publish(string_msg);
        }
        // input = "那日语呢？";
    // }
    
    // 垃圾回收
    if (syncllm != nullptr)
    {
        LLM::destroy(syncllm);
    }
}

void asyncLLMTest()
{
    cout << "\n######### 异步调用 #########" << endl;
    // 配置大模型参数
    LLMConfig *llmConfig = LLMConfig::builder();
    llmConfig->domain("generalv3.5");
    llmConfig->url("ws(s)://spark-api.xf-yun.com/v3.5/chat");

    Memory* window_memory = Memory::WindowMemory(5);
    LLM *asyncllm = LLM::create(llmConfig,window_memory);

    // Memory* token_memory = Memory::TokenMemory(500);
    // LLM *asyncllm = LLM::create(llmConfig,token_memory);

    if (asyncllm == nullptr)
    {
        printf(RED "\nLLMTest fail, please setLLMConfig before" RESET);
        return;
    }
    // 注册监听回调
    SparkCallbacks *cbs = new SparkCallbacks();
    asyncllm->registerLLMCallbacks(cbs);

    // 异步请求
    int i = 0;
    char* input = "你好用英语怎么说？";
    while (i++ < 2)
    {
        finish = false;
        char *usrContext = "myContext";
        int ret = asyncllm->arun(input, usrContext);
        if (ret != 0)
        {
            printf(RED "\narun failed: %d\n\n" RESET, ret);
            finish = true;
            continue;
        }

        int times = 0;
        while (!finish)
        { // 等待结果返回退出
            sleep(1);
            if (times++ > 10) // 等待十秒如果没有最终结果返回退出
                break;
        }
        input = "那日语呢？";
    }   
    // 垃圾回收
    
    if (asyncllm != nullptr)
    {
        LLM::destroy(asyncllm);
    }
    if (cbs != nullptr)
        delete cbs;
}
//
void VoiceCallBack(const std_msgs::String::ConstPtr & msg){
    string strListen = msg->data.c_str();
    std::cout<<"strListen:::strListen "<<strListen<<std::endl;
    char* strListenC = new char[strlen(msg->data.c_str())+ 1]; 
    strcpy(strListenC, msg->data.c_str()); // string转换成C-string
    if(strcmp(strListenC,"q")  != 0){
        syncLLMTest(strListenC);
    }
    else{
    }
}

int main(int argc, char *argv[])
{
    ros::init(argc,argv,"demo");
    ros::NodeHandle nh_;
    ros::Subscriber sub_sr = nh_.subscribe("/voice_detector", 10, VoiceCallBack);
    pub_res = nh_.advertise<std_msgs::String>("/voice_creator", 1000);
    cout << "\n######### llm Demo #########" << endl;
    // 全局初始化
    int ret = initSDK();
    if (ret != 0)
    {
        cout << "initSDK failed:" << ret << endl;
        return -1;
    }
    // 同步调用和异步调用选择一个执行
    // syncLLMTest(); // 同步调用
    // asyncLLMTest(); // 异步调用

    ros::Rate r(1);
    while(ros::ok()){
        ros::spinOnce();
        r.sleep();
    }
    // 退出
    uninitSDK();
    return 0;
}

