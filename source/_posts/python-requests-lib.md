title: python的requests库
date: 2016-01-29 23:10:43
tags: [python, 转载]
categories: 网络
---
requests库适合我这种懒人。
为什么要用requests而不用urllib2呢？
官方文档中是这样说的：
<!-- more -->

> python的标准库urllib2提供了大部分需要的HTTP功能，但是API太逆天了，一个简单的功能就需要一大堆代码。

### 发送HTTP请求

	r = requests.get('http://www.qq.com')
    r = requests.get('https://twitter.com')
    
HTTP请求还有很多类型，比如POST,PUT,DELETE,HEAD,OPTIONS。

	r = requests.post("http://httpbin.org/post")
    r = requests.put("http://httpbin.org/put")
    r = requests.delete("http://httpbin.org/delete")
    r = requests.head("http://httpbin.org/get")
    r = requests.options("http://httpbin.org/get")
    
### 传递参数

比如在采集百度搜索结果时，我们wd参数（搜索词）和rn参数（搜素结果数量），你可以手工组成URL:

	payload = {'wd': '张亚楠', 'rn': '100'}
    r = requests.get("http://www.baidu.com/s", params=payload)
    print r.url
    
### 获取response

	print r.text
    print r.content
    
r.content是以字节的方式去显示,在输出结果中以b开头。但我在cygwin中用起来并没有，下载网页正好。所以就替代了urllib2的urllib2.urlopen(url).read()功能。

### 获取网页编码

	print r.encoding
    
当你发送请求时，requests会根据HTTP头部来猜测网页编码，当你使用r.text时，requests就会使用这个编码。当然你还可以修改requests的编码形式。

	 r = requests.get('http://www.zhidaow.com')
     r.encoding = 'ISO-8859-1'
     # 对encoding修改后就直接会用修改后的编码去获取网页内容。
     

    
### 获取网页状态码

	print r.status_code
    
遇到跳转的页面，例如访问http://github.com是302跳转到https
r.status_code会显示最终的状态码，若查看跳转前的状态码：

	print r.history
    # 结果：
    # (<Response [302]>,)
    
禁止跳转：

	r = requests.get('http://github.com', allow_redirects = False)

### 请求头内容

	print r.request.headers
    
自定义headers:

	headers = {'User-Agent': 'alexkh'}
	r = requests.get('http://www.zhidaow.com', headers = headers)
    
### 获取响应头内容

	print r.headers
    
以字典的形式返回了全部内容，我们也可以访问部分内容。

	print r.headers['Content-Type']
    # 结果：
    # 'text/html; charset=utf-8'
    
### 设置超时

一旦超过这个时间还没获得响应内容，抛出timeout异常。

	r = requests.get('http://github.com', timeout=0.001)

### 代理访问

    proxies = {
      "http": "http://10.10.1.10:3128",
      "https": "http://10.10.1.10:1080",
    }

    r = requests.get("http://www.zhidaow.com", proxies=proxies)
    
    
### 抓取HTTPS网页

当使用requests.get(url)抓取HTTPS网页时，会遇到requests.exceptions.SSLError错误
解决方法：禁用验证

	r = requests.get('https://github.com', verify=False)
    
### 解析json

像urllib和urllib2，如果用到json，就要引入新模块，如json和simplejson
但在requests中已经有了内置的函数，r.json()。就拿查询IP的API来说：

	r = requests.get('http://ip.taobao.com/service/getIpInfo.php?ip=122.88.60.28')
    print r.json()['data']['country']

### 参考文献

[requests的官方指南文档](http://docs.python-requests.org/en/latest/user/quickstart)
[requests的高级指南文档](http://docs.python-requests.org/en/latest/user/advanced/#advanced)
[requests的中文文档](http://cn.python-requests.org/en/latest)

尤其是上面第二个《高级指南》，不错。

原文地址：[http://www.zhidaow.com](http://www.zhidaow.com/post/python-requests-install-and-brief-introduction)

