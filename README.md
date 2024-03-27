# 前端
采用了chatai-vue，ChatGPT高仿前端模板。
# 后端
使用了c++ drogon框架，数据库采用MariaDB（原生提供异步读写接口）。
# 注意事项
采用全异步接口加之本人编程水平有限，因此代码相当晦涩。
由于drogon框架不支持SSE，因此需要等待远程服务彻底生成完毕后，服务端才能得到结果。
替换DashScope中的kKey才能正常访问服务
# 依赖
chatai-vue: ``https://github.com/GPTKing/chatai-vue``
drogon:``https://github.com/drogonframework/drogon.git``
