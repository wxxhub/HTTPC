#include <iostream>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>

#include <sstream>
#include <string.h>
#include <jsoncpp/json/json.h>

char *url = "http://127.0.0.1:8000/api";

using namespace std;

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
    if( NULL == str || NULL == buffer )
    {
        return -1;
    }

    char* pData = (char*)buffer;
    str->append(pData, size * nmemb);

    // 处理两端`"`
    str->erase(0, str->find_first_not_of("\""));
    str->erase(str->find_last_of("\""));

    // 处理斜杠
    string::iterator iter;

    for (iter = str->begin(); iter != str->end(); ) {
        if (*iter == '\\') {
            str->erase(iter);
        } else {
            iter++;
        }
    }
    return nmemb;
}

int main() {
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);

    if (CURLE_OK != res) {
        cout << "failed!" << endl;
        return 1;
    }

    CURL *pCurl;

    pCurl = curl_easy_init();

    if (NULL == pCurl) {
        cout << "Init CURL failed..." << endl;
        return -1;
    }

    curl_slist *pList = NULL;
    pList = curl_slist_append(pList,"Content-Type:application/x-www-form-urlencoded; charset=UTF-8");
    pList = curl_slist_append(pList,"Accept:application/json, text/javascript, */*; q=0.01");
    pList = curl_slist_append(pList,"Accept-Language:zh-CN,zh;q=0.8");
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList);

    curl_easy_setopt(pCurl, CURLOPT_URL, url ); //提交表单的URL地址

    curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);  //启用时会将头文件的信息作为数据流输
    curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

    //将返回结果通过回调函数写到自定义的对象中
    string strResponse;
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void *)&strResponse);

    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
    //post表单参数
    string strJsonData;
    strJsonData = "name=test_data&";
    strJsonData += "test=test&" ;

    //libcur的相关POST配置项
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


    res = curl_easy_perform(pCurl);

    long res_code=0;
    res = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

    if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
    {
        cout << strResponse << endl;

        Json::Reader reader;
        Json::Value root;
        if (reader.parse(strResponse, root)) {

            int size = root.size();
            cout << "root size:" << size << endl;
            for (int i = 0; i < size; i++) {
                cout << "name:" << root[i]["name"] << "commit:" << root[i]["commit"]  << endl;
            }
        } else {
            cout << "parse failed!" << endl;
        }
        return true;
    }

    curl_slist_free_all(pList);
    curl_easy_cleanup(pCurl);
    curl_global_cleanup();

    return 0;
}